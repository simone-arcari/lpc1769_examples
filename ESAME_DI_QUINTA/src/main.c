#include "lpc_joystick.h"
#include "lpcio.h"
#include "servo.h"			// include file with servo-pwm declarations
#include "lpc_registri.h"	// include file with LPC1769 declarations
#include "i2c.h"			// include file with I2C declarations
#include "HD44780.h"		// include file with HD44780 declarations
#include "HC_SR04.h"


// carica in SysTick Timer il valore di semi-aggiornamento LCD
#define FREQ_HZ 60
#define SYSTICK_VAL CORE_CLK/FREQ_HZ - 1		// valore per il Timer di SysTick
#define RTI_CLK CORE_CLK/4						// Repetitive Timer clock frequency

/* PIN UTILIZZATI PER IL DISPLAY 4BIT */
#define   rs   13 	//-----Register Select pin of LCD  +++YELLOW+++
#define   rw   28 	//-----Read/Write pin select	   +++WHITE+++
#define   en   27 	//-----Enable pin of LCD		   +++GREEN+++
#define	  d4   22 	//-----Data line				   +++ORANGE+++
#define   d5   21 	//         “							“
#define   d6   3 	//         “							“
#define   d7   2 	//         “							“

/* PORTE DEI PIN UTILIZZATI PER IL DISPLAY 4 BIT*/
#define   rs_PORT   DUE 	//-----Register Select pin port of LCD
#define   rw_PORT   ZERO	//-----Read/Write pin port select
#define   en_PORT   ZERO 	//-----Enable pin port of LCD
#define	  d4_PORT   ZERO 	//-----Data line port
#define   d5_PORT   ZERO 	//         “
#define   d6_PORT   ZERO 	//         “
#define   d7_PORT   ZERO 	//         “

/* PIN UTILIZZATI PER IL JOYSTICK */
#define   X	  30
#define   Y	  31

/* PIN UTILIZZATI PER I SERVOMOTORI */
#define   servo_x	1
#define   servo_y	2

int joy_x = 0, joy_y = 0, pwm_x = 0, pwm_y =0;
unsigned long TimerVal=0;


/***** Proto function *****/
void lcd_presentation();
void EnContinuosMisure();
void DsContinuosMisure();
void StartHC_SR04();
/**************************/


// Gestore di interrupt dal timer di sistema
void SysTick_Handler(void)
{
	joy_x = value_x();
	joy_y = value_y();

    pwm_x = ((joy_x*(PWM_dc_MAX-PWM_dc_MIN))/4095)+PWM_dc_MIN;
    pwm_y = ((joy_y*(PWM_dc_MAX-PWM_dc_MIN))/4095)+PWM_dc_MIN;
    servo_write(pwm_x, servo_x);
    servo_write(pwm_y , servo_y);
	/*
	GoToLine(0,0);    		//---First line 1st Position
	lcd_msg("Cord X: ");
	lcd_int_msg(joy_x);
	GoToLine(1,0);    		//---First line 1st Position
	lcd_msg("Cord Y: ");
	lcd_int_msg(joy_y);
	GoToLine_i2c(0,0);		//---First line 1st Position (i2c)
	WriteString_i2c((unsigned char*)"Distanza misurata: ", true);
*/
}
// Gestore di interrupt dal RITimer
void RIT_IRQHandler(void)
{
	StartHC_SR04(); 				// lancia l'impulso di 15uS
	RICTRL |= 0b0001;					// Set RITINT in RICTRL register in order to clear the int flag
}
// Gestore di interrupt dal TIMER2
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

		int val;

		val= TimerVal/58;			// converte i microsecondi in cm (con velocità del suono di 343 m/s)
		lcd_cmd(DISPLAY_CLEAR_CMD);          						//-----Clear display
	    GoToLine(0,0);    		//---First line 1st Position
	    lcd_msg("Distanza: ");
	    lcd_int_msg(val);
	    lcd_msg("cm");

	}
	T2IR = 1<<4;			// reset dell'interrupt su CR0 di TIMER2
}
// Gestore di interrupt dal External Interrupt 3
void EINT3_IRQHandler(void)
{
	if(IO0IntStatF & (1<<9)) 			//interrupt proveniente da P0.9
	{
		StartHC_SR04(); 				// lancia l'impulso di 15uS
		IO0IntClr |= 1<<9;				//cancella l'interrupt appena gestito di P0.9
	}

	if(IO0IntStatF & (1<<8)) 			//interrupt proveniente da P0.8
	{
		int click = 1;
		if(click){
			EnContinuosMisure();				//attiva il RIT per fare misure continue della distanza
			click = 0;
		}else{
			DsContinuosMisure();				//disattiva il RIT per fare misure continue della distanza
			click = 1;
		}
		IO0IntClr |= 1<<8;			//cancella l'interrupt appena gestito di P0.8
	}
}


int main(void) {
	lcd_pin(rs,rw,en,d4,d5,d6,d7, rs_PORT, rw_PORT, en_PORT, d4_PORT, d5_PORT, d6_PORT, d7_PORT);	// LCD pin declaration
	lcd_init();				    //---LCD initialization

    setupI2C1();				// I2C1 initialization
    InitLCD_i2c();				// LCD with i2c initialization

    lcd_presentation();

	joystick_pin(X, Y);			// Joystick pin declaration
	joystick_init();			// Joystick initialization(ADC)

	servo_pin_init(servo_x);	// Servo 1 initialization(PWM)
	servo_pin_init(servo_y);	// Servo 2 initialization(PWM)


	STRELOAD = SYSTICK_VAL;		// Carica il contatore per la frequenza di aggiornamento(Impostazioni System Tick Timer)
	STCTRL = 7;					// Abilita il Timer, il suo interrupt, con clock interno

	initTimer3Match();			// Inizializza Timer3 come Match su P0.10
    initTimer2Capture();		// Inizializza Timer2 come Capture su P0.4

	//*************IMPOSTAZIONI INTERRUPT tasto***************
	IO0IntEnF |= (1<<9) | (1<<8);//imposta P0.9-8 come fonti di interrupt (fronte di discesa)
	IPR5 |= 0x10<<11; 			 //priorità interrupt di valore 16 --> 0x10 = 16
	ISER0 |= 1<<21;				 //abilita external interrupt 3



    while(1) {
/*
    	joy_x = value_x();
    	joy_y = value_y();

    	GoToLine(0,0);    		//---First line 1st Position
    	lcd_msg("Cord X: ");
    	lcd_int_msg(joy_x);
    	GoToLine(1,0);			//---Second line 1st Position
    	lcd_msg("Cord Y: ");
    	lcd_int_msg(joy_y);
    	GoToLine_i2c(0,0);		//---First line 1st Position (i2c)
    	WriteString_i2c((unsigned char*)"Distanza misurata: ", true);

    	//serve per cancellare solo le cifre che non servono
    	if(joy_x < 10){
    		GoToLine(0,9);			//---First line 10st Position
        	lcd_msg("   ");
    	}
    	if(joy_x >= 10 && joy_x < 100){
    		GoToLine(0,10);			//---First line 11st Position
        	lcd_msg("  ");
    	}
    	if(joy_x >= 100 && joy_x < 1000){
    		GoToLine(0,11);			//---First line 12st Position
        	lcd_msg(" ");
    	}

    	//serve per cancellare solo le cifre che non servono
    	if(joy_y < 10){
    		GoToLine(1,9);			//---Second line 10st Position
    		lcd_msg("   ");
    	}
        if(joy_y >= 10 && joy_y < 100){
        	GoToLine(1,10);			//---Second line 11st Position
        	lcd_msg("  ");
        }
        if(joy_y >= 100 && joy_y < 1000){
        	GoToLine(1,11);			//---Second line 12st Position
        	lcd_msg(" ");
        }


        pwm_x = ((joy_x*(PWM_dc_MAX-PWM_dc_MIN))/4095)+PWM_dc_MIN;
        pwm_y = ((joy_y*(PWM_dc_MAX-PWM_dc_MIN))/4095)+PWM_dc_MIN;
        servo_write(pwm_x, servo_x);
        servo_write(pwm_y , servo_y);
*/
        __asm volatile ("wfi");		// Wait for interrupt
    }
    return 0 ;
}


void lcd_presentation(){
	lcd_cmd(DISPLAY_CLEAR_CMD);          						//-----Clear display
	GoToLine(0,0);    											//---First line 1st Position
	lcd_msg("Display LCD 4bit");
	PutCommand_i2c(DISPLAY_CLEAR_CMD);							//-----Clear display (i2c)
	GoToLine_i2c(0,0);											//---First line 1st Position (i2c)
	WriteString_i2c((unsigned char*)"Display LCD i2c", false);

	delayMs(5000);

	GoToLine(1,0);												//---Second line 1st Position
	lcd_msg("LPC1769");
	GoToLine_i2c(1,0);											//---Second line 1st Position (i2c)
	WriteString_i2c((unsigned char*)"ARM Cortex-M3", false);

	delayMs(5000);

	lcd_cmd(DISPLAY_CLEAR_CMD);         				 		//-----Clear display
	GoToLine(0,0);    											//---First line 1st Position
	lcd_msg("Maturita' 2020");
	GoToLine(1,0);												//---Second line 1st Position
	lcd_msg("Simone Arcari");
	PutCommand_i2c(DISPLAY_CLEAR_CMD);							//-----Clear display (i2c)
	GoToLine_i2c(0,0);											//---First line 1st Position (i2c)
	WriteString_i2c((unsigned char*)"Esame di Stato", false);
	GoToLine_i2c(1,0);											//---Second line 1st Position (i2c)
	WriteString_i2c((unsigned char*)"Automazione", false);

	delayMs(5000);

	lcd_cmd(DISPLAY_CLEAR_CMD);          						//-----Clear display
	//PutCommand_i2c(DISPLAY_CLEAR_CMD);							//-----Clear display (i2c)
}
void EnContinuosMisure(){
	// Repetitive Interrupt Timer settings
	PCONP |= 1<<16;				 		// enable Repetitive Interrupt Timer power/clock
	RICTRL |= 0b10;				 		// Set RITENCLR bit in RICTRL register
	RICOMPVAL =	RTI_CLK/2;				// Carica il contatore per la frequenza di aggiornamento(0.5s)
	ISER0 |= 1<<29;				 		// enable in NVIC interrupt reception from RTI
}
void DsContinuosMisure(){
	// Repetitive Interrupt Timer settings
	PCONP |= 0<<16;				 		// disable Repetitive Interrupt Timer power/clock
	ISER0 |= 0<<29;				 		// disable in NVIC interrupt reception from RTI
}
void StartHC_SR04(){
	// lancia l'impulso di 15uS
	T3TCR = 0b10;					// reset counter del Timer3, conteggio fermo
	T3EMR |= 1;						// fronte di salita su P0.10 per inizio impulso di trigger
	T3TCR = 0b01;					// avvio conteggio Timer3 (si fermerà dopo 15us con fronte di discesa su P0.10)
}




