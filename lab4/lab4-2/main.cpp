#include "mbed.h"
#define MAXIMUM_BUFFER_SIZE 80
char buf[MAXIMUM_BUFFER_SIZE] ;

static DigitalOut led(LED1) ;
static BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ;

void rx_handler()
{
    led = !led ;
}

int main(void)
{
    pc.set_blocking(false) ;
    pc.sigio(rx_handler) ;

    sprintf(buf, "Hello New Serial function in mbed-os v.%.1f\r\n", 6.0) ;
    pc.write(buf, strlen(buf)) ;
    sprintf(buf, "Enter characters...\r\n") ;
    pc.write(buf, strlen(buf)) ;
    
    while(1)
    {
        int i = 0 ;    // 버퍼의 인덱스 접근을 위함
        while(pc.readable() && i < MAXIMUM_BUFFER_SIZE - 1)
        {
            pc.read(&buf[i], 1) ;
            pc.write(&buf[i], 1) ;

            if (buf[i] == '\r') 
            {
                pc.write("\n", 1) ;
                break ;    // 엔터가 나오면 읽는 것을 중단하고 새로운 입력 받음
            }
            i++ ;
        }
        buf[i] = '\0' ;    // 문자열의 끝을 나타냄, 다음 문자열은 buf[0] 부터 덮어쓰기 함
    }
}
