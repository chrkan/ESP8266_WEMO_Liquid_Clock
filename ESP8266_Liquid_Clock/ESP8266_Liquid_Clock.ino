/**
 *  Liquid Clock WEMO 
 * 
 *  bassierend auf der Liquid Clock von Christian Aschoff
 * 
 * 
 * 
 * */
#include <ArduinoOTA.h>
      
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>   
#include <ArduinoOTA.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <Adafruit_NeoPixel.h>
#include <Syslog.h>
#include <TimeLib.h>
#include <Timezone.h>
#include "Settings.h"
#include "LDR.h"
#include "Configuration.h"
#include "Timezones.h"

Settings settings;
// ------------------ Pixel Einstellungen ---------------------

#define NUM_PIXEL      60       // Anzahl der NeoPixel LEDs
#define STRIP_PIN            2        // Digital  ESP8266
#define LDR_SIGNAL A0
IPAddress myIP = { 0,0,0,0 };
// Der lichtabhaengige Widerstand
LDR ldr(LDR_SIGNAL);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXEL, STRIP_PIN, NEO_GRB + NEO_KHZ800);
// Die gelesene Zeit...
int hours, minutes, seconds;
bool startled;
char location[LEN_LOC_STR];
unsigned long second_befor, milli_befor;
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

    
  startShow(3); // Blue
wlan(true);
  if (WiFi.status() == WL_CONNECTED)
  {
   setupWebServer();
    
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
if((minute() == 0|| minute() == 5 || minute() == 10 || minute() == 15|| minute() == 20|| minute() == 25|| minute() == 30 || minute() == 35|| minute() == 35|| minute() == 40|| minute() == 45|| minute() == 50|| minute() == 55) && second() == 0)
{
  
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
    if(ldr.value() > settings.getBrightness() && settings.getUseLdr()) {
      for(int i=0; i<60; i += settings.getldrDot()) {
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
    time_t tempNtpTime = getNtpTime(settings.getntpServer(location, sizeof(location)));
    if (tempNtpTime)
    {
    setTime(timeZone.toLocal(tempNtpTime));
    
    

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
   startShow(1); //red
  Serial.printf("Error (NTP): %u\r\n", errorCounterNtp);
#endif
  return 0;
}

void wlan(bool an){
  /* ------------------ Wifi --------------------- */
  if(an) {
    
    WiFiManager wifiManager;
     if (WiFi.status() != WL_CONNECTED)
     {
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
                startShow(1); //red
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
            startShow(2);  // Green
              }
            // mDNS is needed to see HOSTNAME in Arduino IDE.
            Serial.println("Starting mDNS responder.");
            MDNS.begin(HOSTNAME);
            //MDNS.addService("http", "tcp", 80);
          }
     }

    
 
  
  
  }else{
   
    WiFi.mode(WIFI_OFF);
   
  }

/* ------------------ Wifi Ende--------------------- */
  
}
/* ------------------ Webserver--------------------- */


void setupWebServer()
{
  esp8266WebServer.onNotFound(handleNotFound);
  esp8266WebServer.on("/", handleRoot);

  esp8266WebServer.on("/factoryReset", handleFactoryReset);
   
  esp8266WebServer.on("/Settings", handleSettings);
  esp8266WebServer.on("/commitSettings", []() {handleCommitSettings();});//; callBack();});
  
  esp8266WebServer.on("/wifiReset", handleWiFiReset);
  esp8266WebServer.on("/reset", handleReset);
  esp8266WebServer.begin();
}

String htmlTop(String page)
{
  String message = "<!doctype html>";
  message += "<html>";
  message += "<head>";
  message += "<title>" + String(HOSTNAME) + "-" +page +"</title>";
  message += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  //message += "<meta http-equiv=\"refresh\" content=\"60\" charset=\"UTF-8\">";
  message += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css\">";
  message += "<style>";
  message += "body{background-color:#53bbf4;text-align:center;color:#333333;font-family:Sans-serif;font-size:16px;}";
  message += "button{background-color:#FFFFFF;text-align:center;color:#53bbf4;width:50px;padding:10px;border:5px solid #FFFFFF;font-size:24px;border-radius:10px;}";
  
  message += "</style>";
  message += "</head>";
  message += "<body>";
  message += "<button onclick=\"window.location.href='/'\"><i class=\"fa fa-home\"></i></button> ";
  message += "<button onclick=\"window.location.href='/reset'\"><i class=\"fa fa-power-off\"></i></button> ";
  message += "<button onclick=\"window.location.href='/Settings'\"><i class=\"fa fa-gear\"></i></button> ";
   message += "<span style=\"color:white\">";
  return message;
}


String htmlButton()
{
  String message = "</span>";
  message += "</body>";
  message += "</html>";
  return message;
}
void callRoot()
{
  esp8266WebServer.send(200, "text/html", "<!doctype html><html><head><script>window.onload=function(){window.location.replace('/');}</script></head></html>");
}

void callBack()
{
  esp8266WebServer.send(200, "text/html", "<!doctype html><html><head><script>window.onload=function(){if(navigator.userAgent.indexOf(\"Firefox\")!=-1){window.location.replace('/');}else{window.history.back();}}</script></head></html>");
}

// Page 404.
void handleNotFound()
{
  esp8266WebServer.send(404, "text/plain", "404 - File Not Found.");
}


void handleRoot()
{
  //Handler for the rooth path
 
    String message = htmlTop("Home");

  WiFiManager wifiManager;
  message += "<br><br>Wlan: " + WiFi.SSID();
  
  time_t tempEspTime = now();
  message += "<br><br>Time: " + String(hour(tempEspTime)) + ":";
  if (minute(tempEspTime) < 10) message += "0";
  message += String(minute(tempEspTime));
  if (timeZone.locIsDST(now())) message += " (DST)";
  message += "<br>" + String(dayStr(weekday(tempEspTime))) + ", " + String(monthStr(month(tempEspTime))) + " " + String(day(tempEspTime)) + ". " + String(year(tempEspTime));
  
  message += "<br><br>Free RAM: " + String(system_get_free_heap_size()) + " bytes";
  message += "<br>Reset reason: " + ESP.getResetReason();
  
  message += "<br><br>LDR: ";
  
  if(settings.getUseLdr())
  {
    message +="Active ";
  }else{
    message +="deactive ";
  }
  message += String(ldr.value()) + " (min: " + String(LDR_MANUAL_MIN) + ", max: " + String(LDR_MANUAL_MAX) + ")";
  message += "<br><br>NTP Server: "+ String(settings.getntpServer(location, sizeof(location)));


  message += "<br><br>Setting Version: "+String(settings.getSettingVersion());
  
  message += htmlButton();
  esp8266WebServer.send(200, "text/html", message);
  
 
}

void handleCommitSettings()
{



String message = htmlTop("CommitSettings");
message += "<br><br>"+esp8266WebServer.arg("ntpserver");

message += "<br>Help Dots: ";
if(esp8266WebServer.arg("DEFAULT_LDR_Status") == "1")
{
  message += "true";
  settings.setUseLdr(true);
}else{
  message += "false";
  settings.setUseLdr(false);
}

settings.setBrightness(esp8266WebServer.arg("br").toInt());
 
message += "<br>Brightness: "+esp8266WebServer.arg("br") +" %";

message += "<br>Help Dots every "+esp8266WebServer.arg("ldrdots") +" Pixel";
settings.setldrDot(esp8266WebServer.arg("ldrdots").toInt());

  char text[LEN_LOC_STR];
  memset(text, 0, sizeof(text));
  esp8266WebServer.arg("ntp").toCharArray(text, sizeof(text), 0);
  settings.setntpServer(text, sizeof(text));
  message +="<br><br>NTP Server: "+esp8266WebServer.arg("ntp");



  
  message += htmlButton();
  esp8266WebServer.send(200, "text/html", message);  
  delay(500);
  settings.saveToEEPROM();

  
}


void handleSettings()
{
  //Handler for the rooth path
 
    String message = htmlTop("Settings");
    message += "<form action=\"/commitSettings\">";
    
    
  

  message += "<br>LDR: <input type=\"radio\" name=\"DEFAULT_LDR_Status\" value=\"1\"";
    if (settings.getUseLdr()) message += " checked";
    message += "> on ";
    message += "<input type=\"radio\" name=\"DEFAULT_LDR_Status\" value=\"0\"";
    if (!settings.getUseLdr()) message += " checked";
    message += "> off";

message += "<br><select name=\"br\">";
  for (int i = 10; i <= 100; i += 10)
  {
    message += "<option value=\"" + String(i) + "\"";
    if (i == settings.getBrightness()) message += " selected";
    message += ">";
    message += String(i) + "</option>";
  }
  message += "</select> %";


    message += "<br>Help Dots every <select name=\"ldrdots\">";;
    message += "<option value=\""+String(settings.getldrDot())+"\" selected>"+String(settings.getldrDot())+"</option>";
    message += "<option value=\"5\">5</option>";
    message += "<option value=\"10\">10</option>";
    message += "<option value=\"15\">15</option>";
    message += "<option value=\"30\">30</option>";
    message += "</select> Pixels.";
    message += "<br><br>NTP:<input type=\"text\" name=\"ntp\" value=\"";
  settings.getntpServer(location, sizeof(location));
  message += String(location) + "\" pattern=\"[\\x20-\\x7e]{0," + String(LEN_LOC_STR-1) + "}\" placeholder=\"Enter NTP Server ...\">";
    message += "<br><br><button title=\"Save Settings.\"><i class=\"fa fa-check\"></i></button>";
  message += "</form>";
  
    message += htmlButton();

  esp8266WebServer.send(200, "text/html", message);

}
void handleReset()
{
  esp8266WebServer.send(200, "text/plain", "By...");
  delay(1000);
  callRoot();
  ESP.restart();
  
}

void handleFactoryReset()
{
  settings.resetToDefault();
  esp8266WebServer.send(200, "text/plain", "OK.");
  ESP.restart();
}

void handleWiFiReset()
{
  esp8266WebServer.send(200, "text/plain", "OK.");
      WiFiManager wifiManager;
      wifiManager.resetSettings();

  WiFi.disconnect(true);
  ESP.restart();
}
