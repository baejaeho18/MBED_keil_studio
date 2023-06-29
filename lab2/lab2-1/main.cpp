#include "mbed.h"
#include <chrono>
#include <ratio>

void led2_init(void) ;
void button_init(void) ;
uint32_t button_input(void) ;
void led2_toggle(void) ;

void led2_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE() ;

    GPIOA->MODER &= ~(0b11 << 10) ;
    GPIOA->MODER |= (0b01 << 10) ;
}

void button_init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE() ;
    
    GPIOC->MODER &= ~(0b11 << 26) ;
}

uint32_t button_input(void)
{
    uint32_t a = GPIOC->IDR ;
    uint32_t bit13 = (a & 0b1<<13) >> 13 ;
    return bit13 ;
}

void led2_toggle(void)
{
    uint32_t a = GPIOA->ODR ;
    uint32_t bit5 = (a & 0b1<<5) >> 5 ;
    /*
    * GPIOA->ODR &= ~(0b1 << 5) ;
    * GPIOA->ODR |= (~bit5 << 5) ;
    */
    GPIOA->ODR ^= (0b1 << 5) ;
}


int main()
{
    int interval = 100 ;
    uint32_t val ;
    int flag = 5 ;
    int counter = 0 ;

    led2_init() ;
    button_init() ;

    while (true) 
    {
        val = button_input() ;

        if (val == 0)
        {
            flag = flag << 1 ;
            if (flag > 20)
                flag = 5 ;
        }
        if (flag > counter)
        {
            counter += 1 ;
        }
        else
        {
            counter = 0 ;
            led2_toggle() ;
        }        

        ThisThread::sleep_for((std::chrono::milliseconds)interval) ;
    }
}
