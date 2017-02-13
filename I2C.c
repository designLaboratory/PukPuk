/////////////////////////////////////////////
// Systemy Mikroprocesorowe II             //
// Projekt zaliczeniowy                    //
// Filip Polednia, Mateusz Kaczmarczyk     //
// Elektronika III                         //
// AGH                                     //
/////////////////////////////////////////////

#include "MKL46Z4.h"

#include "i2c.h"
#include "slcd.h"

#define I2C0_ALARM (IRQn_Type) 8

//i2c initialize
void I2C0_Init(){          ////////////////////////////////////////////////////////////////////nwm wtf////////////////////////
	//SIM->SCGC5  |=  SIM_SCGC5_PORTC_MASK;	// Wlaczenie zegara dla portu C
	/*PORTC->PCR[8] |= PORT_PCR_MUX(2);
	PORTC->PCR[9] |= PORT_PCR_MUX(2);
	PORTE->PCR[24] = PORT_PCR_MUX(5);
	PORTE->PCR[25] = PORT_PCR_MUX(5);*/
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[2] = PORT_PCR_MUX(2UL);
	PORTB->PCR[3] = PORT_PCR_MUX(2UL);
	
	SIM->CLKDIV1 |= ((1u<<17) | (1u<<16)); 	//bus clock is 24/8 = 4MHz
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK; 		//podpiecie zegara do I2C0
	I2C0->F   = 0x16;                     // baudrate: ~98kHz
	I2C0->C1 |= I2C_C1_IICEN_MASK;
	//NVIC_ClearPendingIRQ(I2C0_ALARM); 		//wyczyszczenie przerwania nvic
	//NVIC_EnableIRQ(I2C0_ALARM);				//nvic enable
	I2C0->C1 |= I2C_C1_IICIE_MASK;
}

//disable i2c module
void I2C_Disable(I2C_Type* i2c){
	i2c->C1 &= ~I2C_C1_IICEN_MASK; //module enable mask - negation
}

//disable i2c interrupts
void I2C_DisableInt(I2C_Type* i2c){
	i2c->C1 &= ~I2C_C1_IICIE_MASK; //interrupt enable mask - negation
}

//enable i2c module
void I2C_Enable(I2C_Type* i2c){
	i2c->C1 |= I2C_C1_IICEN_MASK; //module enable mask
}

//enable i2c interrupts
void I2C_EnableInt(I2C_Type* i2c){
	i2c->C1 |= I2C_C1_IICIE_MASK; //interrupt enable mask
}

//read byte (with i2c module selection)
uint8_t I2C_ReadByte(I2C_Type* i2c, uint8_t ack){
	
	i2c->C1 &= ~I2C_C1_TX_MASK; //transmission mode disable
  
	if((i2c->SMB & I2C_SMB_FACK_MASK) == 0) // prepare ACK/NACK bit if FACK == 0
		i2c->C1 = (ack == I2C_NACK) ? i2c->C1 | I2C_C1_TXAK_MASK : i2c->C1 & ~I2C_C1_TXAK_MASK;
  
	i2c->S |= I2C_S_IICIF_MASK; //clear IICIF flag
  
	while((i2c->S & I2C_S_IICIF_MASK) == 0);  // waiting for the end of transmission

  
	if((i2c->SMB & I2C_SMB_FACK_MASK) != 0)   // send ACK/NACK bit if FACK == 1
		i2c->C1 = (ack == I2C_NACK) ? i2c->C1 | I2C_C1_TXAK_MASK : i2c->C1 & ~I2C_C1_TXAK_MASK;
  
	return i2c->D; //return received data
}

//i2c restart
void I2C_Restart(I2C_Type* i2c){
  i2c->C1 |= I2C_C1_RSTA_MASK; //restart i2c module
}

//i2c start (transmission mode)
void I2C_Start(I2C_Type* i2c){
	i2c->C1 |= I2C_C1_TX_MASK; //transmission mode 
	i2c->C1 |= I2C_C1_MST_MASK; //master mode
}

//i2c stop 
void I2C_Stop(I2C_Type * i2c){
  
  i2c->FLT |= I2C_FLT_STOPF_MASK; //clear STOP flag
  
  i2c->C1 &= ~I2C_C1_MST_MASK;  //send stop bit

  while((i2c->FLT & I2C_FLT_STOPF_MASK) == 0){ // wait for the end of transmission of stop bit
    i2c->C1 &= ~I2C_C1_MST_MASK;
  }
}

//write byte (with i2c module selection)
uint8_t I2C_WriteByte(I2C_Type * i2c, uint8_t data){
  I2C_Start(i2c);   //transmission mode 
  
  i2c->S |= I2C_S_TCF_MASK; //clear transmission complete flag 
  
  i2c->D = data; //send data
	
  while((i2c->S & I2C_S_TCF_MASK) == 0); //wait for the end of transmission
  
  return (i2c->S & I2C_S_RXAK_MASK) == I2C_S_RXAK_MASK ? 0 : 1); //return ack bit
}

//handler for i2c0 interrupt
void I2C0_IRQHandler(){
	//code here plox
}

/////////////////////tego nie wiem/////////////////////////////////////
void I2C_ReadMultiRegisters(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress, unsigned char n, unsigned char *r){
	char i;
	
	I2C0_Init();	          
	I2C0->D = u8SlaveAddress << 1;									/* Send I2C device address with W/R bit = 0 */
	// Oczekiwanie na zakonczenie transferu
	while((I2C0->S & I2C_S_IICIF_MASK) == 0);										

	I2C0->D = u8RegisterAddress;										/* Send register address */
	// Oczekiwanie na zakonczenie transferu
	while((I2C0->S & I2C_S_IICIF_MASK) == 0);

	I2C0->C1 |= I2C_C1_RSTA_MASK;							//restart
		
	I2C0->D = (u8SlaveAddress << 1) | 0x01;							/* Send I2C device address this time with W/R bit = 1 */ 
	// Oczekiwanie na zakonczenie transferu
	while((I2C0->S & I2C_S_IICIF_MASK) == 0);	

	I2C0->C1 &= ~I2C_C1_TX_MASK; 		//tryb odbierania
	I2C0->C1 &= ~I2C_C1_TXAK_MASK;		//wlaczenie potwierdzen
	
	i = I2C0->D;
	// Oczekiwanie na zakonczenie transferu
	while((I2C0->S & I2C_S_IICIF_MASK) == 0);	
	
	for(i=0; i<n-2; i++) 
	{
	    *r = I2C0->D;
	    r++;
	    // Oczekiwanie na zakonczenie transferu
		while((I2C0->S & I2C_S_IICIF_MASK) == 0);	
	}
	
	I2C0->C1 |= I2C_C1_TXAK_MASK;
	*r = I2C0->D;
	r++;
	// Oczekiwanie na zakonczenie transferu
	while((I2C0->S & I2C_S_IICIF_MASK) == 0);
	I2C0->C1 &= ~I2C_C1_MST_MASK;
    I2C0->C1 &= ~I2C_C1_TX_MASK;
	*r = I2C0->D; 		
}



void I2C_WriteRegister(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress, /*unsigned*/ char u8Data){
	I2C0->C1 |= I2C_C1_TX_MASK;
    I2C0->C1 |= I2C_C1_MST_MASK;	          
	I2C0->D = u8SlaveAddress << 1;									/* Send I2C device address with W/R bit = 0 */
	// Oczekiwanie na zakonczenie transferu
	while((I2C0->S & I2C_S_TCF_MASK) == 0);
	

	I2C0->D = u8RegisterAddress;										/* Send register address */
	// Oczekiwanie na zakonczenie transferu
	while((I2C0->S & I2C_S_TCF_MASK) == 0);

	I2C0->D = u8Data;												/* Send the data */
	// Oczekiwanie na zakonczenie transferu
	while((I2C0->S & I2C_S_TCF_MASK) == 0);

	I2C0->C1 &= ~I2C_C1_MST_MASK;
    I2C0->C1 &= ~I2C_C1_TX_MASK;
}

char I2C_ReadRegister(unsigned char u8SlaveAddress, unsigned char u8RegisterAddress){
	char result;
	  
	I2C0->C1 |= I2C_C1_TX_MASK;
    I2C0->C1 |= I2C_C1_MST_MASK;		          
	I2C0->D = u8SlaveAddress << 1;									/* Send I2C device address with W/R bit = 0 */
	// Oczekiwanie na zakonczenie transferu
	while((I2C0->S & I2C_S_TCF_MASK) == 0);										

	I2C0->D = u8RegisterAddress;										/* Send register address */
	// Oczekiwanie na zakonczenie transferu
	while((I2C0->S & I2C_S_IICIF_MASK) == 0);

	I2C0->C1 |= I2C_C1_RSTA_MASK;
	
	I2C0->D = (u8SlaveAddress << 1) | 0x01;							/* Send I2C device address this time with W/R bit = 1 */ 
	// Oczekiwanie na zakonczenie transferu
	while((I2C0->S & I2C_S_IICIF_MASK) == 0);	

	I2C0->C1 &= ~I2C_C1_TX_MASK;
	I2C0->C1 |= I2C_C1_TXAK_MASK;

	result = I2C0->D;												
	// Oczekiwanie na zakonczenie transferu
	while((I2C0->S & I2C_S_IICIF_MASK) == 0);
	I2C0->C1 &= ~I2C_C1_MST_MASK;
    I2C0->C1 &= ~I2C_C1_TX_MASK; 
	result = I2C0->D;
	return result;
}
