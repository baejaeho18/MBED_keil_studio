#include "mbed.h"

#define MPU6050_ADDR 0xD0
// mpu address --> 110100=68
  
#define SMPLT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0X41
#define GYRO_XOUT_H_REG 0X43
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75


 
void pin_setup();
void mpu_setup();
void mpu_write(uint8_t Address, uint8_t Reg , uint8_t Data);
void mpu_read(uint8_t Address, uint8_t Reg , uint8_t *buffer, uint8_t size);


void I2C_Config (void);
void I2C_Write(uint8_t data);
void I2C_Start (void);
void I2C_Address (uint8_t Address);
void I2C_Stop(void);
void I2C_WriteMulti (uint8_t * data, uint8_t size);
void I2C_Read (uint8_t Address, uint8_t *buffer, uint8_t size);