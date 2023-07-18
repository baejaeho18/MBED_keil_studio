

#include "mbed.h"
#include "mpu6050.h"
#include "move_alpha.h"


 void MPU6050_Init() ;
 void mpu6050_read_accel() ;
//--------------------------------------------------MPU VALUE------------------------------------------------------------------------

//-------------------------------------------------bluetooth code and timer------------------------------------------------------------
Ticker timer ;
BufferedSerial hm10(PA_9, PA_10, 9600) ;  // TX, RX, baud rate 9600 
BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 9600) ;


volatile int mpu_flag = 0 ;
char buf[80] ;

void trig_mpu_flag()
{ 
    mpu_flag = 1 ;
}

//----------------------------------------------------------------------------------------------------------------------------------

void MPU6050_Init (void)
{
    uint8_t check ;
    uint8_t Data ;

    mpu_read (MPU6050_ADDR, WHO_AM_I_REG, &check, 1) ; 
    
    if(check == 104)
    {// 0X68 WILL BE RETURNED BY THE SENSOR
        Data = 0 ;
        mpu_write(MPU6050_ADDR, PWR_MGMT_1_REG, Data) ;
        //PWR_MGMT_1_REG
        Data = 0x07 ;

        mpu_write(MPU6050_ADDR, SMPLT_DIV_REG, Data) ;
        //->
        //SMPLT_DIV_REG 0x19
        Data = 0x00 ;
        mpu_write(MPU6050_ADDR, ACCEL_CONFIG_REG, Data) ;
        //ACCEL_CONFIG_REG 0x1C
        Data = 0x00 ;

        mpu_write(MPU6050_ADDR, GYRO_CONFIG_REG, Data);
        //GYRO_CONFIG_REG 0x1B

    }
}



void SetingDataReceived()
{
    while (hm10.readable()) // flush hm10 buffer
    {
        char raw[80] ;
        char buf[80] ;
        
        hm10.read(raw, sizeof(raw)) ; 
        if (raw[0] == 'O')
        {
            for (int i = 0 ; i < sizeof(raw) ; i++)
            {
                if (raw[i] == '\r' || raw[i] == '\n' || raw[i] == '\0')
                {
                    buf[i] = '\r' ;
                    buf[i + 1] = '\n' ;
                    break ;
                }
                else
                {
                    buf[i] = raw[i] ;
                }
            }
            pc.write(buf, sizeof(buf)) ;
        }
    }
    ThisThread::sleep_for(50ms);
}



int main()
{   
    while (!hm10.writable()) {} // wait until writeable
    ThisThread::sleep_for(2000ms) ;
    pc.write("\r\nStarting Master Program\r\n", 27) ;
    hm10.write("AT+RENEW\r\n", 10) ;
    pc.write("Device Reset\r\n", 14) ;
    ThisThread::sleep_for(3000ms) ;
    SetingDataReceived() ;

    hm10.write("AT+ROLE1\r\n", 10) ; // set chip to master mode
    ThisThread::sleep_for(3000ms) ;
    SetingDataReceived() ;

    hm10.write("AT+NAMEKDH\r\n", 13) ;
    pc.write("Device Set to Master\r\n", 22) ;
    ThisThread::sleep_for(3000ms) ;
    SetingDataReceived() ;


    pc.write("Connecting to device YS\r\n", 27) ;
    hm10.write("AT+CONN=YS\r\n", 14) ; // connects to the first device in the list
    ThisThread::sleep_for(5000ms) ; // wait to connect
    SetingDataReceived() ;

    while(!hm10.writable()) {}
    hm10.write("&", 1) ; // send & to test if the connection is established
    pc.write("Sending test character, waiting for ACK\r\n", 41) ;
    ThisThread::sleep_for(2000ms) ;

    while (!hm10.readable()) {}
    
    char c = '0' ;
    int ack_counter = 0; 
    while(c != '&' && ack_counter < 1000)
    {
        hm10.read(&c, 1) ;
        if (c != '&')
        {
            hm10.write("&", 1) ;
            ack_counter += 1 ;
        }
        else
        {
            break ;
        }
    }
    if(c != '&') 
    {
        pc.write("no ACK. Reset Please\r\n", 22) ;
        while(1) {}
    }

    pc.write("ACK received. Sending start!\r\n", 30) ;  // received &, and can proceed with instructions
    ThisThread::sleep_for(3000ms) ;
    char cmd[21] = {0x0} ;
    mpu_setup() ;
    //TIM6Config() ;
    MPU6050_Init() ;
    timer.attach(&trig_mpu_flag, 70ms) ;
    
    while (1) // send instructions to slave to turn on LEDs
    {
        while (mpu_flag == 0)
        {

        }
        if (hm10.writable())
        { 
            timer.detach() ;     
            uint8_t Rx_data[6] ;        
            mpu_read(MPU6050_ADDR,ACCEL_XOUT_H_REG, Rx_data, 6) ;
            mpu_flag = 0 ;
          
            uint8_t Rxr_data[6] ;
            for(int i = 0 ; i < 6 ; i++)
            {
                Rxr_data[i] = Rx_data[i] ;
            }
            ThisThread::sleep_for(50ms) ;
            sprintf(cmd, "%u,%u,%u,%u,%u,%u\r\n", Rxr_data[0], Rxr_data[1], Rxr_data[2], Rxr_data[3], Rxr_data[4], Rxr_data[5]);
            ThisThread::sleep_for(50ms) ;
            pc.write(cmd,sizeof(cmd)) ;
            hm10.write(cmd, sizeof(cmd)) ;
            
            timer.attach(&trig_mpu_flag, 70ms) ;    
        }

        
        ThisThread::sleep_for(100ms) ;
        
    }
}
