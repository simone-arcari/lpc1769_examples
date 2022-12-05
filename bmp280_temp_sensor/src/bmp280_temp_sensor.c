/*
===============================================================================
 Name        : bmp280_temp_sensor.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include "bmp280.h"
#include "delay.h"
#include "i2c_bit_banging.h"

/*===============================================================================*/

// registri PIN CONNECT BLOCK
//#define PINSEL4 *(volatile unsigned long*) 0x4002C010

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

#define FIO2DIR *(volatile unsigned long*) 0x2009C040
#define FIO2SET *(volatile unsigned long*) 0x2009C058
#define FIO2CLR *(volatile unsigned long*) 0x2009C05C

/* registri PWM
#define PWM1PCR *(volatile unsigned long*) 0x4001804C

#define PWM1MR0 *(volatile unsigned long*) 0x40018018
#define PWM1MR1 *(volatile unsigned long*) 0x4001801C
#define PWM1MR2 *(volatile unsigned long*) 0x40018020
#define PWM1MR3 *(volatile unsigned long*) 0x40018024

#define PWM1TCR *(volatile unsigned long*) 0x40018004
#define PWM1MCR *(volatile unsigned long*) 0x40018014
#define PWM1LER *(volatile unsigned long*) 0x40018050
*/
/*===============================================================================*/

// PCF8574 defines
#define ADDR_PCF8574_W				0b01001110	// PCF8574 address for write operation (A0-A1-A2 at Vdd)
#define ADDR_PCF8574_R				0b01001111	// PCF8574 address for read operation (A0-A1-A2 at Vdd)

#define true  1
#define false 0

#define IR 0
#define DR 1

#define TOTAL_CHARACTERS_OF_LCD 32
#define LCD_LINE_LENGHT 16

// Display commands
#define DISPLAY_CLEAR_CMD				0x01		//	Clears display and set DDRAM address 0
#define RETURN_HOME_CMD					0x02		//	DDRAM address 0, display in original position

#define ENTRY_MODE_DEC_NO_SHIFT_CMD		0b00000100	//	Entry mode decrement, no display shift
#define ENTRY_MODE_DEC_SHIFT_CMD		0b00000101	//	Entry mode decrement, display shift
#define ENTRY_MODE_INC_NO_SHIFT_CMD		0x06		//	Entry mode increment, no display shift
#define ENTRY_MODE_INC_SHIFT_CMD		0b00000111	//	Entry mode increment, display shift

#define DISP_OFF_CUR_OFF_BLINK_OFF_CMD	0b00001000	//	Display off, cursor off, blinking off
#define DISP_OFF_CUR_OFF_BLINK_ON_CMD	0b00001001	//	Display off, cursor off, blinking on
#define DISP_OFF_CUR_ON_BLINK_OFF_CMD	0b00001010	//	Display off, cursor on, blinking off
#define DISP_OFF_CUR_ON_BLINK_ON_CMD	0b00001011	//	Display off, cursor on, blinking on
#define DISP_ON_CUR_OFF_BLINK_OFF_CMD	0x0c		//  Display on, cursor off, blinking off
#define DISP_ON_CUR_OFF_BLINK_ON_CMD	0b00001101	//	Display on, cursor off, blinking on
#define DISP_ON_CUR_ON_BLINK_OFF_CMD	0b00001110	//	Display on, cursor on, blinking off
#define DISP_ON_CUR_ON_BLINK_ON_CMD		0b00001111	//	Display on, cursor on, blinking on

#define CURSOR_MOVE_SHIFT_LEFT_CMD		0b00010000	// 	Cursor move, shift to the left
#define CURSOR_MOVE_SHIFT_RIGHT_CMD		0b00011100	// 	Cursor move, shift to the right
#define DISPLAY_MOVE_SHIFT_LEFT_CMD		0b00011000	// 	Display move, shift to the left
#define DISPLAY_MOVE_SHIFT_RIGHT_CMD	0b00011100	// 	Display move, shift to the right

#define FOUR_BIT_ONE_LINE_5x8_CMD		0b00100000	// 	4 bit, 1 line, 5x8 font size
#define FOUR_BIT_ONE_LINE_5x10_CMD		0x24		// 	4 bit, 1 line, 5x10 font size
#define FOUR_BIT_TWO_LINE_5x8_CMD		0b00101000	// 	4 bit, 2 line, 5x8 font size
#define FOUR_BIT_TWO_LINE_5x10_CMD		0b00101100	// 	4 bit, 2 line, 5x10 font size
#define EIGHT_BIT_ONE_LINE_5x8_CMD		0b00110000	// 	8 bit, 1 line, 5x8 font size
#define EIGHT_BIT_ONE_LINE_5x10_CMD		0b00110100	// 	8 bit, 1 line, 5x10 font size
#define EIGHT_BIT_TWO_LINE_5x8_CMD		0b00111000	// 	8 bit, 2 line, 5x8 font size
#define EIGHT_BIT_TWO_LINE_5x10_CMD		0b00111100	// 	8 bit, 2 line, 5x10 font size

#define CGRAM_ADDRESS(addr) (addr|=0x40)				// 	CGRAM addressing
#define DDRAM_ADDRESS(addr) (addr|=0x80)				// 	DDRAM addressing

#define NUM_TO_CODE(num)(num+0x30)						//  0-9 ROM codes
#define CODE_TO_NUM(code)(code-0x30)        			//  0-9 ROM num


//#define CORE_CLK 96000000					// frequenza di clock del processore ARM
//#define PWM_FREQ 25000						// frequenza PWM
//#define PWM_PERIOD	(CORE_CLK/4)/PWM_FREQ   // periodo PWM

#define T_threshold 30
/*===============================================================================*/
/*
char SCL;													// valore logico del Serial Clock[SCL]
char SDA;													// valore logico del Serial Data[SDA]
int status = 0, bit = 0, ACKM = 0, NOACKM = 0;				// variabile di stato per la macchina a stati e varibile per il conto dei bit
unsigned char i2c_buffer[64];								//buffer contenente indirizzo e dati da trasmettere
unsigned char i2c_data_rec[64];								//buffer contenente dati ricevuti
unsigned char dati[64];										//buffer contenente dati ricevuti dalla sola EEPROM
int byte = 0, I2CbufferCnt;									// variabile da usare come indice per il buffer
int i2c_anable = 0;
*/

extern int I2CbufferCnt;
extern int status;
extern int i2c_anable;
extern unsigned char i2c_buffer[64];
extern unsigned char i2c_data_rec[64];

/*===============================================================================*/

void delay_ms(uint32_t period_ms);
int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t spi_reg_write(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t spi_reg_read(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
void print_rslt(const char api_name[], int8_t rslt);

/*===============================================================================*/
/*
void delayUs(unsigned short delayInUs)
{

  TIMER1_TCR = 0x02;          			//stop and reset timer
  TIMER1_PR  = 0x00;          			//set prescaler to zero

  TIMER1_MR0 = (96000000/4) / 1000000 * delayInUs;

  TIMER1_IR  = 0xFF;          			//reset all interrrupt flags
  TIMER1_MCR = 0x04;          			//stop timer on match
  TIMER1_TCR = 0x01;          			//start timer

  //wait until delay time has elapsed
  while (TIMER1_TCR & 0x01);
}



void delayMs(unsigned short delayInMs)
{

  TIMER1_TCR = 0x02;          //stop and reset timer
  TIMER1_PR  = 0x00;          //set prescaler to zero
  TIMER1_MR0 = (96000000/4) / 1000 * delayInMs;
  TIMER1_IR  = 0xff;          //reset all interrupt flags
  TIMER1_MCR = 0x04;          //stop timer on match
  TIMER1_TCR = 0x01;          //start timer

  //wait until delay time has elapsed
  while (TIMER1_TCR & 0x01);
}
*/
unsigned char WriteByte_i2c(unsigned char rs, unsigned char data_to_LCD)
{
	unsigned int dataVal, PCF8574Val;

	i2c_anable = 1;
	status = 0;
	i2c_buffer[0] = ADDR_PCF8574_W;				// PCF8574 IC address for write operation
    if(rs)
    	PCF8574Val = 0b00001001;				// RS = 1, LED = 1
    else
    	PCF8574Val = 0b00001000;				// RS = 0, LED = 1
    i2c_buffer[1] = PCF8574Val;					// PCF8574 output value
    I2CbufferCnt = 1;							// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);									// Wait a minimum of 60ns

	i2c_anable = 1;
	status = 0;
	PCF8574Val |= 0b00000100;					// EN = 1, LED = 1
	i2c_buffer[0] = ADDR_PCF8574_W;				// PCF8574 IC address for write operation
    i2c_buffer[1] = PCF8574Val;					// PCF8574 output value
    I2CbufferCnt = 1;							// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);									// Wait a minimum of 1ms

	i2c_anable = 1;
	status = 0;
	dataVal = data_to_LCD & 0xF0;				// Copy upper nibble of data_to_LCD into dataVal
	PCF8574Val |= dataVal;						// Update upper nibble of PCF8574Val
	i2c_buffer[0] = ADDR_PCF8574_W;				// PCF8574 IC address for write operation
	i2c_buffer[1] = PCF8574Val;					// PCF8574 output value
    I2CbufferCnt = 1;							// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);									// Wait a minimum of 1ms

	i2c_anable = 1;
	status = 0;
	PCF8574Val &= 0b11111011;					// EN = 0, LED = 1, finish first write operation
	i2c_buffer[0] = ADDR_PCF8574_W;				// PCF8574 IC address for write operation
	i2c_buffer[1] = PCF8574Val;					// PCF8574 output value
    I2CbufferCnt = 1;							// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);									// Wait a minimum of 530ns between writes

	i2c_anable = 1;
	status = 0;
	PCF8574Val |= 0b00000100;					// EN = 1, LED = 1, start second write operation
	i2c_buffer[0] = ADDR_PCF8574_W;				// PCF8574 IC address for write operation
	i2c_buffer[1] = PCF8574Val;					// PCF8574 output value
    I2CbufferCnt = 1;							// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);									// Wait a minimum of 530ns between writes

	i2c_anable = 1;
	status = 0;
	dataVal = data_to_LCD & 0x0F;				// Copy lower nibble of data_to_LCD into dataVal
	dataVal = dataVal << 4;						// shift left on dataVal to align DB4
	PCF8574Val &= 0x0F;							// Clear upper nibble of PCF8574Val
	PCF8574Val |= dataVal;						// Update upper nibble of PCF8574Val
	i2c_buffer[0] = ADDR_PCF8574_W;				// PCF8574 IC address for write operation
	i2c_buffer[1] = PCF8574Val;					// PCF8574 output value
    I2CbufferCnt = 1;							// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);									// Wait a minimum of 195ns

	i2c_anable = 1;
	status = 0;
	PCF8574Val &= 0b11111011;					// EN = 0, finish second write operation
	i2c_buffer[0] = ADDR_PCF8574_W;				// PCF8574 IC address for write operation
	i2c_buffer[1] = PCF8574Val;					// PCF8574 output value
    I2CbufferCnt = 1;							// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);									// Wait a minimum of 30ns

	return 0;
}

void InitLCD_i2c(){
	// Init bus lines
	i2c_anable = 1;
	status = 0;
	i2c_buffer[0] = ADDR_PCF8574_W;										// PCF8574 IC address for write operation
    i2c_buffer[1] = 0b00000000;											// PCF8574 output value (full bus clear)
    I2CbufferCnt = 1;													// Total buffer size: only data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(15);	// Delay a minimum of 15 ms

	// This part is on 8 bit interface
	i2c_anable = 1;
	status = 0;
	i2c_buffer[0] = ADDR_PCF8574_W;										// PCF8574 IC address for write operation
    i2c_buffer[1] = 0b00100000;											// PCF8574 output value (DB5 = 1)
    I2CbufferCnt = 1;													// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);		// Wait a minimum of 195ns

	i2c_anable = 1;
	status = 0;
	i2c_buffer[0] = ADDR_PCF8574_W;										// PCF8574 IC address for write operation
    i2c_buffer[1] = 0b00100100;											// PCF8574 output value (EN = 1, DB5 = 1)
    I2CbufferCnt = 1;													// Total buffer size: only data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);		// Wait a minimum of 195ns

	i2c_anable = 1;
	status = 0;
	i2c_buffer[0] = ADDR_PCF8574_W;										// PCF8574 IC address for write operation
    i2c_buffer[1] = 0b00100000;											// PCF8574 output value (EN = 0, DB5 = 1)
    I2CbufferCnt = 1;													// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);		// Wait a minimum of 195ns

	// This part is on 4 bit interface

	WriteByte_i2c(IR, FOUR_BIT_TWO_LINE_5x8_CMD);	  		// Define function set 4 bit interface, 2-line line display, 5x8 font
	delayMs(1);					// Wait until LCD is free
	WriteByte_i2c(IR, DISP_ON_CUR_OFF_BLINK_OFF_CMD);		// Define display on/off control
	delayMs(1);					// Wait until LCD is free
	WriteByte_i2c(IR, DISPLAY_CLEAR_CMD);	            	// Clear display
	delayMs(1);					// Wait until LCD is free
	WriteByte_i2c(IR, ENTRY_MODE_INC_NO_SHIFT_CMD);	  		// Entry mode set cursor direction increment, do not shift display
	delayMs(1);
}

unsigned char WriteChar_i2c(unsigned char character)
{
	delayMs(1);							// Wait until LCD is free
	return WriteByte_i2c(DR, character);	// Write characterto DR
}

unsigned char PutCommand_i2c(unsigned char Command)
{
	delayMs(1);							// Wait until LCD is free
	return WriteByte_i2c(IR, Command);		// Write characterto IR
}

unsigned char GoToLine_i2c(unsigned char line, unsigned char col)
{
	unsigned char address;
	switch(line)
	{
		case 0:
			address = 0x00;
		break;
		case 1:
			address = 0x40;
		break;
		case 2:
			address = 0x10;
		break;
		case 3:
			address = 0x50;
		break;
		default:
		address = 0x00;
		break;
	}
	if(col >= 0 && col <= LCD_LINE_LENGHT)	address += col;

	return PutCommand_i2c(DDRAM_ADDRESS(address));		// Disabled for 8 char LCD DM0810
}

void WriteString_i2c(unsigned char LineOfCharacters[TOTAL_CHARACTERS_OF_LCD], char OverLenghtCharacters)
{
	unsigned char i=0, line=-1;
	while(LineOfCharacters[i] && i<TOTAL_CHARACTERS_OF_LCD)
	{
		if(OverLenghtCharacters){
			if((i%LCD_LINE_LENGHT)==0)
				GoToLine_i2c(++line, 0);
		}

		WriteChar_i2c(LineOfCharacters[i]);
		i++;
	}
}

void WriteNumber_i2c(int number){
	int i = 0, INDICE = 10, A[INDICE], Temp = 0, ciclo = 0;

	while(number!=0){
		A[i] = number%10;
		number/=10;
		i++;
		INDICE = i;
	}

	if(i == 0){
		WriteByte_i2c(DR, 0x30);                  // stampa il simbolo zero
	}else{
		int comodo = INDICE - 1; 		/* effettivo indice in C */

		/* algoritmo inversione elementi di un array intero da indice totale divido sempre 2 */
		for (ciclo=0; ciclo < (INDICE-((INDICE/2))); ciclo++)
		{
			Temp = A[comodo]; 			/* salvo ultimo elemento */
			A[comodo] = A[ciclo]; 		/* Scambio primo elemento con ultimo elemento */
			A[ciclo] = Temp; 			/* scambio ultimo elemento con primo elemento */
			comodo--;
		}

		for (ciclo = 0; ciclo < INDICE; ciclo++){
			int base = 0x30;			/*corrisponde al simbolo 0 nella tabella ASCII*/
			WriteByte_i2c(DR, base + A[ciclo]);
		}
	}
}

/*===============================================================================*/

int main(void)
{
	// GPIO settings

	FIO0MASK = 0xFFFFFFFF;
	FIO0DIR = 0x180000;				//set OUTPUT P0.19[SDA] e P0.20[SCL]
	FIO2DIR = 1;					//set OUTPUT P2.0



	// Repetitive Interrupt Timer settings
	PCONP |= 1<<16;					// enables Repetitive Interrupt Timer power/clock
	RICTRL |= 0b10;					// Set RITENCLR bit in RICTRL register
    ISER0 |= 1<<29;					// enables in NVIC interrupt reception from RTI
    IPR7 |= 0x0<<11;				// set the priority of RIT(interrupt)
	RICOMPVAL = 960;				// load counter for update frequency[100KHz]


/*
	// impostazioni PWM
	PINSEL4 |= 0b010000;		// collega al P2.2 l'uscita PWM 1.3
	PWM1PCR |= 0b10000000000;	// abilita l'uscita PWM3

	PWM1MR0 = PWM_PERIOD;		 // prepara per MR0 il valore per il periodo desiderato
	PWM1MR3 = PWM_PERIOD*100/100;  // prepara per MR3 un duty cycle del 10%

	PWM1LER |= 0b1001;			// carica i valori preparati per MR0 e MR3

	PWM1MCR |= 0b10;			// PWMMR0R=1 imposta MR0 affinchè il conteggio del periodo riparta automaticamente

	PWM1TCR = 0b1001;			// NOTA BENE: da eseguire DOPO tutte le altre impostazioni. PWM Enable + Counter Enable
*/


	/* Initializing i2c LCD display */
	InitLCD_i2c();



    int8_t rslt;
    struct bmp280_dev bmp;
    struct bmp280_config conf;
    struct bmp280_uncomp_data ucomp_data;
    int32_t temp32;
    double temp;
    uint32_t pres32, pres64;
    double pres;

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
//    print_rslt(" bmp280_init status", rslt);

    /* Always read the current settings before writing, especially when
     * all the configuration is not modified
     */
    rslt = bmp280_get_config(&conf, &bmp);
//    print_rslt(" bmp280_get_config status", rslt);

    /* configuring the temperature oversampling, filter coefficient and output data rate */
    /* Overwrite the desired settings */
    conf.filter = BMP280_FILTER_COEFF_2;

    /* Temperature oversampling set at 4x */
    conf.os_temp = BMP280_OS_4X;

    /* Pressure oversampling set at 4x */
    conf.os_pres = BMP280_OS_4X;

    /* Setting the output data rate as 1HZ(1000ms) */
    conf.odr = BMP280_ODR_1000_MS;
    rslt = bmp280_set_config(&conf, &bmp);
//    print_rslt(" bmp280_set_config status", rslt);

    /* Always set the power mode after setting the configuration */
    rslt = bmp280_set_power_mode(BMP280_NORMAL_MODE, &bmp);
//    print_rslt(" bmp280_set_power_mode status", rslt);



	PutCommand_i2c(DISPLAY_CLEAR_CMD);
	GoToLine_i2c(0,0);
	WriteString_i2c((unsigned char*)"temp:", false);
	GoToLine_i2c(0,8);
	WriteString_i2c((unsigned char*)"C:", false);

	GoToLine_i2c(1,0);
	WriteString_i2c((unsigned char*)"press:", false);
	GoToLine_i2c(1,13);
	WriteString_i2c((unsigned char*)"Pa", false);



	while(1)
    {
        /* Reading the raw data from sensor */
        rslt = bmp280_get_uncomp_data(&ucomp_data, &bmp);



        /* Getting the 32 bit compensated temperature */
        rslt = bmp280_get_comp_temp_32bit(&temp32, ucomp_data.uncomp_temp, &bmp);
        /* Getting the compensated temperature as floating point value */
        rslt = bmp280_get_comp_temp_double(&temp, ucomp_data.uncomp_temp, &bmp);
        //printf("UT: %ld, T32: %ld, T: %f \r\n", ucomp_data.uncomp_temp, temp32, temp);

		GoToLine_i2c(0,6);
		WriteString_i2c((unsigned char*)"  ", false);
		GoToLine_i2c(0,6);
		WriteNumber_i2c(temp);



        /* Getting the compensated pressure using 32 bit precision */
        rslt = bmp280_get_comp_pres_32bit(&pres32, ucomp_data.uncomp_press, &bmp);
        /* Getting the compensated pressure as floating point value */
        rslt = bmp280_get_comp_pres_double(&pres, ucomp_data.uncomp_press, &bmp);
        //printf("UP: %ld, P32: %ld, P64: %ld, P64N: %ld, P: %f\r\n",ucomp_data.uncomp_press,pres32,pres64,pres64 / 256,pres);

		GoToLine_i2c(1,7);
		WriteString_i2c((unsigned char*)"      ", false);
		GoToLine_i2c(1,7);
		WriteNumber_i2c(pres);



		if(temp >= T_threshold)
			FIO2SET = 1;
		else
			FIO2CLR = 1;

/*
	    if(temp <= 20)
	    {
			PWM1MR3 = PWM_PERIOD*10/100;
			PWM1LER |= 0b1000;
	    }
	    else if (temp > 20 && temp <=25)
	    {
			PWM1MR3 = PWM_PERIOD*25/100;
			PWM1LER |= 0b1000;
	    }
	    else if (temp > 25 && temp <= 30)
	    {
			PWM1MR3 = PWM_PERIOD*100/100;
			PWM1LER |= 0b1000;
	    }
	    else if (temp > 30 && temp <=50)
	    {
			PWM1MR3 = PWM_PERIOD*55/100;
			PWM1LER |= 0b1000;
	    }
	    else if (temp > 50 && temp >= 60)
	    {
			PWM1MR3 = PWM_PERIOD*70/100;
			PWM1LER |= 0b1000;
	    }
	    else if (temp > 70)
	    {
			PWM1MR3 = PWM_PERIOD*100/100;
			PWM1LER |= 0b1000;
	    }

*/

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

/*===============================================================================*/

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

	i2c_anable = 1;
	status = 0;

	i2c_buffer[0] = i2c_addr << 1;	// bmp280 i2c address for write operations
	i2c_buffer[1] = reg_addr;

	for(int i = 2; i < length + 2; i++)
	{
		i2c_buffer[i] = reg_data[i-2];
	}

	I2CbufferCnt = length + 1;

	while(status <= 4){				// il ciclo si interrompre prima di giungere al case 5
		__asm volatile ("wfi");		// aspetta qualsiasi interrupt
	}

	i2c_anable = 0;

	return BMP280_OK;
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

	i2c_anable = 1;
	status = 0;

	i2c_buffer[0] = i2c_addr << 1;	// bmp280 i2c address for write operations
	i2c_buffer[1] = reg_addr;
	I2CbufferCnt = 1;

    while(status <= 3){				// il ciclo si interrompre prima di giungere al case 4
		__asm volatile ("wfi");		// aspetta qualsiasi interrupt
    }

    status = 0;
	i2c_buffer[0] = (i2c_addr << 1) | 0x1;	// bmp280 i2c address for read operations
	I2CbufferCnt = length-1;				// Total buffer size: only data

	while(status <= 4){				// il ciclo si interrompre dopo il case 4 cioè lo stop
		__asm volatile ("wfi");		// aspetta qualsiasi interrupt
	}


	for(int i = 0; i < length; i++)
	{
		reg_data[i] = i2c_data_rec[i];
		i2c_data_rec[i] = 0;
	}

	i2c_anable = 0;

	return BMP280_OK;
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
//        printf("%s\t", api_name);
        if (rslt == BMP280_E_NULL_PTR)
        {
//            printf("Error [%d] : Null pointer error\r\n", rslt);
        }
        else if (rslt == BMP280_E_COMM_FAIL)
        {
//            printf("Error [%d] : Bus communication failed\r\n", rslt);
        }
        else if (rslt == BMP280_E_IMPLAUS_TEMP)
        {
//            printf("Error [%d] : Invalid Temperature\r\n", rslt);
        }
        else if (rslt == BMP280_E_DEV_NOT_FOUND)
        {
//            printf("Error [%d] : Device not found\r\n", rslt);
        }
        else
        {
            /* For more error codes refer "*_defs.h" */
//            printf("Error [%d] : Unknown error code\r\n", rslt);
        }
    }
}



/*===============================================================================*/
/*
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
