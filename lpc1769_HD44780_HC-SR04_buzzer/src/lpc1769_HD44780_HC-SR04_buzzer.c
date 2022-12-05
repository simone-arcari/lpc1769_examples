/*
===============================================================================
 Name        : lpc1769_HD44780_HC-SR04_buzzer.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/


#include "HD44780.h"
#include "lpcio.h"
#include "i2c.h"
#include "lpc1769_registri.h"


#define FREQ_HZ 64								// 64 Hz
#define SYSTICK_VAL CORE_CLK/FREQ_HZ - 1		// valore per il Timer di SysTick
#define MAX_FREQUENCY 988						// frequenza massima pari a 988Hz
#define MIN_FREQUENCY 262						// frequenza minima pari a 262Hz
#define VOLUME 80								// percentuale duty-cycle corrispondente al volume al contrario (100 spento, 0 al massimo)


unsigned long TimerVal=0;

// Gestore di interrupt dal timer di sistema
void SysTick_Handler(void)
{
	int val, valP;

	/**************************buzzer**************************/
	static int frequency=MIN_FREQUENCY, countBuzzer = 0, buzzerstatus = 0;

	if(val <= 30 && buzzerstatus==0){
		if(countBuzzer >= 64*val/40){
			buzzerstatus = 1;
			countBuzzer = 0;
		}
		countBuzzer++;

		PWM1MR2 = PWM1MR0*VOLUME/100;			// prepara per MR2 un duty cycle pari a VOLUME
		PWM1LER |= 0b0101;						// carica il valori preparato per MR0 e MR2
	}else{
		if(countBuzzer >= 64*val/40){
			buzzerstatus = 0;
			countBuzzer = 0;
		}
		countBuzzer++;

		PWM1MR2 = PWM1MR0*100/100;						// prepara per MR2 un duty cycle pari a PWM1MR0 (buzzer OFF)
		PWM1LER |= 0b0101;						// carica il valori preparato per MR0 e MR2
	}



	/**************************LCD**************************/

	static int updateCnt=FREQ_HZ;	// contatore per l'aggiornamento del dato ogni secondo (1Hz)

	updateCnt--;
	if(updateCnt==0)				// tempo di visualizzare la misura precedente e farne partire una nuova
	{
		updateCnt=FREQ_HZ;			// riparte il conteggio per la prossima lettura/visualizzazione

		val= TimerVal/72;			// converte i microsecondi in cm (con velocità del suono di 343 m/s)
		TimerVal = 0;				// il valore viene annullato per segnalare eventuale mancata lettura

		if(val != valP){
			GoToLine(1,10);    										//---Second line 11° Position
			WriteString("      ", false);							//---Clean Message

			GoToLine(0,0);    										//---First line 1st Position
			WriteString("distanza", false);							//---Message
			GoToLine(1,0);    										//---Second line 1st Position
			WriteString("misurata: ", false);						//---Message
			WriteNumber(val);
			WriteString("cm", false);						    //---Message
		}

		valP = val;

		/**************************lancia l'impulso di 15uS**************************/

		T3TCR = 0b10;				// reset counter del Timer3, conteggio fermo
		T3EMR |= 1;					// fronte di salita su P0.10 per inizio impulso di trigger
		T3TCR = 0b01;				// avvio conteggio Timer3 (si fermerà dopo 15us con fronte di discesa su P0.10)
	}



}

void TIMER2_IRQHandler(void)
{
	FIO0MASK = 0;
	if(FIO0PIN & (1<<4))
	{	// interrupt per fronte di salita su P0.4
		T2TCR = 0b10;					// ferma il TIMER2 e lo azzera (reset)
		T2TCR = 0b01;					// avvia il TIMER2
	}
	else
	{	// interrupt per fronte di discesa su P0.4
		T2TCR = 0b00;								// ferma il TIMER2
		TimerVal = T2CR0/((CORE_CLK/4)/1000000);	// calcola il tempo trascorso in microsecondi
		// riesce a misurare fino a quasi 3 minuti di durata
	}
	T2IR = 1<<4;			// reset dell'interrupt su CR0 di TIMER2
}

// Inizializza Timer2 come Capture su P0.4
void initTimer2Capture(void)
{
	PCONP |= 1<<22; 		// PCTIM2=1 alimentazione TIMER2 ON
	// PCLKSEL1 |= 0b00<<12;// ridondante, a RESET è già impostato a 00, PCLK_peripheral = CCLK/4
	PINSEL0 |= (0b11<<8);	// abilita la funzionalità CAP2.0 (capture Timer2, canale0) su P0.4
	PINMODE0 |= (0b10<<8);	// disabilita su P0.4 i resistori interni di pull-up e pull-down
	T2CCR = 0b000111;		// abilita caricamento di CR0 con valore TIMER2 e genera interrupt su fronte di salita e discesa
	ISER0 = (1<<3);			// abilita gestione interrupt TIMER2 su NVIC
}

// Inizializza Timer3 come Match su P0.10
void initTimer3Match(void)
{
	PCONP |= 1<<23; 		// PCTIM3=1 alimentazione TIMER3 ON
	// PCLKSEL1 |= 0b00<<12;// ridondante, a RESET è già impostato a 00, PCLK_peripheral = CCLK/4
	PINSEL0 |= (0b11<<20);	// abilita la funzionalità MAT3.0 (match Timer3, canale0) su P0.10
	PINMODE0 |= (0b10<<20);	// disabilita su P0.10 i resistori interni di pull-up e pull-down
	PINMODE_OD0 |= (1<<10);	// imposta P0.10 come OpenDrain, permettendo l'utilizzo a 5V con pull-up e alimentazione esterni

	T3MCR |= (1<<2);		// imposta l'arresto di Timer3 su match MR0
	T3EMR |= (0b01<<4);		// imposta P0.10 al valore basso quando il Timer3 raggiunge il valore previsto (EMC0=01)
	T3MR0 = ((CORE_CLK/4)/1E6)*15;	// il match avviene dopo 15us (durata dell'impulso di trigger)
	//T3MR0 = (CORE_CLK/4)/10;// TEST **** il match avviene dopo 0.1s (durata dell'impulso di trigger) TEST **** TEST ****
	T3EMR &= 0b11111111110;	// livello iniziale basso su P0.10 (trigger)
}


int main(void) {
	/**************************Setup LCD**************************/
	lcd_pin(13, 28, 27, 22, 21, 3, 2, DUE, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO  );
	InitLCD();

	PutCommand(DISPLAY_CLEAR_CMD);          					//---Clear display
	GoToLine(0,0);    											//---First line 1st Position
	WriteString("Display LCD 4bit", false);						//---Message

	delayMs(1000);

	PutCommand(DISPLAY_CLEAR_CMD);          					//---Clear display

	/**************************Setup timer for HC-SR04**************************/
	initTimer3Match();				// Inizializza Timer3 come Match su P0.10
    initTimer2Capture();			// Inizializza Timer2 come Capture su P0.4

    /**************************Setup System Tick Timer**************************/
	STRELOAD = SYSTICK_VAL;			// Carica il contatore per la frequenza di aggiornamento
	STCTRL = 7;						// Abilita il Timer, il suo interrupt, con clock interno

	/**************************Setup PWM for buzzer**************************/
	PINSEL4 |= 0b0100;			// collega al pin P2.1, l'uscita PWM 1.2 (per problemi su PWM 1.1 della mia scheda :-( )
	PWM1PCR |= 0b10000000000;	// abilita l'uscita PWM2
	PWM1MR0 = (CORE_CLK/4)/MIN_FREQUENCY;	// prepara per MR0 il valore per il periodo desiderato
	PWM1MR2 = PWM1MR0*VOLUME/100;			// prepara per MR2 un duty cycle pari a VOLUME
	PWM1LER |= 0b101;			// carica i valori preparati per MR0 e MR2
	PWM1MCR |= 0b10;			// PWMMR0R=1 imposta MR0 affinchè il conteggio del periodo riparta automaticamente
	PWM1TCR = 0b1001;			// NOTA BENE: da eseguire DOPO tutte le altre impostazioni. PWM Enable + Counter Enable

	/************************************************************************************/

    while(1) {
    	__asm volatile ("wfi");		// Wait for interrupt
    }
    return 0 ;
}
