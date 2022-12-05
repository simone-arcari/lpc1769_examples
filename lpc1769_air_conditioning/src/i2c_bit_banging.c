/*
 *  Created on: 29 mag 2020
 *  Author: Simone Arcari
 *
 *
 *  This program emulates the i2c standard through the bit banging technique and uses the
 *  GPIO pins of the microcontroller lpc1769[ARM Cortex M-3, f_clock=96MHz].
 *  Repetitive Interrupt Timer [RIT] is used to generate the i2c communication clock.
 *	When using this library do not use P0.19 and P0.20 and Repetitive Interrupt Timer [RIT]
 *	for other purposes.
 *  User Manual: 	https://www.nxp.com/docs/en/user-guide/UM10360.pdf
 *
 *  NOTA: The RIT_IRQHandler(void) function is not declared because it is already
 *  declared within the cr_startup_lpc175_6x.c file.
 *
 * @file	i2c_bit_banging.h.h
 * @date	2020-05-29
 * @version	v1.0
 *
 */

#include "i2c_bit_banging.h"
#include "delay.h"

/* Variable declaration */
char SCL;													// Serial Clock logic value[SCL]
char SDA;													// Serial Data logic value[SDA]
int bit = 0;												// variable for bit count
int byte = 0;												// variable for byte count
int ACKM = 0;
int NOACKM = 0;
int status = 0;												// state variable for the state machine
int i2c_anable = 0;											// i2c communication activation variable
int I2CbufferCnt;											// variable to be used as an index for the buffer
unsigned char i2c_buffer[64];								// buffer containing address and data to be transmitted
unsigned char i2c_data_rec[64];								// buffer containing received data



/*!
 * @brief This internal API is used to initialize the GPIO pins and
 * i2c communication clock using the Repetitive Interrupt Timer [RIT].
 *
 * @param[in] Null.
 *
 */
void i2c_BB_init(){

	// GPIO settings
	FIO0MASK = 0xFFFFFFFF;
	FIO0DIR = 0x180000;										//set OUTPUT P0.19[SDA] e P0.20[SCL]

	// Repetitive Interrupt Timer settings
	PCONP |= 1<<16;											// enables Repetitive Interrupt Timer power/clock
	RICTRL |= 0b10;											// Set RITENCLR bit in RICTRL register
    ISER0 |= 1<<29;											// enables in NVIC interrupt reception from RTI
    IPR7 |= 0x0<<11;										// set the priority of RIT(interrupt)
	RICOMPVAL = 960;										// load counter for update frequency[100KHz]
}

/*!
 * @brief This internal API is used to start i2c communication.
 *
 * @param[in] Null.
 *
 */
void start_i2c(){

	FIO0MASK = ~(0b11<<19);									// set the mask to modify only the bits 19 and 20 of P0
	FIO0DIR = 0x180000;										// Set as OUTPUT P0.19 [SDA] and P0.20 [SCL]
	FIO0SET = 0b11<<19;										// sets bits 19 and 20 of P0 at high logic level [SDA & SCL]

	delayUs(15);											// delay of 30us

	FIO0MASK = ~(1<<19);									// set the mask to modify only bit 19 of P0
	FIO0CLR = 1<<19;										// set bit 19 of P0 at low logic level[SDA]

	delayUs(10);											// delay of 30us
}

/*!
 * @brief This internal API is used to stop i2c communication.
 *
 * @param[in] Null.
 *
 */
void stop_i2c(){

	delayUs(30);											//delay of 20us

	FIO0MASK = ~(0b11<<19);									// set the mask to modify only the bits 19 and 20 of P0
	FIO0DIR |= 0b11<<19;									// Set as OUTPUT P0.19 [SDA] and P0.20 [SCL]
	FIO0PIN = 0<<19;										// set bit 19 of P0 at low logic level[SDA]

	delayUs(5);												// delay of 30us

	FIO0PIN = 1<<20;										// sets bit 20 of P0 at high logic level [SCL]

	delayUs(15);											// delay of 30us

	FIO0MASK = ~(1<<19);									// set the mask to modify only bit 19 of P0
	FIO0SET = 1<<19;		  								// sets bit 19 of P0 at a high logic level[SDA]

	delayUs(10);											//delay of 30us

	FIO0MASK = ~(0b11<<19);									// set the mask to modify only the bits 19 and 20 of P0
	FIO0DIR &= ~(0b11<<19);									// set as INPUT pin 19 and 20 of P0
}

/*!
 * @brief This internal API is used to check the ACK bit
 * provided by the slave device.
 *
 * @param[in] Null.
 *
 */
void ACK(){

	FIO0MASK = ~(1<<19);									// set the mask to modify only bit 19 of P0
	FIO0DIR &= ~(1<<19);									// set as input pin 19 of P0[SDA]

	if(FIO0PIN & 0x80000)									// if the bit ACK is 1 (NACK) then we stop the communication because evidently it has failed
		status = 4;											// set the next state (STOP COMMUNICATION)
}


void RIT_IRQHandler(void){

	if(i2c_anable)
	{
		static int Counter = 7;								// it is used for bit operations as a counting variable

		// gestione SCL su P0.20
		SCL = !SCL;											// inversion of the logical value of the Clock
		FIO0MASK = ~(1<<20);								// set the mask to modify only the bit 20 of P0[SCL]
		FIO0PIN = SCL<<20;									// updates the value of P0.20[SCL]

		switch(status)
		{
			/* START SIGNAL */
			case 0:
				start_i2c();								// i2c communication start
				SCL = 1;									// declare the SCL value high because in the next cycle it will be inverted to the low value

				status = 1;									// set the next state
				bit = 0;									// reset variable
				byte = 0;									// reset variable
			break;

			/* TRANSMISSION OF THE ADDRESS */
			case 1:

				if(!SCL && Counter>=0)						// sends the bits only when SCL is 0, that is when it is allowed to change the state of the bits
				{
					if(i2c_buffer[byte] & 1<<Counter)		// extrapolates all the bits with each cycle
						SDA = 1;
					else
						SDA = 0;


					delayUs(20);							// delay of 20us
					FIO0MASK = ~(1<<19);					// set the mask to modify only the bit 19 of P0[SCL]
					FIO0PIN = SDA<<19;						// send the current bit
					Counter--;								// decrease variable
				}

				if(!SCL)
					bit++;									// variable that keeps count of the bits appeared on SDA

				if(bit == 9 && SCL)
				{
					if(SDA)
					{
						status = 3;							// set the next state (RECEPTION)
						byte = 0;							// if it receives, fill the array from the beginning
					}else{
						status = 2;							// set the next state (TRANSMISSION)
						byte = 1;							// if it receives, send the data of the array from the second byte since the first was the address
					}

					ACK();									// check ACK bit

					Counter = 7;							// reset variable
					bit = 0;								// reset variable
				}
			break;

			/* DATA TRANSMISSION */
			case 2:
				FIO0MASK = ~(1<<19);						// set the mask to modify only bit 19 of P0
				FIO0DIR |= 1<<19;		  					// sets pin 19 of P0 as output[SDA]

				if(!SCL && Counter>=0)						// sends the bits only when SCL is 0, that is when it is allowed to change the state of the bits
				{
					if(i2c_buffer[byte] & 1<<Counter)		// extrapolates all the bits with each cycle
						SDA = 1;
					else
						SDA = 0;

					delayUs(20);							// delay of 20us
					FIO0MASK = ~(1<<19);					// set the mask to modify only the bit 19 of P0[SCL]
					FIO0PIN = SDA<<19;						// send the current bit
					Counter--;								// decrease variable
				}

				if(!SCL)
					bit++;									// variable that keeps count of the bits appeared on SDA

				if(bit == 9 && SCL)
				{
					Counter = 7;							// reset variable
					bit = 0;								// reset variable
					byte++;									// increment variable

					if(byte > I2CbufferCnt)
					{
						byte = 0;							// reset variable
						status = 4;							// set the next state (STOP COMMUNICATION)
					}

					ACK();									// check ACK bit
				}
			break;

			/* RECEIVING DATA */
			case 3:
				if(SCL && Counter>=0)						// receives bits only when SCL is 1
				{
					FIO0MASK = ~(1<<19);					// set the mask to modify only bit 19 of P0
					FIO0DIR &= ~(1<<19);					// set as input pin 19 of P0[SDA]

					if(FIO0PIN & 0x80000)					// if pin 19 of P0 is 1:
						i2c_data_rec[byte] |= 1<<Counter;		// ASSIGN 1
					else
						i2c_data_rec[byte] &= ~(1<<Counter);	// ASSIGN 0

					Counter--;								// decrease variable
					bit++;									// variable that keeps count of the bits appeared on SDA

				}

				if(bit == 8 && !SCL)
				{
					bit = 0;								/* sending the ACKM bit */
					byte++;									// increment variable
					ACKM = 1;								// activates the sending of the ACKM bit

					if(byte > I2CbufferCnt)
					{
						byte = 0;
						NOACKM = 1;
						ACKM = 0;
					}

					if(ACKM){
						/* sending the ACKM bit */
						FIO0MASK = ~(1<<19);				// set the mask to modify only bit 19 of P0
						FIO0DIR |= 1<<19;		  			// sets pin 19 of P0 as output[SDA]
						FIO0CLR |= 1<<19;					// set low SDA
					}

					if(NOACKM){
						/* sending the NOACKM bit */
						FIO0MASK = ~(1<<19);				// set the mask to modify only bit 19 of P0
						FIO0DIR |= 1<<19;		  			// sets pin 19 of P0 as output[SDA]
						FIO0SET |= 1<<19;					// set high SDA
					}

				}

				if(ACKM && SCL){
					ACKM = 2;
				} else if(ACKM == 2 && !SCL){
					ACKM = 0;								// reset variable
					Counter = 7;							// reset variable

					FIO0MASK = ~(1<<19);					// set the mask to modify only bit 19 of P0
					FIO0DIR |= 1<<19;		  				// sets pin 19 of P0 as output[SDA]
					FIO0SET |= 1<<19;						// set high SDA
				}

				if(NOACKM && SCL){
					NOACKM = 0;								// reset variable
					Counter = 7;							// reset variable
					status = 4;								// set the next state (STOP COMMUNICATION)
				}
			break;

			/* STOP SIGNAL */
			case 4:
				stop_i2c();									// i2c communication stop
				status = 5; 								// set the next state (I2C DISABLE)
			break;

			/* I2C DISABLE */
			case 5:
				i2c_anable = 0;								// disabling variable i2c
			break;

			default:
				stop_i2c();									// i2c communication stop
				status = 0;									// set the next state (START COMMUNICATION)
			}
	}
	RICTRL |= 0b0001;										// Set RITINT in RICTRL register in order to clear the interrupt flag
}
