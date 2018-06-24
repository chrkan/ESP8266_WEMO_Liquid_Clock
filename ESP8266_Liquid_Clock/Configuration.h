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

/* ------------------ Wifi --------------------- */


//#define WIFI_RESET
//#define WLANOFF  //Schaltet Wlan nur für Sync an
#define HOSTNAME "Liquid"
#define WIFI_SETUP_TIMEOUT 120
#define WIFI_AP_PASS "12345678"
#define OTA_PASS "1234"
#define SHOW_IP

/* ------------------ Zeitsserver Einstellungen --------------------- */

#define NTP_SERVER "192.168.178.254"
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
/*
 *
 * Seltener zu aendernde Einstellungen...
 *
 */

// ------------------ Tasten --------------------- 
/*
 * Die Zeit in Millisekunden, innerhalb derer Prellungen der Taster nicht als Druecken zaehlen.
 * (Und damit auch die Tastaturwiederholrate)
 * Default: 300
 */
  #define BUTTON_TRESHOLD 300



// ------------------ Lichtabhaengiger Widerstand ---------------------
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

#endif


