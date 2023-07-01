#include "mbed.h"

SPI spi(ARDUINO_UNO_D11, ARDUINO_UNO_D12, ARDUINO_UNO_D13) ;
DigitalOut cs(PA_4) ;

UnbufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ; 
AnalogIn ain0(ARDUINO_UNO_A0) ;

char rx_buffer[100] ;
char tx_buffer[100] ;
int index = 0 ;
volatile int flag = 0 ; 

void set_MCP41010(void)
{
    int num ;
    char tx_buff[2] ;
    
    num = atoi((const char*)rx_buffer) ; // 문자열을 정수로 변환
    sprintf(tx_buffer, "Potentiometer = %d(%0.f[mV])\r\n", num , 3300.0 * num / 255) ;    // 2^16=65536.0
    pc.write(tx_buffer, strlen(tx_buffer)) ;
    tx_buff[0] = 0x11 ;    // 0001 0001, Command Byte
    tx_buff[1] = (char)num ;    // Data Byte
    
    cs = 0 ;
    spi.write(tx_buff, 2 ,0, 0) ;    //value 값 전송 tx_buff에 저장된 데이터를 전송
    cs = 1 ;
}

void rx_cb(void)
{
    char ch ;
    pc.read(&ch, 1) ; //버퍼로 수신한 데이터 읽음
    pc.write(&ch, 1) ; //버퍼의 내용을 전송
    rx_buffer[index++] = ch ;
    if (ch == '\r')
    {
        pc.write("\n",1) ;
        rx_buffer[--index] = '\0' ;    // \0 문자의 끝 \r을 \0로
        index = 0 ;
        flag = 1 ;
    }
}

int main()
{
    float meas ;
    cs = 1 ;
    spi.format(8, 0) ;    // 8 bits, mode = 0 ->16bit
    spi.frequency(1000000) ;    // clock 1 MHz
    
    sprintf(tx_buffer, "\r\n SPI Test Program\r\n") ;
    pc.write(tx_buffer, strlen(tx_buffer)) ;
    pc.attach(callback(rx_cb)) ; 
    
    while(1)
    {
        flag = 0 ;
        sprintf(tx_buffer, "\r\nEnter the value for the potentiometer [0, 255]:") ;
        pc.write(tx_buffer, strlen(tx_buffer)) ;
        
        while (flag != 1) {}

        set_MCP41010() ;
        meas = ain0.read() * 3300 ;    // [0.0 [1.0] --> 0 ~ 3300 mV
        sprintf(tx_buffer, "ADC value = %0.f[mV]\r\n", meas) ;
        pc.write(tx_buffer, strlen(tx_buffer)) ;
    }
}
