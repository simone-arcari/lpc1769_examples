/*
===============================================================================
 Name        : lpc1769_bmp280.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

/*!
 *  @brief Example shows basic application to configure and read the temperature.
 */

#include "stdio.h"
#include "bmp280.h"

/* NVIC peripherals interrupt */
#define ISER0 *(volatile unsigned long*) 0xE000E100

/* Power Control for Peripherals register */
#define PCONP *(volatile unsigned long*) 0x400FC0C4

/* registri priorità interrupt */
#define IPR5 *(volatile unsigned long*) 0xE000E414
#define IPR7 *(volatile unsigned long*) 0xE000E41C

/* Timer1 registers */
#define TIMER1_TCR *(volatile unsigned long*) 0x40008004
#define TIMER1_PR  *(volatile unsigned long*) 0x4000800C
#define TIMER1_MR0 *(volatile unsigned long*) 0x40008018
#define TIMER1_IR *(volatile unsigned long*) 0x40008000
#define TIMER1_MCR *(volatile unsigned long*) 0x40008014

/* Repetitive Interrupt Timer registers */
#define RICOMPVAL *(volatile unsigned long*) 0x400B0000
#define RIMASK *(volatile unsigned long*) 0x400B0004
#define RICTRL *(volatile unsigned long*) 0x400B0008
#define RICOUNTER *(volatile unsigned long*) 0x400B000C

/* GPIO registers */
#define FIO0DIR *(volatile unsigned long*) 0x2009C000
#define FIO0SET *(volatile unsigned long*) 0x2009C018
#define FIO0CLR *(volatile unsigned long*) 0x2009C01C
#define FIO0MASK *(volatile unsigned long*) 0x2009C010
#define FIO0PIN *(volatile unsigned long*) 0x2009C014

char SCL;								// valore logico del Serial Clock[SCL]
char SDA;								// valore logico del Serial Data[SDA]
int status = 0, bit = 0;				// variabile di stato per la macchina a stati e varibile per il conto dei bit
unsigned char i2c_buffer[64];			//buffer contenente indirizzo e dati da trasmettere
unsigned char i2c_data_rec[64];			//buffer contenente dati ricevuti
unsigned char dati[64];					//buffer contenente dati ricevuti dalla sola EEPROM
int byte = 0, I2CbufferCnt;				// variabile da usare come indice per il buffer
int i2c_anable = 0;

void delay_ms(uint32_t period_ms);
int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t spi_reg_write(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t spi_reg_read(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
void print_rslt(const char api_name[], int8_t rslt);

int main(void)
{
	// GPIO settings
	FIO0MASK = 0xFFFFFFFF;
	FIO0DIR = 0x180000;				//set OUTPUT P0.19[SDA] e P0.20[SCL]

	// Repetitive Interrupt Timer settings
	PCONP |= 1<<16;					// enables Repetitive Interrupt Timer power/clock
	RICTRL |= 0b10;					// Set RITENCLR bit in RICTRL register
    ISER0 |= 1<<29;					// enables in NVIC interrupt reception from RTI
    IPR7 |= 0x0<<11;				// set the priority of RIT(interrupt)
	RICOMPVAL = 960;				// load counter for update frequency[100KHz]

    int8_t rslt;
    struct bmp280_dev bmp;
    struct bmp280_config conf;
    struct bmp280_uncomp_data ucomp_data;
    int32_t temp32;
    double temp;

    /* Map the delay function pointer with the function responsible for implementing the delay */
    bmp.delay_ms = delay_ms;

    /* Assign device I2C address based on the status of SDO pin (GND for PRIMARY(0x76) & VDD for SECONDARY(0x77)) */
    bmp.dev_id = BMP280_I2C_ADDR_PRIM;

    /* Select the interface mode as I2C */
    bmp.intf = BMP280_I2C_INTF;

    /* Map the I2C read & write function pointer with the functions responsible for I2C bus transfer */
    bmp.read = i2c_reg_read;
    bmp.write = i2c_reg_write;


    rslt = bmp280_init(&bmp);
    print_rslt(" bmp280_init status", rslt);

    /* Always read the current settings before writing, especially when
     * all the configuration is not modified
     */
    rslt = bmp280_get_config(&conf, &bmp);
    print_rslt(" bmp280_get_config status", rslt);

    /* configuring the temperature oversampling, filter coefficient and output data rate */
    /* Overwrite the desired settings */
    conf.filter = BMP280_FILTER_COEFF_2;

    /* Temperature oversampling set at 4x */
    conf.os_temp = BMP280_OS_4X;

    /* Pressure over sampling none (disabling pressure measurement) */
    conf.os_pres = BMP280_OS_NONE;

    /* Setting the output data rate as 1HZ(1000ms) */
    conf.odr = BMP280_ODR_1000_MS;
    rslt = bmp280_set_config(&conf, &bmp);
    print_rslt(" bmp280_set_config status", rslt);

    /* Always set the power mode after setting the configuration */
    rslt = bmp280_set_power_mode(BMP280_NORMAL_MODE, &bmp);
    print_rslt(" bmp280_set_power_mode status", rslt);
    while (1)
    {
        /* Reading the raw data from sensor */
        rslt = bmp280_get_uncomp_data(&ucomp_data, &bmp);

        /* Getting the 32 bit compensated temperature */
        rslt = bmp280_get_comp_temp_32bit(&temp32, ucomp_data.uncomp_temp, &bmp);

        /* Getting the compensated temperature as floating point value */
        rslt = bmp280_get_comp_temp_double(&temp, ucomp_data.uncomp_temp, &bmp);
        printf("UT: %ld, T32: %ld, T: %f \r\n", ucomp_data.uncomp_temp, temp32, temp);

        /* Sleep time between measurements = BMP280_ODR_1000_MS */
        bmp.delay_ms(1000);
    }

    return 0;
}

/*!
 *  @brief Function that creates a mandatory delay required in some of the APIs such as "bmg250_soft_reset",
 *      "bmg250_set_foc", "bmg250_perform_self_test"  and so on.
 *
 *  @param[in] period_ms  : the required wait time in milliseconds.
 *  @return void.
 *
 */
void delay_ms(uint32_t period_ms)
{
    /* Implement the delay routine according to the target machine */
	  /*
	   * setup timer #1 for delay
	   */
	  TIMER1_TCR = 0x02;          //stop and reset timer
	  TIMER1_PR  = 0x00;          //set prescaler to zero
	  TIMER1_MR0 = (96000000/4) / 1000 * period_ms;
	  TIMER1_IR  = 0xff;          //reset all interrupt flags
	  TIMER1_MCR = 0x04;          //stop timer on match
	  TIMER1_TCR = 0x01;          //start timer

	  //wait until delay time has elapsed
	  while (TIMER1_TCR & 0x01);
}

/*!
 *  @brief Function for writing the sensor's registers through I2C bus.
 *
 *  @param[in] i2c_addr : sensor I2C address.
 *  @param[in] reg_addr : Register address.
 *  @param[in] reg_data : Pointer to the data buffer whose value is to be written.
 *  @param[in] length   : No of bytes to write.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

    /* Implement the I2C write routine according to the target machine. */
    return -1;
}

/*!
 *  @brief Function for reading the sensor's registers through I2C bus.
 *
 *  @param[in] i2c_addr : Sensor I2C address.
 *  @param[in] reg_addr : Register address.
 *  @param[out] reg_data    : Pointer to the data buffer to store the read data.
 *  @param[in] length   : No of bytes to read.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

    /* Implement the I2C read routine according to the target machine. */
    return -1;
}

/*!
 *  @brief Prints the execution status of the APIs.
 *
 *  @param[in] api_name : name of the API whose execution status has to be printed.
 *  @param[in] rslt     : error code returned by the API whose execution status has to be printed.
 *
 *  @return void.
 */
void print_rslt(const char api_name[], int8_t rslt)
{
    if (rslt != BMP280_OK)
    {
        printf("%s\t", api_name);
        if (rslt == BMP280_E_NULL_PTR)
        {
            printf("Error [%d] : Null pointer error\r\n", rslt);
        }
        else if (rslt == BMP280_E_COMM_FAIL)
        {
            printf("Error [%d] : Bus communication failed\r\n", rslt);
        }
        else if (rslt == BMP280_E_IMPLAUS_TEMP)
        {
            printf("Error [%d] : Invalid Temperature\r\n", rslt);
        }
        else if (rslt == BMP280_E_DEV_NOT_FOUND)
        {
            printf("Error [%d] : Device not found\r\n", rslt);
        }
        else
        {
            /* For more error codes refer "*_defs.h" */
            printf("Error [%d] : Unknown error code\r\n", rslt);
        }
    }
}

void delayUs(unsigned short delayInUs)
{
  /*
   * setup timer #1 for delay
   */
  TIMER1_TCR = 0x02;          			//stop and reset timer
  TIMER1_PR  = 0x00;          			//set prescaler to zero

  TIMER1_MR0 = (96000000/4) / 1000000 * delayInUs;

  TIMER1_IR  = 0xFF;          			//reset all interrrupt flags
  TIMER1_MCR = 0x04;          			//stop timer on match
  TIMER1_TCR = 0x01;          			//start timer

  //wait until delay time has elapsed
  while (TIMER1_TCR & 0x01);
}

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

void ACK()
{
	//imposta sda input contralla se i bit ack vale zero e rimposta sda output se invece vale 1 metti status = 0
	FIO0MASK = ~(1<<19);		// imposta la maschera per modificare solo iL bit 19 di P0
	FIO0DIR &= ~(1<<19);		// imposta come input il pin 19 di P0[SDA]

	if(FIO0PIN & 0x80000)		// se il bit ACK vale 1(NACK) allora ricominciamo la comunicazione perchè evidentemente non è riuscita
		status = 0;
}

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
				FIO0MASK = ~(1<<19);		// imposta la maschera per modificare solo iL bit 19 di P0
				FIO0DIR &= ~(1<<19);		// imposta come input il pin 19 di P0[SDA]

				if(SCL && Counter>=0)		// riceviamo i bit solo quando SCL vale 1
				{
					if(FIO0PIN & 0x80000)	// se il pin 19 di P0 vale 1
						i2c_data_rec[byte] |= 1<<Counter;			//ASSEGNA 1
					else
						i2c_data_rec[byte] &= ~(1<<Counter);		//ASSEGNA 0

					Counter--;
				}

				if(SCL)
					bit++;		// variabile che tiene conto dei bit apparsi su SDA

				if(bit == 8 && !SCL)
				{
					Counter = 7;
					bit = 0;
					byte++;

					if(byte > I2CbufferCnt)
					{
						byte = 0;
						status = 4;
					}

					// ivio del bit NACK
					delayUs(20);				//delay di 20us
					FIO0MASK = ~(1<<19);		// imposta la maschera per modificare solo iL bit 19 di P0
					FIO0DIR |= 1<<19;		  	// imposta come output il pin 19 di P0[SDA]
					FIO0SET = 0x8000;			// imposta alto SDA
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
