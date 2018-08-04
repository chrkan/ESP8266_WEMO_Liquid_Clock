/**
 *  Liquid Clock WEMO 
 * 
 *  bassierend auf der Liquid Clock von Christian Aschoff
 *  Board: Wemos D1 R2 & Mini
 *  Flashsize 4M (3M SPIFFS)
 * 
 * */
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>    
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
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
#include "Colors.h"


/******************************************************************************
  Init.
******************************************************************************/

Settings settings;
// ------------------ Pixel Einstellungen ---------------------

#define NUM_PIXEL      60       // Anzahl der NeoPixel LEDs
#define STRIP_PIN            2        // Digital  ESP8266
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXEL, STRIP_PIN, NEO_GRB + NEO_KHZ800);

// ------------------ LDR Einstellungen ---------------------
#define LDR_SIGNAL A0

// Der lichtabhaengige Widerstand
LDR ldr(LDR_SIGNAL);
#define LOG Serial.print


// ------------------ Globale Variablen ---------------------
// Die gelesene Zeit...
int hours, minutes, seconds;
bool startled;
String updateInfo="0";
char location[LEN_LOC_STR];
unsigned long second_befor, milli_befor;

// ------------------ Syslog Einstellungen ---------------------
#ifdef SYSLOGSERVER
WiFiUDP wifiUdp;
Syslog syslog(wifiUdp, SYSLOGSERVER_SERVER, SYSLOGSERVER_PORT, HOSTNAME, "QLOCKWORK2", LOG_INFO);
#endif

// ------------------ Web Einstellungen ---------------------
ESP8266WebServer esp8266WebServer(80);
ESP8266HTTPUpdateServer httpUpdater;



/******************************************************************************
  Setup
******************************************************************************/

void setup() {

startled =true;

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.print("Neo-Pixel-Liquid-Clock-WEMOS"); 
  strip.begin();
  strip.setBrightness(254);
  
  delay(200);

    
  startShow(3); // Blue
wlan(true);
  if (WiFi.status() == WL_CONNECTED)
  {



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
setupWebServer();
}

/******************************************************************************
  Loop.
******************************************************************************/

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
if((minute() == 0) && second() == 0 && milli == 0)  //|| minute() == 5 || minute() == 10 || minute() == 15|| minute() == 20|| minute() == 25|| minute() == 30 || minute() == 35|| minute() == 35|| minute() == 40|| minute() == 45|| minute() == 50|| minute() == 55
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
    
   
    int color = settings.getColHel();
  // bei Dunkelheit kleine Hilfslichter einschalten...
    if(ldr.value() > settings.getBrightness() && settings.getUseLdr()) {
      for(int i=0; i<60; i += settings.getldrDot()) {
        strip.setPixelColor(i, defaultColors[color].red,defaultColors[color].green,defaultColors[color].blue);
      }
    }


 /* ------------------ led --------------------- */
  
    // Positionen berechnen und ausgeben...
   // double doubleDisplaySeconds = (double) second() + ((millis() - milliSecondsSyncPoint) / (double) syncTimeInMillis);
   double doubleDisplaySeconds = (double) second() + (milli * 0.001);
   color = settings.getColSec();
    setFloatPixelColor(doubleDisplaySeconds, defaultColors[color].red,defaultColors[color].green,defaultColors[color].blue);
    
    color = settings.getColMin();
    double doubleDisplayMinutes = (double) minutes + (doubleDisplaySeconds / 60.0);
    setFloatPixelColor(doubleDisplayMinutes, defaultColors[color].red,defaultColors[color].green,defaultColors[color].blue);
    
    color = settings.getColHou();
    double doubleDisplayHours = doubleMap((double) (hours * 60.0 + minutes), 0.0, 12.0 * 60.0, 0.0, 60.0);
    setFloatPixelColor(doubleDisplayHours, defaultColors[color].red,defaultColors[color].green,defaultColors[color].blue);
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
    case 10: colorWipe(strip.Color(255,140,0), 50);    // Orange
            break;
    case 11: theaterChase(strip.Color(255,140,0), 50);    // Orange
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

 /* ------------------ NTP --------------------- */

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


 /* ------------------ Wifi --------------------- */

 
void wlan(bool an){
 
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
     
              if(startled){
                startShow(1); //red
              }
              
          }
          else
          {
            WiFi.mode(WIFI_STA);
            Serial.println("WLAN connected. Switching to STA mode.");
            delay(1000);
          
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

/******************************************************************************
  Get update info.
******************************************************************************/

void getUpdateInfo()
{

  Serial.println("Sending HTTP-request for update info.");

  char server[] = UPDATE_SERVER;
  WiFiClient wifiClient;
  HttpClient client = HttpClient(wifiClient, server, 80);
  client.get(UPDATE_INFOFILE);
  uint16_t statusCode = client.responseStatusCode();
  if (statusCode == 200)
  {
    String response = client.responseBody();
    response = response.substring(response.indexOf('{'), response.lastIndexOf('}') + 1);

    Serial.printf("Status: %u\r\n", statusCode);
    Serial.printf("Response is %u bytes.\r\n", response.length());
    Serial.println(response);
    Serial.println("Parsing JSON.");

    //DynamicJsonBuffer jsonBuffer;
    StaticJsonBuffer<256> jsonBuffer;
    JsonObject &responseJson = jsonBuffer.parseObject(response);
    if (responseJson.success())
    {

      updateInfo = responseJson["channel"]["stable"]["version"].as<String>();


     // updateInfo = responseJson["channel"]["unstable"]["version"].as<String>();

      return;
  }
}

  else Serial.printf("Status: %u\r\n", statusCode);
  Serial.println("Error (" + String(UPDATE_SERVER) + ")");

}

/******************************************************************************
  Do Update
******************************************************************************/

void doupdate() {
    wlan(false);
    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }
    
    wlan(true);
    startShow(10); //orange
    // wait for WiFi connection
    if(WiFi.status() == WL_CONNECTED) {
        
        
        
        t_httpUpdate_return ret = ESPhttpUpdate.update(String(UPDATE_SERVER) + String(UPDATE_FILE) );

        switch(ret) {
            case HTTP_UPDATE_FAILED:
                Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                startShow(1); //red
                break;

            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("HTTP_UPDATE_NO_UPDATES");
                startShow(1); //red
                break;

            case HTTP_UPDATE_OK:
                Serial.println("HTTP_UPDATE_OK");
               startShow(2); //red
                break;
        }
    }
}


/* ------------------ Webserver--------------------- */


void setupWebServer()
{
  Serial.println("Starte WebServer...");
  esp8266WebServer.onNotFound(handleNotFound);
  esp8266WebServer.on("/", handleRoot);

  esp8266WebServer.on("/factoryReset", handleFactoryReset);
  esp8266WebServer.on("/updates", handleupdates); 
  esp8266WebServer.on("/Settings", handleSettings);
  esp8266WebServer.on("/commitSettings", []() {handleCommitSettings();});//; callBack();});
  
  esp8266WebServer.on("/wifiReset", handleWiFiReset);
  esp8266WebServer.on("/reset", handleReset);
  
  esp8266WebServer.begin();
  Serial.print("gestarte!!");
}

String htmlTop(String page)
{
  Serial.print("Starte WebServer: "+ page);
  String message = "<!doctype html>";
  message += "<html style=\"height: 100%;\">";
  message += "<head>";
  message += "<title>" + String(HOSTNAME) + "-" +page +"</title>";
  message += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  //message += "<meta http-equiv=\"refresh\" content=\"60\" charset=\"UTF-8\">";
  message += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css\">";
  message += "<style>";
  message += "body{text-align:center;color:#333333;font-family:Sans-serif;font-size:16px;}";
  message += "button{background-color:#FFFFFF;text-align:center;color:#53bbf4;width:50px;padding:10px;border:5px solid #FFFFFF;font-size:24px;border-radius:10px;}";
  
  message += "</style>";
  message += "</head>";
  message += "<body style=\"height: 100%;\">";
   message += "<span style=\"color:white\">";
  message += "<table style=\"height: 100%; ; margin-left: auto; margin-right: auto; width: 300px; background-color: #53bbf4;\"><tbody><tr><td style=\"height: 10%;\">";
  message += "&nbsp;&nbsp;<button onclick=\"window.location.href='/'\"><i class=\"fa fa-home\"></i></button>&nbsp;&nbsp; ";
  message += "<button onclick=\"window.location.href='/reset'\"><i class=\"fa fa-refresh\"></i></button>&nbsp;&nbsp;";
  message += "<button onclick=\"window.location.href='/Settings'\"><i class=\"fa fa-wrench\"></i></button>&nbsp;&nbsp; ";
   message += "<button onclick=\"window.location.href='/update'\"><i class=\"fa fa-upload\"></i></button> &nbsp;&nbsp;";
   message +="</td></tr><tr><td style=\"height: 80%; vertical-align: top;\">";
  
  return message;
}


String htmlButton()
{
  String message = "</td></tr><tr><td  style=\"height: 10%;\">";
 message += "________________________________<br>";
  message += "ckany 2018<br>";
  
  message+="</td></tr></tbody></table>     ";
  message += "</span>";
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
{getUpdateInfo();
  //Handler for the rooth path
 
    String message = htmlTop("Home");

  WiFiManager wifiManager;
  message += "<br><br>Wlan: " + WiFi.SSID();
  message += "<br>"+WiFi.localIP();
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
  message += String(ldr.value()) + "  %(min: " + String(LDR_MANUAL_MIN) + ", max: " + String(LDR_MANUAL_MAX) + ")";
  message += "<br>Help Dots every: "+String(settings.getldrDot())+" Pixels, by "+String(settings.getBrightness())+"% Brightness." ;
  message += "<br><br>NTP Server: "+ String(settings.getntpServer(location, sizeof(location)));
/**
  if (updateInfo > String(FirmewareVersion))
  {
    message += "<br><span style=\"color:red;\"><a href=\"/updates\">Firmwareupdate available! (" + updateInfo + ")</a></span>";
  }else{
     message += "<br><span style=\"color:green;\">Your Firmeware: "+ String(FirmewareVersion) +" (" + updateInfo + ")</span>";
  }

  **/
  message += "<br><br>Setting Version: "+String(settings.getSettingVersion());
  
  message += htmlButton();
  esp8266WebServer.send(200, "text/html", message);
  
 
}

void handleupdates()
{

 doupdate();

}

void handleCommitSettings()
{
startShow(2); //Green


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

  settings.setColSec(esp8266WebServer.arg("colsec").toInt());
  settings.setColMin(esp8266WebServer.arg("colmin").toInt());
  settings.setColHou(esp8266WebServer.arg("colhour").toInt());
  settings.setColHel(esp8266WebServer.arg("colhel").toInt());

  
  message += htmlButton();
  esp8266WebServer.send(200, "text/html", message);  
  delay(500);
  settings.saveToEEPROM();


  if(esp8266WebServer.arg("ntp") != esp8266WebServer.arg("ntp_old"))
  {
    handleReset();
  }

  
}


void handleSettings()
{
  //Handler for the rooth path
 
    String message = htmlTop("Settings");
    message += "<form action=\"/commitSettings\">";
    
    message += "<br><br><table align=\"center\"><tr><th>";
  

  message += "LDR: </th><th><input type=\"radio\" name=\"DEFAULT_LDR_Status\" value=\"1\"";
    if (settings.getUseLdr()) message += " checked";
    message += "> on ";
    message += " <input type=\"radio\" name=\"DEFAULT_LDR_Status\" value=\"0\"";
    if (!settings.getUseLdr()) message += " checked";
    message += "> off</th></tr>";

message += "<tr><td>Help Dots by </td><td><select name=\"br\">";
  for (int i = 10; i <= 100; i += 10)
  {
    message += "<option value=\"" + String(i) + "\"";
    if (i == settings.getBrightness()) message += " selected";
    message += ">";
    message += String(i) + "</option>";
  }
  message += "</select> %  LDR</td></tr>";

    message += "<tr><th>&nbsp;</th><th>&nbsp;</th></tr>";
    
    message += "<tr><td>Help Dots every </td><td><select name=\"ldrdots\">";
    message += "<option value=\""+String(settings.getldrDot())+"\" selected>"+String(settings.getldrDot())+"</option>";
    message += "<option value=\"5\">5</option>";
    message += "<option value=\"10\">10</option>";
    message += "<option value=\"15\">15</option>";
    message += "<option value=\"30\">30</option>";
    message += "</select> Pixels.</td></tr>";

    message += "<tr><th>&nbsp;</th><th>&nbsp;</th></tr>";
    
    message += "<tr><td>NTP:</td><td><input type=\"text\" size=\"20\" name=\"ntp\" value=\"";
    settings.getntpServer(location, sizeof(location));
    message += String(location) + "\" pattern=\"[\\x20-\\x7e]{0," + String(LEN_LOC_STR-1) + "}\" placeholder=\"Enter NTP Server ...\">";
    message += "<input type=\"hidden\" name=\"ntp_old\" value=\"";
    settings.getntpServer(location, sizeof(location));
    message += String(location) + "\" pattern=\"[\\x20-\\x7e]{0," + String(LEN_LOC_STR-1) + "}\" placeholder=\"Enter NTP Server ...\">";
    message +="<br></td></tr>";

    message += "<tr><th>&nbsp;</th><th>&nbsp;</th></tr>";
    
    message += "<tr><td>Second: </td><td><select name=\"colsec\">";
    uint8_t colorNum = settings.getColSec();
    for(uint8_t j = 0; j <= COLOR_COUNT; j++){
      message += "<option value=\"" +String(j) + "\"";
      if (colorNum == j) message += " selected";
      message += ">";
      message += String(FPSTR(sColorStr[j])) + "</option>";
    }
    message += "</select></td></tr>";
    message += "<tr><td>Minutes: </td><td><select name=\"colmin\">";
    colorNum = settings.getColMin();
    for(uint8_t j = 0; j <= COLOR_COUNT; j++){
      message += "<option value=\"" +String(j) + "\"";
      if (colorNum == j) message += " selected";
      message += ">";
      message += String(FPSTR(sColorStr[j])) + "</option>";
    }
    message += "</select></td></tr>";
    message += "<tr><td>Hour: </td><td><select name=\"colhour\">";
    colorNum = settings.getColHou();
    for(uint8_t j = 0; j <= COLOR_COUNT; j++){
      message += "<option value=\"" +String(j) + "\"";
      if (colorNum == j) message += " selected";
      message += ">";
      message += String(FPSTR(sColorStr[j])) + "</option>";
    }
    message += "</select></td></tr>";

     message += "<tr><td>Help Dots:</td><td> <select name=\"colhel\">";
    colorNum = settings.getColHel();
    for(uint8_t j = 0; j <= COLOR_COUNT; j++){
      message += "<option value=\"" +String(j) + "\"";
      if (colorNum == j) message += " selected";
      message += ">";
      message += String(FPSTR(sColorStr[j])) + "</option>";
    }
    message += "</select></td></tr></table>";
    message += "<br><br><button title=\"Save Settings.\"><i class=\"fa fa-check\"></i></button>";
  message += "</form>";
  
    message += htmlButton();

  esp8266WebServer.send(200, "text/html", message);

}
void handleReset()
{
  startShow(10); //Orange
  callBack();
  esp8266WebServer.send(200, "text/plain", "By...");
  
  delay(1000);
  callRoot();
  ESP.restart();
  
}

void handleFactoryReset()
{
  startShow(10); //Orange
  callBack();
  settings.resetToDefault();
  esp8266WebServer.send(200, "text/plain", "OK.");
  ESP.restart();
}

void handleWiFiReset()
{
  startShow(10); //Orange
  callBack();
  esp8266WebServer.send(200, "text/plain", "OK.");
      WiFiManager wifiManager;
      wifiManager.resetSettings();

  WiFi.disconnect(true);
  ESP.restart();
}
