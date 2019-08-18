uint32_t effTimer;

void effectsTick() {
  if (ONflag && millis() - effTimer >= 1) {
    effTimer = millis();
    if (!currentMode) colorRoutine();
    else fireRoutine();
    FastLED.show();
  }
}

void changePower() {
  for (uint i = 0; i < brightness[currentMode]; i += 8) {
    FastLED.setBrightness(ONflag ? i : brightness[currentMode] - i);
    delay(1);
    FastLED.show();
  }

  ONflag ? FastLED.setBrightness(brightness[currentMode]) : FastLED.clear();
  delay(1);
  FastLED.show();
}
