#pragma once
#include <FlexCAN.h>
#include "config.h"   // CLMod: CSC_MAX_CELLS / CELLS_PER_MODULE

class BMSModule
{
  public:
    BMSModule();
    void decodecan(int Id, CAN_message_t &msg, bool Ign);
    void decodetemp(CAN_message_t &msg, int CSC);
    void clearmodule();
    void readStatus();
    int getscells();
     int getbalstat();
    bool readModuleValues();
    float getCellVoltage(int cell);
    float getLowCellV();
    float getHighCellV();
    float getAverageV();
    float getLowTemp();
    float getHighTemp();
    float getHighestModuleVolt();
    float getLowestModuleVolt();
    float getHighestCellVolt(int cell);
    float getLowestCellVolt(int cell);
    float getHighestTemp();
    float getLowestTemp();
    float getAvgTemp();
    float getModuleVoltage();
    float getTemperature(int temp);
    uint8_t getFaults();
    uint8_t getAlerts();
    uint8_t getCOVCells();
    uint8_t getCUVCells();
     uint32_t getError();
    void setAddress(int newAddr);
    int getAddress();
    bool isExisting();
    void setExists(bool ex);
    bool isReset();
    void setReset(bool ex);
    void settempsensor(int tempsensor);
    void setIgnoreCell(float Ignore);
    void setTempOff( int16_t tempoff);


  private:
    // CLMod: storage stays 16 because decodecan() writes up to index 15 (CSC frames 0x14x-0x17x).
    // Only the first CELLS_PER_MODULE entries are used for limits, sums and printing.
    float cellVolt[CSC_MAX_CELLS];          // calculated as 16 bit value * 6.250 / 16383 = volts
    float lowestCellVolt[CSC_MAX_CELLS];
    float highestCellVolt[CSC_MAX_CELLS];
    float moduleVolt;          // calculated as 16 bit value * 33.333 / 16383 = volts
    float temperatures[4];     // Don't know the proper scaling at this point
    float lowestTemperature;
    float highestTemperature;
    float lowestModuleVolt;
    float highestModuleVolt;
    float IgnoreCell;
    bool exists;
    bool reset;
    int alerts;
    int faults;
    int COVFaults;
    int CUVFaults;
    int sensor;
    uint8_t moduleAddress;     //1 to 0x3E
    int scells;
    int balstat;
    uint32_t error;
    int variant;
    int16_t TempOff;
};
