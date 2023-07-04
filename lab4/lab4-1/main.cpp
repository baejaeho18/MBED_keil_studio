#include "mbed.h"
#define MAXIMUM_BUFFER_SIZE 80
char buf[MAXIMUM_BUFFER_SIZE] ;

static DigitalOut led(LED1) ;
static BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ;

int main(void)
{
    printf("mbed-os version: %d. %d. %d\r\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION) ;
    sprintf(buf, "Hello New Serial function in mbed-os v.%.1f\r\n",6.0) ;
    pc.write(buf, strlen(buf)) ;

    while(1)
    {   // 데이터 흐름  serial->board->serial 다시말해 시리얼에서 보이는 단어는 에코된 단어이다.
        led = !led ;    // 기본 설정이 blocking mode이기 때문에 write에 데이터를 다 쓰기 전까지 기다려서
                        // 한 데이터가 전송되기 전 까지 led는 toggle되지 않는다.
        // Echo the input back to the terminal.
        int num = pc.read(buf, sizeof(buf)) ;
        pc.write(buf, num) ;    // 에코 1회만
        if (buf[0] == '\r')
            pc.write("\n",1) ;
    }
}
