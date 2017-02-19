#include "MKL46Z4.h" 				// Reg names
#include "I2C.h"
#include "slcd.h"
#include "leds.h"
#include "MMA.h"




#define PORTC_D_IRQ_NBR (IRQn_Type) 31

/*---------------------------------------------------------------------------
- Global variables
---------------------------------------------------------------------------*/

unsigned char RawData[6];
uint16_t Zaccel, i, j = 0;
uint8_t DataReady, TapDetected, PulseSrcReceived = 0;


/*---------------------------------------------------------------------------
- Setup
---------------------------------------------------------------------------*/

void Setup(void){
	I2C_Init(I2C0);							// I2C0 initialization
	Accelerometer_Init();					// Accelerometer initialization 
	slcdInitialize();						// slcd initialization
	ledsInitialize();						// leds initialization
	
	// Interrupt pins init
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK |
				  SIM_SCGC5_PORTD_MASK;		// Clock for Port C & D
	
	PORTC->PCR[5] |= (PORT_PCR_ISF_MASK|	// Clear interrupt flag 
					  PORT_PCR_MUX(0x1)|	// PTC5 as GPIO 
					  PORT_PCR_IRQC(0xA));	// PTC5 falling edge interrupts 
	
	PORTD->PCR[1] |= (PORT_PCR_ISF_MASK|	// Clear the interrupt flag 
					  PORT_PCR_MUX(0x1)|	// PTD1 as GPIO 
					  PORT_PCR_IRQC(0xA));	// PTD1	falling edge interrupts
	
	
	//NVIC enable PORTC&D interrupt
	NVIC_ClearPendingIRQ(PORTC_D_IRQ_NBR);	// Clear pending interrupts for PORTC&D
	NVIC_EnableIRQ(PORTC_D_IRQ_NBR);		// Enable NVIC interrupts for PORTC&D
	NVIC_SetPriority (PORTC_D_IRQ_NBR, 3);
	
	
	
	slcdDisplay(9999,10);					// Show 9999 on slcd at the end of setup
}

/*---------------------------------------------------------------------------
- Main
---------------------------------------------------------------------------*/
int main (void) {
	Setup();
	
  		
  	while(1) {	
		__wfi();
	}

}




/*---------------------------------------------------------------------------
- Interrupt handlers
---------------------------------------------------------------------------*/

void PORTC_PORTD_IRQHandler(){
	if (PORTC -> ISFR & (1<<5)){					// New data ready interrupt
		PORTC->PCR[5] |= PORT_PCR_ISF_MASK;			// Clear the interrupt flag 
		I2C_ReadMultiRegisters(MMA845x_I2C_ADDRESS, OUT_X_MSB_REG, 6, RawData);	// 3 axes accel data from snesor
		Zaccel = (((uint16_t) (RawData[4]<<8 | RawData[5])) >> 2) - 4096;			// Get Z-axis value from two bytes of data
		
		///////// Uncomment to show raw Z data
		//slcdDisplay(Zaccel,16);
	}
	else if (PORTD -> ISFR & (1<<1)){				// Tap detection interrupt
		PORTD -> PCR[1] |= PORT_PCR_ISF_MASK;		// Clear interrupt flag
		PulseSrcReceived = I2C_ReadRegister(MMA845x_I2C_ADDRESS, PULSE_SRC_REG);	// Get PULSE data
			if ((PulseSrcReceived & 0x40) && (PulseSrcReceived & 0x08)){	// Double Z axis tap
				i = 0;				//Clear counters
				j = 0;
				ledsOn();			// Leds sequence
				delay_mc(300);
				ledsOff();
				delay_mc(150);
				ledsOn();
				delay_mc(150);
				ledsOff();
			}
			else if (PulseSrcReceived & 0x40){	// Single Z axis tap
				i++;
				ledsToggle();
			}
			
			
			if ((PulseSrcReceived & 0x10) && (PulseSrcReceived & 0x01)){	// Single, right-side X axis tap
				j++;
				ledsGreenToggle();
			}
			else if (PulseSrcReceived & 0x10){	// Single, left-side X axis tap
				j--;
				ledsRedToggle();
			}
			
			///////// Uncomment to show PULSE counts data
			slcdDisplay(((j<<8)|i),16);	//Display j on two left digits and i on right two				
	}
	
}
