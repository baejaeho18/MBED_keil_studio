//SLAVE 

#include "mbed.h"
#include "move_alpha.h"
#include <string.h>
#include "stm32f4xx_hal.h"

#define WS2812_NUM_LEDS 8
#define WS2812_RESET_PULSE 60
#define WS2812_BUFFER_SIZE (WS2812_NUM_LEDS * 24 + WS2812_RESET_PULSE)
SPI spi(ARDUINO_UNO_D11, ARDUINO_UNO_D12, ARDUINO_UNO_D13) ;
uint8_t ws2812_buffer[WS2812_BUFFER_SIZE] ;
uint8_t empty = 0 ;
uint8_t* address = 0 ;
uint8_t* address2 = 0 ;
uint8_t* address3 = 0 ;
void turnOnLED(int color) ;
void ws2812_init(void) ;
void ws2812_send_spi(void) ;
void ws2812_pixel(uint16_t led_no, uint8_t r, uint8_t g, uint8_t b);
void ws2812_pixel_all(uint8_t r, uint8_t g, uint8_t b);
volatile int flag = 0 ;
volatile int flag2 = 0 ;
volatile int j = 0 ;
volatile int led_num ;
volatile int busy = 0 ;


float stack ;
int pre_direction ;
//--------------------------------------------------dma------------------------------
static void MX_DMA_Init(void) ; 
static void DMA_IRQHandler(void) ;
static void DMA2_Stream3_IRQHandler(void) ;

DigitalOut led1(LED1) ;

BufferedSerial hm10(PA_9, PA_10, 9600) ; // TX, RX, baud rate 9600
BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ;

void flag_func()
{
    if (pc.readable() && flag == 0)
        flag = 1 ;
}


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
volatile int mpu_flag=0;
char buff[80];

void SetingDataReceived()
{
    while (hm10.readable()) // flush hm10 buffer
    {
        char raw[80];
        char buf[80];
        
        hm10.read(raw, sizeof(raw));
        if (raw[0]=='O'){
            for (int i = 0; i < sizeof(raw); i++){
                if (raw[i]=='\r' || raw[i]=='\n' || raw[i]=='\0'){
                    buf[i]='\r';
                    buf[i+1]='\n';
                    break;
                }
                else{
                    buf[i]=raw[i];
                }
            }
        }
        pc.write(buf, sizeof(buf));
    }
    ThisThread::sleep_for(50ms);
}


int main()
{
    
    while (!hm10.writable()) {} // wait until writeable
    ThisThread::sleep_for(2000ms);
    pc.write("\r\nStarting Slave Program\r\n", 26);
    hm10.write("AT+RENEW\r\n", 10);
    pc.write("Device Reset\r\n", 14);
    ThisThread::sleep_for(3000ms);
    SetingDataReceived();

    hm10.write("AT+NAMEYS\r\n", 13);
    pc.write("Device Naming\r\n", 15);
    ThisThread::sleep_for(3000ms);
    SetingDataReceived();

    hm10.write("AT+ROLE0\r\n", 10); // set chip to slave mode
    pc.write("Device Set to Slave\r\n", 21);
    ThisThread::sleep_for(3000ms);
    SetingDataReceived();

    char c='0';
    pc.write("Waiting for conn\r\n", 18);
    while (hm10.read(&c, 1) != 1 || c != '&') {} // wait for initialization character from master
    hm10.write("&", 1); // send acknowledgement back to the master
    pc.write("Received test character, sending ACK\r\n", 38);
    ThisThread::sleep_for(2000ms);
    
    int idx = 0;
    char cmd[40];
    setup();

    address=&ws2812_buffer[0];
    ws2812_init();
    ws2812_pixel_all( 0,  0,  0);
    //ws2812_pixel(led_num,1,0,0);//G-R-B
    ws2812_send_spi();

    stack = 0;
    pre_direction=0;

    while (1) 
    {
        char buf;
        
        uint8_t ble_data[6];
        int b_data_counter=0;
       
        if(hm10.readable()){
            hm10.read(&buf, 1);
            //pc.write(&buf,1);
            if(buf != '\n' || buf=='\r'){
                cmd[idx]=buf;
                idx += 1;
            }
            else if(buf=='\n'||buf=='\r'){
                cmd[idx]=buf;
                //pc.write(cmd,idx+1);
                idx=0;
                char temp[40];
                int temp_idx=0;
                for(int k=0; k < sizeof(cmd); k++){
                    if(b_data_counter<6){
                        if (cmd[k] >= '0' && cmd[k] <= '9'){
                            temp[temp_idx]=cmd[k];
                            temp_idx+=1;
                        }
                        else if(cmd[k]==','){
                            temp[temp_idx]='\0';
                            ble_data[b_data_counter]=atoi(temp);
                            b_data_counter += 1;
                            char temp[40];
                            temp_idx=0;
                        }
                        else if(cmd[k]=='\r' || cmd[k]=='\n'){
                            temp[temp_idx]='\0';
                            ble_data[b_data_counter]=atoi(temp);
                            break;
                        }
                    }
                    else {
                        break;
                    }
                }
                char DaTa[21];
                //sprintf(DaTa, "\r\n%d,%d,%d,%d,%d,%d\r\n",ble_data[0],ble_data[1],ble_data[2],ble_data[3],ble_data[4],ble_data[5]);
                //pc.write(DaTa,sizeof(DaTa));
                char cmd[40];
                
           
                Accel_X_RAW = (int16_t) (ble_data[0] << 8 | ble_data[1]);
                Accel_Y_RAW = (int16_t) (ble_data[2] << 8 | ble_data[3]);
                Accel_Z_RAW = (int16_t) (ble_data[4] << 8 | ble_data[5]);
                Ax= Accel_X_RAW/16384.0;
                Ay= Accel_Y_RAW/16384.0;
                Az= Accel_Z_RAW/16384.0;

                sprintf(buff, "\r\n%f,%f,%f\r\n", Ax,Ay,Az);
                pc.write(buff,sizeof(buff));

                if((Ax <= -0.2))
                {
                    moveAlpha(stack,1);
                    //moveForward(stack);
                    turnOnLED(1);
                }
                else if((0.2 <= Ax))
                {
                    moveAlpha(stack, 2);
                    // moveBackward(stack);
                    turnOnLED(2);
                }
                else if((0.2 <= Ay))
                {
                    moveAlpha(stack, 3);
                    // turnRight(stack); 
                    turnOnLED(3);
                }
                else if(Ay <= -0.2)
                {
                    moveAlpha(stack, 4);
                    //turnLeft(stack); 
                    turnOnLED(4);
                }
                else if(stack != 0)
                {
                    // stopMoving(stack);
                    moveAlpha(stack, pre_direction);
                    turnOnLED(0);
                }
                else{   
                    moveAlpha(stack, 0);
                    // stopMoving(stack);
                    turnOnLED(0);
                }
            }

        }
        wait_us(1000);
    }
}

// void turnOnLED(int direction)
// {
//     // wait_us(50000);
//     ws2812_pixel_all(0,0,0);
//     // ws2812_send_spi();
//     // wait_us(50000);
//     int g=0,r=0,b=0;

//     if (pre_direction == 1) {
//         g=1;
//     }
//     else if (pre_direction ==2) {
//         b=1; 
//     }
//     else if (pre_direction ==3) {
//         g=1;
//         r=1;
//         b=1;
//     }
//     else if (pre_direction == 4) {
//         r=1;
//     }

//     int i;
//     if(direction == 0)
//         stack--;
//     else if(pre_direction == direction)
//         stack++;
//     else
//         stack=0;

//     if (stack < 0)
//         stack = 0;
//     if (stack >7)
//         stack = 7;

//     for(i=0;i<=stack;i++){
//         if(pre_direction != 3)
//             ws2812_pixel(i,g,r,b);//G-R-B
//         else    // right side first
//             ws2812_pixel(7-i,g,r,b);
//         ws2812_send_spi();
//     }
//     if(direction != 0)
//         pre_direction = direction;
    
// }


void turnOnLED(int direction)
{
    // wait_us(50000);
    ws2812_pixel_all(0,0,0);
    // ws2812_send_spi();
    // wait_us(50000);
    int g=0,r=0,b=0;

    if (direction == 1) {
        g=1;
    }
    else if (direction ==2) {
        b=1; 
    }
    else if (direction ==3) {
        g=1;
        r=1;
        b=1;
    }
    else if (direction == 4) {
        r=1;
    }

    int i;
    if(pre_direction == direction)
        stack++;
    else
        stack=0;

    if (stack < 0)
        stack = 0;
    if (stack >7)
        stack = 7;

    for(i=0;i<=stack;i++){
        if(direction != 3)
            ws2812_pixel(i,g,r,b);//G-R-B
        else    // right side first
            ws2812_pixel(7-i,g,r,b);
        ws2812_send_spi();
    }
    pre_direction = direction;
    // wait_us(50000);
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




    


