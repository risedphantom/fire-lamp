#define STORAGE_SIZE 202
#define MODE_ADDRESS 200
#define FIRST_RUN_ADDRESS 198
#define NOT_FIRST_RUN_VAL 20
#define MODE_SETTINGS_ADDRESS 40

void initStorage() {
  EEPROM.begin(STORAGE_SIZE);
}

void saveModeBrightness(byte mode, byte brightness) {
  EEPROM.put(MODE_SETTINGS_ADDRESS + mode, brightness);
  EEPROM.commit();
}

byte readModeBrightness(byte mode) {
  return EEPROM.read(MODE_SETTINGS_ADDRESS + mode);
}

boolean isFirstRun() {
  return EEPROM.read(FIRST_RUN_ADDRESS) != NOT_FIRST_RUN_VAL;
}

void saveFirstRun() {
  EEPROM.write(FIRST_RUN_ADDRESS, NOT_FIRST_RUN_VAL);
  EEPROM.commit();
 
  saveModeBrightness(0, BRIGHTNESS);
  saveModeBrightness(1, BRIGHTNESS);
  saveCurrentMode(0);
}

void saveCurrentMode(byte mode) {
  EEPROM.write(MODE_ADDRESS, mode);
  EEPROM.commit();
}

byte readCurrentMode() {
  return EEPROM.read(MODE_ADDRESS);
}

void eepromTick() {
  if (settChanged && millis() - eepromTimer > 30000) {
    settChanged = false;
    eepromTimer = millis();
    saveModeBrightness(currentMode, brightness[currentMode]);
    saveCurrentMode(currentMode);
  }
}