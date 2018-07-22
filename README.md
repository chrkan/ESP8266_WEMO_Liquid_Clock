# ESP8266_WEMO_Liquid_Clock

bassierend auf der Liquid Clock von Christian Aschoff
Web Setup Idee von http://tmw-it.ch/qlockwork/

======================= Needed Libraries =======================


ArduinoOTA.h

ESP8266HTTPUpdateServer.h

ESP8266mDNS.h

ESP8266WebServer.h

ESP8266WiFi.h   

WiFiManager.h          //https://github.com/tzapu/WiFiManager

Adafruit_NeoPixel.h

TimeLib.h

======================= Initial Setup =======================

You dont need to edit the sketch, all settings can make wireless over wifi.

After the first flash the Liqud Clock circle will be blue, now you will find a wifi network withe the name Liquid.
Connect your divice with it and edit the wifi settings.

If it workt the Liquid Clock restart and the ring will show blue followed of green. The LiquidClock has connect to your Wifinetwork. 

Now the Liquid Clock is connect to the NTP Server if it works you see an rainbow cirle. If it is not posible to connect to your NTP Server please connect your browser withe the LiquidClock an edit in the Settings the NTP Server.


