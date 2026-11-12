/* Practable Power Hat Firmware -> with OLED



Example functions for printing bitmap images to SSD1306 OLED screen
// Converted using https://mischianti.org/images-to-byte-array-online-converter-cpp-arduino/
*/

/* Changelog
V0.0.2 (in dev) -> OLED allocating correctly
Sketch uses 18854 bytes (61%) of program storage space. Maximum is 30720 bytes.
Global variables use 623 bytes (30%) of dynamic memory, leaving 1425 bytes for local variables. Maximum is 2048 bytes.

V1.0.1
-> added versioning to screen

*/



#include "globals.h"



void setup() {
  // Setup Serial
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(2500);
  Serial.print(F("\nPractable Portal\n"));
  Serial.print("FW: ");
  Serial.println(FIRMWARE_VERSION);
  Wire.end();

  // i2c_scan();
  // Setup IO Pins
  // pinMode(SELF_SBC_TRIGGER, INPUT);
  // pinMode(SELF_EXP_TRIGGER, INPUT);
  //pinMode(EXTERN_SBC_TRIGGER, INPUT);
  pinMode(SBC_POWER_CONTROL, OUTPUT);
  pinMode(EXP_POWER_CONTROL, OUTPUT);
  pinMode(OUTPUT_ENABLE, OUTPUT);
  // Write initial state to output pins
  digitalWrite(SELF_SBC_TRIGGER, false);
  digitalWrite(SELF_EXP_TRIGGER, false);
  digitalWrite(EXTERN_SBC_TRIGGER, false);

  digitalWrite(SBC_POWER_CONTROL, false);
  digitalWrite(EXP_POWER_CONTROL, false);
  digitalWrite(OUTPUT_ENABLE, true);  // ensure that level shift IC is active
  delay(1000);


  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, OUTPUT);
  Serial.println(F("Unsticking Bus"));
  unstick_bus();
  delay(100);
  Serial.println(F("Wire Begin"));
  Wire.begin();
  Wire.setClock(100000UL);
  Wire.setTimeout(50);




  Serial.println("Starting OLED");
  oled_begin();
  show_splash();
  // Set up Triggers as button Objects
  sbcTrig.begin();
  extTrig.begin();
  expTrig.begin();


  bool do_cal = check_cal_mode();

  if (do_cal) {
    run_calibration();
  }

  // Serial.println(F("Loading Dummy Data"));
  // load_data();  // just for testing screen layout


  //Wire.onRequest(requestEvent);


  screen_change_mS = millis();
  Serial.println(F("end of setup"));
}



void loop() {

  // Sample Trigger inputs
  sbcTrig.buttonLoop(TRIGGER_DETECT_TIME_mS);
  extTrig.buttonLoop(TRIGGER_DETECT_TIME_mS);
  expTrig.buttonLoop(TRIGGER_DETECT_TIME_mS);
  // SBC power cycle trigger
  if (sbcTrig.longPress || extTrig.longPress) {
    sbc_power_cycle_triggered = true;  // only used for reporting
    sbc_power_cycle_state = 1;
    sbcTrig.buttonReset();
    extTrig.buttonReset();
  }
  // Power Control State Machines
  sbc_state_machine();
  exp_state_machine();


  // Oled Rpi Stats Screen
  if (requestDelay.millisDelay(4000)) {
    // requestData();
    //get_serial_data();
    request_json_data();
  }

  // Voltage/Current Data Collection & Reporting
  if (millis() - lastSample_mS >= 200) {
    lastSample_mS = millis();
    sample_power();
    samples_taken++;
  }
  if (samples_taken >= samples_required) {
    //samplingComplete = true;  // this does nothing lol
    calculate_power();
    print_serial_stats();
    print_trigger_status();
    Serial.println();
    samples_taken = 0;
  }
  if (screenRefresh.millisDelay(200)) {
    update_oled();
  }
}


bool check_cal_mode() {
  uint32_t timer = 0;
  if (digitalRead(EXTERN_SBC_TRIGGER)) {
    while (timer < 2000) {
      if (!digitalRead(EXTERN_SBC_TRIGGER)) {
        Serial.println("No Calibnration");
        return 0;
      } else {
        timer++;
        Serial.println(timer);
      }
    }
    Serial.println("Calibration Triggered");
    return 1;
  } else {
    return 0;
  }
}

void run_calibration() {
  Serial.println("Doing Calibration...");
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  display.print(F("Self Calibration Mode\n\nPlease remove jumper"));
  display.display();
  while (digitalRead(EXTERN_SBC_TRIGGER)) {
    delay(10);
  }
  display.clearDisplay();
  display.setCursor(0, 20);
  display.print(F("Calibrating..."));
  display.display();
 // delay(10000);
  // do calibration instead of display
  display.clearDisplay();
  display.setCursor(0, 20);
  display.print(F("Calibration Complete"));
  display.display();
  delay(1500);
}


void do_calibration(){
  bool calibration_complete = false;
   sample_power();  // throw away first readings
  while(!calibration_complete){
    sample_power();  // use second readings
  float baseline_I_val =  calculate_power();
  vexp_current_offset = (baseline_I_val*-1.0);
  calibration_complete = true;
  }
}
