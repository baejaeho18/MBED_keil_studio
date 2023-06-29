#include "mbed.h"
#include <memory>

//D2:red, D3:green, D4:yellow
PinName pins[3] = { D2, D3, D4 } ;  
BusOut RGYLeds(pins) ;

int 
main()
{
    while (true)
    {       
       for (int color = 0 ; color < 8 ; color++)
       {
           RGYLeds = color ;
           ThisThread::sleep_for(500ms) ;
       }
    }
}
