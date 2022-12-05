/*
===============================================================================
 Name        : lpc1769_Braccio_6_assi.c
 Author      : $(Simone Arcari)	06/12/2019
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
// registro di selezione funzioni sui pin
#define PINSEL1 *(volatile unsigned long*) 0x4002C004
#define PINSEL3 *(volatile unsigned long*) 0x4002C00C
#define PINSEL4 *(volatile unsigned long*) 0x4002C010

// registri del convertitore analogico-digitale (ADC)
#define AD0CR *(volatile unsigned long*) 0x40034000
#define AD0INTEN *(volatile unsigned long*) 0x4003400C
#define AD0DR0 *(volatile unsigned long*) 0x40034010
#define AD0DR1 *(volatile unsigned long*) 0x40034014
#define AD0DR2 *(volatile unsigned long*) 0x40034018
#define AD0DR3 *(volatile unsigned long*) 0x4003401C
#define AD0DR4 *(volatile unsigned long*) 0x40034020
#define AD0DR5 *(volatile unsigned long*) 0x40034024

// registri di configurazione di NVIC
#define ISER0 *(volatile unsigned long*) 0xE000E100

// registro di alimentazione delle periferiche
#define PCONP *(volatile unsigned long*) 0x400FC0C4

// registri PWM
#define PWM1PCR *(volatile unsigned long*) 0x4001804C
#define PWM1MR0 *(volatile unsigned long*) 0x40018018
#define PWM1MR1 *(volatile unsigned long*) 0x4001801C
#define PWM1MR2 *(volatile unsigned long*) 0x40018020
#define PWM1MR3 *(volatile unsigned long*) 0x40018024
#define PWM1MR4 *(volatile unsigned long*) 0x40018040
#define PWM1MR5 *(volatile unsigned long*) 0x40018044
#define PWM1MR6 *(volatile unsigned long*) 0x40018048
#define PWM1TCR *(volatile unsigned long*) 0x40018004
#define PWM1MCR *(volatile unsigned long*) 0x40018014
#define PWM1LER *(volatile unsigned long*) 0x40018050

// registri System Tick Timer
#define STCTRL *(volatile unsigned long*) 0xE000E010
#define STRELOAD *(volatile unsigned long*) 0xE000E014

#define CORE_CLK 96000000				// frequenza di clock del processore ARM
#define SYSTICK_VAL CORE_CLK/50			// 1/50 di secondo
#define PWM_PERIOD	(CORE_CLK/4)/50		// periodo PWM pari a 1/50 di secondo ossia 20ms (specifiche servomotore)

// valori estremi dutycycle per PWM e periodo base
#define PWM_dc_MAX	2400		// espresso in microsecondi
#define PWM_dc_MIN	600			// espresso in microsecondi
#define PWM_period  20000		// espresso in microsecondi


// valore letto dal convertitore analogico-digitale
unsigned int ADCvalue0 = 0, ADCvalue1 = 0, ADCvalue2 = 0, ADCvalue3 = 0, ADCvalue4 = 0, ADCvalue5 = 0;

void SysTick_Handler(void)
{
	PWM1MR1 = PWM1MR0/PWM_period*PWM_dc;
	PWM1MR2 = PWM1MR0/PWM_period*PWM_dc;
	PWM1MR3 = PWM1MR0/PWM_period*PWM_dc;
	PWM1MR4 = PWM1MR0/PWM_period*PWM_dc;
	PWM1MR5 = PWM1MR0/PWM_period*PWM_dc;
	PWM1MR6 = PWM1MR0/PWM_period*PWM_dc;
}

void ADC_IRQHandler(void)
{
	ADCvalue0 = ((AD0DR0 & 0xFFFF)>>4);	// il risultato di conversione in AD0DR0 è contenuto nei bit 15:4	[0:4095]
	ADCvalue1 = ((AD0DR0 & 0xFFFF)>>4);	// il risultato di conversione in AD0DR1 è contenuto nei bit 15:4	[0:4095]
	ADCvalue2 = ((AD0DR0 & 0xFFFF)>>4);	// il risultato di conversione in AD0DR2 è contenuto nei bit 15:4	[0:4095]
	ADCvalue3 = ((AD0DR0 & 0xFFFF)>>4);	// il risultato di conversione in AD0DR3 è contenuto nei bit 15:4	[0:4095]
	ADCvalue4 = ((AD0DR0 & 0xFFFF)>>4);	// il risultato di conversione in AD0DR4 è contenuto nei bit 15:4	[0:4095]
	ADCvalue5 = ((AD0DR0 & 0xFFFF)>>4);	// il risultato di conversione in AD0DR5 è contenuto nei bit 15:4	[0:4095]

}

int main(void){
	//impostazioni dei 6 canali ADC
	PCONP |= 1<<12;			// attiva l'alimentazione del convertitore analogico-digitale
	AD0CR |= 0x21FF3F;		// bit PDN=1(interruttore interno ADC), bit BURST=1, bits CLKDIV valore massimo (255), bits SEL= 0b111111(3F) (canali 0-1-2-3-4-5)
	AD0INTEN |= 0x3F;		// abilita l'interrupt per fine conversione su AD0.0-1-2-3-4-5
	PINSEL1 |= 0x154000;	// imposta la funzione di AD0.0 su P0.23, AD0.1 su P0.24, AD0.2 su P0.25, AD0.3 su P0.26
	PINSEL3 |= 0xF<<28;		// imposta la funzione di AD0.4 su P1.30, AD0.5 su P1.31
	ISER0 |= 1<<22;			// attiva la ricezione interrupt da ADC su NVIC

	// impostazioni PWM
	PINSEL4 |= 0x555;							// collega ai pin P2.0, P2.1, P2.2, P2.3, P2.4 e P2.5 le uscite PWM 1.1, PWM 1.2, PWM 1.3, PWM 1.4, PWM 1.5 e PWM 1.5
	PWM1PCR |= 0x7E00;							// abilita le uscite PWM1, PWM2, PWM3, PWM4, PWM5 e PWM6
	PWM1MR0 = PWM_PERIOD;						// prepara per MR0 il valore per il periodo desiderato
	PWM1MR1 = PWM1MR0/PWM_period*PWM_dc_MIN;	// prepara per MR1 un duty cycle min
	PWM1MR2 = PWM1MR0/PWM_period*PWM_dc_MIN;	// prepara per MR2 un duty cycle min
	PWM1MR3 = PWM1MR0/PWM_period*PWM_dc_MIN;	// prepara per MR3 un duty cycle min
	PWM1MR4 = PWM1MR0/PWM_period*PWM_dc_MIN;	// prepara per MR1 un duty cycle min
	PWM1MR5 = PWM1MR0/PWM_period*PWM_dc_MIN;	// prepara per MR2 un duty cycle min
	PWM1MR5 = PWM1MR0/PWM_period*PWM_dc_MIN;	// prepara per MR3 un duty cycle min
	PWM1LER |= 0x7F;							// carica i valori preparati per MR0, MR1, MR2, MR3, MR4, MR5 e MR6
	PWM1MCR |= 0b10;							// PWMMR0R=1 imposta MR0 affinchè il conteggio del periodo riparta automaticamente
	PWM1TCR = 0b1001;							// NOTA BENE: da eseguire DOPO tutte le altre impostazioni. PWM Enable + Counter Enable

	// Impostazioni System Tick Timer
	STRELOAD = SYSTICK_VAL;		// Carica il contatore per la frequenza di aggiornamento
	STCTRL = 7;					// Abilita il Timer, il suo interrupt, con clock interno

    // Enter an infinite loop, just waiting for interrupt
    while(1) {
    	__asm volatile ("wfi");		// Wait for interrupt
    }
    return 0 ;
}
