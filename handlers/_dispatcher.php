<?php

$isGET = ($_SERVER['REQUEST_METHOD'] == 'GET');
$isPOST = ($_SERVER['REQUEST_METHOD'] == 'POST');
$isXHR = (array_key_exists('HTTP_X_REQUESTED_WITH', $_SERVER) && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest');

$fooQuery = $rewriteQuery;
$resource = array_shift($fooQuery);

include_once(CONTEXTIO);

if ($resource == 'photos') {
	
	if (empty($fooQuery[0]) || substr($fooQuery[0], 0, 4) == 'page') {
		// get list of files and return it
		$pics = $sessUser->fetchPictureList();
		$page = (empty($fooQuery[0])) ? 0 : intval(preg_replace("/^page/",'',$fooQuery[0]));
		$bodyContent = "<ul>";
		for ($i = $page * THUMBNAIL_PAGE_SIZE; $i < (($page + 1) * THUMBNAIL_PAGE_SIZE); ++$i) {
			if ($pics[$i]) {
				$bodyContent .= "<li><a href=\"/photos/{$pics[$i]['fileId']}\"><img src=\"/photos/{$pics[$i]['fileId']}/thumb\" border=\"0\" />{$pics[$i]['fileName']}</a></li>";
			}
		}
		$bodyContent .= "</ul>";
		include_once(UI_WEB_MAIN);
	}
	else {
		// retrieve a given picture
		$picId = array_shift($fooQuery);
		$size = array_shift($fooQuery);
		
		$file = ($size == 'thumb') ? $sessUser->getPictureThumbnail($picId) : $sessUser->getPicture($picId);
		header('Content-type: image/jpeg');
        header('Content-length: ' . filesize($file));
        readfile($file);
		die();
	}
}

else {
	if (!$isXHR) {
		include_once(UI_WEB_MAIN);
	}
	else {
		// unknown request
	}
}


?>