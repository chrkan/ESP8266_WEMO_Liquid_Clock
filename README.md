# ESP8266_WEMO_Liquid_Clock

bassierend auf der Liquid Clock von Christian Aschoff
Web Setup Idee von http://tmw-it.ch/qlockwork/

======================= Needed Libraries =======================


ArduinoJson.h

ArduinoOTA.h

Adafruit_NeoPixel.h

ESP8266HTTPUpdateServer.h

ESP8266mDNS.h

ESP8266httpUpdate.h    

ESP8266WebServer.h

ESP8266WiFi.h   

Syslog.h

TimeLib.h

Timezone.h

WiFiClientSecure.h //https://github.com/esp8266/Arduino/blob/4897e0006b5b0123a2fa31f67b14a3fff65ce561/doc/esp8266wifi/client-secure-examples.md

WiFiManager.h          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

======================= Initial Setup =======================

You do not have to edit the sketch, all settings can be wireless via WLAN.

After the first flash, the Liqud Clock circle will be blue, now you will find a WiFi network called Liquid. Connect your device and edit the Wi-Fi settings.

If it works, restart the Liquid Clock and the ring will turn blue followed by green. The LiquidClock has a connection to your Wifinetwork.

Now the Liquid Clock is connected to the NTP server, if it works, you will see a rainbow circle. If it is not possible (red circle) to connect to your NTP server, please connect your browser with the LiquidClock and edit in the settings of the NTP server.


