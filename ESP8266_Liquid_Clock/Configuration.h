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


