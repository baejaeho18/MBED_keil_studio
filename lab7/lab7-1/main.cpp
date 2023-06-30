#include "mbed.h"
#include <stdio.h>

#define MAXIMUM_BUFFER_SIZE  80
char buf[MAXIMUM_BUFFER_SIZE];

AnalogIn ain0(A0);
BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200);

int main(){
    float meas;
    
    sprintf(buf,"\r\n Welcome to Analog In Lab.\r\n");
    pc.write(buf, strlen(buf));

    while(1){
        meas= ain0.read(); // Converts and read the analog input value[0.0 1.0]
        meas= meas* 3300; // Change the value to be in the 0 to 3300 mVrange
        sprintf(buf, "A0 input voltage = %.2f mV\r\n", meas);
        pc.write(buf, strlen(buf));
        ThisThread::sleep_for(chrono::milliseconds(1000));

    }
}