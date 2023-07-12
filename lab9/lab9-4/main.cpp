#include "mbed.h"
#include "SRF05.h"

SRF05 srf05(ARDUINO_UNO_D3,ARDUINO_UNO_D2) ;

BufferedSerial pc(CONSOLE_TX,CONSOLE_RX, 115200) ;
char buffer[80] ;

int main()
{
    sprintf(buffer,"Mbed OS version %d.%d%d\r\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    pc.write(buffer,strlen(buffer)) ;
    sprintf(buffer, "\r\n Welcome to Utrasonic Sensor Lab.!\r\n");
    pc.write(buffer, strlen(buffer)) ;
    while(1)
    {    
        sprintf(buffer,"The ditance is %.2f [cm] \n\r",srf05.getDistance()) ;
        pc.write(buffer,strlen(buffer)) ;
        
        ThisThread::sleep_for(3000ms) ;
    }
}
