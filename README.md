# eMeter
<div id="badges">
  <img src="https://img.shields.io/badge/Espressif ESP8266-ED4549" alt="ESP8266"/>
  <img src="https://img.shields.io/badge/Home Assistant-01A9F4" alt="HA"/>
  <img src="https://komarev.com/ghpvc/?username=vals55&style=flat-square&color=green" alt="vals"/>
</div>
Когда-то давным-давно меня заинтересовал проект сбора данных от счетчика воды, тем более, что у моего счетчика был "хвостик" провода с импульсным выходом. И как оказалось, не только меня волновал этот вопрос, я даже нашел замечательный проект [https://github.com/dontsovcmc/waterius] который успешно повторил, модифицировав под себя. Питание от батареек заменил на постоянное, сделал собственную компоновку платы и немного переработал код прошивки под свои соображения.

И вот некоторое время назад я решил проделать такой же фокус с электричеством. Казалось бы счетчики электроэнергии вполне функциональные устройства, но у меня к этому времени уже был умный дом - Home Assistant и в нем не хватало красивых графиков параметров потребляемой энергии. Так и родился этот проект. В нем используется известный неинвазивный измеритель параметров электроэнергии PZEM-004T v3.0. (забегая вперед, скажу, что подходит и более ранняя версия). Библиотека к нему вполне хороша, как и ряд других полезных библиотек, мне оставалось только собрать это все воедино.

Я задумал сделать эту конструкцию в корпусе на din-рейку для типового электрощитка. Кроме измерения параметров сети, 4 контакта: напряжения и датчика тока, я добавил также две пары входов для счетчиков импульсов, к которым можно подключить фоторезисторы, которые в свою очередь можно закрепить на светодиодах самого счетчика. Это универсальное решение, остается только поинтересоваться коэффициентом пересчета для него, и можно получать информацию от счетчика любого типа у которого есть "моргалка" а то и две (если многотарифный). Импульсы, зная напряжение сети, можно пересчитать в ток и мощность, получится не такая гладкая функция как с непосредственными измерениями из-за дискретности интервалов измерения, которые для точности надо бы делать подлиннее, но вполне приемлемо.

Все намерянные показатели сети устройство может отправлять на сервер MQTT, через него в Home Assistant и по протоколу http на сервер сбора статистики, если она кому-то будет интересна.

Также текущие показатели отражаются на web-странице устройства, но этой же странице можно обновить прошивку и полюбоваться на кривые графиков тока, напряжения, мощности и частоты в вашей электрической сети и их экстремальные значения за сутки.
