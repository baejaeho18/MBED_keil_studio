#include "mbed.h"
#include "I2C.h"
#include "mpu6050.h"

void pin_setup()
{
    //I2C clock
    __HAL_RCC_I2C1_CLK_ENABLE() ;
    __HAL_RCC_GPIOB_CLK_ENABLE() ;
    //MODER-> PB6: ALTERNATIVE FUNCTIONMODE
    GPIOB->MODER &= ~(0b11<<12) ;
    GPIOB->MODER |= (0b10<<12) ;
    //MODER-> PB7: ALTERNATIVE FUNCTIONMODE
    GPIOB->MODER &= ~(0b11<<14) ;
    GPIOB->MODER |= (0b10<<14) ;
    //set GPIO PB6,PB7 port to open-drain
    GPIOB->OTYPER |= (0b11<<6) ;
    // pull-up timp->scl, sda default value is high for i2c communication
    GPIOB->PUPDR &= ~(0b11<<12) ;
    GPIOB->PUPDR |= (0b01<<12) ;
    //set pull-up pb7
    GPIOB->PUPDR &= ~(0b11<<14) ;
    GPIOB->PUPDR |= (0b01<<14) ;
    //HIGH SPEED
    GPIOB->OSPEEDR |= (3 << 12) | ( 3 << 14) ;
    //i2c scl->PB6->D10
    GPIOB->AFR[0] &= ~(0B1111<<24) ;
    GPIOB->AFR[0] |= (0B0100<<24) ;
    //i2c sda->PB7->GND
    GPIOB->AFR[0] &= ~(0B1111<<28) ; 
    GPIOB->AFR[0] |= (0B0100<<28) ;
}

void mpu_setup()
{
    pin_setup() ;
    I2C_Config() ;
}

void mpu_write(uint8_t address, uint8_t reg, uint8_t data)
{
    I2C_Start() ;
    I2C_Address(address) ;
    I2C_Write (reg) ;
    I2C_Write (data) ;
    I2C_Stop() ;
}

void mpu_read(uint8_t Address, uint8_t Reg, uint8_t *buffer, uint8_t size)
{
    I2C_Start() ;
    I2C_Address(Address) ; 
    I2C_Write(Reg) ;
    I2C_Start() ; //REPEATSTART
    I2C_Read(Address + 0x01, buffer, size) ;
    I2C_Stop() ;
    ThisThread::sleep_for(50ms) ;
}
