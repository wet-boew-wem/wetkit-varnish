; WetKit Varnish Makefile

api = 2
core = 7.x

; Modules for WetKit Varnish

projects[varnish][version] = 1.x-dev
projects[varnish][subdir] = contrib
projects[varnish][type] = module
projects[varnish][download][type] = git
projects[varnish][download][revision] = e6726b4
projects[varnish][download][branch] = 7.x-1.x
projects[varnish][patch][1323418] = http://drupal.org/files/varnish-supportincludebaseurl-1323418-9.patch
projects[varnish][patch][1362576] = http://drupal.org/files/varnish-n1362576-13.patch
projects[varnish][patch][2017097] = http://drupal.org/files/varnish-ban-fix-2017097-2.patch

projects[expire][version] = 2.x-dev
projects[expire][subdir] = contrib
projects[expire][type] = module
projects[expire][download][type] = git
projects[expire][download][revision] = 2f5b65e
projects[expire][download][branch] = 7.x-2.x

projects[maintenance_mode_api][version] = 1.0-beta1
projects[maintenance_mode_api][subdir] = contrib
projects[maintenance_mode_api][type] = module

projects[varnish_dashboard][version] = 1.x-dev
projects[varnish_dashboard][subdir] = contrib
projects[varnish_dashboard][type] = module
projects[varnish_dashboard][download][type] = git
projects[varnish_dashboard][download][revision] = 3aa7f96
projects[varnish_dashboard][download][branch] = 7.x-1.x

projects[varnish_maintenance_mode][version] = 1.0-beta1
projects[varnish_maintenance_mode][subdir] = contrib
projects[varnish_maintenance_mode][type] = module
