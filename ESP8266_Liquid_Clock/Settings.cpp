/******************************************************************************
  Settings.cpp
 ******************************************************************************/

#include "Settings.h"

Settings::Settings() {
  loadFromEEPROM();
}


boolean Settings::getUseLdr() {
  return mySettings.useLdr;
}

void Settings::setUseLdr(bool set) {
  mySettings.useLdr = set;
}

void Settings::toggleUseLdr() {
  mySettings.useLdr = !mySettings.useLdr;
}

void Settings::setntpServer(char text[], int size) {
  int tmp = sizeof(mySettings.ntpServer);
  memcpy(mySettings.ntpServer, text, min(tmp, size));
}

char* Settings::getntpServer(char* ntpServer, int size) {
  memset(ntpServer, 0, size);
  memcpy(ntpServer, mySettings.ntpServer, sizeof(mySettings.ntpServer));
  return ntpServer;

}
int Settings::getSettingVersion() {
  return mySettings.SettingVersion;
}

int16_t Settings::getldrDot() {
  return mySettings.ldrDot;
}

void Settings::setldrDot(int16_t ldrDot) {
  mySettings.ldrDot = ldrDot;
}


int16_t Settings::getBrightness() {
  return mySettings.brightness;
}

void Settings::setBrightness(int16_t ledBrightness) {
  mySettings.brightness = ledBrightness;
}


int16_t Settings::getColSec() {
  return mySettings.ColSec;
}

void Settings::setColSec(int16_t ColSec) {
  mySettings.ColSec = ColSec;
}

int16_t Settings::getColMin() {
  return mySettings.ColMin;
}

void Settings::setColMin(int16_t ColMin) {
  mySettings.ColMin = ColMin;
}

int16_t Settings::getColHou() {
  return mySettings.ColHou;
}

void Settings::setColHou(int16_t ColHou) {
  mySettings.ColHou = ColHou;
}

int16_t Settings::getColHel() {
  return mySettings.ColHel;
}

void Settings::setColHel(int16_t ColHel) {
  mySettings.ColHel = ColHel;
}
boolean Settings::getUpdateStable() {
  return mySettings.UpdateStable;
}
void Settings::setUpdateStable(bool set) {
  mySettings.UpdateStable = set;
}



// Set all defaults.
void Settings::resetToDefault() {
  Serial.println("*** Settings set to defaults in EEPROM. ***");

  mySettings.magicNumber = SETTINGS_MAGIC_NUMBER;
  mySettings.SettingVersion = SETTINGS_SettingVersion;
  mySettings.useLdr = DEFAULT_LDR_Status;
  char charntp[50] = DEFAULT_NTP_SERVER;
  int tmp = sizeof(mySettings.ntpServer);
  memcpy(mySettings.ntpServer, charntp, min(tmp, 50));
  mySettings.ldrDot = DEFAULT_LDR_LED;
  mySettings.brightness = DEFAULT_LDR_Brightness;
  mySettings.ColSec = DEFAULT_Second;
  mySettings.ColMin = DEFAULT_Minute;
  mySettings.ColHou = DEFAULT_Hour;
  mySettings.ColHel = DEFAULT_Help_Dot;
  mySettings.UpdateStable = UPDATE_Stable;
  saveToEEPROM();
}

// Load settings from EEPROM.
void Settings::loadFromEEPROM() {
  Serial.println("Settings loaded from EEPROM.");
  EEPROM.begin(512);
  EEPROM.get(0, mySettings);
  if ((mySettings.magicNumber != SETTINGS_MAGIC_NUMBER) || (mySettings.SettingVersion == SETTINGS_SettingVersion))
    resetToDefault();
  EEPROM.end();
}

// Write settings to EEPROM.
void Settings::saveToEEPROM() {
  mySettings.SettingVersion = mySettings.SettingVersion + 1;
  Serial.println("Settings saved to EEPROM.");
  EEPROM.begin(512);
  EEPROM.put(0, mySettings);
  //EEPROM.commit();
  EEPROM.end();
}
