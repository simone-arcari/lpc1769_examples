#include "lpc_joystick.h"
#include "lpc_registri.h"

int pin_X, pin_Y, AD0DR_X, AD0DR_Y, X_Value = 0, Y_Value = 0;

void joystick_pin(int x_pin, int y_pin){
	//controllo se i pin scelti dall'utente siano corretti
	if(x_pin != y_pin){
		pin_X = pin_correct_test(x_pin);
		pin_Y = pin_correct_test(y_pin);
	}
}
void joystick_init(){
	//settings of A/D converter (ADC)
	PCONP |= 1<<12;			//enable power supply for ADC peripherals (bit 12 of this register --> HIGH)
	AD0CR |= 0x21FF00; 		//bit PDN --> HIGH, bit BURST --> HIGH, bitS CLKDIV --> 0xFF(255 max value), bits SEL ancora non dichiarati
	AD0INTEN = 0;		//bit 8(ADGINTEN) --> LOW, enable interrupt only from channel 0


	//assegnazione dei bit SEL in AD0CR e dei rispettivi interrupt in AD0INTEN e anche dei bit di gestione pin in PINSELx
	set_channel(pin_X);
	set_channel(pin_Y);

	//ISER0 |= 1<<22;			//activates interrupt reception from ADC to NVIC (bit 22(ADC) --> HIGH
}
int pin_correct_test(int pin){
	int result;
	if((pin >= 23 && pin <= 26) || (pin >= 2 && pin <= 3) || (pin ==30 || pin == 31)){
		result = pin;
	}
	return result;
}
void set_channel(int pin){
	for(int i=0; i <= 7; i++){
		switch(i){
			case 0:
				if(pin == 23){
					AD0CR |= 1<<i;						//enable channel 0
					AD0INTEN |= 1<<i;					//enable interrupt from channel 0
					PINSEL1 |= 0x01<<14;				//set P0.23 with function 01 --> AD0.0
				}
			break;

			case 1:
				if(pin == 24){
					AD0CR |= 1<<i;						//enable channel 1
					AD0INTEN |= 1<<i;					//enable interrupt from channel 1
					PINSEL1 |= 0x01<<16;				//set P0.24 with function 01 --> AD0.1
				}
			break;

			case 2:
				if(pin == 25){
					AD0CR |= 1<<i;						//enable channel 2
					AD0INTEN |= 1<<i;					//enable interrupt from channel 2
					PINSEL1 |= 0x01<<18;				//set P0.25 with function 01 --> AD0.2
				}
			break;
			case 3:
				if(pin == 26){
					AD0CR |= 1<<i;						//enable channel 3
					AD0INTEN |= 1<<i;					//enable interrupt from channel 3
					PINSEL1 |= 0x01<<20;				//set P0.26 with function 01 --> AD0.3
				}
			break;

			case 4:
				if(pin == 30){
					AD0CR |= 1<<i;						//enable channel 4
					AD0INTEN |= 1<<i;					//enable interrupt from channel 4
					PINSEL3 |= 0x3<<28;					//set P1.30 with function 11 --> AD0.4
				}
			break;

			case 5:
				if(pin == 31){
					AD0CR |= 1<<i;						//enable channel 5
					AD0INTEN |= 1<<i;					//enable interrupt from channel 5
					PINSEL3 |= 0x3<<30;					//set P1.30 with function 11 --> AD0.5
				}
			break;

			case 6:
				if(pin == 3){
					AD0CR |= 1<<i;						//enable channel 6
					AD0INTEN |= 1<<i;					//enable interrupt from channel 6
					PINSEL0 |= 0x2<<6;	    			//set P0.3 with function 10 --> AD0.6
				}
			break;

			case 7:
				if(pin == 2){
					AD0CR |= 1<<i;						//enable channel 7
					AD0INTEN |= 1<<i;					//enable interrupt from channel 7
					PINSEL0 |= 0x2<<4;	    			//set P0.2 with function 10 --> AD0.7
				}
			break;
		}
	}
}
int value_x(){
	static int x, prot_x;

	switch(pin_X){
		case 23:
			prot_x = ((AD0DR0 & 0xFFFF)>>4);
		break;

		case 24:
			prot_x = ((AD0DR1 & 0xFFFF)>>4);
		break;

		case 25:
			prot_x = ((AD0DR2 & 0xFFFF)>>4);
		break;

		case 26:
			prot_x = ((AD0DR3 & 0xFFFF)>>4);
		break;

		case 30:
			prot_x = ((AD0DR4 & 0xFFFF)>>4);
		break;

		case 31:
			prot_x = ((AD0DR5 & 0xFFFF)>>4);
		break;

		case 3:
			prot_x = ((AD0DR6 & 0xFFFF)>>4);
		break;

		case 2:
			prot_x = ((AD0DR7 & 0xFFFF)>>4);
		break;
	}

	if(prot_x < (x-5) || prot_x > (x+5)){
		x = prot_x;
	}else if(prot_x == 0)	  x = prot_x;

	return(x);
}
int value_y(){
	static int y, prot_y;

	switch(pin_Y){
		case 23:
			prot_y = ((AD0DR0 & 0xFFFF)>>4);
		break;

		case 24:
			prot_y = ((AD0DR1 & 0xFFFF)>>4);
		break;

		case 25:
			prot_y = ((AD0DR2 & 0xFFFF)>>4);
		break;

		case 26:
			prot_y = ((AD0DR3 & 0xFFFF)>>4);
		break;

		case 30:
			prot_y = ((AD0DR4 & 0xFFFF)>>4);
		break;

		case 31:
			prot_y = ((AD0DR5 & 0xFFFF)>>4);
		break;

		case 3:
			prot_y = ((AD0DR6 & 0xFFFF)>>4);
		break;

		case 2:
			prot_y = ((AD0DR7 & 0xFFFF)>>4);
		break;
	}

	if(prot_y < (y-5) || prot_y > (y+5)){
		y = prot_y;
	}

	return(y);
}

