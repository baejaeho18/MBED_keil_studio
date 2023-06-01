
#include "mbed.h"
#include "mpu6050.h"
#include "move_alpha.h"
 void MPU6050_Init();
 void mpu6050_read_accel();

//--------------------------------------------------MPU VALUE------------------------------------------------------------------------

int16_t Accel_X_RAW =0;
int16_t Accel_Y_RAW =0;
int16_t Accel_Z_RAW =0;
int16_t Gyro_X_RAW=0;
int16_t Gyro_Y_RAW=0;
int16_t Gyro_Z_RAW=0;
float Ax, Ay, Az;

//-------------------------------------------------bluetooth code and timer------------------------------------------------------------
Ticker timer;
UnbufferedSerial pc(CONSOLE_TX,CONSOLE_RX,115200);
volatile int mpu_flag=0;
char buf[80];

void trig_mpu_flag(){ 
    mpu_flag=1;
}
//----------------------------------------------------------------------------------------------------------------------------------

void MPU6050_Init (void){
    uint8_t check;
    uint8_t Data;


    mpu_read (MPU6050_ADDR, WHO_AM_I_REG,&check,1);
    //PWR_MGMT_1_REG 0x6B
    if(check ==104){// 0X68 WILL BE RETURNED BY THE SENSOR
        Data =0;
        mpu_write(MPU6050_ADDR, PWR_MGMT_1_REG, Data);
        //PWR_MGMT_1_REG
        Data =0x07;
        mpu_write(MPU6050_ADDR, SMPLT_DIV_REG, Data);
        //->
        //SMPLT_DIV_REG 0x19
        Data = 0x00;
        mpu_write(MPU6050_ADDR, ACCEL_CONFIG_REG, Data);
        //ACCEL_CONFIG_REG 0x1C
        Data = 0x00;
        mpu_write(MPU6050_ADDR, GYRO_CONFIG_REG, Data);
        //GYRO_CONFIG_REG 0x1B
    }
}


void mpu6050_read_accel(){
    uint8_t Rx_data[6];
    mpu_read(MPU6050_ADDR,ACCEL_XOUT_H_REG, Rx_data, 6);
    Accel_X_RAW = (int16_t) (Rx_data[0] << 8 | Rx_data[1]);
    Accel_Y_RAW = (int16_t) (Rx_data[2] << 8 | Rx_data[3]);
    Accel_Z_RAW = (int16_t) (Rx_data[4] << 8 | Rx_data[5]);
    Ax= Accel_X_RAW/16384.0;
    Ay= Accel_Y_RAW/16384.0;
    Az= Accel_Z_RAW/16384.0;
}


int main()
{
    sprintf(buf,"start to read mpu6050 sensor");
    pc.write(buf,strlen(buf));

    mpu_setup();
    //TIM6Config();
    MPU6050_Init();
    setup();

    timer.attach(&trig_mpu_flag,10ms);

    while (true) {
        
        if(mpu_flag==1){
           mpu6050_read_accel();
           sprintf(buf,"Accel %f, %f, %f\n",Ax,Ay,Az);
           pc.write(buf,strlen(buf));
           if((Ax <= -0.5)&& (Az >= 0.5) )
           {
               moveForward();
           }
           else if((Ax >= 0.5 ))
           {
               moveBackward();
           }
           else if((Ay >= 0.5) && (Az >= 0.5))
           {
               turnRight(); 
           }
            else if((Ay <= -0.5) && (Az > 0 ) )
           {
               turnLeft(); 
           }
           else if((Ax >= -0.01 || Ax <= 0.01) && (Ay >= -0.01 || Ay <= 0.01) && (Az >1.0))
           {
               stopMoving();
           }
           else 
           {
               stopMoving();
           }
        }
        wait_us(100);
                
    }
}



/*
#include "stm32f4xx.h"
#include <stdio.h>

#define I2C_TIMEOUT 10000

void i2c_scan(I2C_TypeDef *I2Cx)
{
    uint8_t addr;
    uint32_t i;
    
    for (addr = 0; addr <= 127; addr++)
    {
        I2C1->CR1 |= (1<<10);
        // Generate start condition
        I2C1->CR1 |= I2C_CR1_START;
        
        // Wait until start condition is generated
        while (!(I2C1->SR1 & I2C_SR1_SB));
        
        // Send address with read bit set
        I2C1->DR = (addr << 1) | 0x01;
        
        // Wait until address is sen
        //if (I2C1->SR1 & I2C_SR1_ADDR)
        //{
        //    printf("Device found at address 0x%02X\r\n", addr);
        //}
        while(!(I2C1->SR1 & I2C_SR1_ADDR));
        // Generate stop condition
        I2C1->CR1 |= I2C_CR1_STOP;
        
        // Wait until stop condition is generated
        while (I2C1->CR1 & I2C_CR1_STOP);
        
        // Check if the address was acknowledged
        
    }
}

int main(void)
{
    // Declare a pointer to the I2C1 peripheral
    
    
    // Enable the I2C1 peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    
    // Configure the I2C1 peripheral
    I2C1->CR1 &= ~I2C_CR1_PE;       // Disable I2C1 peripheral
    I2C1->CR2 |= 8;                 // Set peripheral clock frequency to 8 MHz
    I2C1->CCR |= I2C_CCR_FS | 42;   // Set I2C clock to 100 kHz
    I2C1->TRISE = 9;                // Set maximum rise time
    I2C1->CR1 |= I2C_CR1_PE;        // Enable I2C1 peripheral
    
    // Scan the I2C bus for connected devices
    i2c_scan(I2C1);
    
    while(1)
    {
        // Main loop
    }
    
    return 0;
}*/


































/*
//------------------------------------------I2C function --------------------------------------------------------------
void I2C_Config (void);
void I2C_Start (void);
void I2C_Write (uint8_t data);
void I2C_Address (uint8_t Address);
void I2C_Stop(void);
void I2C_WriteMulti (uint9_t *data, uint8_t size);
void I2C_Read (uint8_t Address, uint8_t *buffer, uint8_t size);
//-------------------------------------------------mpu6050---------------------------------------------------------------------
void MPU_SETUP();
void mpu_write(uint8_t Address, uint8_t Reg , uint8_t Data);
void mpu_read(uint8_t Address, uint8_t Reg , uint8_t *buffer, uint8_t size);
void MPU_SETUP(){
    pin_setup();
    I2C_Config();
}

void mpu_write(uint8_t Address, uint8_t Reg , uint8_t Data){
    I2C_Start();
    I2C_Start(Address);
    I2c_Write (Reg);
    I2C_Write (Data);
    I2C_Stop();
}



void mpu_read(uint8_t Address, uint8_t Reg , uint8_t *buffer, uint8_t size){
    pin_setup  ();
    I2C_Start (Address);
    I2c_Write (Reg);
    I2C_Start ();
    I2C_read (Address+0x01,buffer,size);
    I2C_Stop ();
}

void I2C_config (void){
    //RESET I2C
    I2C1->CR1 |= (1<<15);
    //AFTER RESET` PULL THE I2C
    I2C1->CR1 &= ~(1<<15);
    //I2C PERIPHERAL CLOCK FREQUENCY-> TO SET THE 1000KHZ CLOCK BUT WE HAVE TO 400KHZ
    //I2C PERIPHERAL CLOCK -> 4MHZ->250ns,,, CCR=(1000+4000)NS/(250)NS=20 AND SET TRISE VALUE
    I2C1->CR2 |= (4<<0);
    I2C1->CCR = 20<<0;
    I2C1->TRISE = 5;
    //ENABLE THE PERIPHERAL
    I2C1->CR1 |= (1<<0); 
  }

void I2C_Write(uint8_t data){
    while (!(I2C1->SR1 & (1<<7));
    I2C1->DR =data;
    while(!(I2C1->SR1 & (1<<2)));
}

void I2C_Start (void){
    I2C1->CR1 |= (1<<10);
    I2C1->CR1 |= (1<<8);//GENERATE START
    while (!(I2C1->SR1 & (1<<7))); // WAIT FOR TXE BIT TO SET
}


void I2C_Address (uint8_t Address)
{
    I2C1->DR = Address;
    while (!(I2C1->SR1 & (1<<1)));
    uint8_t temp = I2C1->SR1 |  I2C1->SR2;
}

void I2C_Stop(void){
    I2C1->CR1 |= (1<<9);
}

void I2C_WriteMulti (uint9_t *data, uint8_t size){
    while (!(I2C1->SR1 & (1<<7));
        while(size){
            while(!(I2C1->SR1 & (1<<7)));
            I2C1->DR = (volatile uint32_t *data++;
            size--;
        }
        while(!(I2C1->SR1 & (1<<2)));
    }

void I2C_Read (uint8_t Address, uint8_t *buffer, uint8_t size){
    int remaining=size;

    if(size--1){
        I2C1->DR = Address;
        while(!(I2C1->SR1 & (1<<1)));

        I2C1->CR1 &= ~(1<<10);
        uint8_t temp = I2C1->SR1 | I2C1->SR2;
        I2C1->CR1 |= (1<<9);

        while(!(I2C1->SR1 & (1<<6)));

        bufer[size-remaining] = I2C1->DR;
    }
    else{
        I2C1->DR =Address;
        while(!(I2C1->SR1 & (1<<1)));

        uint8_t temp = I2C1->SR1 | I2C1->SR2;

        while(remaining>2){
            while(!(I2C1->SR1& (1<<6)));
            buffer[size-remaining] = I2C1->DR;

            I2C1->CR1 |= 1<<10;
            remaining--;
        }
        while(!(I2C1->SR1& (1<<6)));
        buffer[size-remaining]= I2C1->DR;

        I2C1->CR1 &= ~(1<<10);

        I2C1->CR1 |= (1<<9);
        remaining--;

        while(!(I2C1->SR1 & (1<<6)));
        buffer[size-remaing] = I2C1->DR;
    }
}
*/