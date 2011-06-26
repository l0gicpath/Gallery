<?php

// the absolute path to install dir
// this is used for all include or require statements
define('APPROOT',dirname(dirname(__FILE__)));
define('LIBS_DIR', APPROOT.'/libs');
define('HANDLERS_DIR', APPROOT.'/handlers');
define('UTILS',LIBS_DIR."/utils.inc");

define('CONTEXTIO',LIBS_DIR."/ContextIO/class.contextio.php");

define('SESSION_DIR',APPROOT.'/etc/sess');
define('SESSION_NAME','gallery_sess');
define('SESSION_KEYNAME','gallery_key');

define('PIC_CACHE', APPROOT.'/data');

define('DISPATCHER', APPROOT.'/handlers/_dispatcher.php');

define('UI_DIR', APPROOT.'/interface');
define("UI_WEB_LOGIN", UI_DIR.'/html/login.phtml');
define("UI_WEB_MAIN", UI_DIR.'/html/main.phtml');

define('JSON_RESPONSE',APPROOT.'/interface/response.json.php');

define("LIBRARY_VERSION_JQUERY", '1.6.1');
define("LIBRARY_VERSION_JQUERYUI", '1.8.13');

define("THUMBNAIL_MAX_HEIGHT",150);
define("THUMBNAIL_MAX_WIDTH",150);
define("THUMBNAIL_PAGE_SIZE",10);

define('DB_NAME','gallery');

function classAutoload($className) {
	$className = strtolower($className);
	if (file_exists(LIBS_DIR.'/class.'. $className .'.inc')) require_once(LIBS_DIR.'/class.'. $className .'.inc');
	elseif (file_exists(LIBS_DIR.'/class.'. $className .'.php')) require_once(LIBS_DIR.'/class.'. $className .'.php');
}
spl_autoload_register('classAutoload');

define("ERROR_CODE_INVALID_CREDENTIALS", 101);
define("ERROR_CODE_MISSING_REQUIRED_PARAMETERS", 201);
define("ERROR_CODE_CONTACT_ALREADY_IN_LIST", 202);

include_once(APPROOT."/conf/conf.keys.php");

?>