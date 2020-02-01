/**
 *  Liquid Clock WEMO 
 * ESP8266
 *  bassierend auf der Liquid Clock von Christian Aschoff
 *  Board: Wemos D1 R2 & Mini
 *  Flashsize 4M (3M SPIFFS)
 * 
 * */

#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ArduinoHttpClient.h>
#include <Adafruit_NeoPixel.h>

#include <ESP8266WiFi.h>   
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266httpUpdate.h>    
#include <ESP8266WebServer.h>


//#include <Syslog.h>
#include <TimeLib.h>
#include <Timezone.h>

#include <WiFiClientSecure.h> //https://github.com/esp8266/Arduino/blob/4897e0006b5b0123a2fa31f67b14a3fff65ce561/doc/esp8266wifi/client-secure-examples.md
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include "Colors.h"
#include "Configuration.h"
#include "LDR.h"
#include "Settings.h"
#include "Timezones.h"

#define FirmewareVersion  "20181028"

/******************************************************************************
  Init.
******************************************************************************/

Settings settings;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXEL, STRIP_PIN, NEO_GRB + NEO_KHZ800);



// Der lichtabhaengige Widerstand
LDR ldr(LDR_SIGNAL);
#define LOG Serial.print


// ------------------ Globale Variablen ---------------------
// Die gelesene Zeit...
int Brightness, clockInfoColor = 0,hours, minutes, Moonphase, seconds,WeatherTemperatur,WeatherHumidity;
bool startled, WeatherTemperatur_negative;
String updateInfo="0", modus = "clock",WeatherStatus,WeatherIcon,WeatherName,WeatherTime,Info0,Info15,Info30,Info45;
char location[LEN_LOC_STR];
unsigned long second_befor, milli_befor;
uint8_t reset;
uint8_t i;  // counter
uint8_t j;  // counter






// ------------------ Syslog Einstellungen ---------------------
#ifdef SYSLOGSERVER
WiFiUDP wifiUdp;
//Syslog syslog(wifiUdp, SYSLOGSERVER_SERVER, SYSLOGSERVER_PORT, HOSTNAME, "QLOCKWORK2", LOG_INFO);
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
  leds (0, 15, strip.Color(127, 127, 127), 50);

    
 

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

  
  
 
  startled = false;

#ifdef SYSLOGSERVER
    Serial.println("Starting syslog.");
    //syslog.log(LOG_INFO, ";#;dateTime;roomWeatherTemperature;roomWeatherHumidity;outdoorWeatherTemperature;outdoorWeatherHumidity;outdoorCode;ldrValue;errorCounterNtp;errorCounterDht;errorCounterYahoo;freeHeapSize;upTime");
#endif
  
}
//setze milli 
milli_befor = millis();
getMoonphase(year(), month(), day());
getUpdateInfo();
getWeatherTemperature();
setupWebServer();

show_automatic();

modus = "clockInfo";
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

      if(second() % settings.getLdrSync() == 0  )
    {
      ledBrightness();

    if(modus == "clock")
    {
  clearStrip();

  
    }


      
    }

    
}



  
/* ------------------ NTP --------------------- */
if((minute() % settings.getSyncMinute()== 0) && second() == 0 && milli == 0)  //|| minute() == 5 || minute() == 10 || minute() == 15|| minute() == 20|| minute() == 25|| minute() == 30 || minute() == 35|| minute() == 35|| minute() == 40|| minute() == 45|| minute() == 50|| minute() == 55
{
  
wlan(true);


getntp();

getUpdateInfo();

getMoonphase(year(), month(), day());

getWeatherTemperature();

show_automatic();

wlan(!settings.getwlan());
 }
/* ------------------ NTP ENDE--------------------- */


    // Set nightmode/daymode.
    if ((hour() == hour(settings.getNightOffTime())) && (minute() == minute(settings.getNightOffTime())))
    {
      
    clearStrip();
      modus = "blank";
    }

    if ( (hour() == hour(settings.getDayOnTime())) && (minute() == minute(settings.getDayOnTime())))
    {
      
      
      modus = "clock";
    }



  // Call HTTP- and OTA-handle.
  esp8266WebServer.handleClient();
  ArduinoOTA.handle();

  
   
    hours = hour();
    while(hours >= 12) {
      hours -= 12;
    }
    minutes = minute();
    seconds = second();
    
   
  


 /* ------------------ led --------------------- */
  
   


// nightmode/daymode.
    if(modus == "clock" || modus == "clockInfo" || modus == "clockInfo2")
    {
  clearStrip();

  int color = settings.getColHel();
  // bei Dunkelheit kleine Hilfslichter einschalten...

  
    if(ldr.value() > settings.getBrightness()) {
    
      for(int i=0; i<60; i += settings.getldrDot()) {
        if(modus == "clock" || modus == "clockInfo")
        {
        strip.setPixelColor(i, defaultColors[color].red,defaultColors[color].green,defaultColors[color].blue);
        }
      }
    }

  
  
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


if( modus == "clockInfo")
  {
    //http://IP-Adresse/modus?modus=clockInfo&Info0=on
    //modus?modus=clockInfo&color=17&Info0=on&Info15=on&Info30=on&Info45=on


    
      int colorh = settings.getColHou();
      int colorm = settings.getColMin();
       if(Info0=="on")
       {
            if(second()%2==0)
            {
              if(hours==0)
                  strip.setPixelColor(0, defaultColors[colorh].red,defaultColors[colorh].green,defaultColors[colorh].blue);
              else if(minutes == 0)
                  strip.setPixelColor(0, defaultColors[colorm].red,defaultColors[colorm].green,defaultColors[colorm].blue);
              else
                  strip.setPixelColor(0, defaultColors[clockInfoColor].red,defaultColors[clockInfoColor].green,defaultColors[clockInfoColor].blue);
            }
            else
              strip.setPixelColor(0, 0,0,0);
       }

       if(Info15=="on")
       {
            if(second()%2==0)
            {
              if(hours==3)
                strip.setPixelColor(15, defaultColors[colorh].red,defaultColors[colorh].green,defaultColors[colorh].blue);
              else if(minutes == 15)
                  strip.setPixelColor(15, defaultColors[colorm].red,defaultColors[colorm].green,defaultColors[colorm].blue);
              else
                strip.setPixelColor(15, defaultColors[clockInfoColor].red,defaultColors[clockInfoColor].green,defaultColors[clockInfoColor].blue);
            }
            else
              strip.setPixelColor(15, 0,0,0);
       }

       if(Info30=="on")
       {
            if(second()%2==0)
            {
                if(hours==6)
                  strip.setPixelColor(30, defaultColors[colorh].red,defaultColors[colorh].green,defaultColors[colorh].blue);
                else if(minutes == 30)
                  strip.setPixelColor(30, defaultColors[colorm].red,defaultColors[colorm].green,defaultColors[colorm].blue);
                else
                  strip.setPixelColor(30, defaultColors[clockInfoColor].red,defaultColors[clockInfoColor].green,defaultColors[clockInfoColor].blue);
            }
            else
              strip.setPixelColor(30, 0,0,0);
       }

       if(Info45=="on")
       {
            if(second()%2==0)
            {
                if(hours==9)
                  strip.setPixelColor(45, defaultColors[colorh].red,defaultColors[colorh].green,defaultColors[colorh].blue);
                else if(minutes == 45)
                  strip.setPixelColor(45, defaultColors[colorm].red,defaultColors[colorm].green,defaultColors[colorm].blue);
                else
                  strip.setPixelColor(45, defaultColors[clockInfoColor].red,defaultColors[clockInfoColor].green,defaultColors[clockInfoColor].blue);
            }
            else
              strip.setPixelColor(45, 0,0,0);
       }

       
       
  
  }
  if( modus == "clockInfo2")
  {


//http://IP-Adresse/modus?color=14&modus=clockInfo2
    for(int i=0; i<60; i += settings.getldrDot()) {
       
        strip.setPixelColor(i, defaultColors[clockInfoColor].red,defaultColors[clockInfoColor].green,defaultColors[clockInfoColor].blue);
       
      }
  }
    strip.show();
    }

    
    if(modus =="WeatherTemperatur")
    {
   
     
      show_WeatherTemperatur();
    }

 if(modus =="Moonphase")
    {
      show_Moonphase();
    }
   

    if(modus == "blank")
    {
    startShow(0); //blank
    clearStrip();
    }


   if(modus =="northernlight")
   {
    northernlight();
   }
//http://IP-Adresse/modus?modus=fire
   if(modus =="fire")
   {
    fire();
   }
  
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
  for (int j=0; j<20; j++) {  //do 10 cycles of chasing
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
void leds (int startled, int leds, uint32_t c, int wait)
{
  int i=startled;
while(i <= startled+leds) {
  strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  i++;
}
  
}

void ledBrightness()
{
 int Bright = ldr.value();
 
 Bright = ((100 - Bright) * 2.55 )+ settings.getLdrLedOfset();

 if(Bright >= 255)
 {
  Bright = 255;
 }
Brightness = Bright;
strip.setBrightness(Brightness);
 
}


/******************************************************************************
  Get Moonphase
******************************************************************************/
int getMoonphase(int y, int m, int d)
{

  int b;
  int c;
  int e;
  double jd;
  if (m < 3)
  {
    y--;
    m += 12;
  }
  ++m;
  c = 365.25 * y;
  e = 30.6 * m;
  jd = c + e + d - 694039.09; // jd is total days elapsed
  jd /= 29.53;                // divide by the moon cycle (29.53 days)
  b = jd;                   // int(jd) -> b, take integer part of jd
  jd -= b;                // subtract integer part to leave fractional part of original jd
  b = jd * 8 + 0.5;         // scale fraction from 0-8 and round by adding 0.5
  b = b & 7;                // 0 and 8 are the same so turn 8 into 0
  Moonphase = b;
}

void show_Moonphase()
{
  int start_led = 0, stop_led = 0;
  clearStrip();
    switch( Moonphase)
      {
        case 0 :
        start_led = 0;
        stop_led = 0;
        
        break;
    
        case 1 :
        start_led = 10;
        stop_led = 20;
        break;
    
        case 2 :
        start_led = 5;
        stop_led = 25;
        break;
    
        case 3 :
        start_led = 0;
        stop_led = 30;
        break;
    
        case 4 :
        start_led = 0;
        stop_led = 60;
        break;
    
        case 5 :
        start_led = 30;
        stop_led = 60;
        break;
    
        case 6 :
        start_led = 35;
        stop_led = 55;
        break;
    
        case 7 :
        start_led = 40;
        stop_led = 50;
        break;
      }

    if( Moonphase != 0)
    { 
        while(stop_led >= start_led)
        {
        strip.setPixelColor(start_led, strip.Color(127, 127, 127));
        start_led++;
        }
    }else{
        //Neumond Anzeige
            for(int i=0; i<60; i += 10) {
       
        strip.setPixelColor(i, strip.Color(127, 127, 127));
        
      }
    }
    strip.show(); 
  
  
}
/******************************************************************************
  Get outdoor conditions from Yahoo.
******************************************************************************/
void getWeatherTemperature()
{
  Serial.println(F("Connecting..."));
  WeatherStatus = "Connecting...";
WiFiClient client;
  // Connect to HTTP server
  char servername[]="api.openweathermap.org"; 
  if (!client.connect(servername, 80)) {
    Serial.println(F("Connection failed"));
    WeatherStatus  += "Connection failed";
    return;
  }

  Serial.println(F("Connected!"));
WeatherStatus += "Connected!";

String url = "/data/2.5/weather?lat=";
  url += settings.getLat(location, sizeof(location));
  url += "&lon=";
  url += settings.getLon(location, sizeof(location));
  url += "&units=metric&appid=6d9ec0c530006d472308f93656026475";


  // Send HTTP request
  client.println("GET "+ url +" HTTP/1.0");
  client.println(F("Host: api.openweathermap.org"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
    WeatherStatus +="Failed to send request";
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    WeatherStatus +="Unexpected response: ";
    Serial.println(status);
    WeatherStatus += status;
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    WeatherStatus += "Invalid response";
    return;
  }

  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  //DynamicJsonBuffer jsonBuffer(capacity);
  DynamicJsonDocument doc(capacity);


  // Parse JSON object
 // JsonObject& root = jsonBuffer.parseObject(client);
 // if (!root.success()) {
  //  Serial.println(F("Parsing failed!"));
  //   WeatherStatus += "Parsing failed!";
  //  return;
  //}
  auto error = deserializeJson(doc, client);
if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    WeatherStatus += "Parsing failed!";
    return;
}


  // Extract values
  Serial.println(F("Response:"));
   WeatherStatus += "Response:";
   WeatherIcon = doc["weather"]["0"]["icon"].as<String>();
   WeatherTemperatur = doc["main"]["temp"].as<int>();
   WeatherHumidity = doc["main"]["humidity"].as<int>();
   WeatherName = doc["name"].as<String>();
   WeatherTime = doc["dt"].as<String>();

  


  // Disconnect
  client.stop();
  }





  void show_WeatherTemperatur()
    {
       clearStrip();
        int i=0;
        
        int tmp_WeatherTemperatur = WeatherTemperatur;
        if(WeatherTemperatur < 0){
          tmp_WeatherTemperatur = WeatherTemperatur * (-1);
          WeatherTemperatur_negative = true;
        }else{
          WeatherTemperatur_negative = false;
        }
        
              while(tmp_WeatherTemperatur >= i)
              {
                convertRGB(i);
                
                i++;
              }

              
       
      if(!WeatherTemperatur_negative){
              while(i <=NUM_PIXEL)
              {
                  setFloatPixelColor(i, 0,0,0);
                
                i++;
              }
      }else{
        
        int j = NUM_PIXEL -i;
        i= 0;
        while(i <= j)
              {
                  setFloatPixelColor(i, 0,0,0);
                
                i++;
              }

      }
      
              
      
     
       //delay(1000);
      }

  void convertRGB(int rgbWeatherTemperatur) 
{ 
//Maximalwert festlegen 
// Beispiel 25°C 
// Anzahl der Stufen / Maximalwert 
// = 1020 / 25 
// Faktor 40.8 
int farbe = round(40.8 * rgbWeatherTemperatur);  
int rot, gruen, blau;
// 
if (farbe < 1) 
farbe = 0; 
if (farbe > 1020) 
farbe = 1020; 
if (farbe <= 510) 
{ 
rot = 0; 
if (farbe <= 255) 
{ 
gruen = 0 + farbe; 
blau = 255; 
} 
if (farbe > 255) 
{ 
farbe = farbe - 255; 
blau = 255 - farbe; 
gruen = 255; 
} 
if (farbe > 255) 
{ 
farbe = farbe - 255; 
blau = 255 - farbe; 
gruen = 255; 
} 
} 

if (farbe > 510) 
{ 
farbe = farbe - 510; 
blau = 0; 
if (farbe <= 255) 
{ 
rot = 0 + farbe; 
gruen = 255; 
} 
if (farbe > 255) 
{ 
farbe = farbe - 255; 
gruen = 255 - farbe; 
rot = 255; 
} 
} 


if(WeatherTemperatur_negative)
  {
    rgbWeatherTemperatur = NUM_PIXEL - rgbWeatherTemperatur;
  }else{
    
  } 
setFloatPixelColor(rgbWeatherTemperatur, rot,gruen,blau);
strip.show();
} 

void show_automatic()
{
  if(settings.getAutoShow() == true)
  {
            /* ------------------ New Update  --------------------- */
        if (updateInfo > String(FirmewareVersion))
        {
          theaterChase(strip.Color(0, 0, 255), 50);
        }
      
      /*
      show_WeatherTemperatur();
          
      delay(20000); 
      
      show_Moonphase();
      delay(20000); 
      */
  }

  
}

void fire()
{
int r = 255;
int g = 96;
int b = 12;

for(int x = 0; x <60; x++)
{
int flicker = random(0,150);
int r1 = r-flicker;
int g1 = g-flicker;
int b1 = b-flicker;
if(g1<0) g1=0;
if(r1<0) r1=0;
if(b1<0) b1=0;
strip.setPixelColor(x,r1,g1, b1);
}
strip.show();
delay(random(250,750));
  
}

const uint8_t strips = 1;  // number of strips
const uint8_t strip1 = 1;  // number of strips
int cw_pos[strips][NUM_PIXEL];  // current position within colorwheel
uint8_t R[strips][NUM_PIXEL];  // Red value for each LED within each strip
uint8_t G[strips][NUM_PIXEL];  // Green value for each LED within each strip
uint8_t B[strips][NUM_PIXEL];  // Blue value for each LED within each strip
int acc[strips];  // aurora current color
int atc[strips];  // aurora target color
uint8_t acb[strips];  // aurora current brightness
uint8_t atb[strips];  // aurora target brightness
uint8_t cw_val[3];  // temporary storage for a single set of RGB values
float bright = 0.5;
uint8_t cw_speed = 5;  // color change speed


// http://192.168.178.155/modus?modus=northernlight
void northernlight()
{

      delay(ceil(100/cw_speed));
    if (reset){
      reset = 0;
      for (i = 0; i < strip1; i++){
        acc[i] = random(290+248*i,290+248*(i+1));
        atc[i] = acc[i];
        acb[i] = random(1,101);
        atb[i] = acb[i];
        cw(cw_val,acc[i]);
        for (j = 0; j < NUM_PIXEL; j++){
          R[i][j] = ceil(cw_val[0]*acb[i]/100*bright);
          G[i][j] = ceil(cw_val[1]*acb[i]/100*bright);
          B[i][j] = ceil(cw_val[2]*acb[i]/100*bright);
        }
      }
    }
    else{
      for (i = 0; i < strip1; i++){
        if (acc[i] == atc[i]) atc[i] = random(290+248*i,290+248*(i+1));
        else if (acc[i] < atc[i]) acc[i] = acc[i] + 1;
        else acc[i] = acc[i] - 1;
        
        if (atb[i] == acb[i]) atb[i] = random(1,101);
        else if (acb[i] < atb[i]) acb[i]++;
        else acb[i]--;
        
        cw(cw_val,acc[i]);
        for (j = 0; j < NUM_PIXEL; j++){
          R[i][j] = ceil(cw_val[0]*acb[i]/100*bright);
          G[i][j] = ceil(cw_val[1]*acb[i]/100*bright);
          B[i][j] = ceil(cw_val[2]*acb[i]/100*bright);
        }
      }
    }


      // LED updates
  for (i = 0; i < strip1; i++) {
    for (j = 0; j < NUM_PIXEL; j++){
      if (i == 0){
        strip.setPixelColor(j,R[i][j], G[i][j], B[i][j]);
      }
      else if (i == 1){
       
      }
      else if (i == 2){
      
      }
      else if (i == 3){
      
      }
      else if (i == 4){
      
      }
    }
    strip.show();
   random(100,750);
   
   
   
  }
}



    // Colorwheel reference (takes a 3-variable array and a position within the colorwheel, updates the array with RGB values for that position)
void cw(uint8_t c_temp[], int pos_temp){
    if (pos_temp <= 255){
      c_temp[0] = 255;
      c_temp[1] = pos_temp;
      c_temp[2] = 0;
    }
    else if (pos_temp <= 255*2){
      c_temp[0] = 255*2 - pos_temp;
      c_temp[1] = 255;
      c_temp[2] = 0;
    }
    else if (pos_temp <= 255*3){
      c_temp[0] = 0;
      c_temp[1] = 255;
      c_temp[2] = pos_temp - 255*2;
    }
    else if (pos_temp <= 255*4){
      c_temp[0] = 0;
      c_temp[1] = 255*4 - pos_temp;
      c_temp[2] = 255;
    }
    else if (pos_temp <= 255*5){
      c_temp[0] = pos_temp - 255*4;
      c_temp[1] = 0;
      c_temp[2] = 255;
    }
    else if (pos_temp <= 255*6){
      c_temp[0] = 255;
      c_temp[1] = 0;
      c_temp[2] = 255*6 - pos_temp;
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
   // clearStrip();
    for(byte i=45; i<NUM_PIXEL; i++) {
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
   //startShow(1); //red
   leds (45, 15, strip.Color(255, 0, 0) , 50);
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
      if(startled){
        leds (15, 15, strip.Color(0, 0, 255), 50);
      }
      //clearStrip();
  wifiManager.setTimeout(WIFI_SETUP_TIMEOUT);
  wifiManager.autoConnect(HOSTNAME, WIFI_AP_PASS);

  
        if (WiFi.status() != WL_CONNECTED)
        {
         
          WiFi.mode(WIFI_AP);
          Serial.println("No WLAN connected. Staying in AP mode.");
          
     
              if(startled){
              startShow(3); //blue
               
               leds (15, 15, strip.Color(255, 0, 0) , 50);//red
               leds (45, 15, strip.Color(255, 0, 0) , 50);//red
              }
              
          }
          else
          {
            WiFi.mode(WIFI_STA);
            Serial.println("WLAN connected. Switching to STA mode.");
            //delay(1000);
          
            //clearStrip();
              if(startled){
            //startShow(2);  // Green
         leds (30, 15, strip.Color(0, 255, 0), 50);//green
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

void getUpdateInfo()
{
//https://github.com/esp8266/Arduino/blob/4897e0006b5b0123a2fa31f67b14a3fff65ce561/doc/esp8266wifi/client-secure-examples.md
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(UPDATE_SERVER);
  if (!client.connect(UPDATE_SERVER, 443)) {
    updateInfo = "connection failed";
    Serial.println("connection failed");
    return;
  }

  if (client.verify(UPDATE_fingerprint, UPDATE_SERVER)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  Serial.print("requesting URL: ");
  Serial.println(UPDATE_INFOFILE);

  client.print(String("GET ") + UPDATE_INFOFILE + " HTTP/1.1\r\n" +
               "Host: " + UPDATE_SERVER + "\r\n" +
               "User-Agent: ESP8266_Relay01\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  //read Header
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line); //debug
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  //read content
  Serial.println("-------read content -------------------------------");
  
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line != "\r") {
    line = line.substring(line.indexOf('{'), line.lastIndexOf('}') + 1);

    //Serial.printf("Status: %u\r\n", statusCode);
    Serial.printf("Response is %u bytes.\r\n", line.length());
    Serial.println(line);
    Serial.println("Parsing JSON.");

    //DynamicJsonBuffer jsonBuffer;
    //StaticJsonBuffer<256> jsonBuffer;
    DynamicJsonDocument doc(1024);

   //JsonObject &responseJson = jsonBuffer.parseObject(line);
   // if (responseJson.success())
   auto error = deserializeJson(doc, line);
if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    WeatherStatus += "Parsing failed!";
    return;
}else {
      
  if(settings.getUpdateStable())
  {
    Serial.println("Stable Version");
  updateInfo = doc["channel"]["stable"]["version"].as<String>();
  }else{
    updateInfo = doc["channel"]["unstable"]["version"].as<String>();
  Serial.println("Unstable Version");
  }
      

     
      return;
     }
     Serial.println(updateInfo);

      
    }
    else {
      Serial.println("fertig!");
      break;
    }

    
    
}


///  else Serial.printf("Status: %u\r\n", statusCode);
 // Serial.println("Error (" + String(UPDATE_SERVER) + ")");

}

/******************************************************************************
  Do Update
******************************************************************************/

void doupdate() {

  
    //wlan(false);
    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }
    
    wlan(true);
    startShow(10); //orange
    // wait for WiFi connection
    if(WiFi.status() == WL_CONNECTED) {
        
               // Use WiFiClientSecure class to create TLS connection
                WiFiClientSecure client;
                Serial.print("connecting to ");
                Serial.println(UPDATE_SERVER);
                if (!client.connect(UPDATE_SERVER, 443)) {
                  Serial.println("connection failed");
                  return;
                }
              
                if (client.verify(UPDATE_fingerprint, UPDATE_SERVER)) {
                  Serial.println("certificate matches");
                } else {
                  Serial.println("certificate doesn't match");
                  return;
                }
              
                Serial.print("Starting OTA from: ");
                Serial.println(UPDATE_SERVER + String(UPDATE_FILE));
                t_httpUpdate_return ret = ESPhttpUpdate.update("https://"+String(UPDATE_SERVER)+String(UPDATE_FILE),String(""),String(UPDATE_fingerprint));
               
                // if successful, ESP will restart
                
                Serial.println("update failed");
                Serial.println((int) ret);
                
    
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
  esp8266WebServer.on("/modus", []() {handleModus();});//; callBack();});
  esp8266WebServer.on("/moon", []() {handleMoon();});//; callBack();});
  esp8266WebServer.on("/temp", []() {handleTemp();});//; callBack();});
  esp8266WebServer.on("/brightness", []() {handleBrightness();});//; callBack();});
  esp8266WebServer.on("/wifiReset", handleWiFiReset);
  esp8266WebServer.on("/reset", handleReset);
  
  esp8266WebServer.begin();
  Serial.print("gestartet!!");
}

String htmlTop(String page)
{
  Serial.print("Starte WebServer: "+ page);
  String message = "<!doctype html>";
  message += "<html style=\"height: 100%;\">";
  message += "<head>";
  message += "<title>" + String(HOSTNAME) + "-" +page +" - " + modus+"</title>";
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
  message += "<table style=\"height: 100%; ; margin-left: auto; margin-right: auto; width: 425px; background-color: #53bbf4;\"><tbody><tr><td style=\"height: 10%;\">";

//https://fontawesome.com/v4.7.0/icons/
  
  message += "&nbsp;&nbsp;<button onclick=\"window.location.href='/'\"><i class=\"fa fa-home\"></i></button>&nbsp;&nbsp; ";
  message += "<button onclick=\"window.location.href='/reset'\"><i class=\"fa fa-refresh\"></i></button>&nbsp;&nbsp;";
  message += "<button onclick=\"window.location.href='/Settings'\"><i class=\"fa fa-wrench\"></i></button>&nbsp;&nbsp; ";
    message += "<button onclick=\"window.location.href='modus?modus=clock'\"><i class=\"fa fa-clock-o\"></i></button>&nbsp;&nbsp; ";
  message += "<button onclick=\"window.location.href='modus?modus=WeatherTemperatur'\"><i class=\"fa fa-thermometer-half\"></i></button>&nbsp;&nbsp; ";
    message += "<button onclick=\"window.location.href='modus?modus=Moonphase'\"><i class=\"fa fa-moon-o\"></i></button>&nbsp;&nbsp; ";
   message += "<button onclick=\"window.location.href='/update'\"><i class=\"fa fa-upload\"></i></button> &nbsp;&nbsp;";
   message +="</td></tr><tr><td style=\"height: 80%; vertical-align: top;\">";
  
  return message;
}


String htmlButton()
{
  String message = "</td></tr><tr><td  style=\"height: 10%;\">";
 message += "________________________________<br>";
  message += "<a href=\"https://github.com/chrkan/ESP8266_WEMO_Liquid_Clock\">ckany 2018</a><br>";
  
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
  String message = htmlTop("404");
  message ="<br><br>404 - File Not Found.";
  message += htmlButton();
  esp8266WebServer.send(200, "text/html", message);
}


void handleRoot()
{
  //Handler for the rooth path
 
    String message = htmlTop("Home");

  WiFiManager wifiManager;
  message += "<br><br>Wlan: " + WiFi.SSID()+"<br>";
  //message += "<br>"+WiFi.localIP();
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
  message += String(ldr.value()) + " % " + String(Brightness) +" LED (min: " + String(LDR_MANUAL_MIN) + ", max: " + String(LDR_MANUAL_MAX) + ")";
  message += "<br>Help Dots every: "+String(settings.getldrDot())+" Pixels, by "+String(settings.getBrightness())+"% Brightness." ;


  
  message += "<br><br>NTP Server: "+ String(settings.getntpServer(location, sizeof(location)));

  if (updateInfo > String(FirmewareVersion))
  {
    if(settings.getUpdateStable())
      {
        message += "<br><br><span style=\"color:red;\"><a href=\"/update\">Firmwareupdate available! (" + updateInfo + ")<br> Your Firmeware: "+ String(FirmewareVersion) +"</a></span>";
      }else{
        message += "<br><br><span style=\"color:red;\"><a href=\"/updates\">Firmwareupdate available! (" + updateInfo + ")<br> Your Firmeware: "+ String(FirmewareVersion) +"</a></span>";
      }
  }else{
     message += "<br><br><span style=\"color:green;\">Your Firmeware: "+ String(FirmewareVersion) +" (" + updateInfo + ")</span>";
  }
 message += "<br><br>Modus: -"+ String(modus)+"-";
message += "<br><br>Weather for: "+ String(WeatherName);
message += "<br>Temperature: "+ String(WeatherTemperatur)+"&deg; Grad Celsius";
//message += "<br>"+String(WeatherStatus);
message += "<br>Humidity: "+ String(WeatherHumidity)+" &#37;";
//message += "- <img src=\"http://openweathermap.org/img/w/"+String(WeatherIcon)+".png\" >"; 
 String l_line = WeatherTime;
  time_t t = l_line.toInt(); 
message += "<br><br>Moonphase: "+String(Moonphase) +" -  "+ day()+ "." + month()+ "." + year();
  message += "<br><br>Setting Version: "+String(settings.getSettingVersion());
  
  
  message += htmlButton();
  esp8266WebServer.send(200, "text/html", message);
  
 
}
void handleTemp()
{
 // modus ="WeatherTemperatur";
  WeatherTemperatur = esp8266WebServer.arg("WeatherTemperatur").toInt();
    esp8266WebServer.send(200, "text/html", String(WeatherTemperatur)); 
}

void handleMoon()
{
 // modus ="WeatherTemperatur";
  Moonphase = esp8266WebServer.arg("phase").toInt();
    esp8266WebServer.send(200, "text/html", String(WeatherTemperatur)); 
}

void handleBrightness()
{
  strip.setBrightness(esp8266WebServer.arg("brightness").toInt());

    esp8266WebServer.send(200, "text/html", String(WeatherTemperatur)); 
  
}
void handleupdates()
{

 doupdate();

}
void handleModus()
{
  
modus = String(esp8266WebServer.arg("modus"));
clockInfoColor = esp8266WebServer.arg("color").toInt();
if(esp8266WebServer.arg("Info0") != NULL)
  Info0=esp8266WebServer.arg("Info0");
if(esp8266WebServer.arg("Info15") != NULL)
  Info15=esp8266WebServer.arg("Info15");
if(esp8266WebServer.arg("Info30") != NULL)
  Info30=esp8266WebServer.arg("Info30");
if(esp8266WebServer.arg("Info45") != NULL)
  Info45=esp8266WebServer.arg("Info45");

if(modus=="clockInfo")
{
  colorWipe(strip.Color(defaultColors[clockInfoColor].red,defaultColors[clockInfoColor].green,defaultColors[clockInfoColor].blue), 10);
}

if(modus=="northernlight")
{
reset = 1;
  northernlight();
 
}

if(modus=="fire")
{
startShow(0);
  fire();
 
}



callRoot();
    
}
void handleCommitSettings()
{
startShow(2); //Green
String message = htmlTop("Home");

if(esp8266WebServer.arg("Wlan") == "1") 
{ 
  settings.setwlan(true);
}else{
  
  settings.setwlan(false);
}

if(esp8266WebServer.arg("DEFAULT_LDR_Status") == "1") 
{ 
  settings.setUseLdr(true);
}else{
  
  settings.setUseLdr(false);
}
if(esp8266WebServer.arg("autoshow") == "1") 
{ 
  settings.setAutoShow(true);
}else{
  
  settings.setAutoShow(false);
}


settings.setBrightness(esp8266WebServer.arg("br").toInt());

settings.setldrDot(esp8266WebServer.arg("ldrdots").toInt());

settings.setLdrSync(esp8266WebServer.arg("ldrsync").toInt());

settings.setLdrLedOfset(esp8266WebServer.arg("ldrledofset").toInt());

settings.setSyncMinute(esp8266WebServer.arg("SyncMinute").toInt());

  char text[LEN_LOC_STR];
  memset(text, 0, sizeof(text));
  esp8266WebServer.arg("ntp").toCharArray(text, sizeof(text), 0);
  settings.setntpServer(text, sizeof(text));

  settings.setColSec(esp8266WebServer.arg("colsec").toInt());
  settings.setColMin(esp8266WebServer.arg("colmin").toInt());
  settings.setColHou(esp8266WebServer.arg("colhour").toInt());
  settings.setColHel(esp8266WebServer.arg("colhel").toInt());

  if(esp8266WebServer.arg("UpdateStable") == "1") 
{ 
  settings.setUpdateStable(true);
}else{
  
  settings.setUpdateStable(false);
}
if(esp8266WebServer.arg("no") != "00:00" || esp8266WebServer.arg("do") != "00:00")
    {
      settings.setNightOffTime(esp8266WebServer.arg("no").substring(0, 2).toInt() * 3600 + esp8266WebServer.arg("no").substring(3, 5).toInt() * 60);
      // ------------------------------------------------------------------------
      settings.setDayOnTime(esp8266WebServer.arg("do").substring(0, 2).toInt() * 3600 + esp8266WebServer.arg("do").substring(3, 5).toInt() * 60);
    }

  esp8266WebServer.arg("lat").toCharArray(text, sizeof(text), 0);
  settings.setLat(text, sizeof(text));


  esp8266WebServer.arg("lon").toCharArray(text, sizeof(text), 0);
  settings.setLon(text, sizeof(text));
    
  message += settingshtml();
  message += htmlButton();
  esp8266WebServer.send(200, "text/html", message);  
  delay(500);
  settings.saveToEEPROM();
  if(esp8266WebServer.arg("UpdateStable") != esp8266WebServer.arg("UpdateStable_old"))
  {
    getUpdateInfo();
  }

  if(esp8266WebServer.arg("ntp") != esp8266WebServer.arg("ntp_old"))
  {
    handleReset();
  }

  
}
String settingshtml()
{
String message = "<form action=\"/commitSettings\">";
    
    message += "<br><br><table align=\"center\"><tr><th>";
// ------------------------------------------------------------------------   
    message += "Auto Wlan: </th><th><input type=\"radio\" name=\"Wlan\" value=\"1\"";
    if (settings.getwlan()) message += " checked";
    message += "> on ";
    message += " <input type=\"radio\" name=\"Wlan\" value=\"0\"";
    if (!settings.getwlan()) message += " checked";
    message += "> off</th></tr>";

 // ------------------------------------------------------------------------
    message += "<tr><td>Wlan Sync  every </td><td><select name=\"SyncMinute\">";
    message += "<option value=\""+String(settings.getSyncMinute())+"\" selected>"+String(settings.getSyncMinute())+"</option>";
    message += "<option value=\"5\">5</option>";
    message += "<option value=\"10\">10</option>";
    message += "<option value=\"15\">15</option>";
    message += "<option value=\"30\">30</option>";
    message += "</select> Minute.</td></tr>";

 // ------------------------------------------------------------------------
    message += "<tr><th>&nbsp;</th><th>&nbsp;</th></tr>"; 
// ------------------------------------------------------------------------   
    message += "<tr><td>Auto Show at Wlan Sync:</th><th><input type=\"radio\" name=\"autoshow\" value=\"1\"";
    if (settings.getAutoShow()) message += " checked";
    message += "> on ";
    message += " <input type=\"radio\" name=\"autoshow\" value=\"0\"";
    if (!settings.getAutoShow()) message += " checked";
    message += "> off</th></tr>";
 // ------------------------------------------------------------------------
    message += "<tr><th>&nbsp;</th><th>&nbsp;</th></tr>";
// ------------------------------------------------------------------------  
    
  message += "<tr><td>";
  message += "Night off:";
  message += "</td><td>";
  message += "<input type=\"time\" name=\"no\" value=\"";
  if (hour(settings.getNightOffTime()) < 10) message += "0";
  message += String(hour(settings.getNightOffTime())) + ":";
  if (minute(settings.getNightOffTime()) < 10) message += "0";
  message += String(minute(settings.getNightOffTime())) + "\"  placeholder=\"0\">";
  message += " h";
  message += "</td></tr>";
  // ------------------------------------------------------------------------
  message += "<tr><td>";
  message += "Day on:";
  message += "</td><td>";
  message += "<input type=\"time\" name=\"do\" value=\"";
  if (hour(settings.getDayOnTime()) < 10) message += "0";
  message += String(hour(settings.getDayOnTime())) + ":";
  if (minute(settings.getDayOnTime()) < 10) message += "0";
  message += String(minute(settings.getDayOnTime())) + "\" placeholder=\"0\">";
  message += " h";
  message += "</td></tr>";
  // ------------------------------------------------------------------------

    message += "<tr><th>&nbsp;</th><th>&nbsp;</th></tr>";
  // ------------------------------------------------------------------------  
  message += "<tr><th>LDR: </th><th><input type=\"radio\" name=\"DEFAULT_LDR_Status\" value=\"1\"";
    if (settings.getUseLdr()) message += " checked";
    message += "> on ";
    message += " <input type=\"radio\" name=\"DEFAULT_LDR_Status\" value=\"0\"";
    if (!settings.getUseLdr()) message += " checked";
    message += "> off</th></tr>";
// ------------------------------------------------------------------------
message += "<tr><td>Help Dots by </td><td><select name=\"br\">";
  for (int i = 10; i <= 100; i += 10)
  {
    message += "<option value=\"" + String(i) + "\"";
    if (i == settings.getBrightness()) message += " selected";
    message += ">";
    message += String(i) + "</option>";
  }
  message += "</select> %  LDR</td></tr>";
 // ------------------------------------------------------------------------ 
  message += "<tr><td>LDR Sync</td><td><select name=\"ldrsync\">";
  for (int i = 1; i <= 60; i += 1)
  {
    message += "<option value=\"" + String(i) + "\"";
    if (i == settings.getLdrSync()) message += " selected";
    message += ">";
    message += String(i) + "</option>";
  }
  message += "</select> Seconds</td></tr>";

 // ------------------------------------------------------------------------
message += "<tr><td>LDR LED Ofset</td><td><select name=\"ldrledofset\">";
  for (int i = 0; i <= 100; i += 10)
  {
    message += "<option value=\"" + String(i) + "\"";
    if (i == settings.getLdrLedOfset()) message += " selected";
    message += ">";
    message += String(i) + "</option>";
  }
  message += "</select> % </td></tr>";

// ------------------------------------------------------------------------    
    message += "<tr><td>Help Dots every </td><td><select name=\"ldrdots\">";
    message += "<option value=\""+String(settings.getldrDot())+"\" selected>"+String(settings.getldrDot())+"</option>";
    message += "<option value=\"5\">5</option>";
    message += "<option value=\"10\">10</option>";
    message += "<option value=\"15\">15</option>";
    message += "<option value=\"30\">30</option>";
    message += "</select> Pixels.</td></tr>";
// ------------------------------------------------------------------------
    message += "<tr><th>&nbsp;</th><th>&nbsp;</th></tr>";
  // ------------------------------------------------------------------------  
    message += "<tr><td>NTP:</td><td><input type=\"text\" size=\"20\" name=\"ntp\" value=\"";
    settings.getntpServer(location, sizeof(location));
    message += String(location) + "\" pattern=\"[\\x20-\\x7e]{0," + String(LEN_LOC_STR-1) + "}\" placeholder=\"Enter NTP Server ...\">";
    message += "<input type=\"hidden\" name=\"ntp_old\" value=\"";
    settings.getntpServer(location, sizeof(location));
    message += String(location) + "\" pattern=\"[\\x20-\\x7e]{0," + String(LEN_LOC_STR-1) + "}\" placeholder=\"Enter NTP Server ...\">";
    message +="<br></td></tr>";
// ------------------------------------------------------------------------
    message += "<tr><th>&nbsp;</th><th>&nbsp;</th></tr>";
// ------------------------------------------------------------------------    
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
    message += "</select></td></tr>";
    // ------------------------------------------------------------------------
    message += "<tr><th>&nbsp;</th><th>&nbsp;</th></tr>";
  // ------------------------------------------------------------------------  
    message += "<tr><td>Lat:</td><td><input type=\"text\" size=\"20\" name=\"lat\" value=\"";
    settings.getLat(location, sizeof(location));
    message += String(location) + "\" pattern=\"[\\x20-\\x7e]{0," + String(LEN_LOC_STR-1) + "}\" placeholder=\"Enter Lat ...\">";
    message += "<tr><td>Long:</td><td><input type=\"text\" size=\"20\" name=\"lon\" value=\"";
    settings.getLon(location, sizeof(location));
    message += String(location) + "\" pattern=\"[\\x20-\\x7e]{0," + String(LEN_LOC_STR-1) + "}\" placeholder=\"Enter Lat ...\">";
// ------------------------------------------------------------------------
  message += "<tr><th>&nbsp;</th><th>&nbsp;</th></tr>";
// ------------------------------------------------------------------------
  message += "<tr><th>Update: </th><th><input type=\"radio\" name=\"UpdateStable\" value=\"1\"";
    if (settings.getUpdateStable()) message += " checked";
    message += "> Stable ";
// ------------------------------------------------------------------------
    message += "<input type=\"hidden\" name=\"UpdateStable_old\" value=\"";
    message += String(settings.getUpdateStable());
    message += "\" placeholder=\"Enter NTP Server ...\">";
// ------------------------------------------------------------------------    
    message += " <br><input type=\"radio\" name=\"UpdateStable\" value=\"0\"";
    if (!settings.getUpdateStable()) message += " checked";
    message += "> Unstable</th></tr>";
// ------------------------------------------------------------------------
    

    
    message += "</table>";
    message += "<br><br><button title=\"Save Settings.\"><i class=\"fa fa-check\"></i></button>";
  message += "</form>";
  return message;
  
}

void handleSettings()
{
  //Handler for the rooth path
 
    String message = htmlTop("Settings");

    message +=settingshtml();
  
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
  esp8266WebServer.send(200, "text/plain", "OK.");
  startShow(10); //Orange
  callBack();
  settings.resetToDefault();
  
  ESP.restart();
}

void handleWiFiReset()
{
  esp8266WebServer.send(200, "text/plain", "OK.");
  
  WiFi.disconnect(true);
      WiFiManager wifiManager;
      wifiManager.resetSettings();

 
  ESP.restart();
}
