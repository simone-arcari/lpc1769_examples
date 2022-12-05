/*
===============================================================================
 Name        : Es_Interrupt.c
 Author      : $(Simone Arcari)
 data		 :	15/02/2019
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "lpc1769_registri.h"

void EINT3_IRQHandler(void)
{
	if(IO0IntStatF & (1<<9)) 	//interrupt proveniente da P0.9
	{
		if(FIO2PIN & 0x2)	//test sul P2.1
		{
			FIO2CLR |= 0x2;	  //spegni P2.1
		}else{
			FIO2SET |= 0x2;	  //accendi P2.1
		}

		IO0IntClr |= 1<<9;		//cancella l'interrupt appena gestito di P0.9
	}


	if(IO0IntStatF & (1<<8)) 	//interrupt proveniente da P0.8
	{
		if(FIO2PIN & 0x4)	//test sul P2.2
		{
			FIO2CLR |= 0x4;	  //spegni P2.2
		}else{
			FIO2SET |= 0x4;	  //accendi P2.2
		}

		IO0IntClr |= 1<<8;		//cancella l'interrupt appena gestito di P0.8
	}


	if(IO0IntStatF & (1<<7)) 	//interrupt proveniente da P0.7
	{
		if(FIO2PIN & 0x8)	//test sul P2.3
		{
			FIO2CLR |= 0x8;	  //spegni P2.3
		}else{
			FIO2SET |= 0x8;	  //accendi P2.3
		}

		IO0IntClr |= 1<<7;		//cancella l'interrupt appena gestito di P0.7
	}
}


int main(void)
{

	FIO2DIR |= (1<<1) | (1<<2) | (1<<3);	//mette in output i pin P2.1-2-3
	FIO2CLR |= 0xE;							//spegne tutti i led

	//*************IMPOSTAZIONI INTERRUPT***************
	IO0IntEnF |= (1<<9) | (1<<8) | (1<<7);//imposta P0.9-8-7 come fonti di interrupt (fronte di discesa)
	IPR5 |= 0x10<<11; 	//priorità interrupt di valore 16 --> 0x10 = 16
	ISER0 |= 1<<21;		//abilita external interrupt 3


    // Enter an infinite loop, just incrementing a counter
    while(1) {
    	__asm volatile ("wfi");		// Wait for interrupt
    }
    return 0 ;
}
