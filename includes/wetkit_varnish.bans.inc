<?php
/**
 * @file
 * wetkit_varnish.bans.inc
 */


/**
 * Bans single url.
 */
function wetkit_varnish_bans_single($form, &$form_state) {
  drupal_set_message(t('Ban single page'));
  $url = $form_state['input']['url'];
  if (strpos($url, 'http://')) {
    $url = drupal_substr($url, strpos($url, '/', 9));
  }
  wetkit_varnish_bans_ban("^" . $url . "$");
}

/**
 * Bans file types.
 */
function wetkit_varnish_bans_file_extension($form, &$form_state) {
  drupal_set_message(t('Ban based on file type'));
  foreach ($form_state['input']['extension'] as $extension) {
    if (drupal_strlen($extension) > 1) {
      wetkit_varnish_bans_ban("." . $extension);
    }
  }
}

/**
 * Bans content types.
 */
function wetkit_varnish_bans_content_type($form, &$form_state) {
  drupal_set_message(t('Ban based on content type'));
  foreach ($form_state['input']['content'] as $content) {
    if (drupal_strlen($content) > 1) {
      wetkit_varnish_bans_ban("^/" . $content . "/");
    }
  }
}

/**
 * Bans the whole cache.
 */
function wetkit_varnish_bans_whole_cache($form, &$form_state) {
  drupal_set_message(t('Ban whole cache'));
  wetkit_varnish_bans_ban("^/");
}

/**
 * Bans regexp functionality.
 */
function wetkit_varnish_bans_regexp($form, &$form_state) {
  drupal_set_message(t('Ban based on regexp'));
  wetkit_varnish_bans_ban($form_state['input']['advanced']['expression']);
}

/**
 * Helper function to purge items for a host that matches the provided pattern.
 *
 * @param string $pattern
 *   The pattern to look for and purge.
 */
function wetkit_varnish_bans_ban($pattern) {
  drupal_set_message(check_plain('update: ' . $pattern));

  $host = _varnish_get_host();

  // Get the current varnish version, from varnish modul seting.
  // if we are using Varnish 3.x, then we need to use ban instead of purge.
  $version = floatval(variable_get('varnish_version', 2.1));
  $command = $version >= 3 ? "ban" : "purge";
  $run = array();

  $run[] = "$command req.http.host ~ $host && req.url ~ \"$pattern\"";

  // Multi domain support.
  $varnish_multi_domain = variable_get('wetkit_varnish_multi_domain', array(''));

  foreach ($varnish_multi_domain as $key => $domain) {
    if (isset($domain) && $domain != '' && $host != $domain) {
      $run[] = "$command req.http.host ~ $domain && req.url ~ \"$pattern\"";
    }
  }

  // Run comand.
  $status = _varnish_terminal_run($run);

  // Print status.
  foreach ($status as $key => $server) {
    $out = 'SERVER: ' . $key . ' ';
    $error = '';
    foreach ($server as $key => $command) {
      $key = explode (' ', $key);
      $out .= '| ' . $key[3];
      if ($command['code'] != 200) {
        $out .= ' ERROR - ' . $command['msg'] . ' ';
      }
      else {
        $out .= ' OK ';
      }
    }
    drupal_set_message(check_plain($out));
  }
}
