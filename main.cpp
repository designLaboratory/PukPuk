#include "MKL46Z4.h"
#include "i2c.h"
#include "lcd.h"
#include "MMA.h"
#include "buttons.c"
#include "TPM.h"

uint8_t counting = 0;
void setup(){
	slcdInitialize();
	slcdClear();
	I2C0_Init();
	I2C_Enable(I2C0);
	buttonsInitialize();
	TPM_Init();
}


uint8_t i = 0;
int main(){
	setup();
	I2C_Start(I2C0);
	I2C_WriteByte(0x68<<1);
	return 0;	
}
