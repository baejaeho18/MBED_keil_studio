#include "mbed.h"

DigitalOut led2(PB_3) ;
InterruptIn button(BUTTON1) ; 
Ticker led2_ticker ;

float interval = 0.5 ;

void timeout_cb(void)
{
    led2 =! led2 ;
}

void button_onpressed_cb(void)
{
    led2_ticker.detach() ;
    interval = interval * 2 ;
    if (interval > 4.0) 
            interval = 0.125 ;
    led2 = 1 ;
    led2_ticker.attach(&timeout_cb, interval) ;
}

int main()
{
    button.mode(PullUp) ;
    button.fall(&button_onpressed_cb) ;

    led2_ticker.attach(&timeout_cb, interval) ;
    while(1) {}
}
