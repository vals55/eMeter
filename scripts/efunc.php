<?php
if (! defined('ROOT_PATH')) {
    define('ROOT_PATH', $_SERVER['DOCUMENT_ROOT'] . DIRECTORY_SEPARATOR);
}
require_once ROOT_PATH . 'inc/config.php';

function addData($table, $data) {
  $connect = new mysqli(DBHOST, DBUSER, DBPASS, DBASE);

  $stmt = $connect->prepare("INSERT INTO `$table`(
  `voltage0`,
  `current0`,
  `power0`,
  `frequency`,
  `energy0`,
  `pf`,
  `imp01`,
  `imp02`,
  `imp1`,
  `imp2`,
  `voltage1`,
  `current1`,
  `power1`,
  `energy1`,
  `voltage2`,
  `current2`,
  `power2`,
  `energy2`,
  `rssi`,
  `mac`,
  `ip`,
  `ver`,
  `ver_esp`,
  `chip_id`,
  `freemem`,
  `mqtt_period`,
  `stat_period`,
  `timestamp`
  ) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
  $p1 = $data["voltage0"];
  $p2 = $data["current0"];
  $p3 = $data["power0"];
  $p4 = $data["frequency"];
  $p5 = $data["energy0"];
  $p6 = $data["pf"];
  $p7 = $data["imp01"];
  $p8 = $data["imp02"];
  $p9 = $data["imp1"];
  $p10 = $data["imp2"];
  $p11 = $data["voltage1"];
  $p12 = $data["current1"];
  $p13 = $data["power1"];
  $p14 = $data["energy1"];
  $p15 = $data["voltage2"];
  $p16 = $data["current2"];
  $p17 = $data["power2"];
  $p18 = $data["energy2"];
  $p19 = $data["rssi"];
  $p20 = $data["mac"];
  $p21 = $data["ip"];
  $p22 = $data["ver"];
  $p23 = $data["ver_esp"];
  $p24 = $data["chip_id"];
  $p25 = $data["freemem"];
  $p26 = $data["mqtt_period"];
  $p27 = $data["stat_period"];
  $p28 = $data["timestamp"];
  $stmt->bind_param("ddddddiiiiddddddddissisiiiis", $p1, $p2, $p3, $p4, $p5, $p6, $p7, $p8, $p9, $p10, $p11, $p12, $p13, $p14, $p15, $p16, $p17, $p18, $p19, $p20, $p21, $p22, $p23, $p24, $p25, $p26, $p27, $p28);  
  $stmt->execute();

  $last_month = date('m') - 1;
  $last_year = date('Y');
  if ($last_month == 0) {
    $last_month = 12;
    $last_year = $last_year - 1;
  }

  $connect->close();
}

?>