<?php
/**
 * @file
 * wetkit_varnish.expire.inc
 */


/**
 * Implement hook_expire_cache_alter().
 *
 * Rebuilds list of paths to expire based on the domain aliases defined.
 */
function wetkit_varnish_expire_cache_alter(&$expire, $node = NULL, $paths) {
  $domainlist = variable_get('wetkit_varnish_domainlist', '');

  // Don't do anything to the $expire array if we have no configuration.
  if ($domainlist != '') {
    // Convert our textfield string into an array of domains.
    //@todo can we not just use explode() here?
    $domains = preg_split('/[\r\n]+/', $domainlist, -1, PREG_SPLIT_NO_EMPTY);
    $urls = array();

    // We want a full URL for each expiring path, for each domain.
    foreach (array_unique($expire) as $path) {
      foreach ($domains as $domain) {
        $urls[] = $domain . '/' . $path;
      }
    }

    $expire = $urls;
  }
}
