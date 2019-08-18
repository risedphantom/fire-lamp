void parseUDP() {
  int packetSize = Udp.parsePacket();
  if (!packetSize) return; 
    
  int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
  packetBuffer[n] = 0;
  inputBuffer = packetBuffer;

  if (inputBuffer.startsWith("GET")) {
    sendCurrent();
  } else if (inputBuffer.startsWith("EFF")) {
    saveModeBrightness(currentMode, brightness[currentMode]);
    currentMode = 1 - currentMode;
    FastLED.clear();
    delay(1);
    sendCurrent();
    FastLED.setBrightness(brightness[currentMode]);
  } else if (inputBuffer.startsWith("BRI")) {
    brightness[currentMode] = inputBuffer.substring(3).toInt();
    FastLED.setBrightness(brightness[currentMode]);
    settChanged = true;
    eepromTimer = millis();
  } else if (inputBuffer.startsWith("P_ON")) {
    ONflag = true;
    changePower();
    sendCurrent();
  } else if (inputBuffer.startsWith("P_OFF")) {
    ONflag = false;
    changePower();
    sendCurrent();
  }

  char reply[inputBuffer.length() + 1];
  inputBuffer.toCharArray(reply, inputBuffer.length() + 1);
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write(reply);
  Udp.endPacket();
}

void sendCurrent() {
  inputBuffer = "CURR";
  inputBuffer += " ";
  inputBuffer += String(currentMode);
  inputBuffer += " ";
  inputBuffer += String(brightness[currentMode]);
  inputBuffer += " ";
  inputBuffer += String(1);
  inputBuffer += " ";
  inputBuffer += String(1);
  inputBuffer += " ";
  inputBuffer += String(ONflag);
}