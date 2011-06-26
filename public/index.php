<?php

require_once('../conf/conf.php');
require_once(UTILS);

session_name(SESSION_NAME);
session_set_cookie_params(0,'/');
session_save_path(SESSION_DIR);
session_start();

$rewriteQueryStr = (array_key_exists('rwq', $_GET)) ? preg_replace("/^\/|\/$/",'',$_GET['rwq']) : null;
$rewriteQuery = (array_key_exists('rwq', $_GET)) ? explode('/', $rewriteQueryStr) : null;

if ($rewriteQuery[0] == 'signin') {
	
	// validate user connection
	$u = new User();
	$uId = $u->authentify(strval($_POST['u']), strval($_POST['p']));

	if ($uId === false) {
		$feedback = array('class'=>'error', 'code' => ERROR_CODE_INVALID_CREDENTIALS);
		require_once(UI_WEB_LOGIN);
	} else {
		// set sessions cookies and info
		$key = generateKey(mt_rand(64,86));
		setcookie(SESSION_KEYNAME,$key,0,'/');
		$_SESSION['key'] = bin2hex($key);
		$_SESSION['userId'] = $uId;
		header("Location: /");
	}
	
} elseif ($rewriteQuery[0] == 'signout') {
	
	// destroy session cookies and files
	killSession();
	header("Location: /");
	
} elseif (!array_key_exists(SESSION_KEYNAME, $_COOKIE) || is_null($_COOKIE[SESSION_KEYNAME])) {

	// show login form
	require_once(UI_WEB_LOGIN);
	
} else {

	// validate key in cookies matches key for session file
	if (bin2hex($_COOKIE[SESSION_KEYNAME]) != $_SESSION['key']) {
		// invalid access destroy this session
		killSession();
		header("Location: /");
	}
	else {
		$sessUser = new User($_SESSION['userId']);
		require_once(DISPATCHER);
	}
}


function killSession() {
	setcookie(session_name(), '', $_SERVER['REQUEST_TIME']-42000, '/');
	setcookie(SESSION_KEYNAME,'', $_SERVER['REQUEST_TIME']-42000, '/');
	session_destroy();
	$_SESSION = array();	
}

?>