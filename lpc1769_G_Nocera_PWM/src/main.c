/*
===============================================================================
 Name        : lpc1769_G_Nocera_PWM.c
 Author      : Gabriele Nocera
 Version     : 1.0
 Copyright   : N/A
 Description : Questo programma permette di controllare il duty cycle del
 	 	 	   canale PWM1 connesso alla porta di GPIO P2.0 tramite un
 	 	 	   potenziometro collegato all'ADC tramite P0.23
 	 	 	   Il segnale PWM con frequenza base di 50Hz gestisce un LED
 	 	 	   variandone l'intensità luminosa
===============================================================================
*/

/* Registri di selezione funzioni sui pin */
#define PINSEL1 *(volatile unsigned long*) 0x4002C004
#define PINSEL4 *(volatile unsigned long*) 0x4002C010

/* Registri GPIO porta 2 */
#define FIO2DIR *(volatile unsigned long*) 0x2009C040
#define FIO2SET *(volatile unsigned long*) 0x2009C058
#define FIO2CLR *(volatile unsigned long*) 0x2009C05C
#define FIO2PIN *(volatile unsigned long*) 0x2009C054

/* Registri System Tick Timer */
#define STCTRL *(volatile unsigned long*) 0xE000E010
#define STRELOAD *(volatile unsigned long*) 0xE000E014

/* Registro di alimentazione delle periferiche */
#define PCONP *(volatile unsigned long*) 0x400FC0C4

/* Registri del convertitore analogico-digitale (ADC) */
#define AD0CR *(volatile unsigned long*) 0x40034000
#define AD0INTEN *(volatile unsigned long*) 0x4003400C
#define AD0DR0 *(volatile unsigned long*) 0x40034010

/* Registri di configurazione di NVIC */
#define ISER0 *(volatile unsigned long*) 0xE000E100

/* Registri per i canali PWM */
#define PWM1PCR *(volatile unsigned long*) 0x4001804C
#define PWM1MR0 *(volatile unsigned long*) 0x40018018
#define PWM1MR1 *(volatile unsigned long*) 0x4001801C
#define PWM1MR2 *(volatile unsigned long*) 0x40018020
#define PWM1MR3 *(volatile unsigned long*) 0x40018024
#define PWM1TCR *(volatile unsigned long*) 0x40018004
#define PWM1MCR *(volatile unsigned long*) 0x40018014
#define PWM1LER *(volatile unsigned long*) 0x40018050

/* Costanti del programma */
#define CORE_CLK 96000000				// frequenza di clock del processore ARM
#define PWM_PERIOD (CORE_CLK/4)/50 		// periodo PWM pari a 1/50 di secondo
#define SYSTICK_VAL CORE_CLK/100		// periodo timer di sistema pari a 1/100 di secondo

/* variabili globali del programma */
unsigned int ADCvalue = 0;				// variabile che contiene il valore convertito dall'ADC
int valPot = 0;							//variabile che contiene il valore del potenziomentro [0,100]

/* Gestore di interrupt dal timer di sistema per il controllo
 * di un canale PWM tramite il valore analogico convertito da
 * un potenziomentro
 */
void SysTick_Handler(void){

	valPot = (ADCvalue*100/4095); 		// calcolo per riportare i valori [0:4095] a [0:100] (trimmer/potenziometro)

  	PWM1MR1 = PWM_PERIOD*valPot/100;		// prepara per MR1 un duty cycle del valore di valPot
  	PWM1LER |= 0b10; 						// carica il valore preparato per MR1
}

/* Gestore di interrupt dall'ADC: il valore convertito viene
 * allocato in una variabile apposita (ADCvalue)
 */
void ADC_IRQHandler(void){

	ADCvalue = ((AD0DR0 & 0xFFFF)>>4); 		// il risultato di conversione in AD0DR0 è contenuto nei bit 15:4
}

/* Funzione principale (main): vengone eseguite tutte le configurazione
 * e i settaggi necessari per le periferiche del microcontrollore (lpc1769)
 */
int main(void){

	/* IMPOSTAZIONI PWM: usiamo il canale 1 (PWM1), prelevato dal pin P2.0*/

	PINSEL4 |= 0b01;					// collega ai pin P2.0 a PWM 1.1 (codice 01)
	PWM1PCR |= 0b1000000000; 			// abilita l'uscite PWM1
	PWM1MR0 = PWM_PERIOD; 				// prepara per MR0 il valore per il periodo desiderato
	PWM1MR1 = PWM_PERIOD*50/100;		// prepara per MR1 un duty cycle del 50% (LED a media potenza)
	PWM1LER |= 0b11;					// carica i valori preparati per MR0 e MR1
	PWM1MCR |= 0b10; 					// PWMMR0R=1 imposta MR0 affinchè il conteggio del periodo riparta automaticamente
	PWM1TCR = 0b1001; 					// NOTA BENE: da eseguire DOPO tutte le altre impostazioni. PWM Enable + Counter Enable

	/* IMPOSTAZIONI CONVERTITORE ANALOGICO-DIGITALE: usiamo il canale 0 (AD0), prelvato dal pin P0.23 */

	PCONP |= 1<<12; 					// attiva l'alimentazione del convertitore analogico-digitale
	AD0CR |= 0x21FF01; 					// bit PDN=1, bit BURST=1, bits CLKDIV valore massimo (255), bits SEL=1 (canale 0)
	AD0INTEN |= 1; 						// abilita l'interrupt per fine conversione su AD0.0
	PINSEL1 |= 0x01<<14;				// imposta la funzione di AD0.0 su P0.23
	ISER0 |= 1<<22; 					// attiva la ricezione interrupt da ADC su NVIC

	/* Impostazioni System Tick Timer: serve per svolgere le operazioni principali*/

	STRELOAD = SYSTICK_VAL; 			// Carica il contatore per la frequenza di aggiornamento
	STCTRL = 0b111; 						// Abilita il Timer, il suo interrupt, con clock interno


	/* Enter an infinite loop, just waiting for interrupt */
	while(1) {
		__asm volatile ("wfi"); 		// Wait for interrupt
	}
    return 0 ;
}

