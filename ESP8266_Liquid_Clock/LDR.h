/**
 * LDR.h
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
 * V 1.3:  - Beschraenkung der LDR-Werte bei autoscale == false.
 * V 1.4:  - Der LDR mapped die Werte jetzt selbst, dadurch wird flackern bei unguenstigen Lichtverhaeltnissen vermindert.
 * V 1.5:  - Der LDR gibt Werte zwischen 0 und 100% zurueck, das ist besser verstaendlich.
 */
#ifndef LDR_H
#define LDR_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Configuration.h"

class LDR {
public:
  LDR(byte pin);

  unsigned int value();
  
private:
  byte _pin;
  unsigned int _lastValue;
  unsigned int _outputValue;
  unsigned int _meanvalues[LDR_MEAN_COUNT];
  byte _meanpointer;
  unsigned int _min;
  unsigned int _max;
};

#endif

