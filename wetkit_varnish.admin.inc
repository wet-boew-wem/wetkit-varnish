<?php
/**
 * @file
 * wetkit_varnish.admin.inc
 */


/**
 * Bans admin settings.
 */
function wetkit_varnish_bans_admin_settings_form($form, &$form_state) {
  $form = array();
  $form['description'] = array(
    '#markup' => '<p>' . t('Bans is how you invalidate content currently in the cache.') . '</p>',
  );

  // Simple URL.
  $form['single'] = array(
    '#type' => 'fieldset',
    '#title' => t('Simple url'),
  );
  $form['single']['url'] = array(
    '#type' => 'textfield',
    '#title' => t('Url (Use / to update frontpage)'),
    '#value' => '/',
    '#size' => 150,
    '#description' => t('For example: /node/1200/'),
    '#required' => FALSE,
  );
  $form['single']['submit'] = array(
    '#type' => 'submit',
    '#name' => 'single',
    '#value' => t('Ban'),
    '#submit' => array('wetkit_varnish_bans_single'),
  );

  // File Types.
  $form['file_type'] = array(
    '#type' => 'fieldset',
    '#title' => t('File types'),
  );
  $form['file_type']['extension'] = array(
    '#type' => 'checkboxes',
    '#title' => t('File types to ban'),
    '#options' => drupal_map_assoc(array('jpg', 'png', 'gif', 'css', 'js')),
  );
  $form['file_type']['submit'] = array(
    '#type' => 'submit',
    '#name' => 'file_type',
    '#value' => t('Ban'),
    '#submit' => array('wetkit_varnish_bans_file_extension'),
  );

  // Find content type paths, from the url_alias table.
  $query = db_select('url_alias', 'u');
  $query->groupBy('url');
  $query->addExpression("SUBSTRING_INDEX(u.source, '/', 1)", 'url');
  $result = $query->execute();
  $paths = $result->fetchCol();

  $query = db_select('url_alias', 'u');
  $query->groupBy('url');
  $query->addExpression("SUBSTRING_INDEX(u.alias, '/', 1)", 'url');
  $result = $query->execute();
  $paths = array_merge($paths, $result->fetchCol());

  // Remove paths that's shorter then two chars.
  for ($i=0; $i<count($paths); $i++) {
    if (strlen($paths[$i]) < 2) {
      unset($paths[$i]);
    }
  }
  asort($paths);

  // Content Types.
  $form['content_type'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content types'),
  );
  $form['content_type']['content'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content types to ban'),
    '#options' => drupal_map_assoc($paths),
  );
  $form['content_type']['submit'] = array(
    '#type' => 'submit',
    '#name' => 'content_type',
    '#value' => t('Ban'),
    '#submit' => array('wetkit_varnish_bans_content_type'),
  );

  // Advanced.
  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced'),
    '#description' => t('Advanced'),
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );
  $form['advanced']['whole_cache'] = array(
    '#type' => 'submit',
    '#name' => 'whole_cache',
    '#value' => t('Empty the whole cache'),
    '#submit' => array('wetkit_varnish_bans_whole_cache'),
  );
  $form['advanced']['expression'] = array(
    '#type' => 'textfield',
    '#title' => t('url regexp'),
    '#size' => 150,
    '#description' => t('For example: /arcicle/ Remove everything below articles/ on the site in the cache:'),
    '#required' => FALSE,
  );
  $form['advanced']['submit'] = array(
    '#type' => 'submit',
    '#name' => 'regexp',
    '#value' => t('ban'),
    '#submit' => array('wetkit_varnish_bans_regexp'),
  );

  return system_settings_form($form);
}

/**
 * Admin form to list your domains.
 */
function wetkit_varnish_form_expire_admin_settings_form_alter(&$form, &$form_state, $form_id) {
  $form['wetkit_varnish_domainlist_group'] = array(
    '#type' => 'fieldset',
    '#title' => t('Domain Aliases'),
    '#description' => t('Domain aliases to purge whenever content is altered.')
  );

  $form['wetkit_varnish_domainlist_group']['wetkit_varnish_domainlist'] = array(
    '#type' => 'textarea',
    '#title' => t('Domain list'),
    '#default_value' => variable_get('wetkit_varnish_domainlist', ''),
    '#cols' => 60,
    '#rows' => 5,
    '#description' => t('List domains (including protocol) one per line with no trailing slash.'),
  );

  // Prevent the Expire protocol include base url option from being selected
  $form['format']['expire_include_base_url']['#disabled'] = TRUE;
  $form['format']['expire_include_base_url']['#default_value'] = FALSE;
  $form['format']['expire_include_base_url']['#description'] = $form['format']['expire_include_base_url']['#description'] . ' <em>' . t('NOTE: This option is disabled when using Expire Aliases (below).') . '</em>';
  variable_set('expire_include_base_url', FALSE);
}

/**
 * Maxage admin settings.
 */
function wetkit_varnish_maxage_admin_settings_form($form, &$form_state) {

  $form = array();
  $form['description'] = array(
    '#markup' => '<p>' . t('Use the varnish maxage module to manualy set s-maxage and max-age on content.') . '</p>',
  );

  // Find content type paths, from the url_alias table.
  $query = db_select('url_alias', 'u');
  $query->groupBy('url');
  $query->addExpression("CONCAT('/',SUBSTRING_INDEX(u.source, '/', 1))", 'url');
  $result = $query->execute();
  $paths = $result->fetchCol();

  $query = db_select('url_alias', 'u');
  $query->groupBy('url');
  $query->addExpression("CONCAT('/',SUBSTRING_INDEX(u.alias, '/', 1))", 'url');
  $result = $query->execute();
  $paths = array_merge($paths, $result->fetchCol());
  asort($paths);

  // Get paths from menu hook.
  $menu_paths = array();
  $items = module_invoke_all('menu');
  drupal_alter('menu', $items);
  foreach ($items as $path => $info) {
    if(strpos($path, 'admin') === false && strpos($path, '/add') === false && strpos($path, '/delete') === false) {
      $menu_paths[] = preg_replace('/%.*/i', '', $path);
    }
  }
  foreach ($menu_paths as $key => $path) {
    $menu_paths[$key] = '/' . trim($path, '/');
  }
  asort($menu_paths);

  // Merge paths
  $paths = array_merge(array('<default>', '<front>'), $paths, $menu_paths);

  $form['wetkit_varnish_maxage'] = array(
    '#title' => t(''),
    '#description' => t('Add maxage with formate s/m/h/d/ ex: 2h equals two hours. The / is the front page, and default is use fore pages with no value set'),
    '#theme' => 'wetkit_varnish_maxage_table',
    '#tree' => TRUE,
    '#type' => 'fieldset',
  );

  $varnish_maxage = variable_get('wetkit_varnish_maxage', array());

  foreach ($paths as $path) {
    $form['wetkit_varnish_maxage']['paths'][$path]['path'] = array(
      '#default_value' => $path,
      '#title' => t(''),
      '#title_display' => 'invisible',
      '#description' => t(''),
      '#description_display' => 'invisible',
      '#disabled' => TRUE,
      '#size' => 40,
      '#type' => 'textfield',
    );
    $form['wetkit_varnish_maxage']['paths'][$path]['max-age'] = array(
      '#default_value' => !empty($varnish_maxage['paths'][$path]['max-age']) ? $varnish_maxage['paths'][$path]['max-age'] : '',
      '#title' => t(''),
      '#title_display' => 'invisible',
      '#description' => t(''),
      '#description_display' => 'invisible',
      '#size' => 20,
      '#type' => 'textfield',
    );
    $form['wetkit_varnish_maxage']['paths'][$path]['s-maxage'] = array(
      '#default_value' => !empty($varnish_maxage['paths'][$path]['s-maxage']) ? $varnish_maxage['paths'][$path]['s-maxage'] : '',
      '#title' => t(''),
      '#title_display' => 'invisible',
      '#description' => t(''),
      '#description_display' => 'invisible',
      '#size' => 20,
      '#type' => 'textfield',
    );
  }

  return system_settings_form($form);
}

/**
 * Multi-domain admin settings.
 */
function wetkit_varnish_multi_domain_admin_settings_form($form, &$form_state) {

  $varnish_multi_domain = variable_get('wetkit_varnish_multi_domain', array(''));
  // Add one extra at the end.
  $varnish_multi_domain = array_merge($varnish_multi_domain, array(''));

  $form['description'] = array(
    '#markup' => '<p>' . t('Add support for updating multiple domains.') . '</p>',
  );

  $form['wetkit_varnish_multi_domain'] = array(
    '#tree' => TRUE,
    '#type' => 'fieldset',
  );

  foreach ($varnish_multi_domain as $key => $domain) {
    $form['wetkit_varnish_multi_domain'][$key] = array(
      '#type' => 'textfield',
      '#title' => t('Domain %number', array('%number' => ($key + 1))),
      '#default_value' => $varnish_multi_domain[$key],
      '#size' => 150,
      '#maxlength' => 150,
      '#description' => t('For example: www.example.com'),
      '#required' => FALSE,
    );
  }

  return system_settings_form($form);
}
