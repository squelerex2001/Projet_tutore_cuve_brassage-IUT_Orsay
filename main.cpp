#include "mbed.h"
#include "scd30.h"

#define SDA0              PA_10 //PTE25
#define SCL0              PA_9 //PTE24



RawSerial pc(USBTX, USBRX);

scd30 scd(SDA0, SCL0, 400000);                 //Microchip real time clock

//-----------------------------------------------------------------------------
// initial splash display

void initSplash() {
    pc.printf("\r\n\r\n");
    pc.printf("-----------------------------------------------------------------------------\r\n");
}

//-----------------------------------------------------------------------------
// initial the scd30

void initSCD30() {
    pc.printf("Initializing SCD30...\r\n");
    scd.softReset();
    wait_ms(2000);
    scd.getSerialNumber();
    pc.printf(" - SCD30 s/n: ");
    for(int i = 0; i < sizeof(scd.scdSTR.sn); i++) pc.printf("%c", scd.scdSTR.sn[i]);
    pc.printf("\r\n");
        
    scd.setMeasInterval(5);
    scd.startMeasurement(0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main()
{
    pc.baud(230400);
    wait_ms(200);
    initSplash();
       
    initSCD30();
    int count = 0;
    
    pc.printf("Ready...\r\n");
    while(1) {
        wait_ms(250);
        scd.getReadyStatus();
        uint16_t redy = scd.scdSTR.ready;
        if(redy == scd30::SCDisReady) {
            uint8_t crcc = scd.readMeasurement();
            count++;
            if(crcc != scd30::SCDnoERROR) pc.printf("ERROR: %d\r\n", crcc);
            else pc.printf("%5d  -> CO2: %9.3f   Temp: %7.3f   Hum: %5.2f\r\n", 
                            count, scd.scdSTR.co2f, scd.scdSTR.tempf, scd.scdSTR.humf);
            if((int)scd.scdSTR.co2f > 10000) initSCD30();
        }
    }
}
