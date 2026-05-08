
#define GET_COMMAND "GET?"

void get_serial_data() {
  Serial1.print(GET_COMMAND);

  int totalBytes = (NUM_STAT_STRINGS * STAT_STRING_LENGTH);
  uint8_t buffer[totalBytes];
  int received = 0;
  uint32_t start = millis();

  while (received < totalBytes && millis() - start < 1000) {
    if (Serial1.available()) {
      buffer[received++] = Serial1.read();
    }
  }

  if (received != totalBytes) {
    Serial.println("UART Timeout");
    for (int i = 0; i < NUM_STAT_STRINGS; i++) {
      stat_strings[i][0] = '\0';
    }
    return;
  }

  for (int i = 0; i < NUM_STAT_STRINGS; i++) {
    memcpy(stat_strings[i], &buffer[i * STAT_STRING_LENGTH], STAT_STRING_LENGTH);
    stat_strings[i][STAT_STRING_LENGTH] = '\0';
  }
}


#define JSON_RX_SIZE 200
#define PRINT_RX_STRING true

void json_to_const_char(StaticJsonDocument<JSON_RX_SIZE> &jsonDoc, const char *json_cmd, char *target_string, uint8_t target_length, const char *label) {
  const char *temp_buffer;
  if (jsonDoc.containsKey(json_cmd)) {
    temp_buffer = jsonDoc[json_cmd].as<const char *>();
    //  extract = jsonRXdoc["temp"].as<float>();
    memcpy(target_string, temp_buffer, target_length);
    Serial.print(label);
    Serial.println(target_string);
  } else {
    target_string[0] = '\0';
  }
}

void get_json_data() {
  Serial1.print(GET_COMMAND);
  uint8_t received = 0;
  uint32_t start = millis();
  StaticJsonDocument<JSON_RX_SIZE> jsonRXdoc;
  char jsonData[JSON_RX_SIZE];

  while (received == 0 && millis() - start < 1000) {

    if (Serial1.available() > 0) {

      received++;
      Serial1.readBytesUntil(10, jsonData, JSON_RX_SIZE);
      for (int i = 0; i < JSON_RX_SIZE; i++) {  // trim command down to remove redundent characters
        if (jsonData[i] == '}') {
          jsonData[i + 1] = '\0';
          break;
        }
      }
#if PRINT_RX_STRING == true
      Serial.print(F("\n{\"rxed\": \""));
      Serial.print(jsonData);
      Serial.println(F("\"}"));
#endif

      [[maybe_unused]] DeserializationError error = deserializeJson(jsonRXdoc, jsonData);  // more overhead but can print message before processing (good for debugging)

#if PRINT_DESERIAL_ERROR == true
      Serial.print("{\"deserialization\":\"");
      Serial.print(error.c_str());
      Serial.println(F("\"}"));
#endif

      //  JsonObject root = jsonRXdoc.as<JsonObject>();
    }
  }  
  // these need to be here so they update and clear out old data when the rpi fails to respond to a command
  json_to_const_char(jsonRXdoc, "temp", stat_strings[TEMP], STAT_STRING_LENGTH, "Temp: ");
  json_to_const_char(jsonRXdoc, "cpu", stat_strings[CPU], STAT_STRING_LENGTH, "CPU: ");
  json_to_const_char(jsonRXdoc, "mem", stat_strings[MEM], STAT_STRING_LENGTH, "Mem: ");
  json_to_const_char(jsonRXdoc, "disk", stat_strings[DISK], STAT_STRING_LENGTH, "Disk: ");
  json_to_const_char(jsonRXdoc, "uptime", stat_strings[UP], STAT_STRING_LENGTH, "Uptime: ");
  json_to_const_char(jsonRXdoc, "ip", stat_strings[IP], STAT_STRING_LENGTH, "IP: ");
  json_to_const_char(jsonRXdoc, "mac", stat_strings[MAC], STAT_STRING_LENGTH, "MAC: ");
}
