/**
 * LDR.cpp
 * Klasse fuer den Zugriff auf einen lichtabhaengigen Widerstand.
 *
 * @mc       Arduino/RBBB
 * @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  1.5
 * @created  18.3.2012
 * @updated  18.8.2013
 *
 * Versionshistorie:
 * V 1.1:  - Optimierung hinsichtlich Speicherbedarf.
 * V 1.2:  - Verbessertes Debugging.
 * V 1.3:  - Beschraenkund der LDR-Werte bei autoscale == false.
 * V 1.4:  - Der LDR mapped die Werte jetzt selbst, dadurch wird flackern bei unguenstigen Lichtverhaeltnissen vermindert.
 * V 1.5:  - Der LDR gibt Werte zwischen 0 und 100% zurueck, das ist besser verstaendlich.
 */
#include "LDR.h"

#define DEBUG
#include "Debug.h"

/**
 * Initialisierung mit dem Pin, an dem der LDR haengt.
 * Die Maximalwerte vom LDR koennen automatisch
 * eingemessen werden (LDR_AUTOSCALE).
 * Ansonsten muss man diese Werte im #define-DEBUG-Mode
 * ausmessen und eintragen.
 */
LDR::LDR(byte pin) {
  _pin = pin;
  _meanpointer = 0;
  _lastValue = 0;
  _outputValue = 0;
#ifdef LDR_AUTOSCALE
  _min = 1023;
  _max = 0;
#else
  _min = LDR_MANUAL_MIN;
  _max = LDR_MANUAL_MAX;
#endif
}

/**
 * Welchen Wert hat der LDR?
 */
unsigned int LDR::value() {
  unsigned int val = analogRead(_pin);
  if(val != _lastValue) {
    _lastValue = val;
#ifdef LDR_AUTOSCALE
    if(val < _min) {
      _min = val;
    }
    if(val > _max) {
      _max = val;
    }
#else
    val = constrain(val, _min, _max); 
#endif
    unsigned int mapVal = map(val, _min, _max, 100, 0);
    Serial.print(" _min: ");
    Serial.print(_min);
    Serial.print(" _max: ");
    Serial.print(_max);
    Serial.print(" ldr: ");
    Serial.print(val);
    Serial.print(" mapValue: ");
    Serial.print(mapVal);
    
    // glaetten
    _meanvalues[_meanpointer] = mapVal;
    _meanpointer++;
    if(_meanpointer == LDR_MEAN_COUNT) {
      _meanpointer = 0;
    }
    long ret = 0;
    for(byte i=0; i<LDR_MEAN_COUNT; i++) {
      ret += _meanvalues[i];
    }
    _outputValue = (unsigned int)(ret/LDR_MEAN_COUNT);
  }
  return _outputValue;
}

