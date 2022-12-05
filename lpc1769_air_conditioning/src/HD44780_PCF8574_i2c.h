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

#ifndef HD44780_PCF8574_I2C_H_
#define HD44780_PCF8574_I2C_H_

#define true  1
#define false 0

#define IR 0
#define DR 1

#define TOTAL_CHARACTERS_OF_LCD 32
#define LCD_LINE_LENGHT 16

/* PCF8574 defines */
#define ADDR_PCF8574_W				0b01001110		// PCF8574 address for write operation (A0-A1-A2 at Vdd)
#define ADDR_PCF8574_R				0b01001111		// PCF8574 address for read operation (A0-A1-A2 at Vdd)

/* Display commands */
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

#define CGRAM_ADDRESS(addr) (addr|=0x40)			// 	CGRAM addressing
#define DDRAM_ADDRESS(addr) (addr|=0x80)			// 	DDRAM addressing

#define NUM_TO_CODE(num)(num+0x30)					//  0-9 ROM codes
#define CODE_TO_NUM(code)(code-0x30)        		//  0-9 ROM num



/*!
 * @brief This internal API it is used to write a single byte (8 bit) on the display.
 *
 * @param[in] rs.
 * @param[in] data_to_LCD.
 *
 */
unsigned char WriteByte_i2c(unsigned char rs, unsigned char data_to_LCD);

/*!
 * @brief This internal API it is used to initialize the LCD display via i2c communication.
 * The initialization parameters are preset for a 16x2 display but can be changed
 * at any time.
 * use the WriteByte_i2c() function
 *
 * @param[in] Null.
 *
 */
void InitLCD_i2c();

/*!
 * @brief This internal API it is used to write a single character on the display.
 * use the WriteByte_i2c() function
 *
 * @param[in] character.
 *
 */
unsigned char WriteChar_i2c(unsigned char character);

/*!
 * @brief This internal API it is used to write a single command operation on the display.
 * use the WriteByte_i2c() function
 *
 * @param[in] Command.
 *
 */
unsigned char PutCommand_i2c(unsigned char Command);

/*!
 * @brief This internal API it is used to set the cursor position.
 * use the PutCommand_i2c() function
 *
 * @param[in] line.
 * @param[in] col.
 *
 */
unsigned char GoToLine_i2c(unsigned char line, unsigned char col);

/*!
 * @brief This internal API it is used to write a string of characters on the display.
 * use the WriteChar_i2c() function
 *
 * @param[in] Command.
 *
 */
void WriteString_i2c(unsigned char LineOfCharacters[TOTAL_CHARACTERS_OF_LCD], char OverLenghtCharacters);

/*!
 * @brief This internal API it is used to write a a decimal integer on the display.
 * use the WriteByte_i2c() function
 *
 * @param[in] Command.
 *
 */
void WriteNumber_i2c(int number);

#endif /* HD44780_PCF8574_I2C_H_ */
