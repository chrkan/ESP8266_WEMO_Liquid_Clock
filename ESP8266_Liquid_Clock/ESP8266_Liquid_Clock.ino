/**
 *  Liquid Clock WEMO 
 * 
 *  bassierend auf der Liquid Clock von Christian Aschoff
 * 
 * 
 * 
 * */
#include <ArduinoOTA.h>
#include <NtpClientLib.h>        
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>   
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <Adafruit_NeoPixel.h>
#include "LDR.h"
#include "Configuration.h"
// ------------------ Pixel Einstellungen ---------------------

#define NUM_PIXEL      60       // Anzahl der NeoPixel LEDs
#define STRIP_PIN            2        // Digital  ESP8266

#define LDR_SIGNAL A0


IPAddress myIP = { 0,0,0,0 };
//===================================================



// Der lichtabhaengige Widerstand
LDR ldr(LDR_SIGNAL);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXEL, STRIP_PIN, NEO_GRB + NEO_KHZ800);
// Die gelesene Zeit...
int hours, minutes, seconds;
// Hilfsvariablen fuer den organischen Effekt...
unsigned long syncTimeInMillis, milliSecondsSyncPoint;
ESP8266WebServer esp8266WebServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void setup() {

  Serial.begin(115200);
  
  Serial.print("Neo-Pixel-Liquid-Clock-WEMOS"); 
  strip.begin();
  strip.setBrightness(254);
  
  delay(200);

    // alle LEDs an...
  for(byte i=0; i<NUM_PIXEL; i++) {
    strip.setPixelColor(i, 0, 0 ,255);
    strip.show();
    delay(50);
  }
/* ------------------ Wifi --------------------- */
WiFiManager wifiManager;
#ifdef WIFI_RESET
  wifiManager.resetSettings();
#endif
 clearStrip();
  wifiManager.setTimeout(WIFI_SETUP_TIMEOUT);
  wifiManager.autoConnect(HOSTNAME, WIFI_AP_PASS);
  if (WiFi.status() != WL_CONNECTED)
  {
   
    WiFi.mode(WIFI_AP);
    Serial.println("No WLAN connected. Staying in AP mode.");
    delay(1000);
    myIP = WiFi.softAPIP();
  }
  else
  {
    WiFi.mode(WIFI_STA);
    Serial.println("WLAN connected. Switching to STA mode.");
    delay(1000);
    myIP = WiFi.localIP();
    
       // alle LEDs an...
  for(byte i=0; i<NUM_PIXEL; i++) {
    strip.setPixelColor(i, 0, 100 ,0);
    strip.show();
    delay(50);
  }

    // mDNS is needed to see HOSTNAME in Arduino IDE.
    Serial.println("Starting mDNS responder.");
    MDNS.begin(HOSTNAME);
    //MDNS.addService("http", "tcp", 80);
/* ------------------ Wifi Ende--------------------- */

/* ------------------ OTA --------------------- */
    Serial.println("Starting OTA service.");

#ifdef DEBUG
    ArduinoOTA.onStart([]()
    {
      Serial.println("Start OTA update.");
    });
    ArduinoOTA.onError([](ota_error_t error)
    {
      Serial.println("OTA Error: " + String(error));
      if (error == OTA_AUTH_ERROR) Serial.println("Auth failed.");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin failed.");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect failed.");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive failed.");
      else if (error == OTA_END_ERROR) Serial.println("End failed.");
    });
    ArduinoOTA.onEnd([]()
    {
      Serial.println("End OTA update.");
    });
#endif
    ArduinoOTA.setPassword(OTA_PASS);
    ArduinoOTA.begin();

/* ------------------ OTA Ende --------------------- */ 

/* ------------------ NTP --------------------- */
  NTP.begin(ntpServerName, timezone, true); // get time from NTP server pool.
  NTP.setInterval(63); //the time will be re-synchronized every hour
/* ------------------ NTP Ende --------------------- */

  Serial.println("Starting updateserver.");
  httpUpdater.setup(&esp8266WebServer);

  milliSecondsSyncPoint = millis();
  
   // alle LEDs an...
  for(byte i=0; i<NUM_PIXEL; i++) {
    strip.setPixelColor(i, wheel((256 / NUM_PIXEL) * i));
    strip.show();
    delay(50);
  }
  delay(250);
  
}
}

void loop() {

  clearStrip();

  // Call HTTP- and OTA-handle.
  esp8266WebServer.handleClient();
  ArduinoOTA.handle();

  
    syncTimeInMillis = millis() - milliSecondsSyncPoint;
    Serial.println("\nSync after: ");
    Serial.println(syncTimeInMillis);
    milliSecondsSyncPoint = millis();
   
    hours = hour();
    while(hours >= 12) {
      hours -= 12;
    }
    minutes = minute();
    seconds = second();
    Serial.print(hours);
    Serial.print(":");
    Serial.print(minutes);
    Serial.print(":");
    Serial.print(seconds);
  // bei Dunkelheit kleine Hilfslichter einschalten...
    if(ldr.value() > 50) {
      for(int i=0; i<60; i += 5) {
        strip.setPixelColor(i, 12, 12, 0);
      }
    }
  
    // Positionen berechnen und ausgeben...
    double doubleDisplaySeconds = (double) seconds + ((millis() - milliSecondsSyncPoint) / (double) syncTimeInMillis);
    setFloatPixelColor(doubleDisplaySeconds, 0xff, 0x00, 0x00);
  
    double doubleDisplayMinutes = (double) minutes + (doubleDisplaySeconds / 60.0);
    setFloatPixelColor(doubleDisplayMinutes, 0x00, 0xff, 0x00);
  
    double doubleDisplayHours = doubleMap((double) (hours * 60.0 + minutes), 0.0, 12.0 * 60.0, 0.0, 60.0);
    setFloatPixelColor(doubleDisplayHours, 0x00, 0x00, 0xff);
      strip.show();
    delay(18);
}


/**
 * Pixelfarben nicht nur auf ganze Pixel setzten, sonder auch "dazwischen".
 */
void setFloatPixelColor(float pos, byte r, byte g, byte b) {
  float anteil = 1.0 - (pos - (int) pos);
  strip.setPixelColor((byte) pos, (byte) ((float) r * anteil), (byte) ((float) g * anteil), (byte) ((float) b * anteil));
  if((byte) pos + 1 != NUM_PIXEL) {   
    strip.setPixelColor(1 + (byte) pos, (byte) ((float) r * (1.0 - anteil)), (byte) ((float) g * (1.0 - anteil)), (byte) ((float) b * (1.0 - anteil)));
  }
  else {
    strip.setPixelColor(0, (byte) ((float) r * (1.0 - anteil)), (byte) ((float) g * (1.0 - anteil)), (byte) ((float) b * (1.0 - anteil)));
  }
}

/**
 * Funktion zum loeschen des Strips.
 */
void clearStrip(){
  for(int i = 0; i < NUM_PIXEL; i++) {
    strip.setPixelColor(i, 0);
  }
}

void colourstrib(int r, int g, int b ){

   for(int i=0; i<60; i ++) {
        strip.setPixelColor(i, r, g, b);
      }
       strip.show();
}

/**
 * Funktion fuer saubere 'Regenbogen'-Farben.
 * Kopiert aus den Adafruit-Beispielen (strand).
 */
 uint32_t wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

/**
 * Map-Funktion fuer Doublöe-Werte.
 */
double doubleMap(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
