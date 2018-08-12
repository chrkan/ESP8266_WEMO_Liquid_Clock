/**
 * Configuration.h
 * Die Compiler-Einstellungen der Firmware an einer zentralen Stelle.
 *
 * @mc       Arduino/RBBB
 * @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  1.0
 * @created  7.7.2013
 * @updated  7.7.2013
 *
 * Versionshistorie:
 * V 1.0:  - Erstellt
 *
 */


 
#ifndef CONFIGURATION_H
#define CONFIGURATION_H


#define DEBUG

#define FirmewareVersion  "20180812"

// ------------------ Pixel Einstellungen ---------------------

#define NUM_PIXEL      60       // Anzahl der NeoPixel LEDs
#define STRIP_PIN            2        // Digital  ESP8266


#define UPDATE_Stable true
#define UPDATE_SERVER "raw.githubusercontent.com"
#define UPDATE_INFOFILE "/chrkan/ESP8266_WEMO_Liquid_Clock/master/update/version.json"
#define UPDATE_FILE     "/chrkan/ESP8266_WEMO_Liquid_Clock/master/update/ESP8266_Liquid_Clock.ino.d1_mini.bin"
#define fingerprint "CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33"




#define SETTINGS_MAGIC_NUMBER 0x04
#define SETTINGS_SettingVersion 0

/* ------------------ Colors --------------------- */
  #define DEFAULT_Second 1
  #define DEFAULT_Minute 17
  #define DEFAULT_Hour 9
  #define DEFAULT_Help_Dot 25

// 00 WHITE
// 01 RED
// 02 RED_25
// 03 RED_50
// 04 ORANGE
// 05 YELLOW
// 06 YELLOW_25
// 07 YELLOW_50
// 08 GREENYELLOW
// 09 GREEN
// 10 GREEN_25
// 11 GREEN_50
// 12 MINTGREEN
// 13 CYAN
// 14 CYAN_25
// 15 CYAN_50
// 16 LIGHTBLUE
// 17 BLUE
// 18 BLUE_25
// 19 BLUE_50
// 20 VIOLET
// 21 MAGENTA
// 22 MAGENTA_25
// 23 MAGENTA_50
// 24 PINK
// 25 Dark_Yellow


/* ------------------ Zeitsserver Einstellungen --------------------- */

#define DEFAULT_NTP_SERVER "pool.ntp.org"
#define NTP_TIMEOUT 2000


//#define TIMEZONE_IDLW  // IDLW  International Date Line West UTC-12
//#define TIMEZONE_SST   // SST   Samoa Standard Time UTC-11
//#define TIMEZONE_HST   // HST   Hawaiian Standard Time UTC-10
//#define TIMEZONE_AKST  // AKST  Alaska Standard Time UTC-9
//#define TIMEZONE_USPST // USPST Pacific Standard Time (USA) UTC-8
//#define TIMEZONE_USMST // USMST Mountain Standard Time (USA) UTC-7
//#define TIMEZONE_USAZ  // USAZ  Mountain Standard Time (USA) UTC-7 (no DST)
//#define TIMEZONE_USCST // USCST Central Standard Time (USA) UTC-6
//#define TIMEZONE_USEST // USEST Eastern Standard Time (USA) UTC-5
//#define TIMEZONE_AST   // AST   Atlantic Standard Time UTC-4
//#define TIMEZONE_BST   // BST   Eastern Brazil Standard Time UTC-3
//#define TIMEZONE_VTZ   // VTZ   Greenland Eastern Standard Time UTC-2
//#define TIMEZONE_AZOT  // AZOT  Azores Time UTC-1
//#define TIMEZONE_GMT   // GMT   Greenwich Mean Time UTC
#define TIMEZONE_CET     // CET   Central Europe Time UTC+1
//#define TIMEZONE_EST   // EST   Eastern Europe Time UTC+2
//#define TIMEZONE_MSK   // MSK   Moscow Time UTC+3 (no DST)
//#define TIMEZONE_GST   // GST   Gulf Standard Time UTC+4
//#define TIMEZONE_PKT   // PKT   Pakistan Time UTC+5
//#define TIMEZONE_BDT   // BDT   Bangladesh Time UTC+6
//#define TIMEZONE_JT    // JT    Java Time UTC+7
//#define TIMEZONE_CNST  // CNST  China Standard Time UTC+8
//#define TIMEZONE_HKT   // HKT   Hong Kong Time UTC+8
//#define TIMEZONE_PYT   // PYT   Pyongyang Time (North Korea) UTC+8.5
//#define TIMEZONE_CWT   // CWT   Central West Time (Australia) UTC+8.75
//#define TIMEZONE_JST   // JST   Japan Standard Time UTC+9
//#define TIMEZONE_ACST  // ACST  Australian Central Standard Time UTC+9.5
//#define TIMEZONE_AEST  // AEST  Australian Eastern Standard Time UTC+10
//#define TIMEZONE_LHST  // LHST  Lord Howe Standard Time UTC+10.5
//#define TIMEZONE_SBT   // SBT   Solomon Islands Time UTC+11
//#define TIMEZONE_NZST  // NZST  New Zealand Standard Time UTC+12
/* ------------------ Wifi --------------------- */

#define HOSTNAME "Liquid"
#define WIFI_SETUP_TIMEOUT 120
#define WIFI_AP_PASS "12345678"
#define OTA_PASS "1234"
#define SHOW_IP


/* ------------------ LDR --------------------- */
/*
 * Sollen die Grenzwerte vom LDR automatisch angepasst werden? Bei einem Neustart der QlockTwo kann
 * das Display flackern, dann muss man einmal ueber den LDR 'wischen', damit er verschiedene
 * Messwerte bekommt. Ohne AUTOSCALE werden die manuellen Min-/Max-Werte genommen.
 * Default: eingeschaltet.
 */

  #define LDR_SIGNAL A0
  #define LDR_AUTOSCALE
  #define LDR_MANUAL_MIN 0
  #define LDR_MANUAL_MAX 1023
/*
 * Die Menge der Werte fuer die Glaettung. Mehr Werte bringen bessere Ergebnisse aber
 * brauchen auch mehr Zeit (und mehr Speicher! / Gleitender Mittelwert)
 * Default: 32
 */
  #define LDR_MEAN_COUNT 32

  #define DEFAULT_LDR_Status true
  #define DEFAULT_LDR_LED 15
  #define DEFAULT_LDR_Brightness 50
  
#define SYSLOGSERVER
//------------------
#define SYSLOGSERVER_SERVER "192.168.178.99"
#define SYSLOGSERVER_PORT 514


#endif


