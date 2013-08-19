<?php
/**
 * @file
 * wetkit_varnish.theme.inc
 */


/**
 * Layout for the table in the admin settings form.
 */
function theme_wetkit_varnish_maxage_table($variables) {
  $form = $variables['form'];

  $header = array(
    array('data' => t('Path')),
    array('data' => t('max-age (used by user browser)')),
    array('data' => t('s-maxage (used by varnish)')),
  );

  $rows = array();
  foreach (element_children($form['paths']) as $key => $path) {
    $rows[] = array(
      'data' => array(
        drupal_render($form['paths'][$path]['path']),
        drupal_render($form['paths'][$path]['max-age']),
        drupal_render($form['paths'][$path]['s-maxage']),
      ),
    );
  }
  $output = theme('table', array('header' => $header, 'rows' => $rows));

  return $output;
}
