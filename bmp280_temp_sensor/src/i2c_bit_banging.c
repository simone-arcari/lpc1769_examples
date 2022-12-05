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
char SCL;													// valore logico del Serial Clock[SCL]
char SDA;													// valore logico del Serial Data[SDA]
int status = 0, bit = 0, ACKM = 0, NOACKM = 0;				// variabile di stato per la macchina a stati e varibile per il conto dei bit
unsigned char i2c_buffer[64];								// buffer contenente indirizzo e dati da trasmettere
unsigned char i2c_data_rec[64];								// buffer contenente dati ricevuti
unsigned char dati[64];										// buffer contenente dati ricevuti dalla sola EEPROM
int byte = 0, I2CbufferCnt;									// variabile da usare come indice per il buffer
int i2c_anable = 0;

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
	FIO0DIR = 0x180000;				//set OUTPUT P0.19[SDA] e P0.20[SCL]

	// Repetitive Interrupt Timer settings
	PCONP |= 1<<16;					// enables Repetitive Interrupt Timer power/clock
	RICTRL |= 0b10;					// Set RITENCLR bit in RICTRL register
    ISER0 |= 1<<29;					// enables in NVIC interrupt reception from RTI
    IPR7 |= 0x0<<11;				// set the priority of RIT(interrupt)
	RICOMPVAL = 960;				// load counter for update frequency[100KHz]
}

/*!
 * @brief This internal API is used to start i2c communication.
 *
 * @param[in] Null.
 *
 */
void start_i2c()
{
	FIO0MASK = ~(0b11<<19);		// imposta la maschera per modificare solo i bit 19 e 20 di P0
	FIO0DIR = 0x180000;			//Imposta come OUTPUT P0.19[SDA] e P0.20[SCL]
	FIO0SET = 0b11<<19;			// imposta a livello logico alto i bit 19 e 20 di P0[SDA E SCL]

	delayUs(15);				//delay di 30us

	FIO0MASK = ~(1<<19);		// imposta la maschera per modificare solo iL bit 19 di P0
	FIO0CLR = 1<<19;			// imposta a livello logico basso il bit 19 di P0[SDA]

	delayUs(10);				//delay di 30us
}

/*!
 * @brief This internal API is used to stop i2c communication.
 *
 * @param[in] Null.
 *
 */
void stop_i2c()
{
	delayUs(30);				//delay di 20us

	FIO0MASK = ~(0b11<<19);		// imposta la maschera per modificare solo i bit 19 e 20 di P0
	FIO0DIR |= 0b11<<19;		// imposta come output il pin 19 e 20 di P0
	FIO0PIN = 0<<19;			// imposta a livello logico basso il bit 19 di P0[SDA]

	delayUs(5);					//delay di 30us

	FIO0PIN = 1<<20;			// imposta a livello logico alto il bit 20 di P0[SCL]

	delayUs(15);				//delay di 30us

	FIO0MASK = ~(1<<19);		// imposta la maschera per modificare solo iL bit 19 di P0
	FIO0SET = 1<<19;		  	// imposta a livello logico alto il bit 19 di P0[SDA]

	delayUs(10);				//delay di 30us

	FIO0MASK = ~(0b11<<19);		// imposta la maschera per modificare solo i bit 19 e 20 di P0
	FIO0DIR &= ~(0b11<<19);		// imposta come input il pin 19 e 20 di P0
}

/*!
 * @brief This internal API is used to check the ACK bit
 * provided by the slave device.
 *
 * @param[in] Null.
 *
 */
void ACK()
{
	//imposta sda input contralla se i bit ack vale zero e rimposta sda output se invece vale 1 metti status = 0
	FIO0MASK = ~(1<<19);		// imposta la maschera per modificare solo iL bit 19 di P0
	FIO0DIR &= ~(1<<19);		// imposta come input il pin 19 di P0[SDA]

	if(FIO0PIN & 0x80000)		// se il bit ACK vale 1(NACK) allora ricominciamo la comunicazione perchè evidentemente non è riuscita
		status = 0;
}

/*
void RIT_IRQHandler(void)
{
	if(i2c_anable)
	{
		static int Counter = 7;		// serve per delle operazioni sui bit come variabile di conto

		// gestione SCL su P0.20
		SCL = !SCL;				// inversione del valore logico BackPlane
		FIO0MASK = ~(1<<20);	// imposta la maschera per modificare solo il bit 20 di P0[SCL]
		FIO0PIN = SCL<<20;		// aggiorna il valore di P0.20[SCL]

		switch(status)
		{
			// inizio segnale di start
			case 0:
				start_i2c();	// inizio comunicazione i2c
				SCL = 1;		// dichiaro il valore di SCL alto perchè così al prossimo ciclo verrà invertito al valore basso

				status = 1;		// imposta il prossimo stato
				bit = 0;
				byte = 0;
			break;

			// trasmissione indirizzo
			case 1:

				if(!SCL && Counter>=0)		// inviamo i bit solo quando SCL vale 0 cioè il momento in cui è concesso cambiaro stato ai bit
				{
					if(i2c_buffer[byte] & 1<<Counter)	// estrapoliamo tutti i bit a ogni ciclo
						SDA = 1;
					else
						SDA = 0;


					delayUs(20);			//delay di 20us
					FIO0MASK = ~(1<<19);	// imposta la maschera per modificare solo il bit 19 di P0[SCL]
					FIO0PIN = SDA<<19;		// invio il bit corrente
					Counter--;
				}

				if(!SCL)
					bit++;		// variabile che tiene conto dei bit apparsi su SDA

				if(bit == 9 && SCL)
				{
					if(SDA)
					{
						status = 3;			// RICEZIONE
						byte = 0;			// se dobbiamo ricevere riempiremo l'array dall'inizio
					}else{
						status = 2;			// TRASMISSIONE
						byte = 1;			// se dobbiamo trasmettere invieremo i dati dell'array dal secondo byte poiché il primo era l'indirizzo
					}

					ACK();		// controllo del bit ACK

					Counter = 7;
					bit = 0;
				}
			break;

			// trasmissione dati
			case 2:
				FIO0MASK = ~(1<<19);		// imposta la maschera per modificare solo iL bit 19 di P0
				FIO0DIR |= 1<<19;		  	// imposta come output il pin 19 di P0[SDA]

				if(!SCL && Counter>=0)		// inviamo i bit solo quando SCL vale 0 cioè il momento in cui è concesso cambiaro stato ai bit
				{
					if(i2c_buffer[byte] & 1<<Counter)	// estrapoliamo tutti i bit a ogni ciclo
						SDA = 1;
					else
						SDA = 0;

					delayUs(20);			//delay di 20us
					FIO0MASK = ~(1<<19);	// imposta la maschera per modificare solo il bit 19 di P0[SCL]
					FIO0PIN = SDA<<19;		// invio il bit corrente
					Counter--;
				}

				if(!SCL)
					bit++;		// variabile che tiene conto dei bit apparsi su SDA

				if(bit == 9 && SCL)
				{
					Counter = 7;
					bit = 0;
					byte++;

					if(byte > I2CbufferCnt)
					{
						byte = 0;
						status = 4;
					}

					ACK();		// controllo del bit ACK
				}
			break;

			// ricezione dati
			case 3:
				if(SCL && Counter>=0)		// riceviamo i bit solo quando SCL vale 1
				{
					FIO0MASK = ~(1<<19);		// imposta la maschera per modificare solo iL bit 19 di P0
					FIO0DIR &= ~(1<<19);		// imposta come input il pin 19 di P0[SDA]

					if(FIO0PIN & 0x80000)	// se il pin 19 di P0 vale 1
						i2c_data_rec[byte] |= 1<<Counter;			//ASSEGNA 1
					else
						i2c_data_rec[byte] &= ~(1<<Counter);		//ASSEGNA 0

					Counter--;
					bit++;		// variabile che tiene conto dei bit apparsi su SDA

				}

				if(bit == 8 && !SCL)
				{
					bit = 0;
					byte++;
					ACKM = 1;

					if(byte > I2CbufferCnt)
					{
						byte = 0;
						NOACKM = 1;
						ACKM = 0;
					}

					if(ACKM){
						// ivio del bit ACKM
						FIO0MASK = ~(1<<19);		// imposta la maschera per modificare solo iL bit 19 di P0
						FIO0DIR |= 1<<19;		  	// imposta come output il pin 19 di P0[SDA]
						FIO0CLR |= 1<<19;			// imposta basso SDA
					}

					if(NOACKM){
						// ivio del bit ACKM
						FIO0MASK = ~(1<<19);		// imposta la maschera per modificare solo iL bit 19 di P0
						FIO0DIR |= 1<<19;		  	// imposta come output il pin 19 di P0[SDA]
						FIO0SET |= 1<<19;			// imposta alto SDA
					}

				}

				if(ACKM && SCL){
					ACKM = 2;
				} else if(ACKM == 2 && !SCL){
					ACKM = 0;
					Counter = 7;

					FIO0MASK = ~(1<<19);		// imposta la maschera per modificare solo iL bit 19 di P0
					FIO0DIR |= 1<<19;		  	// imposta come output il pin 19 di P0[SDA]
					FIO0SET |= 1<<19;			// imposta alto SDA
				}

				if(NOACKM && SCL){
					NOACKM = 0;
					Counter = 7;
					status = 4;
				}
			break;

			//segnale di stop fine comunicazione
			case 4:
				stop_i2c();
				status = 5; 			//i2c non attivo
			break;

			case 5:
				//disabilito i2c
				i2c_anable = 0;
				status++;
			break;

			default:
				stop_i2c();
				status = 0;
			}
	}
	RICTRL |= 0b0001;					// Set RITINT in RICTRL register in order to clear the int flag
}
*/
