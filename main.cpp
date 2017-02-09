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
	//I2C0_Init();
	//I2C_Enable(I2C0);
	buttonsInitialize();
	TPM_Init();
}


uint8_t i = 0;
int main(){
	setup();

	while(1){
	if((counting == 0) && !button1Read()){
		TPM0->SC |= TPM_SC_CMOD(1);
		counting = 1;
		slcdDisplay(0,10);
	}
	else if((counting == 1) && (TPM0 -> SC & TPM_SC_TOF_MASK)){
		TPM0->SC |= TPM_SC_CMOD(0);
		counting = 0;
		TPM0 -> CNT = 0;
		TPM0 -> SC |=  TPM_SC_TOF_MASK;
		slcdDisplay(1,10);
	}
	else if((counting == 1) && !button1Read() && (TPM0 -> CNT > 125)){
		TPM0->SC |= TPM_SC_CMOD(0);
		counting = 0;
		TPM0 -> CNT = 0;
		TPM0 -> SC |=  TPM_SC_TOF_MASK;
		slcdDisplay(2,10);
		delay_mc(200);
	}
	if(counting)
		slcdDisplay(TPM0 -> CNT,10);
	}
	return 0;	
}
