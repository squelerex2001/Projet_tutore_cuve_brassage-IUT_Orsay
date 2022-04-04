#include "max31865.h"
#include "mbed.h"

max31865::max31865(PinName MOSI, PinName MISO, PinName SCLK, PinName CS) : spi(MOSI, MISO, SCLK), cs(CS) // mosi, miso, sclk
{
    //constructor
    cs = 1; //deselect chip
    spi.format(8,1); //set mode 1 and 8 bit
    spi.frequency(500000); //set frequency to 0.5mhz
    
    
}

int max31865::ReadRTD()
{
    ClearFault();
    EnableBias(true);
    
    int t = ReadRegistor8(MAX31856_CONFIG_REG);
    t |= MAX31856_CONFIG_1SHOT;
    WriteRegistor(MAX31856_CONFIG_REG, t);
    
    int RTD = ReadRegistor16(MAX31856_RTDMSB_REG);
    
    // remove fault
    RTD >>= 1;
     
    return RTD;    
}

void max31865::Begin(max31865_numwires_t wires)
{
    cs = 1;
    SetWires(wires);
    EnableBias(false);
    AutoConvert(false);
    ClearFault();   
}

int max31865::ReadFault()
{
    return ReadRegistor8(MAX31856_FAULTSTAT_REG);
}

void max31865::ClearFault()
{
    int t = ReadRegistor8(MAX31856_CONFIG_REG);
    t &= ~0x2C;
    t |= MAX31856_CONFIG_FAULTSTAT;
    WriteRegistor(MAX31856_CONFIG_REG, t);
}

void max31865::EnableBias(bool b)
{
    int t = ReadRegistor8(MAX31856_CONFIG_REG);
    if (b)
    {
        t |= MAX31856_CONFIG_BIAS;       // enable bias
    }
    else
    {
        t &= ~MAX31856_CONFIG_BIAS;       // disable bias
    }
    WriteRegistor(MAX31856_CONFIG_REG, t);
}

void max31865::AutoConvert(bool b)
{
    int t = ReadRegistor8(MAX31856_CONFIG_REG);
    if (b)
    {
        t |= MAX31856_CONFIG_MODEAUTO;       // enable autoconvert
    }
    else
    {
        t &= ~MAX31856_CONFIG_MODEAUTO;       // disable autoconvert
    }
    WriteRegistor(MAX31856_CONFIG_REG, t);   
}

void max31865::SetWires(max31865_numwires_t wires )
{
    int t = ReadRegistor8(MAX31856_CONFIG_REG);
    
    if (wires == MAX31865_3WIRE) 
    {
        t |= MAX31856_CONFIG_3WIRE;
    } 
    else 
    {
        // 2 or 4 wire
        t &= ~MAX31856_CONFIG_3WIRE;
    }
    WriteRegistor(MAX31856_CONFIG_REG, t);
}


int max31865::ReadRegistor8(int address)
{
    int ret = 0;
    ReadRegistorN(address, &ret, 1);
    return ret;      
}

int max31865::ReadRegistor16(int address)
{
    int buffer[2] = {0, 0};
    ReadRegistorN(address, buffer, 2);

    int ret = buffer[0];
    ret <<= 8;
    ret |=  buffer[1];
  
    return ret;  
}

void max31865::ReadRegistorN(int address, int buffer[], int n)
{
    address &= 0x7F; // make sure top bit is not set 
     
    cs = 0;
     
    spiXfer(address);
     
    while(n--)
    {
        buffer[0] = spiXfer(0xFF);
        buffer++;   
    }
    
    cs = 1;
}

void max31865::WriteRegistor(int address, int data)
{
    
    cs = 0; //select chip
    spiXfer(address | 0x80);   // make sure top bit is set
    spiXfer(data);
    
    cs = 1;
}

int max31865::spiXfer(int x)
{
    return spi.write(x);   
}
