#include "mbed.h"

// D3:PB_3, D4:PB_5, D5:PB_4
// D3:red, D5:green, D4:yellow
PortOut RGYLeds(PortB, 0x0038) ;

int main()
{
    while (true)
    {
       int counter = 0 ;
       for (int color = 0 ; color < 8 ; color++)
       {
           if (color == 2 || color == 3)
               counter = 2 ;
           else if (color == 4 || color == 5)
               counter = -2 ;
           else
                counter = 0 ;
           
           RGYLeds = (color + counter) << 3 ;
           ThisThread::sleep_for(500ms) ;
       }
    }
}
