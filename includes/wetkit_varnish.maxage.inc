<?php
/**
 * @file
 * wetkit_varnish.maxage.inc
 */


/**
 * Implements hook_boot().
 */
function wetkit_varnish_boot() {
  // Set maxage only for users thats not logged in.
  if (!(bool) $GLOBALS['user']->uid) {

    $q = preg_split('/\//', $_GET['q']);
    $varnish_maxage = variable_get('wetkit_varnish_maxage', array());
    $varnish_maxage = $varnish_maxage['paths'];
    $varnish_maxage['/'] = $varnish_maxage['<front>'];

    if (isset($varnish_maxage['/' . $q[0]])) {

      $max_age = _wetkit_varnish_maxage_convert_time($varnish_maxage['/' . $q[0]]['max-age']);
      if (!is_numeric($max_age)) {
        $max_age = _wetkit_varnish_maxage_convert_time($varnish_maxage['<default>']['max-age']);
      }

      $s_maxage = _wetkit_varnish_maxage_convert_time($varnish_maxage['/' . $q[0]]['s-maxage']);
      if (!is_numeric($s_maxage)) {
        $s_maxage = _wetkit_varnish_maxage_convert_time($varnish_maxage['<default>']['s-maxage']);
      }

      drupal_add_http_header('Cache-Control', 'max-age=' . $max_age . ', s-maxage=' . $s_maxage . ', public');
    }
  }
}

/**
 * Convert string to secunds.
 *
 * @param string $string
 *   String with formate s/m/h/d/ ex: 2h equals two hours.
 *
 * @return int
 *   String converted to secunds
 */
function _wetkit_varnish_maxage_convert_time($string) {
  if (strpos($string, 's')) {
    return str_replace('s', '', $string);
  }
  elseif (strpos($string, 'm')) {
    return str_replace('m', '', $string) * 60;
  }
  elseif (strpos($string, 'h')) {
    return str_replace('h', '', $string) * 3600;
  }
  elseif (strpos($string, 'd')) {
    return str_replace('d', '', $string) * 86400;
  }
}
