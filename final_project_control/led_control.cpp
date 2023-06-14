#include "mbed.h"
#include <string.h>
#include "stm32f4xx_hal.h"

// main() runs in its own thread in the OS

#define WS2812_NUM_LEDS 8
#define WS2812_RESET_PULSE 60
#define WS2812_BUFFER_SIZE (WS2812_NUM_LEDS * 24 + WS2812_RESET_PULSE)
SPI spi(ARDUINO_UNO_D11, ARDUINO_UNO_D12, ARDUINO_UNO_D13);
uint8_t ws2812_buffer[WS2812_BUFFER_SIZE];
uint8_t empty=0;
uint8_t* address=0;
uint8_t* address2=0;
uint8_t* address3=0;
void ws2812_init(void);
void ws2812_send_spi(void);
void ws2812_pixel(uint16_t led_no, uint8_t r, uint8_t g, uint8_t b);
void ws2812_pixel_all(uint8_t r, uint8_t g, uint8_t b);
///--------------------------------------------BUFFER--------------------------------------------------------
BufferedSerial pc(CONSOLE_TX,CONSOLE_RX,115200);
volatile int flag=0;
volatile int flag2=0;
volatile int j=0;
volatile int led_num;
volatile int busy=0;
//--------------------------------------------------dma------------------------------
static void MX_DMA_Init(void);
static void DMA_IRQHandler(void);
static void DMA2_Stream3_IRQHandler(void);

void flag_func(){
    if(pc.readable() && flag==0){
        flag=1;
    }
}


int main()
{
    char ch[10];
    pc.sigio(&flag_func);
    sprintf(ch,"hello");
    pc.write(ch,strlen(ch));
    address=&ws2812_buffer[0];
    ws2812_init();
    ws2812_pixel_all( 0,  1,  0);
    //ws2812_pixel(led_num,1,0,0);//G-R-B
    ws2812_send_spi();
    while (true) {
        if(flag==1){      
            // read 1~5 level and print led light accoriding to number of level    
            flag=0;             
            pc.read(&ch[j],1);
            pc.write(&ch[j],1);
            j++;
            if(ch[j-1]=='\r'){
                ch[j]='\n';
                pc.write(&ch[j],1);
                j=0;
                flag2=1;
                led_num=atoi(ch);  
            }
        }
        if(flag2==1){
            flag2=0;
        }

        for(int i=0;i<=led_num;i++){
            ws2812_pixel_all(0,0,0);
            ws2812_send_spi();
            wait_us(50000);
            ws2812_pixel(i,1,1,1);//G-R-B
            ws2812_send_spi();
            wait_us(50000);
        }
    }      
}


void ws2812_init(void) {
    spi.frequency(6250000);//0.08us *8 = 0.64us
    memset(ws2812_buffer, 0, WS2812_BUFFER_SIZE);
    spi.set_dma_usage(DMA_USAGE_ALWAYS);
    MX_DMA_Init();
    ws2812_send_spi();
}

void ws2812_send_spi(void) {
    //spi.write((const char*)ws2812_buffer,WS2812_BUFFER_SIZE,0,0); 
    //spi.transfer((const char*)ws2812_buffer, WS2812_BUFFER_SIZE, (char*) &empty, 0, NULL, SPI_EVENT_COMPLETE);
    
    DMA2_Stream3->NDTR = (uint16_t)WS2812_BUFFER_SIZE;
    DMA2_Stream3->CR |= (0x1UL << (0U));
    
}

void WS2812_FILL_BUFFER(uint8_t COLOR ,uint8_t* ptr) {
    for( uint8_t mask = 0x80; mask; mask >>= 1 ) { 
        if( COLOR & mask ) { 
            *ptr++ = 0x7c;//0111-1100
        } else { 
            *ptr++ = 0x40; //0100-0000;
        }   
    }
}


void ws2812_pixel(uint16_t led_no, uint8_t g, uint8_t r, uint8_t b) {
    uint8_t * ptr = &ws2812_buffer[24 * led_no];
    
        WS2812_FILL_BUFFER(g,ptr);
        WS2812_FILL_BUFFER(r,ptr+8);
        WS2812_FILL_BUFFER(b,ptr+16);
    
}

void ws2812_pixel_all(uint8_t g, uint8_t r, uint8_t b) {
    uint8_t * ptr = ws2812_buffer;
    for( uint16_t i = 0; i < WS2812_NUM_LEDS; i++) {
        WS2812_FILL_BUFFER(g,ptr+i*24);
        WS2812_FILL_BUFFER(r,ptr+i*24+8);
        WS2812_FILL_BUFFER(b,ptr+i*24+16);
    }
}


void MX_DMA_Init(void){

   __HAL_RCC_DMA2_CLK_ENABLE();
   DMA2_Stream3->CR |= 0b11 << 25;       //DMA2 stream 3 channel 3
    DMA2_Stream3->CR &= ~(0B11 << (11U)); // spi data register is 8 bit (half word)
    DMA2_Stream3->CR &= ~(0B11 << (13U)); // memory size is is 8 bit (half word)
    //DMA2_Stream3->CR &= ~(0B00 << (18U)); // double buffer mode
    DMA2_Stream3->CR |=  (0b1UL << (10U)); // memory increment (MSIZE = 8 bit)
    DMA2_Stream3->CR |= (0b1UL << (4U));//  active interrupt after transmition
    // peripheral inc_disable
    DMA2_Stream3->CR &= ~(0B1 << (8U)); // circular mode X normal mode 0
    DMA2_Stream3->CR |= 0B01 << (6U);    // memory to peripheral
    DMA2_Stream3->CR |= 0B11 << (16U);   // priority level very high

    DMA2_Stream3->FCR |= (0b1UL << (2U)); //(FIFO abled)
    DMA2_Stream3->FCR &= ~(0b11<< 0); //dma_fifothreshold_1quarterfull
    DMA2_Stream3->PAR = (uint32_t)&SPI1->DR; // peripheral base address(spi)
    DMA2_Stream3->M0AR = (uint32_t)ws2812_buffer; // memory base address 0
    //dma 스트림 비활성화
    DMA2_Stream3->NDTR = (uint16_t)WS2812_BUFFER_SIZE;              // number of data
    //다시 활성화
    // spi dma 활성화
    SPI1->CR2 |= SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN;
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn,0,0);
    NVIC_SetVector(DMA2_Stream3_IRQn,(uint32_t)&DMA2_Stream3_IRQHandler);
    NVIC_EnableIRQ(DMA2_Stream3_IRQn);

    //DMA 전송 시작
    DMA2_Stream3->CR |= DMA_SxCR_EN;
}

void DMA2_Stream3_IRQHandler(void){
  if (DMA2->LISR & (0x1UL << (27U))) {
    DMA2->LIFCR |= (0x1UL << (27U));
  }
}
