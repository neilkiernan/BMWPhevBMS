#pragma once

#include <Arduino.h>

//Set to the proper port for your USB connection - SerialUSB on Due (Native) or Serial for Due (Programming) or Teensy
#define SERIALCONSOLE   Serial

//Define this to be the serial port the Tesla BMS modules are connected to.
//On the Due you need to use a USART port (Serial1, Serial2, Serial3) and update the call to serialSpecialInit if not Serial1
//Serial3 for teensy
#define SERIALBMS  Serial3

// victron serial VE direct bus config
#define VE Serial2

#define REG_DEV_STATUS      1
#define REG_GPAI            1
#define REG_VCELL1          3
#define REG_VCELL2          5
#define REG_VCELL3          7
#define REG_VCELL4          9
#define REG_VCELL5          0xB
#define REG_VCELL6          0xD
#define REG_TEMPERATURE1    0xF
#define REG_TEMPERATURE2    0x11
#define REG_ALERT_STATUS    0x20
#define REG_FAULT_STATUS    0x21
#define REG_COV_FAULT       0x22
#define REG_CUV_FAULT       0x23
#define REG_ADC_CTRL        0x30
#define REG_IO_CTRL         0x31
#define REG_BAL_CTRL        0x32
#define REG_BAL_TIME        0x33
#define REG_ADC_CONV        0x34
#define REG_ADDR_CTRL       0x3B

#define MAX_MODULE_ADDR     0x3E

// CLMod: BMW X5 45e SP44 pack - 12 CSCs x 8 cells = 96s
#define CSC_MAX_CELLS       16      // cells the CSC CAN frames can carry (array storage size)
#define CELLS_PER_MODULE    8       // cells actually fitted per SP44 module
#define NUM_CSC             12      // CSCs in the pack (CAN IDs 0x10x-0x17x, x = 0..NUM_CSC-1)

// CLMod: balancing duty cycle (rest phase lets cell readings update between bleed phases)
#define BAL_REST_MS         10000   // ms with bleed off between balance phases
#define BAL_SETTLE_MS       3000    // ms after bleed stops before cell readings are trusted again
#define BAL_STOP_DELTA      0.010f  // V: stop balancing once high-low spread is below this
#define BAL_DURING_CHARGE   0       // 1 = also balance in Charge state (cell readings freeze during each bleed phase)
#define BAL_ONLY_CSC        -1      // bench test: -1 = all CSCs; 0..11 = send the bleed command to that CSC only (0 = module 1)

#define EEPROM_VERSION      0x11    //update any time EEPROM struct below is changed.
#define EEPROM_PAGE         0

typedef struct {
  uint8_t version;
  uint8_t checksum;
  uint32_t canSpeed;
  uint8_t batteryID;  //which battery ID should this board associate as on the CAN bus
  uint8_t logLevel;
  float OverVSetpoint;
  float UnderVSetpoint;
  float DischHys;
  float ChargeVsetpoint;
  float DischVsetpoint;
  float ChargeHys;
  float StoreVsetpoint;
  float WarnOff;
  float OverTSetpoint;
  float UnderTSetpoint;
  uint16_t triptime;
  float ChargeTSetpoint;
  float DisTSetpoint;
  float WarnToff;
  float CellGap;
  uint8_t IgnoreTemp;
  float IgnoreVolt;
  float balanceVoltage;
  float balanceHyst;
  int Scells;
  int Pstrings;
  int CAP;
  int chargecurrentmax;
  uint16_t  chargecurrent2max;
  int chargecurrentend;
  int discurrentmax ;
  int socvolt[4];
  int invertcur;
  int cursens;
  int curcan;
  int voltsoc;
  int Pretime;
  int conthold;
  int Precurrent;
  float convhigh;
  float convlow;
    int32_t changecur;
  uint16_t offset1;
  uint16_t offset2;
  uint16_t balanceDuty;
  int ESSmode;
  int gaugelow;
  int gaugehigh;
  int ncur;
  int chargertype;
  int chargerspd;
  uint16_t UnderDur;
  uint16_t CurDead;
  float DisTaper;
  bool ChargerDirect;
  uint8_t tripcont;
  uint8_t CSCvariant;
  int16_t TempOff;
  int chargereff;
  int chargerACv;
} EEPROMSettings;
