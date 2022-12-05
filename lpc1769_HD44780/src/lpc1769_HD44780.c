/*
===============================================================================
 Name        : lpc1769_HD44780.c
 Author      : $(author)
 Version     : 2.0
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "HD44780.h"
#include "lpcio.h"
#include "i2c.h"


int main(void) {
	/**************************Setup LCD**************************/
	lcd_pin(13, 28, 27, 22, 21, 3, 2, DUE, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO  );
	InitLCD();

	setupI2C1();																									// I2C1 initialization
	InitLCD_i2c();

	/************************************************************************************/

	PutCommand(DISPLAY_CLEAR_CMD);          					//---Clear display
	GoToLine(0,0);    											//---First line 1st Position
	WriteString((unsigned char*)"Display LCD 4bit", false);						//---Message

	PutCommand_i2c(DISPLAY_CLEAR_CMD);							//-----Clear display (i2c)
	GoToLine_i2c(0,0);											//---First line 1st Position (i2c)
	WriteString_i2c((unsigned char*)"Display LCD i2c", false);					//---Message (i2c)

	delayMs(1000);

    while(1) {
    	static int Number = 0;

    	GoToLine(1,0);    											//---First line 1st Position
    	WriteString((unsigned char*)"Conteggio: ", false);			//---Message
    	WriteNumber(Number);										//---Send Number

    	GoToLine_i2c(1,0);											//---First line 1st Position (i2c)
    	WriteString_i2c((unsigned char*)"Conteggio: ", false);	    //---Message (i2c)
    	WriteNumber_i2c(Number);									//---Send Number (i2c)

    	Number++;

    	delayMs(1000);

    }
    return 0 ;
}
