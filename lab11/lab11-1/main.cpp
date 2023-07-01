#include "mbed.h"

#define HTU21D_TEMP_READ (0xF3) // no hold master temp meas
#define HTU21D_HUMI_READ (0xF5) // no hold master humid meas    
#define HTU21G_USER_WRITE (0xE6) // write user regiser
#define HTU21D_ADDR (0x40) 
I2C i2c(PB_9, PB_8) ;
BufferedSerial pc(CONSOLE_TX, CONSOLE_RX, 115200) ;
DigitalIn button(BUTTON1) ;
char buffer[80] ;

uint8_t calculate_crc(uint8_t *data, size_t len)
{
    uint8_t crc = 0 ;
    for (size_t i = 0 ; i < len ; i++) 
    {
        crc ^= data[i] ;
        for (uint8_t bit = 8 ; bit > 0 ; bit--)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x131 ; // 0141 = x^8 + x^5 + x^4 + 1 (CRC polynomial)
            else
                crc = (crc << 1) ;
        }
    }
    return crc ;
}
 
int main()
{
    float humid ;
    float temperature ;
    
    sprintf(buffer, "Mbed OS version %d.%d.%d\r\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION) ;
    pc.write(buffer, strlen(buffer)) ;   
    sprintf(buffer,"\r\n Welcome to HTU21D I2C lab. 11_1!\r\n") ;
    pc.write(buffer, strlen(buffer)) ; 
    while (true)
    {
        char cmd[1] ;
        cmd[0] = HTU21D_TEMP_READ ;
        char data[3] ;
        i2c.write(HTU21D_ADDR << 1, cmd, 1, true) ;
        ThisThread::sleep_for(50ms) ; // wait for measurement
        i2c.read(HTU21D_ADDR << 1, data, 3, true) ;
        if (!button)
            data[0] += 1 ;  
        if (calculate_crc((uint8_t*)data, 2) != data[2])
        {
            sprintf(buffer, "Temperature CRC Error\r\n") ;
            pc.write(buffer, strlen(buffer)) ;
        }
        else
        {
                uint16_t raw_temp = ((uint16_t)data[0] << 8) | (uint16_t)data[1];
                raw_temp &= 0xFFFC;
                float temp = -46.85 + 175.72 * ((float)raw_temp / 65536.0);
                sprintf(buffer, "Temperature = %.2f [C]\r\n", temp);
                pc.write(buffer, strlen(buffer));
        }

        cmd[0] = HTU21D_HUMI_READ;
        i2c.write(HTU21D_ADDR << 1, cmd, 1, true);
        ThisThread::sleep_for(50ms); // wait for measurement
        i2c.read(HTU21D_ADDR << 1, data, 2, true);
        if (!button) { 
            data[0] += 1;  
        }
        if (calculate_crc((uint8_t*)data, 2) != data[2]) {
            sprintf(buffer, "Humidity CRC Error\r\n");
            pc.write(buffer, strlen(buffer));
        }
        else {
                uint16_t raw_humi = ((uint16_t)data[0] << 8) | (uint16_t)data[1];
                raw_humi &= 0xFFF0 ;
                float humi = -6.0 + 125.0 * ((float)raw_humi / 65536.0);
                sprintf(buffer, "Relative Humidity = %.2f [%%]\r\n",humi);
                pc.write(buffer, strlen(buffer));
        }

        ThisThread::sleep_for(3000ms);
    }
}
