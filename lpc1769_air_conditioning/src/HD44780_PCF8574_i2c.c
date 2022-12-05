/*
 *  Created on: 29 mag 2020
 *  Author: Simone Arcari
 *
 *	This program allows to use the LCD displays of the type HD44780 which
 *	mount the i2c module signed PCF8574.
 *  To work, you need to include the i2c_bit_banging library and delay library
 *  for lpc1769[ARM Cortex M-3, f_clock=96MHz].
 *  User Manual: 	https://www.nxp.com/docs/en/user-guide/UM10360.pdf
 *
 * @file	HD44780_PCF8574_i2c.h
 * @date	2020-05-29
 * @version	v1.0
 *
 */

#include "HD44780_PCF8574_i2c.h"
#include "delay.h"

/* extern variables */
extern int I2CbufferCnt;
extern int status;
extern int i2c_anable;
extern unsigned char i2c_buffer[64];
extern unsigned char i2c_data_rec[64];

/*!
 * @brief This internal API it is used to write a single byte (8 bit) on the display.
 *
 * @param[in] rs.
 * @param[in] data_to_LCD.
 *
 */
unsigned char WriteByte_i2c(unsigned char rs, unsigned char data_to_LCD){

	unsigned int dataVal, PCF8574Val;

	i2c_anable = 1;
	status = 0;
	i2c_buffer[0] = ADDR_PCF8574_W;										// PCF8574 IC address for write operation
    if(rs)
    	PCF8574Val = 0b00001001;										// RS = 1, LED = 1
    else
    	PCF8574Val = 0b00001000;										// RS = 0, LED = 1
    i2c_buffer[1] = PCF8574Val;											// PCF8574 output value
    I2CbufferCnt = 1;													// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);															// Wait a minimum of 60ns

	i2c_anable = 1;
	status = 0;
	PCF8574Val |= 0b00000100;											// EN = 1, LED = 1
	i2c_buffer[0] = ADDR_PCF8574_W;										// PCF8574 IC address for write operation
    i2c_buffer[1] = PCF8574Val;											// PCF8574 output value
    I2CbufferCnt = 1;													// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);															// Wait a minimum of 1ms

	i2c_anable = 1;
	status = 0;
	dataVal = data_to_LCD & 0xF0;										// Copy upper nibble of data_to_LCD into dataVal
	PCF8574Val |= dataVal;												// Update upper nibble of PCF8574Val
	i2c_buffer[0] = ADDR_PCF8574_W;										// PCF8574 IC address for write operation
	i2c_buffer[1] = PCF8574Val;											// PCF8574 output value
    I2CbufferCnt = 1;													// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);															// Wait a minimum of 1ms

	i2c_anable = 1;
	status = 0;
	PCF8574Val &= 0b11111011;											// EN = 0, LED = 1, finish first write operation
	i2c_buffer[0] = ADDR_PCF8574_W;										// PCF8574 IC address for write operation
	i2c_buffer[1] = PCF8574Val;											// PCF8574 output value
    I2CbufferCnt = 1;													// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);															// Wait a minimum of 530ns between writes

	i2c_anable = 1;
	status = 0;
	PCF8574Val |= 0b00000100;											// EN = 1, LED = 1, start second write operation
	i2c_buffer[0] = ADDR_PCF8574_W;										// PCF8574 IC address for write operation
	i2c_buffer[1] = PCF8574Val;											// PCF8574 output value
    I2CbufferCnt = 1;													// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);															// Wait a minimum of 530ns between writes

	i2c_anable = 1;
	status = 0;
	dataVal = data_to_LCD & 0x0F;										// Copy lower nibble of data_to_LCD into dataVal
	dataVal = dataVal << 4;												// shift left on dataVal to align DB4
	PCF8574Val &= 0x0F;													// Clear upper nibble of PCF8574Val
	PCF8574Val |= dataVal;												// Update upper nibble of PCF8574Val
	i2c_buffer[0] = ADDR_PCF8574_W;										// PCF8574 IC address for write operation
	i2c_buffer[1] = PCF8574Val;											// PCF8574 output value
    I2CbufferCnt = 1;													// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);															// Wait a minimum of 195ns

	i2c_anable = 1;
	status = 0;
	PCF8574Val &= 0b11111011;											// EN = 0, finish second write operation
	i2c_buffer[0] = ADDR_PCF8574_W;										// PCF8574 IC address for write operation
	i2c_buffer[1] = PCF8574Val;											// PCF8574 output value
    I2CbufferCnt = 1;													// Total buffer size: address + data
	while(status <= 4){__asm volatile ("wfi");}i2c_anable = 0;

	delayMs(1);															// Wait a minimum of 30ns

	return 0;
}

/*!
 * @brief This internal API it is used to initialize the LCD display via i2c communication.
 * The initialization parameters are preset for a 16x2 display but can be changed
 * at any time.
 * use the WriteByte_i2c() function
 *
 * @param[in] Null.
 *
 */
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

	WriteByte_i2c(IR, FOUR_BIT_TWO_LINE_5x8_CMD);	  					// Define function set 4 bit interface, 2-line line display, 5x8 font
	delayMs(1);															// Wait until LCD is free
	WriteByte_i2c(IR, DISP_ON_CUR_OFF_BLINK_OFF_CMD);					// Define display on/off control
	delayMs(1);															// Wait until LCD is free
	WriteByte_i2c(IR, DISPLAY_CLEAR_CMD);	            				// Clear display
	delayMs(1);															// Wait until LCD is free
	WriteByte_i2c(IR, ENTRY_MODE_INC_NO_SHIFT_CMD);	  					// Entry mode set cursor direction increment, do not shift display
	delayMs(1);
}

/*!
 * @brief This internal API it is used to write a single character on the display.
 * use the WriteByte_i2c() function
 *
 * @param[in] character.
 *
 */
unsigned char WriteChar_i2c(unsigned char character){

	delayMs(1);															// Wait until LCD is free
	return WriteByte_i2c(DR, character);								// Write character to DR
}

/*!
 * @brief This internal API it is used to write a single command operation on the display.
 * use the WriteByte_i2c() function
 *
 * @param[in] Command.
 *
 */
unsigned char PutCommand_i2c(unsigned char Command){

	delayMs(1);															// Wait until LCD is free
	return WriteByte_i2c(IR, Command);									// Write character to IR
}

/*!
 * @brief This internal API it is used to set the cursor position.
 * use the PutCommand_i2c() function
 *
 * @param[in] line.
 * @param[in] col.
 *
 */
unsigned char GoToLine_i2c(unsigned char line, unsigned char col){

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

	return PutCommand_i2c(DDRAM_ADDRESS(address));						// Disabled for 8 char LCD DM0810
}

/*!
 * @brief This internal API it is used to write a string of characters on the display.
 * use the WriteChar_i2c() function
 *
 * @param[in] Command.
 *
 */
void WriteString_i2c(unsigned char LineOfCharacters[TOTAL_CHARACTERS_OF_LCD], char OverLenghtCharacters){

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

/*!
 * @brief This internal API it is used to write a a decimal integer on the display.
 * use the WriteByte_i2c() function
 *
 * @param[in] Command.
 *
 */
void WriteNumber_i2c(int number){

	int i = 0, INDICE = 10, A[INDICE], Temp = 0, ciclo = 0;

	while(number!=0){
		A[i] = number%10;
		number/=10;
		i++;
		INDICE = i;
	}

	if(i == 0){
		WriteByte_i2c(DR, 0x30);                  						// print the zero symbol
	}else{
		int comodo = INDICE - 1; 										// actual index in C

		/* inversion algorithm elements of an integer array from total index always divide 2 */
		for (ciclo=0; ciclo < (INDICE-((INDICE/2))); ciclo++)
		{
			Temp = A[comodo]; 											// save last element
			A[comodo] = A[ciclo]; 										// exchange first element with last element
			A[ciclo] = Temp; 											// exchange last element with first element
			comodo--;													// decrease index
		}

		for (ciclo = 0; ciclo < INDICE; ciclo++){
			int base = 0x30;											// corresponds to the symbol 0 in the ASCII table
			WriteByte_i2c(DR, base + A[ciclo]);
		}
	}
}
