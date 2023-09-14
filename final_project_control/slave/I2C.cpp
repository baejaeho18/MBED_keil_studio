#include "stm32f4xx.h"
#include "I2C.h"

void I2C_Config (void)
{
    //RESET I2C
    I2C1->CR1 |= (1 << 15) ;
    //AFTER RESET` PULL THE I2C
    I2C1->CR1 &= ~(1 << 15) ;
    //I2C PERIPHERAL CLOCK FREQUENCY-> TO SET THE 1000KHZ CLOCK BUT WE HAVE TO 400KHZ
    //I2C PERIPHERAL CLOCK -> 4MHZ->250ns,,, CCR=(1000+4000)NS/(250)NS=20 AND SET TRISE VALUE
    I2C1->CR2 |= (45 << 0) ;
    I2C1->CCR = 225 << 0 ;
    I2C1->TRISE = 46 ;
    //ENABLE THE PERIPHERAL
    I2C1->CR1 |= (1 << 0) ; 
  }

void I2C_Write(uint8_t data)
{
    while (!(I2C1->SR1 & (1 << 7))) ; //wait for TXE bit to set
    I2C1->DR =data ;
    while (!(I2C1->SR1 & (1 << 2))) ;   //wait for BTF bit to set
}

void I2C_Start (void)
{
    I2C1->CR1 |= (1 << 10) ;
    I2C1->CR1 |= (1 << 8) ;    //GENERATE START
     while (!(I2C1->SR1 & (1 << 0))) ; // wait for SB bit to set
}

void I2C_Address (uint8_t Address)
{
    I2C1->DR = Address ; // send the address
    while (!(I2C1->SR1 & (1 << 1))) ; //wait for addr bit to set
    uint8_t temp = I2C1->SR1 |  I2C1->SR2 ;  //read SR1 and SR2 to clear 
}

void I2C_Stop(void)
{
    I2C1->CR1 |= (1 << 9) ;
}

void I2C_WriteMulti (uint8_t * data, uint8_t size)
{
    while (!(I2C1->SR1 & (1 << 7))); // wait for TXE bit to set
    
    while(size)
    {
        while(!(I2C1->SR1 & (1 << 7))) ;
        I2C1->DR = (uint32_t) * data++ ;
        size-- ;
    }

    while(!(I2C1->SR1 & (1 << 2)));
}

void I2C_Read (uint8_t Address, uint8_t * buffer, uint8_t size)
{
    int remaining=size ;

    if (size == 1)
    {
        I2C1->DR = Address ;
        while (!(I2C1->SR1 & (1 << 1))) ;

        I2C1->CR1 &= ~(1 << 10) ;
        uint8_t temp = I2C1->SR1 | I2C1->SR2 ; 
        I2C1->CR1 |= (1 << 9) ;

        while (!(I2C1->SR1 & (1 << 6))) ;

        buffer[size - remaining] = I2C1->DR ;
    }
    else
    {
        I2C1->DR = Address ;
        while(!(I2C1->SR1 & (1 << 1))) ;

        uint8_t temp = I2C1->SR1 | I2C1->SR2 ;

        while (remaining > 2)
        {
            while (!(I2C1->SR1 & (1 << 6))) ;
            buffer[size - remaining] = I2C1->DR ;

            I2C1->CR1 |= 1 << 10 ;
            remaining-- ;
        }
        while (!(I2C1->SR1 & (1 << 6))) ;
        buffer[size - remaining] = I2C1->DR ;

        I2C1->CR1 &= ~(1 << 10) ;

        I2C1->CR1 |= (1 << 9) ;
        remaining-- ;

        while (!(I2C1->SR1 & (1 << 6))) ;
        buffer[size - remaining] = I2C1->DR ;
    }
}
