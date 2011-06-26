<?php 

header('Content-Type: application/json'); 
if (!isset($jsonData) || is_null($jsonData)) $jsonData = array();
echo json_encode($jsonData);

?>
