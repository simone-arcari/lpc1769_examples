/*
 * display.c
 *
 *  Created on: 10 feb 2019
 *      Author: simon
 */


#include "display.h"
#include "lpc1769_registri.h"

// funzione di attivazione dei LED corrispondenti alla cifra desiderata
void setDigits(char digit)
{
	FIO2SET = 0xFF;			// spegne tutti i LED del 7 segmenti

	switch(digit)
	{
		case 0:
			FIO2CLR = 0x3F;	// LED A, B, C, D, E, F accesi
		break;
		case 1:
			FIO2CLR = 0x06;	// LED B, C accesi
		break;
		case 2:
			FIO2CLR = 0x5B;	// LED A, B, D, E, G accesi
		break;
		case 3:
			FIO2CLR = 0x4F;	// LED A, B, C, D, E, G accesi
		break;
		case 4:
			FIO2CLR = 0x66;	// LED B, C, F, G accesi
		break;
		case 5:
			FIO2CLR = 0x6D;	// LED A, C, D, F, G accesi
		break;
		case 6:
			FIO2CLR = 0x7D;	// LED A, C, D, E, F, G accesi
		break;
		case 7:
			FIO2CLR = 0x07;	// LED A, B, C accesi
		break;
		case 8:
			FIO2CLR = 0x7F;	// LED A, B, C, D, E, F, G accesi
		break;
		case 9:
			FIO2CLR = 0x6F;	// LED A, B, C, D, E, F accesi
		break;
		default:			// valore impossibile
			FIO2SET = 0xFF;	// tutto spento
	}

}

void displayView2Digits(int number)
{
	int digit;

	if(FIO2PIN & 0x0400)	// test sul pin P2.10 (cifra 1, più significativa)
	{
		// se entriamo qui vuol dire che la cifra 1 era spenta e la cifra 2 era accesa, quindi invertiamo
		FIO2SET = 0x0800;	// P2.11 alto => spegni cifra 2
		digit = number/10; 	// calcolo cifra più significativa
		if(digit==0)		// scrive solo cifre diverse da zero
			digit=-1;		// forza la cancellazione della cifra più significativa con un valore impossibile
		setDigits(digit);
		FIO2CLR = 0x0400;	// P2.10 basso => accendi cifra 1
	}
	else
	{
		// se entriamo qui vuol dire che la cifra 1 era accesa e la cifra 2 era spenta, quindi invertiamo
		FIO2SET = 0x0400;	// P2.10 alto => spegni cifra 1
		digit = number%10;	// calcolo cifra meno significativa
		setDigits(digit);
		FIO2CLR = 0x0800;	// P2.11 basso => accendi cifra 2
	}
}



void displayView3Digits(int number)
{
	int digit;

	if(FIO2PIN & 0x0400)			//test on P2.10
	{
		if(FIO2PIN & 0x0800)		//test on P2.11
		{
			FIO2SET = 0x1800;		// P2.11-12 alto => spegni cifra 2-3
			number/=100;
			digit = number%10;
			if(digit == 0)
			{
				digit=-1;
			}
			setDigits(digit);
			FIO2CLR = 0x0400;		// P2.10 basso => accendi cifra 1
		}else{
			FIO2SET = 0x0C00;		// P2.10-11 alto => spegni cifra 1-2
			digit = number%10;		// calcolo cifra meno significativa
			setDigits(digit);
			FIO2CLR = 0x1000;		// P2.12 basso => accendi cifra 3
		}
	}else{
		FIO2SET = 0x1400;			// P2.10-12 alto => spegni cifra 1-3
		number/=10;
		digit = number%10;
		if(digit == 0)
		{
			digit=-1;
		}
		setDigits(digit);
		FIO2CLR = 0x0800;			// P2.11 basso => accendi cifra 2
	}
}


void displayView4Digits(int number)
{
	int digit;

	if(FIO2PIN & 0x0100)			//test on P2.8
	{
		if(FIO2PIN & 0x0400)		//test on P2.10
		{
			if(FIO2PIN & 0x0800)	//test on P2.11
			{

				FIO2SET = 0x1500;			// P2.8-10-12 alti => spegni cifra 1-2-4
				number/=10;
				digit = number%10;
				if(digit == 0)
				{
					digit=-1;
				}
				setDigits(digit);
				FIO2CLR = 0x0800;			// P2.11 basso => accendi cifra 3





			}else{
				FIO2SET = 0x1C00;	// P2.10-11-12 alti => spegni cifra 2-3-4
				number/=1000;
				digit = number%10;
				if(digit == 0)
				{
					digit=-1;
				}
				setDigits(digit);
				FIO2CLR = 0x0100;	// P2.8 basso => accendi cifra 1
			}
		}else{
			FIO2SET = 0x1900;		// P2.8-11-12 alti => spegni cifra 1-3-4
			number/=100;
			digit = number%10;
			if(digit == 0)
			{
				digit=-1;
			}
			setDigits(digit);
			FIO2CLR = 0x0400;		// P2.10 basso => accendi cifra 2
		}
	}else{







		FIO2SET = 0x0D00;	// P2.8-10-11 alti => spegni cifra 1-2-3
		digit = number%10;
		setDigits(digit);
		FIO2CLR = 0x1000;	// P2.12 basso => accendi cifra 4
	}
}








