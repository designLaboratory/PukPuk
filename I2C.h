
#include "MKL46Z4.h"

typedef enum{
  I2C_ACK = 0,
  I2C_NACK = 1,
} I2C_Acknowledge_Bit;

void    i2c_Disable(I2C_Type* i2c);
void    i2c_Disable_Interrupt(I2C_Type* i2c);
void    i2c_Enable(I2C_Type* i2c);
void    i2c_Enable_Interrupt(I2C_Type* i2c);
uint8_t i2c_ReadByte(I2C_Type* i2c, uint8_t ack);
void    i2c_Restart(I2C_Type* i2c);
void    i2c_Start(I2C_Type* i2c);
void    i2c_Stop(I2C_Type* i2c);
uint8_t i2c_WriteByte(I2C_Type* i2c, uint8_t data);