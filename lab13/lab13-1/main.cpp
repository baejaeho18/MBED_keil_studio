#include "mbed.h"

UnbufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ;
UnbufferedSerial esp8266(ARDUINO_UNO_D8, ARDUINO_UNO_D2, 115200) ; 

// main() runs in its own thread in the OS
int main() 
{
    char ch ;
    while (true) 
    {
        if (pc.readable()) 
        {
            pc.read(&ch, 1) ;
            esp8266.write(&ch, 1) ;
        }
        if (esp8266.readable()) 
        {
            esp8266.read(&ch, 1) ;
            pc.write(&ch, 1) ;
        }
    }
}
