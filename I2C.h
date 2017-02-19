/////////////////////////////////////////////
// Systemy Mikroprocesorowe II             //
// Projekt zaliczeniowy                    //
// Filip Polednia, Mateusz Kaczmarczyk     //
// Elektronika III                         //
// AGH                                     //
/////////////////////////////////////////////
#include "MKL46Z4.h" 

void I2C_Init(I2C_Type* i2c);

void I2C_Start(void);
void I2C_Stop(void);	
void cpu_pause(int length);
void I2C_Wait(void);
void I2C_restart(void);
void I2C_rx_mode(void);
void I2C_disable_ack(void);
void I2C_enable_ack(void);

void I2C_WriteRegister(unsigned char device_adress, unsigned char register_adress, char data);
void I2C_WriteAdress(unsigned char device_adress);
unsigned char I2C_ReadRegister(unsigned char device_adress, unsigned char register_adress);
void I2C_ReadMultiRegisters(unsigned char device_adress, unsigned char register_adress, unsigned char number, unsigned char *save_adress);

///////////////////////////
//	    EOF		 //
///////////////////////////
