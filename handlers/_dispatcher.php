<?php

$isGET = ($_SERVER['REQUEST_METHOD'] == 'GET');
$isPOST = ($_SERVER['REQUEST_METHOD'] == 'POST');
$isXHR = (array_key_exists('HTTP_X_REQUESTED_WITH', $_SERVER) && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest');

$fooQuery = $rewriteQuery;
$resource = array_shift($fooQuery);

include_once(CONTEXTIO);

if ($resource == 'photos') {
	// retrieve a given picture
	$picId = array_shift($fooQuery);
	$size = array_shift($fooQuery);
	
	$file = ($size == 'thumb') ? $sessUser->getPictureThumbnail($picId) : $sessUser->getPicture($picId);
	header('Content-type: image/jpeg');
	header('Content-length: ' . filesize($file));
	readfile($file);
	die();
}

else {
	if (!$isXHR) {
		// get list of files and return it
		$pics = $sessUser->fetchPictureList();
		$page = (empty($rewriteQuery[0])) ? 0 : intval(preg_replace("/^page/",'',$rewriteQuery[0]));
		$numPages = ceil(count($pics) / THUMBNAIL_PAGE_SIZE);
		$email = $sessUser->getContextIoAccount();
		include_once(UI_WEB_MAIN);
	}
	else {
		// unknown request
	}
}


?>