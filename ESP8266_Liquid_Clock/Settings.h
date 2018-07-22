/******************************************************************************
 Settings.h
 ******************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <EEPROM.h>
#include "Configuration.h"

#define SETTINGS_MAGIC_NUMBER 0x01
#define SETTINGS_SettingVersion 0

#define LEN_LOC_STR 100


class Settings {
public:
  Settings();

 

  boolean getUseLdr();
  void setUseLdr(bool set);
  void toggleUseLdr();

  void setntpServer(char text[], int size);
  char* getntpServer(char* locationText, int size);
  
  void saveToEEPROM();
  void resetToDefault();

  int getSettingVersion();

  int16_t getldrDot();
  void setldrDot(int16_t ldrDot);

    int16_t getBrightness();
  void setBrightness(int16_t brightness);

private:
  struct MySettings {
    uint8_t magicNumber;
    uint8_t SettingVersion;
    boolean useLdr;
    char ntpServer[LEN_LOC_STR];
    int16_t ldrDot;
    int16_t brightness;
  } mySettings;

  void loadFromEEPROM();
};

#endif
