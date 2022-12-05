#include "HC_SR04.h"
#include "lpc_registri.h"

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
