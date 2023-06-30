#include "mbed.h"
#include <stdlib.h>  // atoi() 함수 사용을 위한 헤더 파일


#define PWM_MIN 550
#define PWM_MAX 2150

PwmOut servoMotor(D6);
UnbufferedSerial pc(PA_2, PA_3, 115200);

char rx_buffer[10];
char tx_buffer[80];
int index = 0;
volatile int flag;

void rx_ISR(void)
{
    char ch;
    pc.read(&ch,1);
    pc.write(&ch,1);
    rx_buffer[index++]=ch;
    if(ch=='\r'){ //CR
        pc.write("\n",1); //LF
        rx_buffer[--index]='\0'; //change CR to 0
        index = 0;
        flag = 1;
    }
}

void control_servo(){
    float degree;
    char ch;
    int index=0;
    degree=550+(1600/180)*atoi(rx_buffer);
    if(degree<PWM_MIN or degree>PWM_MAX){
        sprintf(tx_buffer, "(%f) Enter the degree in [0 - 180]: \r\n",degree);
        pc.write(tx_buffer,strlen(tx_buffer));
        return;
    }
    else{
        
        servoMotor.pulsewidth_us(degree);
        sprintf(tx_buffer,"Pulse ON time = %f \r\n",degree);
        pc.write(tx_buffer,strlen(tx_buffer));
    }
}

int main(){
    sprintf(tx_buffer,"PWM Test Program (Servo Motor)!\r\n");
    pc.write(tx_buffer,strlen(tx_buffer));
    pc.attach(rx_ISR);
    servoMotor.period_ms(20);
    servoMotor.pulsewidth_us(PWM_MIN); //0 degree

    while(1){
        flag=0;
        sprintf(tx_buffer, "Enter the rotation degree [0 - 180]: ");
        pc.write(tx_buffer,strlen(tx_buffer));
        while(flag != 1){};

        control_servo();
    }
}