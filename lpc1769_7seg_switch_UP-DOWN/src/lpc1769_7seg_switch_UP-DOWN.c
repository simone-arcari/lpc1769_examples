/*
===============================================================================
 Name        : lpc1769_7seg_switch_UP-DOWN.c
 Author      : $(Simone Arcari)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include "lpc1769_registri.h"
#include "display.h"

// frequenza di clock del processore ARM
#define CORE_CLK 96000000
// carica in SysTick Timer il valore di aggiornamento del 7-segmenti
#define SYSTICK_VAL CORE_CLK/200 - 1			// 1/150 di secondo   10 millisencondi

// definizioni bit 7-segmenti
// Nota bene: tutti i segnali di uscita sono attivi bassi
// quindi per accendere un LED bisogna impostare a livello logico basso l'output
// P2.0 Led A, P2.1 Led B, P2.2 Led C, P2.3 Led D, P2.4 Led E, P2.5 Led F, P2.6 Led G, P2.7 Led DP
//	P2.10 abilita Cifra 1, P2.11 abilita Cifra 2
// Gestore di interrupt dal timer di sistema
// all'interno Ã¨ realizzato sia l'incremento del numero a due cifre da visualizzare (ogni secondo)
// sia l'aggiornamento delle cifre che vengono accese in modo alternato (ogni centesimo di secondo)

//variabili globali
static int val = 0;

//Gestore Intrrupt esterni
void EINT3_IRQHandler(void)
{
	if(IO0IntStatF & (1<<8)) 	//interrupt proveniente da P0.8
	{
		val+=1;
		IO0IntClr |= 1<<8;		//cancella l'interrupt appena gestito di P0.8
	}


	if(IO0IntStatF & (1<<9)) 	//interrupt proveniente da P0.9
	{
		val-=1;
		IO0IntClr |= 1<<9;		//cancella l'interrupt appena gestito di P0.9
	}
}
void SysTick_Handler(void)
{
	// ciclo di generazione del numero da visualizzare, da 0 a 99

	if(val>99)
		val=0;
	if(val < 0)
		val=99;

	//funzione che permette di visualizzare il numero
	displayView2Digits(val);
}


int main(void) {

	// Imposta GPIO da P2.0 a P2.7 e P2.8, P2.10, P2.11, P2.12 come output
    FIO2DIR |= 0x1DFF;

    // Imposta a livello logico alto tutti gli output (che sono in logica negata, quindi attivi bassi)
    FIO2SET = 0x1DFF;

	// Impostazione interrupt esterni
	IO0IntEnF |= (1<<8) | (1<<9);//imposta P0.10-11 come fonti di interrupt (fronte di discesa)
	IPR5 |= 0x10<<11; 	//priorità interrupt di valore 16 --> 0x10 = 16
	ISER0 |= 1<<21;		//abilita external interrupt 3

	// Impostazioni System Tick Timer
	STRELOAD = SYSTICK_VAL;		// Carica il contatore per la frequenza di aggiornamento
	STCTRL = 7;					// Abilita il Timer, il suo interrupt, con clock interno

    // Enter an infinite loop, just incrementing a counter
    while(1) {
    	__asm volatile ("wfi");		// Wait for interrupt
    }
    return 0 ;
}
