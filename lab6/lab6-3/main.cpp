#include "mbed.h"
#include <stdio.h>

Timer t ;
#define MAXIMUM_BUFFER_SIZE 80
char buf[MAXIMUM_BUFFER_SIZE] ;

static BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ;

int main()
{   
    int i = 0 ; 
    t.start() ;
    while (i < 1000000)
    {
        i += 1 ; 
    }
    t.stop() ;
    sprintf(buf, "the time taked was %llu us\r\n", (t.elapsed_time()).count()) ;
    pc.write(buf, strlen(buf)) ;
}
