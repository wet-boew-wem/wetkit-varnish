<?php
/**
 * @file
 * wetkit_varnish.multi_domain.inc
 */


/**
 * Implements hook_expire_cache().
 *
 * Takes an array from the expire module, and issue purge. You may also safely
 * call this function directly with an array of local urls to purge.
 *
 * @param array $paths
 *   Array with paths to ban.
 */
function wetkit_varnish_expire_cache($paths) {

  $base = base_path();
  $host = _varnish_get_host();

  // Generate ban string, from path array.
  $purge = implode('$|^' . $base, $paths);
  $purge = '^' . $base . $purge . '$';

  // Loop through domains set in configuratin.
  $varnish_multi_domain = variable_get('wetkit_varnish_multi_domain', array(''));
  foreach ($varnish_multi_domain as $key => $domain) {
    if (drupal_strlen($domain) > 4 && $host != $domain) {
      varnish_purge($domain, $purge);
    }
  }
}
