/*
===============================================================================
 Name        : lpc1769_display_16x2_Gabriele_Nocera.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "HD44780.h"
#include "lpcio.h"


int main(void) {
	/**************************Setup LCD**************************/

    lcd_pin(13, 28, 27, 22, 21, 3, 2, DUE, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO  );
	InitLCD();

	/************************************************************************************/

	PutCommand(DISPLAY_CLEAR_CMD);          					//---Clear display
	GoToLine(0,0);    											//---First line 1st Position
	WriteString((unsigned char*)"Display LCD 4bit", false);		//---Message


	delayMs(1000);

    while(1) {
    	static int Number = 0;

    	GoToLine(1,0);    											//---Second line 1st Position    imposti da quale casella iniziare a scriver, è diviso in due righe e 16 collone
    	WriteString((unsigned char*)"Conteggio: ", false);			//---Message     		        il prametro false impedisce che se scrivi oltre la lunghezza del display il testo vado alla riga di sotto ma sevuoi puoi farlo mettendo true
    	WriteNumber(Number);										//---Send Number		        i numeri li devi scrivere con questa funzione

    	Number++;

    	delayMs(1000);

    }
    return 0 ;
}
