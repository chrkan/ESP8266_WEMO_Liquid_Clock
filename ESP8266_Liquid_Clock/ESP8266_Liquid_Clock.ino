/**
 *  Liquid Clock WEMO 
 * 
 *  bassierend auf der Liquid Clock von Christian Aschoff
 * 
 * 
 * 
 * */

//#include <NtpClientLib.h>        
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>   
#include <ArduinoOTA.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <Adafruit_NeoPixel.h>
#include <TimeLib.h>
#include <Timezone.h>
#include "LDR.h"
#include "Configuration.h"
#include "Timezones.h"
// ------------------ Pixel Einstellungen ---------------------

#define NUM_PIXEL      60       // Anzahl der NeoPixel LEDs
#define STRIP_PIN            2        // Digital  ESP8266

#define LDR_SIGNAL A0


IPAddress myIP = { 0,0,0,0 };
//===================================================
unsigned long second_befor, milli_befor;


// Der lichtabhaengige Widerstand
LDR ldr(LDR_SIGNAL);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXEL, STRIP_PIN, NEO_GRB + NEO_KHZ800);
// Die gelesene Zeit...
int hours, minutes, seconds;
bool startled;
// Hilfsvariablen fuer den organischen Effekt...

ESP8266WebServer esp8266WebServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void setup() {

startled =true;

  Serial.begin(115200);
  
  Serial.print("Neo-Pixel-Liquid-Clock-WEMOS"); 
  strip.begin();
  strip.setBrightness(254);
  
  delay(200);

    
  startShow(3);
wlan(true);
  if (WiFi.status() == WL_CONNECTED)
  {
   
    
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
getntp();
/* ------------------ NTP Ende ---------------- */

  Serial.println("Starting updateserver.");
  httpUpdater.setup(&esp8266WebServer);

  
  
  delay(250);
  startled = false;
  
}
//setze milli 
milli_befor = millis();
}

void loop() {
  long milli =0;

  
if(second() == second_befor)
{
  //wenn in der gleichen sekunde dann ziehe milli von millis ab
milli = millis() - milli_befor;
  
}else{
  //setze milli auf 0
  milli = 0;
  //setze second_Befor auf die aktuelle sekunde
  second_befor = second();
  //setze milli_befor auf aktuelle millis
  milli_befor = millis();
}
  


  clearStrip();
/* ------------------ NTP --------------------- */
if((minute() == 0 && second() == 0) || (minute() == 30 && second() == 0)) {
  
wlan(true);


getntp();
 }
/* ------------------ NTP ENDE--------------------- */

  // Call HTTP- and OTA-handle.
  esp8266WebServer.handleClient();
  ArduinoOTA.handle();

  
   
    hours = hour();
    while(hours >= 12) {
      hours -= 12;
    }
    minutes = minute();
    seconds = second();
    
   
    
  // bei Dunkelheit kleine Hilfslichter einschalten...
    if(ldr.value() > 50) {
      for(int i=0; i<60; i += 15) {
        strip.setPixelColor(i, 12, 12, 0);
      }
    }

  
    // Positionen berechnen und ausgeben...
   // double doubleDisplaySeconds = (double) second() + ((millis() - milliSecondsSyncPoint) / (double) syncTimeInMillis);
   double doubleDisplaySeconds = (double) second() + (milli * 0.001);
    setFloatPixelColor(doubleDisplaySeconds, 0xff, 0x00, 0x00);
  
    double doubleDisplayMinutes = (double) minutes + (doubleDisplaySeconds / 60.0);
    setFloatPixelColor(doubleDisplayMinutes, 0x00, 0xff, 0x00);
    
  
    double doubleDisplayHours = doubleMap((double) (hours * 60.0 + minutes), 0.0, 12.0 * 60.0, 0.0, 60.0);
    setFloatPixelColor(doubleDisplayHours, 0x00, 0x00, 0xff);
    strip.show();
 
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

void startShow(int i) {
  switch(i){
    case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
            break;
    case 1: colorWipe(strip.Color(255, 0, 0), 50);  // Red
            break;
    case 2: colorWipe(strip.Color(0, 255, 0), 50);  // Green
            break;
    case 3: colorWipe(strip.Color(0, 0, 255), 50);  // Blue
            break;
    case 4: theaterChase(strip.Color(127, 127, 127), 50); // White
            break;
    case 5: theaterChase(strip.Color(127,   0,   0), 50); // Red
            break;
    case 6: theaterChase(strip.Color(  0,   0, 127), 50); // Blue
            break;
    case 7: rainbow(20);
            break;
    case 8: rainbowCycle(20);
            break;
    case 9: theaterChaseRainbow(50);
            break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

/******************************************************************************
  Get UTC time from NTP.
******************************************************************************/

void getntp()
{

if (WiFi.status() == WL_CONNECTED)
  {
    time_t tempNtpTime = getNtpTime(NTP_SERVER);
    if (tempNtpTime)
    {
    setTime(timeZone.toLocal(tempNtpTime));
    
    #ifdef WLANOFF
      WiFi.mode(WIFI_OFF);
    #endif

    if(startled){
    // alle LEDs an...
    clearStrip();
    for(byte i=0; i<NUM_PIXEL; i++) {
    strip.setPixelColor(i, wheel((256 / NUM_PIXEL) * i));
    strip.show();
    delay(50);
    }
    }
    
   
    }
  }
  
}


uint8_t errorCounterNtp = 0;
time_t getNtpTime(const char server[])
{
#ifdef DEBUG
  Serial.println("Sending NTP request to \"" + String(server) + "\". ");
#endif
  uint8_t packetBuffer[49] = {};
  packetBuffer[0] = 0xE3;
  packetBuffer[1] = 0x00;
  packetBuffer[2] = 0x06;
  packetBuffer[3] = 0xEC;
  packetBuffer[12] = 0x31;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 0x31;
  packetBuffer[15] = 0x34;
  WiFiUDP wifiUdp;
  wifiUdp.begin(2390);
  IPAddress timeServerIP;
  WiFi.hostByName(server, timeServerIP);
  wifiUdp.beginPacket(timeServerIP, 123);
  wifiUdp.write(packetBuffer, 48);
  wifiUdp.endPacket();
  uint32_t beginWait = millis();
  while ((millis() - beginWait) < NTP_TIMEOUT)
  {
    if (wifiUdp.parsePacket() >= 48)
    {
      wifiUdp.read(packetBuffer, 48);
      uint32_t ntpTime = (packetBuffer[40] << 24) + (packetBuffer[41] << 16) + (packetBuffer[42] << 8) + packetBuffer[43];
      // NTP time is seconds from 1900, TimeLib.h is seconds from 1970.
      ntpTime -= 2208988800;
      errorCounterNtp = 0;
#ifdef DEBUG
      Serial.printf("Time (NTP): %02u:%02u:%02u %02u.%02u.%04u (UTC)\r\n", hour(ntpTime), minute(ntpTime), second(ntpTime), day(ntpTime), month(ntpTime), year(ntpTime));
#endif
      return ntpTime;
    }
  }
  if (errorCounterNtp < 255) errorCounterNtp++;
#ifdef DEBUG
   startShow(1);
  Serial.printf("Error (NTP): %u\r\n", errorCounterNtp);
#endif
  return 0;
}

void wlan(bool an){
  /* ------------------ Wifi --------------------- */
  if(an) {

    
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
    if(startled){
      startShow(1);
    }
    
  }
  else
  {
    WiFi.mode(WIFI_STA);
    Serial.println("WLAN connected. Switching to STA mode.");
    delay(1000);
    myIP = WiFi.localIP();
    clearStrip();
      if(startled){
    startShow(2);
      }
    // mDNS is needed to see HOSTNAME in Arduino IDE.
    Serial.println("Starting mDNS responder.");
    MDNS.begin(HOSTNAME);
    //MDNS.addService("http", "tcp", 80);
  }
  
  }else{
   #ifdef WLANOFF 
    WiFi.mode(WIFI_OFF);
   #endif
  }

/* ------------------ Wifi Ende--------------------- */
  
}

