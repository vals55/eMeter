# eMeter
<div id="badges">
  <a href="https://www.espressif.com/">
    <img src="https://img.shields.io/badge/Espressif ESP8266-ED4549" alt="ESP8266"/>
  </a>  
  <a href="https://www.home-assistant.io/">
    <img src="https://img.shields.io/badge/Home Assistant-01A9F4" alt="HA"/>
  </a>  
  <a href="https://https://platformio.org//">
    <img src="https://img.shields.io/badge/PlatformIO-FF7F00" alt="Platformio"/>
  </a>  
  <a href="#">
    <img src="https://komarev.com/ghpvc/?username=vals55&style=flat-square&color=green" alt="vals"/>
  </a>  
</div>
Some time ago I was interested in the project of data collection from a water meter, especially since my meter had a pigtail wire with a pulse output. And as it turned out, not only I was concerned about this issue, I even found a wonderful project [https://github.com/dontsovcmc/waterius], which I successfully repeated, modifying it to suit myself. I replaced the battery power supply with a permanent one, made my own board layout and slightly reworked the firmware code for my own considerations.

And some time ago I decided to do the same trick with electricity. It would seem that electricity meters are quite functional devices, but by this time I already had a smart home - Home Assistant and it lacked beautiful graphs of parameters of consumed energy. That's how this project was born. It uses a well-known non-invasive energy meter PZEM-004T v3.0. (looking ahead, I will say that an earlier version is also suitable). The library for it is quite good, as well as a number of other useful libraries, I just had to put it all together.

I planned to make this design in a din-rail enclosure for a typical electrical panel. In addition to measuring the energy parameters, 4 contacts: voltage and current sensor, I also added two pairs of inputs for pulse counters, to which you can connect photoresistors, which in turn can be attached to the LEDs of the meter itself. This is a universal solution, you only need to ask about the conversion factor for it, and you can get information from any type of meter that has a "blinker" or even two (if multi-tariff). Pulses, knowing the voltage of the line, can be recalculated into current and power, the function will not be as smooth as with direct measurements because of the discreteness of the measurement intervals, which for accuracy should be made longer, but quite acceptable.

The device can send all the measured energy parameters to the MQTT server, through it to Home Assistant and via http protocol to the server of statistics collection, if it will be interesting for someone.

Also current measured energy parameters are reflected on the device web-page, and on the same page you can update the firmware and admire the curves of current, voltage, power and frequency graphs in your electric line and their extreme values for a day.

## Created with
- PlatformIO (https://platformio.org/)
- ESP8266 board definition 3.1.2 (https://github.com/esp8266/Arduino)
- PZEM-004T-v30 (https://github.com/mandulaj/PZEM-004T-v30)
- WiFiManafer (https://github.com/tzapu/WiFiManager)
- ArduinoJson (https://github.com/bblanchon/ArduinoJson)
- Arduino Client for MQTT (PubSubClient 2.8) by Nick O'Leary (https://github.com/knolleary/pubsubclient)
- rLog (https://github.com/kotyara12/rLog)

### Credits 
Evgeny Dontsov (https://github.com/dontsovcmc/waterius), 2018 for the idea of a successful approach