<?php

header('Content-type: text/plain; charset=utf8', true);

function check_header($name, $value = false) {
    if(!isset($_SERVER[$name])) {
        return false;
    }
    if($value && $_SERVER[$name] != $value) {
        return false;
    }
    return true;
}

function sendFile($path) {
    header($_SERVER["SERVER_PROTOCOL"].' 200 OK', true, 200);
    header('Content-Type: application/octet-stream', true);
    header('Content-Disposition: attachment; filename='.basename($path));
    header('Content-Length: '.filesize($path), true);
    header('x-MD5: '.md5_file($path), true);
    readfile($path);
}

$fname = array(
    "C4:5B:BE:42:CD:B2" => "firmware",
    "18:FE:AA:AA:AA:AA" => "DOOR-7-g14f53a19",
    "18:FE:AA:AA:AA:BB" => "TEMP-1.0.0"
);

$version = array(
    "C4:5B:BE:42:CD:B2" => "0.38",
    "18:FE:AA:AA:AA:AA" => "1",
    "18:FE:AA:AA:AA:BB" => "2.55"
);

if(isset($_GET['mac'])){
    $localBinary = "./bin/".$fname[$_GET['mac']].".bin";
    $localVer = "firmware:".$version[$_GET['mac']];
    if(isset($fname[$_GET['mac']])) {
      echo $localVer."md5:".md5_file($localBinary);
    } else {
      echo "firmware:0md5:-1";
    }
    exit();
}

if(!check_header('HTTP_USER_AGENT', 'ESP8266-http-Update')) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    echo "only for ESP8266 updater!\n";
    exit();
}

if(
    !check_header('HTTP_X_ESP8266_STA_MAC') ||
    !check_header('HTTP_X_ESP8266_AP_MAC') ||
    !check_header('HTTP_X_ESP8266_FREE_SPACE') ||
    !check_header('HTTP_X_ESP8266_SKETCH_SIZE') ||
    !check_header('HTTP_X_ESP8266_SKETCH_MD5') ||
    !check_header('HTTP_X_ESP8266_CHIP_SIZE') ||
    !check_header('HTTP_X_ESP8266_SDK_VERSION')
) {
    header($_SERVER["SERVER_PROTOCOL"].' 403 Forbidden', true, 403);
    echo "only for ESP8266 updater! (header)\n";
    exit();
}

if(!isset($fname[$_SERVER['HTTP_X_ESP8266_STA_MAC']])) {
    header($_SERVER["SERVER_PROTOCOL"].' 500 ESP MAC not configured for updates', true, 500);
}

$localBinary = "./bin/".$fname[$_SERVER['HTTP_X_ESP8266_STA_MAC']].".bin";

// проверяем, прислал ли ESP8266 версию SDK;
// если она есть в заголовках, проверяем соответствие MD5-хэшэй между 
// бинарным файлом на сервере и бинарным файлом на ESP8266;
// если они равны, то апдейта выполнено не будет:
if(check_header('HTTP_X_ESP8266_SDK_VERSION') && $_SERVER["HTTP_X_ESP8266_SKETCH_MD5"] != md5_file($localBinary)) {
    sendFile($localBinary);
} else {
    header($_SERVER["SERVER_PROTOCOL"].' 304 Not Modified', true, 304);
}
?>
