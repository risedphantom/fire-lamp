boolean brightDirection;

void buttonTick() {
  touch.tick();
  if (touch.isSingle()) {
    ONflag = !ONflag;
    Serial.printf("Got turn %s signal\n", ONflag ? "ON" : "OFF");
    changePower();
    return;
  }

  if (!ONflag) return;
  
  if (touch.isDouble()) {
    currentMode = 1 - currentMode;
    Serial.printf("Switching to %s mode\n", currentMode ? "FIRE" : "LIGHT");
    FastLED.setBrightness(brightness[currentMode]);
    settChanged = true;
    eepromTimer = millis();
    FastLED.clear();
    delay(1);
  } else if (touch.isHolded()) {
    Serial.printf("Got hold\n");
    brightDirection = !brightDirection;
  } else if (touch.isStep()) {
    if (brightDirection) {
      if (brightness[currentMode] < 10) brightness[currentMode] += 1;
      else if (brightness[currentMode] < 250) brightness[currentMode] += 5;
      else brightness[currentMode] = 255;
    } else {
      if (brightness[currentMode] > 15) brightness[currentMode] -= 5;
      else if (brightness[currentMode] > 1) brightness[currentMode] -= 1;
      else brightness[currentMode] = 1;
    }
    FastLED.setBrightness(brightness[currentMode]);
    settChanged = true;
    eepromTimer = millis();
  }
}
