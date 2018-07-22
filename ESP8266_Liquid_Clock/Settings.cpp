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
  memcpy(mySettings.ntpServer, text, min(tmp,size));
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

// Set all defaults.
void Settings::resetToDefault() {
  Serial.println("*** Settings set to defaults in EEPROM. ***");
  mySettings.magicNumber = SETTINGS_MAGIC_NUMBER;
  mySettings.SettingVersion = SETTINGS_SettingVersion;
  mySettings.useLdr = DEFAULT_LDR_Status;
  char charntp[50] = DEFAULT_NTP_SERVER;
  int tmp = sizeof(mySettings.ntpServer);
  memcpy(mySettings.ntpServer, charntp, min(tmp,50));
  mySettings.ldrDot = DEFAULT_LDR_LED;
  mySettings.brightness = DEFAULT_LDR_Brightness;
  saveToEEPROM();
}

// Load settings from EEPROM.
void Settings::loadFromEEPROM() {
  Serial.println("Settings loaded from EEPROM.");
  EEPROM.begin(512);
  EEPROM.get(0, mySettings);
  if ((mySettings.magicNumber != SETTINGS_MAGIC_NUMBER) ||(mySettings.SettingVersion == SETTINGS_SettingVersion))
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
