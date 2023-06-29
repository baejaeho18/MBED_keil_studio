#include "mbed.h"

void led2_init(void) ;
void button_init(void) ;
void button_Handler(void) ;    // ISR of EXTI15_10_IRQn
void led2_toggle(void) ;

volatile int interval = 500 ;

void led2_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE() ; 

    GPIOA->MODER &= ~(0b11 << 10) ;
    GPIOA->MODER |= (0b01 << 10) ;
}

void button_init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE() ;
    
    GPIOC->MODER &= ~(0b11 << 26) ;    // C13을 output mode로

    __HAL_RCC_SYSCFG_CLK_ENABLE() ;    // SYSCFG 클럭 제공

    SYSCFG->EXTICR[3] |= (0b1 << 5) ;    // PC_13 활성화
    EXTI->IMR |= 0b1 << 13 ;    // interrupt 활성화
    EXTI->FTSR |= 0b1 << 13 ;    // rising event 선택
    NVIC->ISER[1] |= 0b1 << 8 ;    // NVIC interrupt set enable bit -> 40번 인터럽트 활성화
}

void led2_toggle(void)
{
    uint32_t a = GPIOA->ODR ;
    uint32_t bit5 = (a & 0b1<<5) >> 5 ;
    GPIOA->ODR ^= (0b1 << 5) ;
}

void button_Handler(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE() ;    // SYSCFG 클럭 제공
    if (interval == 2000)
        interval=500 ;
    else
        interval = interval << 1 ;
    EXTI->PR = (0b1 << 13) ;
}


int main()
{
    led2_init() ;
    button_init() ;
    NVIC_SetVector(EXTI15_10_IRQn, (uint32_t)button_Handler) ;

    while (true)
    {
        led2_toggle() ;
        ThisThread::sleep_for((std::chrono::milliseconds)interval) ;
    }
}
