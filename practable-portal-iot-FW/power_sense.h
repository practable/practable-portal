


void sample_power() {
  adj_V_array[samples_taken] = analogRead(ADJ_V_SENSE_PIN);
  adj_I_array[samples_taken] = analogRead(ADJ_I_SENSE_PIN);
  sbc_V_array[samples_taken] = analogRead(SBC_V_SENSE_PIN);
  sbc_I_array[samples_taken] = analogRead(SBC_I_SENSE_PIN);
}


float return_average(int16_t *data, uint8_t num_samples) {
  float average = 0;
  for (int i = 0; i < num_samples; i++) {
    average = average + float(data[i]);
  }
  return average / num_samples;
}

// calculates the power from arrays and stores in global char buffers for printing
void calculate_power() {
  float adj_V = return_average(adj_V_array, samples_taken);
  float adj_I = return_average(adj_I_array, samples_taken);
  float sbc_V = return_average(sbc_V_array, samples_taken);
  float sbc_I = return_average(sbc_I_array, samples_taken);
  samplingComplete = true;  // this does nothing lol

  adj_V = adj_V * VOLTAGE_SCALER * ADJ_VOLTAGE_MULTIPLIER;
  adj_I = (adj_I * VOLTAGE_SCALER);
  adj_I = ((adj_I - 1.5) * CURRENT_SCALER);  // calculation broken down to interrogate value as it is being calculated

  sbc_V = sbc_V * VOLTAGE_SCALER * SBC_VOLTAGE_MULTIPLIER;
  sbc_I = (sbc_I * VOLTAGE_SCALER);
  sbc_I = ((sbc_I - 1.5) * CURRENT_SCALER);

  float exp_W = adj_I * adj_V;
  float sbc_W = sbc_I * sbc_V;

  dtostrf(adj_V, 3, 2, adj_V_buf);
  dtostrf(adj_I, 3, 2, adj_I_buf);
  dtostrf(sbc_V, 3, 2, sbc_V_buf);
  dtostrf(sbc_I, 3, 2, sbc_I_buf);

  dtostrf(exp_W, 3, 2, exp_W_buf);
  dtostrf(sbc_W, 3, 2, sbc_W_buf);



}


// Loads buffers with trigger status bools (good place to save memory)
void print_trigger_status() {
  //strcpy(sbcTrig_buf, sbcTrig.buttonIsDown ? trueStr : falsStr);
  //strcpy(extTrig_buf, extTrig.buttonIsDown ? trueStr : falsStr);
  //strcpy(expTrig_buf, expTrig.buttonIsDown ? trueStr : falsStr);
  //strcpy(SBC_reboot, sbc_power_cycle_triggered ? trueStr : falsStr);
  //strcpy(EXP_reboot, exp_power_triggered ? trueStr : falsStr);
  char buffer[60];  // 56ish 260 was working before screen // 150 should be max width
  sprintf(buffer, " sbc: %2i, ext: %2i, exp: %2i, reboot: %2i, exper: %2i",
          sbcTrig.buttonIsDown, extTrig.buttonIsDown, expTrig.buttonIsDown,
          sbc_power_cycle_triggered, exp_power_triggered);
  Serial.print(buffer);
}


void print_serial_stats() {
  char buffer[80];  // 56ish 260 was working before screen // 150 should be max width
                    //  sprintf(buffer, "Vadj: %6s, Iadj: %6s, Vsbc: %6s, Isbc: %6s, sbcTrig: %6s, extTrig: %6s, expTrig: %6s, SBCrbt: %6s, EXPrbt: %6s, ",
                    //          adj_V_buf, adj_I_buf, sbc_V_buf, sbc_I_buf,
                    //          sbcTrig_buf, extTrig_buf, expTrig_buf,
                    //         SBC_reboot, EXP_reboot);
  sprintf(buffer, "Vexp: %5s, Iexp: %5s, Vsbc: %5s, Isbc: %5s", adj_V_buf, adj_I_buf, sbc_V_buf, sbc_I_buf);
  Serial.print(buffer);
}
