#include <Arduino.h>
#include <ArduinoJson.h>

// ----- Fixed constraints -----
const float SAMPLE_RATE_HZ = 10.0f;
const uint32_t SAMPLE_PERIOD_MS = 100;

// Frequency constraint: 0.1 .. 1.0 Hz
// (Square wave only; duty cycle is adjustable via command)
float freqHz = 0.5f;          // must remain within 0.1..1.0
float dutyCycle = 0.5f;       // 0..1

// ----- Timing -----
uint32_t startMs = 0;
uint32_t nextSampleMs = 0;

// ----- Serial line input (newline-delimited JSON) -----
static char lineBuf[96];
size_t lineLen = 0;

float clampf(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

void handleCommand(const char* jsonLine) {
  StaticJsonDocument<96> doc;
  DeserializationError err = deserializeJson(doc, jsonLine);
  if (err) return; // ignore malformed input (no extra reports)

  const char* setKey = doc["set"];
  if (!setKey) return;

  // Only supported command: {"set":"duty_cycle","to":0.5}
  if (strcmp(setKey, "duty_cycle") != 0) return;
  if (!doc.containsKey("to")) return;

  float toVal = doc["to"].as<float>();
  dutyCycle = clampf(toVal, 0.0f, 1.0f);
}

int synthSquare(float tSec) {
  // phase in [0,1)
  float phase = fmod(freqHz * tSec, 1.0f);
  if (phase < 0.0f) phase += 1.0f;

  // amplitude 0 or 1
  return (phase < dutyCycle) ? 1 : 0;
}

void setup() {
  Serial.begin(115200);
  delay(50);

  // Ensure freq stays within required range
  freqHz = clampf(freqHz, 0.1f, 1.0f);

  startMs = millis();
  nextSampleMs = startMs;
}

void loop() {
  // --- Read commands (newline-delimited JSON) ---
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\r') continue;

    if (c == '\n') {
      lineBuf[lineLen] = '\0';
      if (lineLen > 0) handleCommand(lineBuf);
      lineLen = 0;
    } else {
      if (lineLen < sizeof(lineBuf) - 1) {
        lineBuf[lineLen++] = c;
      } else {
        // line too long: drop it
        lineLen = 0;
      }
    }
  }

  // --- Emit samples at 20 Hz ---
  uint32_t now = millis();
  if ((int32_t)(now - nextSampleMs) >= 0) {
    nextSampleMs += SAMPLE_PERIOD_MS;

    float tSec = (now - startMs) / 1000.0f;
    int out = synthSquare(tSec);

    StaticJsonDocument<64> report;
    report["timestamp"] = (uint32_t)(now - startMs);
    report["output"] = out;
    serializeJson(report, Serial);
    Serial.println();
  }
}
