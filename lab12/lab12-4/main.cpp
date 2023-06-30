#include "mbed.h"
#include "Adafruit_SSD1306.h"
#define HTU21D_TEMP_READ (0xF3) // no hold master temp meas
#define HTU21D_HUMI_READ (0xF5) // no hold master humid meas    
#define HTU21G_USER_WRITE (0xE6) // write user regiser
#define HTU21D_ADDR (0x40) 
I2C i2c(PB_9, PB_8) ;

Adafruit_SSD1306_Spi gOLED(ARDUINO_UNO_D11, ARDUINO_UNO_D13, ARDUINO_UNO_D10, ARDUINO_UNO_D4, ARDUINO_UNO_D7, 64);

int main()
{
    float humid ;
    float temperature ;
    int tmp ;
    ThisThread::sleep_for(5000ms);
    gOLED.clearDisplay();
    gOLED.setTextSize(2);
    gOLED.printf("YeSeung\r\n\n");
    gOLED.setTextSize(1);

    gOLED.printf("Temp:\r\n\n");
    gOLED.printf("Humid:");

    while (1) {
        char cmd[1];
        cmd[0] = HTU21D_TEMP_READ;
        char data[2];
        i2c.write(HTU21D_ADDR << 1, cmd, 1, true);
        ThisThread::sleep_for(50ms); // wait for measurement
        i2c.read(HTU21D_ADDR << 1, data, 2, true);
        uint16_t raw_temp = ((uint16_t)data[0] << 8) | (uint16_t)data[1];
        raw_temp &= 0xFFFC ;
        float temp = -46.85 + 175.72 * ((float)raw_temp / 65536.0);

        cmd[0] = HTU21D_HUMI_READ;
        i2c.write(HTU21D_ADDR << 1, cmd, 1, true);
        ThisThread::sleep_for(50ms); // wait for measurement
        i2c.read(HTU21D_ADDR << 1, data, 2, true);
        uint16_t raw_humi = ((uint16_t)data[0] << 8) | (uint16_t)data[1];
        raw_humi &= 0xFFF0 ;
        float humi = -6.0 + 125.0 * ((float)raw_humi / 65536.0);
        
        
        gOLED.setTextCursor(13*6, 4*8);
        gOLED.printf("%.1f C", temp);

        gOLED.setTextCursor(13*6, 6*8);
        gOLED.printf("%.1f %%", humi);

        gOLED.display();
        ThisThread::sleep_for(5000ms);
        }
}