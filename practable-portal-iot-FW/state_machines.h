



void sbc_state_machine() {
  if (sbc_power_cycle_state == 1) {  // Shutdown command received, waiting for SBC shutdown
    sbc_power_cycle_timer_mS = millis();
    sbc_power_cycle_state = 2;
  }

  if (sbc_power_cycle_state == 2) {
    if (millis() - sbc_power_cycle_timer_mS >= PRE_POWER_DELAY_S * mS_to_S) {  // Power to SBC off
      digitalWrite(SBC_POWER_CONTROL, true);
      sbc_power_cycle_timer_mS = millis();
      sbc_power_cycle_state = 3;
    }
  }

  if (sbc_power_cycle_state == 3) {
    if (millis() - sbc_power_cycle_timer_mS >= SHUT_DOWN_TIME_S * mS_to_S) {  // Power to SBC on
      digitalWrite(SBC_POWER_CONTROL, false);
      sbc_power_cycle_timer_mS = millis();
      sbc_power_cycle_triggered = false;
      sbc_power_cycle_state = 4;  // used just to send flag to OLED
    }
  }
  if (sbc_power_cycle_state == 4) {
    if (millis() - sbc_power_cycle_timer_mS >= 10000) {
      sbc_power_cycle_state = 0;          // reset back to default
    }
  }
}

void exp_state_machine() {
  if (expTrig.longPress) {
    exp_power_triggered = true;
    if (exp_state == 0) {
      exp_power_cycle_timer_mS = millis();
      exp_state = 1;
    }
    if (expTrig.buttonIsDown) {
      digitalWrite(EXP_POWER_CONTROL, true);
    } else {
      if (millis() - exp_power_cycle_timer_mS >= SHUT_DOWN_TIME_S * mS_to_S) {
        digitalWrite(EXP_POWER_CONTROL, false);
        exp_power_triggered = false;
        exp_state = 0;
        expTrig.buttonReset();
      }
    }
  }
}
