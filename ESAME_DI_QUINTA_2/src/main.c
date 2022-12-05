/*
===============================================================================
 Name        : ESAME_DI_QUINTA_2.c
 Author      : $(author)
 Version     : 2.0
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "HD44780.h"
#include "lpcio.h"
#include "i2c.h"
#include "servo.h"			// include file with servo-pwm declarations
#include "lpc_joystick.h"
#include "lpc_registri.h"



/* PIN UTILIZZATI PER I SERVOMOTORI */
#define   servo_x	1
#define   servo_y	2

#define ADC_avg	100

// valori letti dal convertitore analogico-digitale
unsigned int ADCvalue0, ADCvalue1;


/***** Proto function *****/

void lcd_presentation();

/**************************/


void lcd_presentation()
{
	PutCommand(DISPLAY_CLEAR_CMD);          					//-----Clear display
	GoToLine(0,0);    											//---First line 1st Position
	WriteString((unsigned char*)"Display LCD 4bit", false);
	PutCommand_i2c(DISPLAY_CLEAR_CMD);							//-----Clear display (i2c)
	GoToLine_i2c(0,0);											//---First line 1st Position (i2c)
	WriteString_i2c((unsigned char*)"Display LCD i2c", false);

	delayMs(5000);

	GoToLine(1,0);												//---Second line 1st Position
	WriteString((unsigned char*)"LPC1769", false);
	GoToLine_i2c(1,0);											//---Second line 1st Position (i2c)
	WriteString_i2c((unsigned char*)"ARM Cortex-M3", false);

	delayMs(5000);

	PutCommand(DISPLAY_CLEAR_CMD);         				 		//-----Clear display
	GoToLine(0,0);    											//---First line 1st Position
	WriteString((unsigned char*)"Maturita' 2020", false);
	GoToLine(1,0);												//---Second line 1st Position
	WriteString((unsigned char*)"Simone Arcari", false);
	PutCommand_i2c(DISPLAY_CLEAR_CMD);							//-----Clear display (i2c)
	GoToLine_i2c(0,0);											//---First line 1st Position (i2c)
	WriteString_i2c((unsigned char*)"Esame di Stato", false);
	GoToLine_i2c(1,0);											//---Second line 1st Position (i2c)
	WriteString_i2c((unsigned char*)"Automazione", false);

	delayMs(5000);

	PutCommand(DISPLAY_CLEAR_CMD);          					//-----Clear display
	PutCommand_i2c(DISPLAY_CLEAR_CMD);							//-----Clear display (i2c)
}


void ADC_IRQHandler(void)
{
	ADCvalue0 = (ADCvalue0*(ADC_avg-1) + ((AD0DR0 & 0xFFFF)>>4))/ADC_avg;	// il risultato di conversione in AD0DR1 è contenuto nei bit 15:4
	ADCvalue1 = (ADCvalue1*(ADC_avg-1) + ((AD0DR1 & 0xFFFF)>>4))/ADC_avg;	// il risultato di conversione in AD0DR1 è contenuto nei bit 15:4

	PWM1MR2 = PWM1MR0/PWM_period*(ADCvalue0*(PWM_dc_MAX-PWM_dc_MIN)/4095+PWM_dc_MIN);
	PWM1MR3 = PWM1MR0/PWM_period*(ADCvalue1*(PWM_dc_MAX-PWM_dc_MIN)/4095+PWM_dc_MIN);

	PWM1LER |= 0b1111110;					// carica il valori preparato per MR1, MR2, MR3, MR4, MR5, MR6
}


int main(void)
{
	/****************************************Setup LCD****************************************/



	lcd_pin(13, 28, 27, 22, 21, 3, 2, DUE, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO  );
	InitLCD();

	setupI2C1();																									// I2C1 initialization
	InitLCD_i2c();
	lcd_presentation();

	/***************************************Setup servo***************************************/

	servo_pin_init(servo_x);	// Servo 1 initialization(PWM)
	servo_pin_init(servo_y);	// Servo 2 initialization(PWM)

	/*************************************Setup joystick**************************************/

	// impostazioni convertitore analogico-digitale (ADC)
	PCONP |= 1<<12;									// attiva l'alimentazione del convertitore analogico-digitale
	AD0CR |= 0x21FF03;								// bit PDN=1, bit BURST=1, bits CLKDIV valore massimo (255), bits SEL=03 (canali da 0 a 1)
	AD0INTEN |= 0b00000011;						// abilita l'interrupt globale per fine conversione
	PINSEL1 |= 0b010100000000000000;			// imposta la funzione di AD0.0, AD0.1, su P0.23, P0.24
	ISER0 |= 1<<22;									// attiva la ricezione interrupt da ADC su NVIC

	ADCvalue0 = ADCvalue1 = 0;

	lcd_presentation();

    // Enter an infinite loop, just waiting for interrupt
    while(1) {
    	__asm volatile ("wfi");		// Wait for interrupt
    }

    return 0 ;
}
