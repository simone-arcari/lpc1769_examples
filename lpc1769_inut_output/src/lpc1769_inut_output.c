/*
===============================================================================
 Name        : lpc1769_inut_output.c
 Author      : $(Simone Arcari)
 Version     : 1.0
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include "lpcio.h"


int main(void) {
	pinMode(22, OUTPUT, zero);	//dichiaro in OUTPUT P0.22 --> (pin 22 porta zero come OUTPUT)

	while(1) {
    	digitalWrite(22, HIGH, zero);	//accendo P0.22 --> (pin 22 porta zero in HIGH)
    	delayMs(250);					//metto in pausa per 250 millisecondi
    	digitalWrite(22, LOW, zero);	//spengo P0.22 --> (pin 22 porta zero in LOW)
    	delayMs(250);					//metto in pausa per 250 millisecondi
    }
    return 0 ;
}
