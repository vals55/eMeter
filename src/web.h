#ifndef _WEB_H
#define _WEB_H
#include <avr/pgmspace.h>

extern void startWeb();
extern void stoptWeb();
extern void handleWeb();

const char HTTP_MAIN[] PROGMEM = "<!DOCTYPE html><html lang='en'><head><meta name='format-detection' content='telephone=no'><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1,user-scalable=no'/><title>eMeter</title>"
"<script type='text/javascript' src='http://github.com/joewalnes/smoothie/raw/master/smoothie.js'>"
"</script><script>function _elem(b){return document.getElementById(b)}function send(url, milli, _process){const xhr=new XMLHttpRequest(); xhr.open('GET', url); xhr.timeout=milli; xhr.onreadystatechange=function(a){if (xhr.readyState===4 && xhr.status===200){_process(xhr.responseText)}}; xhr.send()}window.onload=function(){function _refresh(){function _process(msg){var obj=JSON.parse(msg); Object.keys(obj).forEach(function(id){if (_elem(id)){_elem(id).innerHTML=obj[id]}})}send('/states', 500, _process);}_refresh(); var d=setInterval(_refresh, 2000);}</script>"
"<style>.pic{animation:5s linear infinite rotate;}.v,.c,.p,.f,.e{margin-right:5px;text-align:end;font-weight:600;}body{text-align:center;font-family:verdana}div{padding:0;font-size:1em;margin:0;box-sizing:border-box}input[type='file']{border:1px solid #C038D3}.logo-cont{display:flex}.logo-text{display:inline;font-weight:700;color:#9303A7;font-size:30px;line-height:30px;padding-left:15px;margin-top:0}.wrap{text-align:left;display:inline-block;min-width:260px;max-width:500px;margin:5px}a{color:#333;font-weight:500;text-decoration:none}a:hover{color:#C038D3;text-decoration:none;}h1{background-color:#060606;color:#fff;}.tbl{display:flex}.value{width:150px;border:1px solid #eee;text-align:center;}.col{width:200px;border:1px solid #eee;text-align:center;}.grey{background-color:#ddd;}.cnt{margin-right:10px;}"
".info{width:100%; text-align:end;font-size:.7em;padding-top:5px;}"
"@keyframes rotate{from{transform:rotate(0deg);}to{transform:rotate(360deg);}}"
"</style>"
"</head>"
"<body class='b'><div class='wrap'><div class='logo-cont'><div class='logo-img'><svg class='pic' fill='none' height='34' viewBox='0 0 34 34' width='34' xmlns='http://www.w3.org/2000/svg'><circle stroke='dodgerblue' stroke-width='4' r='15.0' cx='17.0' cy='17.0'></circle><rect fill='lightcoral' x='14' y='14' height='6' width='15' ry='3' transform='rotate(-40 17 17)'></svg></div><p class='logo-text'>eMeter</p><div class='info'><div>CPU <span id='freq'></span> MHz</div><div>memory: <span id='heap'></span>Kbyte</div><div>RSSI: <span id='rssi'></span> dBm</div><div>firmware: <span id='firmware'></span></div><div>Проверить обновления <a href='/update'>прошивки</a></div></div></div><div class='tbl'><div class='col grey'><p>Линия</p></div><div class='value grey'><p>U, В</p></div><div class='value grey'><p>I, А</p></div><div class='value grey'><p>P, Вт</p></div><div class='value grey'><p>Частота, Гц</p></div></div><div class='tbl'><div class='col grey'><p>Фаза 1</p></div><div class='value'><p class='v' id='voltage'></p></div><div class='value'><p class='c' id='current'></p></div><div class='value'><p class='p' id='power'></p></div><div class='value'><p class='f' id='frequency'></p></div></div><div class='tbl'><div class='col grey'><p>Максимум</p></div><div class='value'><p class='v' id='maxvoltage'></p></div><div class='value'><p class='c' id='maxcurrent'></p></div><div class='value'><p class='p' id='maxpower'></p></div><div class='value'><p class='f' id='maxfreq'></p></div></div><div class='tbl'><div class='col grey'><p>Минимум</p></div><div class='value'><p class='v' id='minvoltage'></p></div><div class='value'><p class='c' id='mincurrent'></p></div><div class='value'><p class='p' id='minpower'></p></div><div class='value'><p class='f' id='minfreq'></p></div></div><div class='tbl'><span class='cnt'><p >Показания счетчика электроэнергии</p></span><span class='cnt'><p class='e' id='energy'></p></span><span class='cnt'><p>кВт-ч</p></span></div><div class='canvas'><canvas id='mycanvas' width='500' height='200'></canvas></div><script type='text/javascript'> (function(){var line1=new TimeSeries(); var line2=new TimeSeries(); setInterval(function(){line1.append(new Date().getTime(), parseInt(_elem('voltage').value)); line2.append(new Date().getTime(), parseInt(_elem('current').value));}, 1000); var smoothie=new SmoothieChart(); smoothie.addTimeSeries(line1); smoothie.addTimeSeries(line2); smoothie.streamTo(document.getElementById('mycanvas'));})();</script></div></body>"
"</html>";


#endif