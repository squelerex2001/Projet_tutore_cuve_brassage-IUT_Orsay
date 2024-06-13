#ifndef MBED_MAX31865_H
#define MBED_MAX31865_H

#include "mbed.h"

#define MAX31856_CONFIG_REG            0x00
#define MAX31856_CONFIG_BIAS           0x80
#define MAX31856_CONFIG_MODEAUTO       0x40
#define MAX31856_CONFIG_MODEOFF        0x00
#define MAX31856_CONFIG_1SHOT          0x20
#define MAX31856_CONFIG_3WIRE          0x10
#define MAX31856_CONFIG_24WIRE         0x00
#define MAX31856_CONFIG_FAULTSTAT      0x02
#define MAX31856_CONFIG_FILT50HZ       0x01
#define MAX31856_CONFIG_FILT60HZ       0x00

#define MAX31856_RTDMSB_REG           0x01
#define MAX31856_RTDLSB_REG           0x02
#define MAX31856_HFAULTMSB_REG        0x03
#define MAX31856_HFAULTLSB_REG        0x04
#define MAX31856_LFAULTMSB_REG        0x05
#define MAX31856_LFAULTLSB_REG        0x06
#define MAX31856_FAULTSTAT_REG        0x07


#define MAX31865_FAULT_HIGHTHRESH     0x80
#define MAX31865_FAULT_LOWTHRESH      0x40
#define MAX31865_FAULT_REFINLOW       0x20
#define MAX31865_FAULT_REFINHIGH      0x10
#define MAX31865_FAULT_RTDINLOW       0x08
#define MAX31865_FAULT_OVUV           0x04

typedef enum max31865_numwires { 
  MAX31865_2WIRE = 0,
  MAX31865_3WIRE = 1,
  MAX31865_4WIRE = 0
} max31865_numwires_t;

class max31865 {
    public:
    
    max31865(PinName MOSI, PinName MISO, PinName SCLK, PinName CS);
    
    void Begin(max31865_numwires_t x = MAX31865_2WIRE);
    int ReadFault();
    void ClearFault();
    int ReadRTD();
    
    void SetWires(max31865_numwires_t wires);
    void AutoConvert(bool b);
    void EnableBias(bool b);
    
    private:
    SPI spi;
    DigitalOut cs;
    
    void ReadRegistorN(int address, int buffer[], int n);
    
    int ReadRegistor8(int address);
    int ReadRegistor16(int address);
    
    void WriteRegistor(int address, int reg);
    int spiXfer(int address);
};


#endif
