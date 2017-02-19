/////////////////////////////////////////////
// Systemy Mikroprocesorowe II             //
// Projekt zaliczeniowy                    //
// Filip Polednia, Mateusz Kaczmarczyk     //
// Elektronika III                         //
// AGH                                     //
/////////////////////////////////////////////

#include "MKL46Z4.h"
#include "I2C.h"

void I2C_Init(I2C_Type* i2c)
{
	SIM -> SCGC5 |= SIM_SCGC5_PORTE_MASK;		//enabling port E clock
	
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;		//enabling I2C0 clock
	PORTE->PCR[24] |= PORT_PCR_MUX(5);		//PORT mux selection for internal accelerometer
	PORTE->PCR[25] |= PORT_PCR_MUX(5);		
	PORTE->PCR[18] |= PORT_PCR_MUX(4);
	PORTE->PCR[19] |= PORT_PCR_MUX(4);
	
	//SIM->CLKDIV1 |= ((1u<<17) | (1u<<16)); 	//bus clock is 24/8 = 4MHz
	i2c->F   = 0x14;                     		// baudrate: ~98kHz
	I2C0->C1 = I2C_C1_IICEN_MASK;			//i2c module enable
	
	/////////////////
	//  nvic init  //
	/////////////////
	//NVIC_ClearPendingIRQ(I2C0_ALARM); 		//clear nvic interrupts
	//NVIC_EnableIRQ(I2C0_ALARM);			//nvic enable
	//i2c->C1 |= I2C_C1_IICIE_MASK;			//enable i2c interrupts
}

void I2C_Start(void)
{
	I2C0->C1 |= I2C_C1_TX_MASK; 			//enter tx mode
	I2C0->C1 |= I2C_C1_MST_MASK;			//enter master mode -> generate start bit
}

void I2C_Stop(void)
{
	I2C0->C1 &= ~I2C_C1_MST_MASK;			//master mode off -> generate stop bit
	I2C0->C1 &= ~I2C_C1_TX_MASK;			//turn off tx mode
}

void cpu_pause(int length)
{
	int temp;
	for(temp=0; temp<length; temp++)
	{
			__nop();	//wait *lenght* number of cpu ticks
	};
}

void I2C_Wait(void){
	while((I2C0->S & I2C_S_IICIF_MASK)==0) {}	//wait for the end of transmission
	I2C0->S |= I2C_S_IICIF_MASK;			//clear IICIF flag
	}

void I2C_restart(void){
	I2C0->C1 |= I2C_C1_RSTA_MASK;	//restart i2c module
}

void I2C_rx_mode(void)
{
	I2C0->C1 &= ~I2C_C1_TX_MASK;	//turn off tx mode
}

void I2C_disable_ack(void)
{
       I2C0->C1 |= I2C_C1_TXAK_MASK;	//disable ack (0 - on, 1 - off)
}

void I2C_enable_ack(void)
{
        I2C0->C1 &= ~I2C_C1_TXAK_MASK;	//enable ack (0 - on, 1 - off)
}

void I2C_WriteRegister(unsigned char device_adress, unsigned char register_adress, char data)
{
	I2C_Start();				//switch to master -> send start bit	          
	I2C0->D = device_adress << 1;		//send device adress (select slave). adress w/ left shift -> write bit = 0
	I2C_Wait();				//wait for the end of transmission

	I2C0->D = register_adress;		//send register adress
	I2C_Wait();				//wait for the end of transmission

	I2C0->D = data;				//send data
	I2C_Wait();				//wait for the end of transmission

	I2C_Stop();				//master off -> send stop bit

    	cpu_pause(50);				//wait 50 cpu ticks
}

void I2C_WriteAdress(unsigned char device_adress)	//to check if we connected with device (checking ACK bit)
{
	I2C_Start();	          		//switch to master -> send start bit
	I2C0->D = device_adress << 1;		//send device adress (select slave). adress w/ left shift -> write bit = 0
	I2C_Wait();				//wait for the end of transmission

	I2C_Stop();				//master off -> send stop bit

    	cpu_pause(50);				//wait 50 cpu ticks
}

unsigned char I2C_ReadRegister(unsigned char device_adress, unsigned char register_adress)
{
	unsigned char reg;			//variable collecting received data

	I2C_Start();	          		//switch to master -> send start bit
	I2C0->D = device_adress << 1;		//send device adress (select slave). adress w/ left shift ->write bit = 0
	I2C_Wait();				//wait for the end of transmission

	I2C0->D = register_adress;		//send register adress
	I2C_Wait();				//wait for the end of transmission

	I2C_restart();

	I2C0->D = (device_adress << 1) | 0x01;	//send device adress (select slave). adress w/ left shift +1 -> write bit = 1
	I2C_Wait();				//wait for the end of transmission

	I2C_rx_mode();				//enter receive mode
	I2C_disable_ack();			//disable sending ack bit

	reg = I2C0->D;			//read register & save in variable

	I2C_Wait();				//wait for the end of transmission
	I2C_Stop(); 				//master off -> send stop bit
	reg = I2C0->D; 			//rewrite to variable

	cpu_pause(50);				//wait 50 cpu ticks
	return reg;			//return data
}

void I2C_ReadMultiRegisters(unsigned char device_adress, unsigned char register_adress, unsigned char number, unsigned char *save_adress)
{
	char i;					//loop iterator
	
	I2C_Start();	          		//switch to master -> send start bit
	I2C0->D = device_adress << 1;		//send device adress (select slave). adress w/ left shift ->write bit = 0
	I2C_Wait();				//wait for the end of transmission

	I2C0->D = register_adress;		//send register adress
	I2C_Wait();				//wait for the end of transmission

	I2C_restart();

	I2C0->D = (device_adress << 1) | 0x01;	//send device adress (select slave). adress w/ left shift +1 -> write bit = 1
	I2C_Wait();				//wait for the end of transmission

	I2C_rx_mode();				//enter receive mode
	I2C_enable_ack();			//enable sending ack bit (*)

	i = I2C0->D;				//read first send register
	I2C_Wait();				//wait for the end of transmission

	for(i = 0; i < number - 2; i++) 	//loop reading registers (**)
	{
	    *save_adress = I2C0->D;		//save received data to save_adress
	    save_adress++;			//move save_adress
	    I2C_Wait();				//wait for the end of transmission
	}
	
	I2C_disable_ack();			//disable sending ack bit (***)
	*save_adress = I2C0->D;			//save received data to save_adress (****)
	save_adress++;				//move save_adress
	I2C_Wait();				//wait for the end of transmission
	
	I2C_Stop();				//master off -> send stop bit
	*save_adress = I2C0->D; 		//rewrite to variable
	cpu_pause(50);				//wait 50 cpu ticks

	//(*) every received register, we send ack bit to slave. after execution of loop (**), we disable ack (due to datasheet, it will turn off after next received data -> (****)).
	//(**) when *number* = 2: loop is omitted, first register save after (***), ack will be disabled after next register (so slave will send next register), second data save from I2C0->D 	buffer, after stop bit.
}	

/////////////////////////
//	    EOF	       //
/////////////////////////
