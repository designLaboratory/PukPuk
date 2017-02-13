/////////////////////////////////////////////
// Systemy Mikroprocesorowe II             //
// Projekt zaliczeniowy                    //
// Filip Polednia, Mateusz Kaczmarczyk     //
// Elektronika III                         //
// AGH                                     //
/////////////////////////////////////////////

#include "MKL46Z4.h"

typedef enum{ //define enum data type
  I2C_ACK = 0,
  I2C_NACK = 1,
} I2C_Acknowledge_Bit;

//i2c functions
void	I2C0_Init();
void    I2C_Disable(I2C_Type* i2c);
void    I2C_DisableInt(I2C_Type* i2c);
void    I2C_Enable(I2C_Type* i2c);
void    I2C_EnableInt(I2C_Type* i2c);
uint8_t I2C_ReadByte(I2C_Type* i2c, uint8_t ack);
void    I2C_Restart(I2C_Type* i2c);
void    I2C_Start(I2C_Type* i2c);
void    I2C_Stop(I2C_Type* i2c);
uint8_t I2C_WriteByte(uint8_t data);
char	I2C_ReadRegister(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress);
void 	I2C_WriteRegister(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress, /*unsigned*/ char u8Data);
void 	I2C_ReadMultiRegisters(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress, unsigned char n, unsigned char *r);
void 	I2C0_IRQHandler();

//////////////////////////
//         eof          //
//////////////////////////
