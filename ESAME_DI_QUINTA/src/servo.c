#include "servo.h"
#include "lpc_registri.h"

#define CORE_CLK 96000000									// ARM core clock



void servo_pin_init(int pin){
	if(pin >= 0 && pin <= 5){
		PINSEL4 |= 0b01<<((pin)*2);		// collega il pin desiderato alla sua rispettiva uscita pwm
		PWM1PCR |= 1<<(pin+9);				// abilita l'uscita PWM desiderata

		PWM1MR0 = (CORE_CLK/4)/50;			// prepara per MR0 il valore per un periodo pari a 20ms (specifiche servomotore)

		switch(pin){						// prepara per MR2 un duty cycle min (posizione inizio rotazione)
		case 0:
			PWM1MR1 = PWM1MR0/PWM_period*PWM_dc_MIN;
		break;

		case 1:
			PWM1MR2 = PWM1MR0/PWM_period*PWM_dc_MIN;
		break;

		case 2:
			PWM1MR3 = PWM1MR0/PWM_period*PWM_dc_MIN;
		break;

		case 3:
			PWM1MR4 = PWM1MR0/PWM_period*PWM_dc_MIN;
		break;

		case 4:
			PWM1MR5 = PWM1MR0/PWM_period*PWM_dc_MIN;
		break;

		case 5:
			PWM1MR6 = PWM1MR0/PWM_period*PWM_dc_MIN;
		break;
		}
		PWM1LER |= 0b10<<pin;			// carica i valori preparati per MR desiderato
		PWM1LER |= 1;					// carica i valori preparati per MR0
		PWM1MCR |= 0b10;				// PWMMR0R=1 imposta MR0 affinchè il conteggio del periodo riparta automaticamente
		PWM1TCR = 0b1001;
	}
}

void servo_write(int PWM_dc, int pin){
	switch(pin){
	case 0:
		PWM1MR1 = PWM1MR0/PWM_period*PWM_dc;
	break;

	case 1:
		PWM1MR2 = PWM1MR0/PWM_period*PWM_dc;
	break;

	case 2:
		PWM1MR3 = PWM1MR0/PWM_period*PWM_dc;
	break;

	case 3:
		PWM1MR4 = PWM1MR0/PWM_period*PWM_dc;
	break;

	case 4:
		PWM1MR5 = PWM1MR0/PWM_period*PWM_dc;
	break;

	case 5:
		PWM1MR6 = PWM1MR0/PWM_period*PWM_dc;
	break;
	}
	PWM1LER |= 0b10<<pin;			// carica i valori preparati per MR desiderato
}



