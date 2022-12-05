/*
===============================================================================
 Name        : lpc1769.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#define FIO2DIR *(volatile unsigned long*) 0x2009C040
#define FIO2SET *(volatile unsigned long*) 0x2009C058
#define FIO2CLR *(volatile unsigned long*) 0x2009C05C
#define FIO2PIN *(volatile unsigned long*) 0x2009C054

#define FIO0DIR *(volatile unsigned long*) 0x2009C000		//imposta i pin in output (1) input(0)
#define FIO0SET *(volatile unsigned long*) 0x2009C018		//imposta i pin a livello logico alto (1)
#define FIO0CLR *(volatile unsigned long*) 0x2009C01C		//imposta i pin a livello logico basso (1)

#define IO0IntEnF *(volatile unsigned long*) 0x40028094
#define IO0IntClr *(volatile unsigned long*) 0x4002808C

// registri System Tick Timer
#define STCTRL *(volatile unsigned long*) 0xE000E010
#define STRELOAD *(volatile unsigned long*) 0xE000E014

// registro di abilitazione interrupt periferiche su NVIC
#define ISER0 *(volatile unsigned long*) 0xE000E100		//
#define IPR5 *(volatile unsigned long*) 0xE000E414		// registro per priorità interrupt di periferiche

// frequenza di clock del processore ARM
#define CORE_CLK 96000000

// carica in SysTick Timer il valore di aggiornamento del 7-segmenti
#define SYSTICK_VAL CORE_CLK/100			// 1/100 di secondo

// definizioni bit 7-segmenti
// Nota bene: tutti i segnali di uscita sono attivi bassi
// quindi per accendere un LED bisogna impostare a livello logico basso l'output
// P2.0 Led A, P2.1 Led B, P2.2 Led C, P2.3 Led D, P2.4 Led E, P2.5 Led F, P2.6 Led G, P2.7 Led DP
//	P2.10 abilita Cifra 1, P2.11 abilita Cifra 2

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
		case 10:
			FIO2CLR = 0x77; // LED A, B, C, E, F, G accesi
		break;
		case 11:
			FIO2CLR = 0x7C; // LED C, D, E, F, G accesi
		break;
		case 12:
			FIO2CLR = 0x39; // LED A, D, E, F accesi
		break;
		case 13:
			FIO2CLR = 0x5E; // LED B, C, D, E, G accesi
		break;
		case 14:
			FIO2CLR = 0x79; // LED A, D, E, F, G accesi
		break;
		case 15:
			FIO2CLR = 0x71; // LED A, E, F, G accesi
		break;

		default:			// valore impossibile
			FIO2SET = 0xFF;	// tutto spento
	}

}


// Gestore di interrupt dal timer di sistema
// all'interno è realizzato sia l'incremento del numero a due cifre da visualizzare (ogni secondo)
// sia l'aggiornamento delle cifre che vengono accese in modo alternato (ogni centesimo di secondo)

int Verso = 1, total = 100;

unsigned long counter = 0;

void SysTick_Handler(void)
{
	static int number = 0;
	int digit;

	// ciclo di generazione del numero da visualizzare, da 0 a 255
	counter++;
	if(counter==total)
	{
		counter=0;

		if(Verso){
			number++;
			if(number==255)
				Verso=0;
		}else{
			number--;
			if(number==0)
				Verso=1;
		}
	}
	//************************************************

	if(FIO2PIN & 0x0400)	// test sul pin P2.10 (cifra 1, più significativa)
	{
		// se entriamo qui vuol dire che la cifra 1 era spenta e la cifra 2 era accesa, quindi invertiamo
		FIO2SET = 0x0800;				// P2.11 alto => spegni cifra 2
		digit = (number & 0xF0)>>4; 	// calcolo cifra più significativa
		if(digit==0)					// scrive solo cifre diverse da zero
			digit=-1;					// forza la cancellazione della cifra più significativa con un valore impossibile
		setDigits(digit);
		FIO2CLR = 0x0400;				// P2.10 basso => accendi cifra 1
	}
	else
	{
		// se entriamo qui vuol dire che la cifra 1 era accesa e la cifra 2 era spenta, quindi invertiamo
		FIO2SET = 0x0400;				// P2.10 alto => spegni cifra 1
		digit = number & 0x0F;			// calcolo cifra più significativa
		setDigits(digit);
		FIO2CLR = 0x0800;				// P2.11 basso => accendi cifra 2
	}

}


// Gestore di interrupt dal timer da GPIO (condiviso con ExtINT3)
void EINT3_IRQHandler (void)
{
	static int step = 0;

	step++;							// contatore a 3 valori: 0, 1, 2
	if(step>3)
		step=0;

    FIO0SET = 0x5860000C;

	switch(step)
	{
		case 0:
		    FIO0CLR = 0x5820000C;   // LED A, B, C, D, E, F accesi
			total = 100;
		break;
		case 1:
			FIO0CLR = 0x40000004;			// LED B, C accesi
			total = 50;
		break;
		case 2:
			FIO0CLR = 0x1840000C;	// LED A, B, D, E, G accesi
			total = 25;
		break;
		case 3:
			FIO0CLR = 0x4840000C;	// LED A, B, C, D, E, G accesi
			total = 10;
		break;

		default:
			total = 100;
	}
	counter = 0;

	IO0IntClr |= 1<<9;					// cancella l'interrupt di P0.9 appena gestito
}


int main(void) {

	// Imposta GPIO da P2.0 a P2.7 e P2.10, P2.11 come output
    FIO2DIR |= 0x0CFF;

    // Imposta GPIO P0.(2-3-21-22-27-28-31)
    FIO0DIR |= 0x5860000C;

    // Imposta a livello logico alto tutti gli output (che sono in logica negata, quindi attivi bassi)
    FIO2SET = 0x0CFF;
    FIO0SET = 0x5860000C;

	// Impostazioni System Tick Timer
	STRELOAD = SYSTICK_VAL;		// Carica il contatore per la frequenza di aggiornamento
	STCTRL = 7;					// Abilita il Timer, il suo interrupt, con clock interno

    // Impostazioni GPIO P0.9 input con pull-up (default al Reset) e abilitazione interrupt
    IO0IntEnF |= 1<<9;		// abilita trasmissione interrupt da GPIO su fronte di discesa del pin 0.9
    IPR5 |= 0x0F<<11;		// imposta la priorità interrupt su GPIO a 0x0F (un valore intermedio)
    ISER0 |= 1<<21;			// abilita in NVIC ricezione interrupt da GPIO

    // Enter an infinite loop, just incrementing a counter
    while(1) {
    	__asm volatile ("wfi");		// Wait for interrupt
    }
    return 0 ;
}
