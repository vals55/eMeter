<?php
if (! defined('ROOT_PATH')) {
    define('ROOT_PATH', $_SERVER['DOCUMENT_ROOT'] . DIRECTORY_SEPARATOR);
}

require_once ROOT_PATH . 'efunc.php';

if(!(isset($_SERVER['HTTP_USER_AGENT']) && $_SERVER['HTTP_USER_AGENT'] == 'ESP8266HTTPClient')) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    header('Content-type: text/plain; charset=utf8', true);
    echo "only for ESP8266!\n";
    exit;
}

if($_SERVER['REQUEST_METHOD'] == 'POST' && $_SERVER["CONTENT_TYPE"] == 'application/json') {
  $postData = file_get_contents('php://input');
  $data = json_decode($postData, true);

//  $file_post = $_SERVER["DOCUMENT_ROOT"] . "/log/json.log";
//  $fw = fopen($file_get, "a");
//  fwrite($fw, "GET " . var_export($data, true));
//  fclose($fw);

  if($data != NULL) {
    addData("emeter_stat", $data);
  }
  exit;
}
?>
