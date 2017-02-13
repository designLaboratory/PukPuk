/////////////////////////////////////////////
// Systemy Mikroprocesorowe II             //
// Projekt zaliczeniowy                    //
// Filip Polednia, Mateusz Kaczmarczyk     //
// Elektronika III                         //
// AGH                                     //
/////////////////////////////////////////////


#include "MKL46Z4.h"
#include "i2c.h"
#include "lcd.h"
#include "MMA.h"
#include "buttons.c"
#include "TPM.h"

uint8_t counting = 0;
void setup(){
	slcdInitialize(); //initialize lcd display
	slcdClear(); //clear display
	I2C0_Init(); //initialize i2c
	I2C_Enable(I2C0); //i2c enable
	buttonsInitialize(); //initialize buttons
	TPM_Init(); //initialize timer module
}


int main(){
	setup(); //start setup
	I2C_Start(I2C0); //start i2c transmission
	I2C_WriteByte(0x68<<1); //send byte 
	return 0; //end	
}

/////////////////////////
//         eof         //
/////////////////////////
