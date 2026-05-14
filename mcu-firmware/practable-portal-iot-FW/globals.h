



// External Libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

#include <Fonts/FreeMonoBold12pt7b.h >
#include <Fonts/FreeSansBold12pt7b.h >
#include <Fonts/FreeSansBold9pt7b.h >

#include <avr/dtostrf.h>  // needed for SAMD21 only

#include <buttonObject.h>
#include <autoDelay.h>


// Configuration
#define ENABLE_LOCAL_OLED true  // Not implemented yet, when true Arduino controls OLED screen, when false, RPi controls OLED Screen
#define SHOW_SPASH_SCREEN true

// Definitions
//Logic Level Shifter
#define OUTPUT_ENABLE 7
// Voltage/Current sensing pins
#define ADJ_V_SENSE_PIN A0
#define ADJ_I_SENSE_PIN A1
#define SBC_V_SENSE_PIN A6
#define SBC_I_SENSE_PIN A7
// Power Control Inputs
#define SELF_SBC_TRIGGER 4
#define SELF_EXP_TRIGGER 5
#define EXTERN_SBC_TRIGGER 6
// Power Control Outputs
#define SBC_POWER_CONTROL 8
#define EXP_POWER_CONTROL 9
// i2c Bus
#define SDA_PIN A4
#define SCL_PIN A5


// ADC Consts
#define CURRENT_ADC_OFFSET 0  //-323
#define CURRENT_I_OFFSET -5

// Timing Consts
#define TRIGGER_DETECT_TIME_mS 3000
#define PRE_POWER_DELAY_S 30  // time after trigger to keep power active & allow time to shut down safely
#define SHUT_DOWN_TIME_S 20   // time after shutdown starts to keep power low


// Global Objects
buttonObject sbcTrig(SELF_SBC_TRIGGER, BUTTON_PULL_HIGH);
buttonObject extTrig(EXTERN_SBC_TRIGGER, BUTTON_PULL_HIGH);
buttonObject expTrig(SELF_EXP_TRIGGER, BUTTON_PULL_HIGH);
autoDelay requestDelay;
autoDelay screenRefresh;


// Consts
//const char SYSTEM_NAME = {"Practable Power Hat V1.0"}
const uint8_t I2C_ADDRESS = 0x08;  // Address of RPi i2c
uint16_t mS_to_S = 1000;

// Sensor Scaling Consts
const float VOLTAGE_SCALER = 3.2227E-3;     //<- value for IoT  value for nano -> 4.8838E-3;     // scales to actual measured voltage after voltage division
const float SBC_VOLTAGE_MULTIPLIER = 1.69;  //  scales to voltage before voltage division
const float ADJ_VOLTAGE_MULTIPLIER = 7.725;
// whole range of current sensor is -5 to +5A, output is 3.3v
const float CURRENT_SCALER = 2.425;  //2.75;  //2.09  //3.03 //1.52 //30//1.52  //1.52  // I = V*current_scaler
const uint8_t samples_required = 6;  // 6 working, 8 failed

// Vars
uint32_t sbc_power_cycle_timer_mS;
uint32_t exp_power_cycle_timer_mS;
bool sbc_power_cycle_triggered = false;
uint8_t sbc_power_cycle_state = 0;
bool exp_power_triggered = false;
uint8_t exp_state = 0;
uint32_t lastSample_mS;
bool power_disabled = true;
//uint16_t lastSample;

uint8_t samples_taken = 0;
bool samplingComplete = false;

// Arrays for sampled ADC data
int16_t adj_V_array[samples_required];
int16_t adj_I_array[samples_required];
int16_t sbc_V_array[samples_required];
int16_t sbc_I_array[samples_required];



// Buffers to hold extracted and averaged values, ready for printing & display
char adj_V_buf[6];
char adj_I_buf[6];
char exp_W_buf[6];
char sbc_V_buf[6];
char sbc_I_buf[6];
char sbc_W_buf[6];


//MAC:   DC:A6:32:A2:68:D4

typedef enum {
  TEMP,
  CPU,
  MEM,
  DISK,
  UP,
  IP,
  MAC,
  PING
} statsIDX_t;

#define NUM_STAT_STRINGS 8
#define STAT_STRING_LENGTH 20

char temp_stats[20];
char CPU_stats[20];
char mem_stats[20];
char disk_stats[20];
char up_stats[20];
char ip_stats[20];
char mac_stats[20];
char ping_stats[20];

char *stat_strings[] = { temp_stats, CPU_stats, mem_stats, disk_stats, up_stats, ip_stats, mac_stats, ping_stats };

char temp_name[12] = {"Temp:   "};
char cpu_name[12]  = {" CPU:   "};
char mem_name[12]  = {" Mem:   "};
char disk_name[12] = {"Disk:   "};
char up_name[12]   = {"Runtime: "};
char IP_name[9]   = {"IP: "};
char mac_name[9]  = {"MAC:"};
char ping_name[9] = {"Ping: "};

char *stat_names[] = {temp_name, cpu_name, mem_name, disk_name, up_name, IP_name, mac_name, ping_name};

char temp_unit[6] = {" degC"};
char cpu_unit[6] = {" %"};
char mem_unit[6] = {" %"};
char disk_unit[6] = {" %"};
char up_unit[6] = {""};
char IP_unit[6] = {""};
char mac_unit[6] = {""};
char ping_unit[6] = {""};

char *stat_units[] = {temp_unit, cpu_unit, mem_unit, disk_unit, up_unit, IP_unit, mac_unit, ping_unit};

char sample_temp[20] = { "44.8 degC" };
char sample_cpu[20] = { "0.1 %" };
char sample_mem[20] = { "7.8 %" };
char sample_disk[20] = { "22.3911 %" };
char sample_uptime[20] = { "1:52:26" };
char sample_ip[20] = { "10.42.0.20" };
char sample_mac[20] = { "DC:A6:32:A2:68:D4" };
char sample_ping[20] = {"2"};

char *sample_strings[] = { sample_temp, sample_cpu, sample_mem, sample_disk, sample_uptime, sample_ip, sample_mac, sample_ping };

uint8_t i2c_attempts = 0;

typedef enum {
  POWER,
  NETWORK,
  STATS
} screenState_t;

screenState_t currentScreen = POWER;

uint32_t screen_change_mS = 0;

#define POWER_DISP_TIME_mS 8000
#define NETWORK_DISP_TIME_mS 6000
#define STATS_DISP_TIME_mS 4000

// Internal Headers
#include "oled.h"
#include "state_machines.h"
#include "power_sense.h"
#include "i2c.h"
#include "UART.h"