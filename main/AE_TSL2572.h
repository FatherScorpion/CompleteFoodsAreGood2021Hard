//---------------------------------------------------//
//  AKIZUKI DENSHI TSUSHO CO.,LTD.
//  Version 0.1  2020-07-18
//---------------------------------------------------//

#include "Arduino.h"
#include <Wire.h>

class AE_TSL2572 {
  public:
    AE_TSL2572();
    void Reset();
    void SetGain(uint8_t gain_step);
    void SetGainAuto();
    void SetLimit();
    void SetIntegralTime(uint8_t atime_cnt);
    bool CheckID();
    float GetLux();
    uint16_t GetLux16();
    uint8_t GetGainStep();
    uint8_t GetIntegrationTime();

  private:
    void ReadAdc();
    uint8_t ReadReg(uint8_t reg);
    void WriteReg(uint8_t reg, uint8_t dat);
    void WriteIntegralTime();
    void WriteGain();
    float CalcLux();

    //used to calculate Lux
    uint16_t adc0, adc1;
    uint8_t atime;
    float again;

    //used to settings
    uint8_t atime_cnt, gain_step;

    //used to autogain
    uint16_t limit_lower,limit_upper;
    float coefficient_lower = 0.05;
    float coefficient_upper = 0.75;

    //TSL2572 Register Set;
    const uint8_t TSL2572_ADR      = 0x39;
    const uint8_t TSL2572_COMMAND  = 0x80;
    const uint8_t TSL2572_TYPE_REP = 0x00;
    const uint8_t TSL2572_TYPE_INC = 0x20;
    const uint8_t TSL2572_ALSIFC   = 0x66;

    const uint8_t TSL2572_SAI   = 0x40;
    const uint8_t TSL2572_AIEN  = 0x10;
    const uint8_t TSL2572_WEN   = 0x80;
    const uint8_t TSL2572_AEN   = 0x02;
    const uint8_t TSL2572_PON   = 0x01;

    const uint8_t TSL2572_ENABLE   = 0x00;
    const uint8_t TSL2572_ATIME    = 0x01;
    const uint8_t TSL2572_WTIME    = 0x03;
    const uint8_t TSL2572_AILTL    = 0x04;
    const uint8_t TSL2572_AILTH    = 0x05;
    const uint8_t TSL2572_AIHTL    = 0x06;
    const uint8_t TSL2572_AIHTH    = 0x07;
    const uint8_t TSL2572_PRES     = 0x0C;
    const uint8_t TSL2572_CONFIG   = 0x0D;
    const uint8_t TSL2572_CONTROL  = 0x0F;
    const uint8_t TSL2572_ID       = 0x12;
    const uint8_t TSL2572_STATUS   = 0x13;
    const uint8_t TSL2572_C0DATA   = 0x14;
    const uint8_t TSL2572_C0DATAH  = 0x15;
    const uint8_t TSL2572_C1DATA   = 0x16;
    const uint8_t TSL2572_C1DATAH  = 0x17;
};

