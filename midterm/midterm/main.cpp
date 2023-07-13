#include "mbed.h"
#include "IntDebouncedIn.h"
#include <string.h>

#define BLINKING_RATE 500ms

IntDebounceIn debouncedSW(D7, 20ms) ;
UnbufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 230400) ;
DigitalOut led(LED1) ;

void isr_pushed () 
{
    char ch ;
    ch = 'P' ;
    pc.write(&ch, 1) ;
}

void isr_released () 
{
    char ch ;
    ch = 'R' ;
    pc.write(&ch, 1) ;
}

int main()
{   
    debouncedSW.fall(&isr_pushed) ; 
    debouncedSW.rise(&isr_released) ;
    while (true) 
    {
        led = !led ;
        ThisThread::sleep_for(BLINKING_RATE) ;
    }
}
