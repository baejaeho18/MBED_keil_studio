#include "mbed.h"

// D3:PB_3, D4:PB_5, D5:PB_4
// D3:red, D5:yellow, D4:green
PortOut RGYLeds(PortB, 0x0038) ;    // 0011_1000
InterruptIn button (BUTTON1) ;
volatile int shift=0;

void buttonPressed()
{
    if (shift == 0)
        shift = 2 ;
    else
        shift -= 1 ;
}

int main()
{
    button.fall(&buttonPressed) ;

    while (true) 
    {
       for (int color = 0 ; color <= 63 ; color += 9) 
       {
           RGYLeds = color << shift ;
           ThisThread::sleep_for(500ms) ;
       }
    }
}
