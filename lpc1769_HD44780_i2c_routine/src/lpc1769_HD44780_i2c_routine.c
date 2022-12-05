/*
===============================================================================
 Name        : lpc1769_HD44780_i2c_routine.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/



// NVIC peripherals interrupt
#define ISER0 *(volatile unsigned long*) 0xE000E100

// Power Control for Peripherals register
#define PCONP *(volatile unsigned long*) 0x400FC0C4

// registri System Tick Timer
#define STCTRL *(volatile unsigned long*) 0xE000E010
#define STRELOAD *(volatile unsigned long*) 0xE000E014

// Repetitive Interrupt Timer Registers
#define RICOMPVAL *(volatile unsigned long*) 0x400B0000
#define RIMASK *(volatile unsigned long*) 0x400B0004
#define RICTRL *(volatile unsigned long*) 0x400B0008
#define RICOUNTER *(volatile unsigned long*) 0x400B000C

#define FIO0DIR *(volatile unsigned long*) 0x2009C000
#define FIO0SET *(volatile unsigned long*) 0x2009C018
#define FIO0CLR *(volatile unsigned long*) 0x2009C01C
#define FIO0MASK *(volatile unsigned long*) 0x2009C010
#define FIO0PIN *(volatile unsigned long*) 0x2009C014

//REGISTRI TIMER1
#define TIMER1_TCR *(volatile unsigned long*) 0x40008004
#define TIMER1_PR  *(volatile unsigned long*) 0x4000800C
#define TIMER1_MR0 *(volatile unsigned long*) 0x40008018
#define TIMER1_IR *(volatile unsigned long*) 0x40008000
#define TIMER1_MCR *(volatile unsigned long*) 0x40008014

// registri del convertitore analogico-digitale (ADC)
#define AD0CR *(volatile unsigned long*) 0x40034000
#define AD0INTEN *(volatile unsigned long*) 0x4003400C
#define AD0DR0 *(volatile unsigned long*) 0x40034010

// registro di selezione funzioni sui pin
#define PINSEL1 *(volatile unsigned long*) 0x4002C004

// registri priorità interrupt
#define IPR5 *(volatile unsigned long*) 0xE000E414
#define IPR7 *(volatile unsigned long*) 0xE000E41C

/*===============================================================================*/

// 24LC64 defines
#define ADDR_24LC64_W	0b10100000		// 24LC64 address for write operation
#define ADDR_24LC64_R	0b10100001		// 24LC64 address for read operation

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

/*===============================================================================*/

char SCL;								// valore logico del Serial Clock[SCL]
char SDA;								// valore logico del Serial Data[SDA]
int status = 0, bit = 0;				// variabile di stato per la macchina a stati e varibile per il conto dei bit
unsigned char i2c_buffer[64];			//buffer contenente indirizzo e dati da trasmettere
unsigned char i2c_data_rec[64];			//buffer contenente dati ricevuti
unsigned char dati[64];					//buffer contenente dati ricevuti dalla sola EEPROM
int byte = 0, I2CbufferCnt;				// variabile da usare come indice per il buffer
int i2c_anable = 0;
int indice = 0;
unsigned int ADCvalue = 0;

/*===============================================================================*/

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

void delayMs(unsigned short delayInMs)
{
  /*
   * setup timer #1 for delay
   */
  TIMER1_TCR = 0x02;          //stop and reset timer
  TIMER1_PR  = 0x00;          //set prescaler to zero
  TIMER1_MR0 = (96000000/4) / 1000 * delayInMs;
  TIMER1_IR  = 0xff;          //reset all interrupt flags
  TIMER1_MCR = 0x04;          //stop timer on match
  TIMER1_TCR = 0x01;          //start timer

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

/*===============================================================================*/

void SEND_EEPROM(int memory_address, int data)
{
	i2c_anable = 1;
	status = 0;

	i2c_buffer[0] = ADDR_24LC64_W;	// 24LC64 Address WRITE
	i2c_buffer[1] = 0;				// 24LC64 Address High Byte
	i2c_buffer[2] = memory_address; // 24LC64 Address Low Byte
	i2c_buffer[3] = data;			// 24LC64 Byte0 value
	I2CbufferCnt = 3;				// Total buffer size: only data

	while(status <= 4){				// il ciclo si interrompre prima di giungere al case 5
		__asm volatile ("wfi");		// aspetta qualsiasi interrupt
	}

	i2c_anable = 0;
}

void REC_EEPROM(int memory_address)
{
	i2c_anable = 1;
	status = 0;

	i2c_buffer[0] = ADDR_24LC64_W;	// 24LC64 Address WRITE
	i2c_buffer[1] = 0;				// 24LC64 Address High Byte
	i2c_buffer[2] = memory_address;	// 24LC64 Address Low Byte
	I2CbufferCnt = 2;				// Total buffer size: only data

    while(status <= 3){				// il ciclo si interrompre prima di giungere al case 4
		__asm volatile ("wfi");		// aspetta qualsiasi interrupt
    }

    status = 0;
	i2c_buffer[0] = ADDR_24LC64_R;	// 24LC64 Address READ
	I2CbufferCnt = 0;				// Total buffer size: only data

	while(status <= 5){				// il ciclo si interrompre dopo il case 5 cioè la disabilitazione dell'i2c
		__asm volatile ("wfi");		// aspetta qualsiasi interrupt
	}

	dati[indice] = i2c_data_rec[0];
	indice++;

	i2c_anable = 0;
}

/*===============================================================================*/

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

void ADC_IRQHandler(void)
{
	ADCvalue = ((AD0DR0&0xFFFF)>>4);	// il risultato di conversione in AD0DR0 è contenuto nei bit 15:4
}

/*===============================================================================*/

int main(void)
{
	// GPIO settings
	FIO0MASK = 0xFFFFFFFF;
	FIO0DIR = 0x180000;				//Imposta come OUTPUT P0.19[SDA] e P0.20[SCL]

	// Repetitive Interrupt Timer settings
	PCONP |= 1<<16;					// enables Repetitive Interrupt Timer power/clock
	RICTRL |= 0b10;					// Set RITENCLR bit in RICTRL register
    ISER0 |= 1<<29;					// enables in NVIC interrupt reception from RTI
    IPR7 |= 0x0<<11;				// set the priority of RIT(interrupt)
	RICOMPVAL = 960;				// Carica il contatore per la frequenza di aggiornamento[100KHz]

	// ADC settings
	PCONP |= 1<<12;					// attiva l'alimentazione del convertitore analogico-digitale
	AD0CR |= 0x21FF01;				// bit PDN=1, bit BURST=1, bits CLKDIV valore massimo (255), bits SEL=1 (canale 0)
	//AD0INTEN |= 1;					// abilita l'interrupt per fine conversione su AD0.0
	PINSEL1 |= 0x01<<14;			// imposta la funzione di AD0.0 su P0.23
	//ISER0 |= 1<<22;					// attiva la ricezione interrupt da ADC su NVIC
	//IPR5 |= 0x0F<<19;				// set the priority of ADC(interrupt)

	InitLCD_i2c();

	PutCommand_i2c(DISPLAY_CLEAR_CMD);								//---Clear display (i2c)
	GoToLine_i2c(0,0);												//---First line 1st Position (i2c)
	WriteString_i2c((unsigned char*)"Display i2c OK", false);		//---Message (i2c)

	delayMs(1000);

	WriteString_i2c((unsigned char*)"Lettura ADC in  corso...", true);		//---Message (i2c)

	unsigned int delay;
	for(int i = 0; i < 64; i++)
	{
		//invio dato all'EEPROM
		int adc = ((AD0DR0&0xFFFF)>>4)*255/4095;
		SEND_EEPROM(i, adc);

		delay = 1000000;
		while(delay--);
	}

	PutCommand_i2c(DISPLAY_CLEAR_CMD);							//---Clear display (i2c)
	GoToLine_i2c(0,0);											//---First line 1st Position (i2c)
	WriteString_i2c((unsigned char*)"EEPROM = ", false);		//---Message (i2c)

	for(int j = 0; j < 64; j++)
	{
		//ricezione dato dall'EEPROM
		REC_EEPROM(j);

		delay = 1000000;
		while(delay--);

		GoToLine_i2c(0,9);											//---First line 10st Position (i2c)
		WriteString_i2c((unsigned char*)"   ", false);				//---Message (i2c)
		GoToLine_i2c(0,9);											//---First line 10st Position (i2c)
		WriteNumber_i2c(dati[j]);									//---Send Number (i2c)
		GoToLine_i2c(1,0);											//---Second line 1st Position (i2c)
		WriteString_i2c((unsigned char*)"[", false);				//---Message (i2c)
		WriteNumber_i2c(j);											//---Send Number (i2c)
		WriteString_i2c((unsigned char*)"]", false);				//---Message (i2c)

    	delayMs(1000);
	}

	while(1)
		__asm volatile ("wfi");		// aspetta qualsiasi interrupt

    return 0 ;
}
