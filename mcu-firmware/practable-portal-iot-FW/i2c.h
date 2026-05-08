




void i2c_scan() {
  //  Wire.begin();
  //Serial.begin(115200);

  Serial.println(F("Scanning..."));
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print(F("Found: 0x"));
      Serial.println(addr, HEX);
    }
  }
}

void unstick_bus() {
  for (int i = 0; i < 9; i++) {
    digitalWrite(SCL_PIN, HIGH);
    delay(10);
    digitalWrite(SCL_PIN, LOW);
    delay(10);
  }
  digitalWrite(SCL_PIN, HIGH);
}


void load_data() {
  for (int i = TEMP; i <= MAC; i++) {
    strcpy(stat_strings[i], sample_strings[i]);
    Serial.println(stat_strings[i]);
  }
}
/*

void requestData() {  // note try this command on raspberry pi to get data in hex `i2cget -y 1 0x08 0x00 i` then decode hex as UTF-16 big endian
  Wire.requestFrom((uint8_t)I2C_ADDRESS, sizeof(stat_strings));

  char single_line[30];
  if (Wire.available() == sizeof(stat_strings)) {
    i2c_attempts = 0;
    Wire.readBytes((char*)&single_line, sizeof(single_line));
    Serial.println(single_line);
    //if (packet.header == PACKET_HEADER) {
    //  data = packet.data;
    // return data;
  }  //else {
     // Serial.println("warning Bad i2c header");
     /// }

  else {
    i2c_attempts++;
    Serial.println("warning Wire Unavailable");
    Serial.print("Attempts: ");
    Serial.println(i2c_attempts);
  }
  //ram.getPrintStats("returnSenseData");
  //  return sensorData;  // return the global with data unchanged
}

*/