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
//--------------------------------------------------dma------------------------------
static void MX_DMA_Init(void);
static void DMA_IRQHandler(void);
static void DMA2_Stream3_IRQHandler(void);
//-----------------------------------------------Color------------------------------------------
static void setGauge(int color, int guage); 

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
    pc.write(ch, strlen(ch));
    address=&ws2812_buffer[0];
    ws2812_init();
    //MX_DMA_Init();
    ws2812_pixel_all( 0,  0,  0);
    //ws2812_pixel(led_num,1,0,0);//G-R-B
    ws2812_send_spi();
    while (true) {
        
        if(flag==1){
                            
            flag=0;// read 1~5 level and print led light accoriding to number of level                 
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
            ws2812_pixel(led_num,0,0,1);//G-R-B
            ws2812_send_spi();
            ThisThread::sleep_for(2s);
            ws2812_pixel(led_num,0,1,0);//G-R-B
            ws2812_send_spi();
            ThisThread::sleep_for(2s);
            ws2812_pixel(led_num,1,0,0);//G-R-B
            ws2812_send_spi();

        }
    }      
}


void ws2812_init(void) {
    spi.frequency(6250000);//0.08us *8 = 0.64us
    memset(ws2812_buffer, 0, WS2812_BUFFER_SIZE);
    spi.set_dma_usage(DMA_USAGE_ALWAYS);
    ws2812_send_spi();
}

void SPI_Write(uint8_t *pu8Data, uint32_t u32Len)
{
  uint32_t i;
   for( i = 0; i< u32Len; i++)
   {
     SPI2->DR = pu8Data[i];
      while(SPI2->SR & 0x80); // SPI is busy
   }
}

void ws2812_send_spi(void) {
    //spi.write((const char*)ws2812_buffer,WS2812_BUFFER_SIZE,0,0); 
    spi.transfer((const char*)ws2812_buffer, WS2812_BUFFER_SIZE, (char*) &empty, 0, NULL, SPI_EVENT_COMPLETE);
}

void WS2812_FILL_BUFFER(uint8_t COLOR ,uint8_t* ptr) {
    for( uint8_t mask = 0x80; mask; mask >>= 1 ) { 
        if( COLOR & mask ) { 
            *ptr++ = 0x7c;//0111-1100
        } else { 
            *ptr++ = 0x40; //1000-0000;
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

void setGauge(int color, int led_no){    // color 0:G, 1:R, 2:B
    for (int i = 0 ; i < led_no ; i++){
        uint8_t * ptr = &ws2812_buffer[24*led_no];
        
        WS2812_FILL_BUFFER(0,ptr);
        WS2812_FILL_BUFFER(0,ptr+8);
        WS2812_FILL_BUFFER(0,ptr+16);
        switch(color){
            case 0:
                WS2812_FILL_BUFFER(1,ptr);
                break;
            case 1:
                WS2812_FILL_BUFFER(1,ptr+8);
                break;
            case 2:
                WS2812_FILL_BUFFER(1,ptr+16);
                break;
        }
    }
}














/*
void MX_DMA_Init(void){

   __HAL_RCC_DMA2_CLK_ENABLE();

   DMA2_Stream3->CR |= 11 << 25;       //DMA2 stream 2 channel 3
    DMA2_Stream3->FCR &= ~(0x1UL << (2U)); // direct mode (FIFO disabled)
    //dma_fifothreshold_1quarterfull
    DMA2_Stream3->CR |= 0x00 << DMA_SxCR_PSIZE_Pos;   //spi data register is 8 bit (half word)
   DMA2_Stream3->CR |= 0x00 << DMA_SxCR_MSIZE_Pos;   // memory size is is 8 bit (half word)

   DMA2_Stream3->CR |= DMA_SxCR_DBM;                 // double buffer mode
    DMA2_Stream3->CR |= (0x1UL << (10U)); // memory increment (MSIZE = 16 bit)
    // peripheral inc_disable
    DMA2_Stream3->CR |= (0x1UL << (8U));          // circular mode X normal mode 0

    DMA2_Stream3->CR |= 0x01 << (6U);             // memory to peripheral
    DMA2_Stream3->CR |= 0x02 << (16U);            // priority level very high
    DMA2_Stream3->PAR = (uint32_t)&SPI1->DR; // peripheral base address(spi)
    DMA2_Stream3->M0AR = (uint32_t)ws2812_buffer; // memory base address 0
    //DMA2_Stream3->NDTR = NUM_SAMPLES;              // number of data
   HAL_DMA_Init(&hdma_spi);
    __HAL_LINKDMA(&spi, hdmatx, hdma_spi);
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn,0,0);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
}
*/
