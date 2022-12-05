/*
**
**	Filename: HD44780.c
**
** ###################################################################
**
** P0-RS
** P1-RW
** P2-E
** P3-LED
** P4-D4
** P5-D5
** P6-D6
** P7-D7
** (P: PCF8574)
*/


#include "HD44780.h"
#include "lpcio.h"

/*********************** normal display 4bit mode ***********************/
int rs, rw, en, d4, d5, d6, d7, rsP, rwP, enP, d4P, d5P, d6P, d7P;

void lcd_pin(int register_select,  int read_write,  int enable,  int dataline4,  int dataline5,  int dataline6, int dataline7, pin_Port rs_port, pin_Port rw_port, pin_Port en_port, pin_Port d4_port, pin_Port d5_port, pin_Port d6_port, pin_Port d7_port)
{
	rs = register_select;
	rw = read_write;
	en = enable;

	d4 = dataline4;
	d5 = dataline5;
	d6 = dataline6;
	d7 = dataline7;

	rsP = rs_port;
	rwP = rw_port;
	enP = en_port;

	d4P = d4_port;
	d5P = d5_port;
	d6P = d6_port;
	d7P = d7_port;
}

void InitLCD()
{
	pinMode(rs, OUTPUT, rsP);
	pinMode(rw, OUTPUT, rwP);
	pinMode(en, OUTPUT, enP);
	pinMode(d4, OUTPUT, d4P);
	pinMode(d5, OUTPUT, d5P);
	pinMode(d6, OUTPUT, d6P);
	pinMode(d7, OUTPUT, d7P);

	lcd_cmd_hf(0x30);       	//-----Sequence for initializing LCD
    lcd_cmd_hf(0x30);       	//-----     "            "              "               "
    lcd_cmd_hf(0x20);       	//-----     "            "              "               "
    PutCommand(0x28);          //-----Selecting 16 x 2 LCD in 4Bit mode with 5x7 pixels
    PutCommand(0x0C);          //-----Display ON Cursor OFF Blinking OFF
    PutCommand(0x01);          //-----Clear display
    PutCommand(0x06);          //-----Cursor Auto Increment
    PutCommand(0x80);          //-----1st line 1st location of LCD 0 offset 80h
}

void clear_line()
{
	int dataLine[4] = {d4, d5, d6, d7};
	int dataLinePort[4] = {d4P, d5P, d6P, d7P};
	for(int i=0; i<=3; i++)
	{
		digitalWrite(dataLine[i], LOW, dataLinePort[i]);
	}
}

void toggle()
{
	digitalWrite(en, HIGH, enP);             //-----Latching data in to LCD data register using High to Low signal
	delayMs(2);
    digitalWrite(en, LOW, enP);
}

void send_high(char v0){
    if(v0 & 0x10){
    	digitalWrite(d4, HIGH, d4P);
    }else{
    	digitalWrite(d4, LOW, d4P);
    }

    if(v0 & 0x20){
    	digitalWrite(d5, HIGH, d5P);
    }else{
    	digitalWrite(d5, LOW, d5P);
    }

    if(v0 & 0x40){
    	digitalWrite(d6, HIGH, d6P);
    }else{
    	digitalWrite(d6, LOW, d6P);
    }

    if(v0 & 0x80){
    	digitalWrite(d7, HIGH, d7P);
    }else{
    	digitalWrite(d7, LOW, d7P);
    }
}

void send_low(char v0){
    if(v0 & 0x01){
    	digitalWrite(d4, HIGH, d4P);
    }else{
    	digitalWrite(d4, LOW, d4P);
    }

    if(v0 & 0x02){
    	digitalWrite(d5, HIGH, d5P);
    }else{
    	digitalWrite(d5, LOW, d5P);
    }

    if(v0 & 0x04){
    	digitalWrite(d6, HIGH, d6P);
    }else{
    	digitalWrite(d6, LOW, d6P);
    }

    if(v0 & 0x08){
    	digitalWrite(d7, HIGH, d7P);
    }else{
    	digitalWrite(d7, LOW, d7P);
    }
}

void lcd_cmd_hf(char v1)
{
    digitalWrite(rs, LOW, rsP);         //----Selecting register as Command register
    digitalWrite(rw, LOW, rwP);			//----Selecting write mode
    clear_line();                    	//----clearing the 4 bits data line
    send_high(v1);
    toggle();
}

void PutCommand(char v2)
{
    digitalWrite(rs, LOW, rsP);         //----Selecting register as command register
    digitalWrite(rw, LOW, rwP);			//----Selecting write mode
    clear_line();                    	//----clearing the 4 bits data line
    send_high(v2);
    toggle();

    clear_line();                    	//----clearing the Higher 4 bits
    send_low(v2);
    toggle();
}

void lcd_dwr(char v3)
{
    digitalWrite(rs, HIGH, rsP);        //----Selecting register as data register
    digitalWrite(rw, LOW, rwP);			//----Selecting write mode
    clear_line();                       //----clearing the Higher 4 bits
    send_high(v3);

    toggle();

    clear_line();                       //----clearing the Higher 4 bits
    send_low(v3);

    toggle();
}

void WriteString(unsigned char LineOfCharacters[TOTAL_CHARACTERS_OF_LCD], char OverLenghtCharacters)
{
	unsigned char i=0, line=-1;
	while(LineOfCharacters[i] && i<TOTAL_CHARACTERS_OF_LCD)
	{
		if(OverLenghtCharacters){
			if((i%LCD_LINE_LENGHT)==0)
				GoToLine(++line, 0);
		}

		lcd_dwr(LineOfCharacters[i]);
		i++;
	}
}


void WriteNumber(int intero){
	int i = 0, INDICE = 10, A[INDICE], Temp = 0, ciclo = 0;

	//if(intero < 10 && intero > 0) lcd_dwr(0x30);                  // stampa il simbolo zero

	while(intero!=0){
		A[i] = intero%10;
		intero/=10;
		i++;
		INDICE = i;
	}

	if(i == 0){
		lcd_dwr(0x30);                  // stampa il simbolo zero
	}else{
		int comodo = INDICE - 1; 		/* effettivo indice in C */

		/* algoritmo inversione elementi di un array intero da indice totale divido sempre 2 */
		for (ciclo=0; ciclo < (INDICE-((INDICE/2))); ciclo++)
		{
			Temp = A[comodo]; 			/* salvo ultimo elemento */
			A[comodo] = A[ciclo]; 		/* Scambio primo elemento con ultimo elemento */
			A[ciclo] = Temp; 			/* scambio ultimo elemento con primo elemento */
			comodo--;
		}

		for (ciclo = 0; ciclo < INDICE; ciclo++){
			int base = 0x30;			/*corrisponde al simbolo 0 nella tabella ASCII*/
			lcd_dwr(base + A[ciclo]);
		}
	}
}

void lcd_rig_sh()
{
	PutCommand(0x1C);      //----Command for right Shift
	delayMs(100);
}

void lcd_lef_sh()
{
	PutCommand(0x18);      //----Command for Left Shift
    delayMs(100);
}

void GoToLine(unsigned char line, unsigned char col){
	unsigned char address;
	switch(line)
	{
		case 0:
			address = 0x00;
		break;
		case 1:
			address = 0x40;
		break;
		case 2:
			address = 0x10;
		break;
		case 3:
			address = 0x50;
		break;
		default:
		address = 0x00;
		break;
	}
	if(col >= 0 && col <= LCD_LINE_LENGHT)	address += col;

	PutCommand(DDRAM_ADDRESS(address));		// Disabled for 8 char LCD DM0810
}

