#include "mbed.h"

DigitalOut led2(PB_3) ; 
DigitalOut led3(PB_5) ;

Ticker led2_ticker ;
Ticker led3_ticker ;

float interval = 1 ;

void timeout_cb(void)
{
    led2_ticker.detach() ;
    led2 =! led2 ;
    led2_ticker.attach(&timeout_cb,interval) ;
}

void timeout_cb2(void)
{
    led3 =! led3 ;
}

int main()
{
    led3 = 1 ;
    led2_ticker.attach(&timeout_cb, interval) ;
    led3_ticker.attach(&timeout_cb2, interval*10) ;
    while(1){}
}
