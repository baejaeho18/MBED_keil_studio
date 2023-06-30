#include "mbed.h"
#include <stdio.h>

BufferedSerial pc(USBTX, USBRX, 115200);
AnalogIn tempSensor(ADC_TEMP);
Ticker t;
char buf[80];
volatile bool flag=false;

void timeout_cb(){
    flag=true;
}

int main() {
    float meas;
    
    sprintf(buf,"\r\n *** ADC Example (Internal Temp Sensor) ***\r\n");
    pc.write(buf, strlen(buf));
    t.attach(&timeout_cb, 5s);
    while (true) {
        if (flag){
            meas = tempSensor.read();
            meas = ((meas*3.3-0.76)/0.0025)+25.0;            
            sprintf(buf, "Internal Temperature = %.2f [C]\r\n ",meas);
            pc.write(buf, strlen(buf));

            flag=false;
        }
    }
}