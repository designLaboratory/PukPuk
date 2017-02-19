#include "MMA.h"
#include "I2C.h"

void Accelerometer_Init (void){
	uint8_t reg_val = 0;
	
	I2C_WriteRegister(MMA845x_I2C_ADDRESS, CTRL_REG2, 0x40);		// Reset all MMA registers
	
	do		// Wait till the end of reset
	{
		reg_val = I2C_ReadRegister(MMA845x_I2C_ADDRESS, CTRL_REG2) & 0x40; 
	} 	while (reg_val);
	
	I2C_WriteRegister(MMA845x_I2C_ADDRESS, XYZ_DATA_CFG_REG, 0x00);	// Set 2g range
	
	I2C_WriteRegister(MMA845x_I2C_ADDRESS, PULSE_CFG_REG, 0x71);	// Enable event flags and double events on Z and X axis
	I2C_WriteRegister(MMA845x_I2C_ADDRESS, PULSE_THSZ_REG, 0x50);	// Z tap thereshold equal 
	I2C_WriteRegister(MMA845x_I2C_ADDRESS, PULSE_THSX_REG, 0x40);	// X tap thereshold equal 
	I2C_WriteRegister(MMA845x_I2C_ADDRESS, PULSE_TMLT_REG, 0x06);	// Pulse limit set for 3.75ms
	I2C_WriteRegister(MMA845x_I2C_ADDRESS, PULSE_WIND_REG, 0x50);	// 180 ms double tap window
	
	I2C_WriteRegister(MMA845x_I2C_ADDRESS, CTRL_REG3, 0x00);		// Push-pull, active low interrupt 
	I2C_WriteRegister(MMA845x_I2C_ADDRESS, CTRL_REG4, 0x09);		// Enable Data ready and PULSE interrupt 
	I2C_WriteRegister(MMA845x_I2C_ADDRESS, CTRL_REG5, 0x01);		// Data ready INT1 - PTC5, PULSE interrupt INT2 - PTD1 
	
	I2C_WriteRegister(MMA845x_I2C_ADDRESS, CTRL_REG1, 0xC5);		// ODR = 800Hz, Reduced noise, Active mode	
}
