/**
 *  Liquid Clock WEMO 
 * 
 *  bassierend auf der Liquid Clock von Christian Aschoff
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * */
#include <NtpClientLib.h>
#include <ESP8266WiFi.h>          
#include <DNSServer.h>            
#include <ESP8266WebServer.h>     
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <Adafruit_NeoPixel.h>
#include "LDR.h"
// ------------------ Pixel Einstellungen ---------------------

#define NUM_PIXEL      60       // Anzahl der NeoPixel LEDs
#define STRIP_PIN            2        // Digital  ESP8266

#define LDR_SIGNAL A0
// ------------------ WLAN Einstellungen --------------------- 

char ssid[] = "SSID";  //  your network SSID (name)
char pass[] = "Pass";       // your network password

// ------------------ Zeitserver Einstellungen ---------------------

const char* ntpServerName = "192.168.178.254"; //NTPServer
int timezone = 1;


//===================================================

// Der lichtabhaengige Widerstand
LDR ldr(LDR_SIGNAL);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXEL, STRIP_PIN, NEO_GRB + NEO_KHZ800);
// Die gelesene Zeit...
int hours, minutes, seconds;
// Hilfsvariablen fuer den organischen Effekt...
unsigned long syncTimeInMillis, milliSecondsSyncPoint;

void setup() {
clearStrip();
  Serial.begin(115200);
  WiFiManager wifiManager; // wifi configuration wizard
  Serial.print("Neo-Pixel-Liquid--Clock WEMOS"); 
   
  Serial.print("Connecting to ");
  Serial.println(ssid);
  wifiManager.autoConnect(ssid, pass); 
  Serial.println("WiFi Client connected!)");
  
  NTP.begin(ntpServerName, timezone, true); // get time from NTP server pool.
  NTP.setInterval(600000); //the time will be re-synchronized every hour

  
  strip.begin();
  strip.setBrightness(254);

  milliSecondsSyncPoint = millis();
  
   // alle LEDs an...
  for(byte i=0; i<NUM_PIXEL; i++) {
    strip.setPixelColor(i, wheel((256 / NUM_PIXEL) * i));
    strip.show();
    delay(50);
  }
  delay(200);
  
}

void loop() {

  clearStrip();

    
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
