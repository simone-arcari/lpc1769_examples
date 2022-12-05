/*
 * lpc_registri.h
 *
 *  Created on: 10 feb 2019
 *      Author: Simone Arcari
 */

#ifndef LPC_REGISTRI_H_
#define LPC_REGISTRI_H_


#define CORE_CLK 96000000				// ARM core clock

#define PINMODE0 *(volatile unsigned long*) 0x4002C040
#define PINMODE1 *(volatile unsigned long*) 0x4002C044
#define PINMODE_OD0 *(volatile unsigned long*) 0x4002C068


// REGISTRI GPIO PORTA 0
#define FIO0DIR *(volatile unsigned long*) 0x2009C000		//imposta i pin in output (1) input(0)
#define FIO0MASK *(volatile unsigned long*) 0x2009C010		//imposta una maschera sui pin desiderati
#define FIO0SET *(volatile unsigned long*) 0x2009C018		//imposta i pin a livello logico alto (1)
#define FIO0CLR *(volatile unsigned long*) 0x2009C01C		//imposta i pin a livello logico basso (1)
#define FIO0PIN *(volatile unsigned long*) 0x2009C014		//imposta i pin a livello logico alto (1) e a livello logico basso (0)   viene usato per verificare lo stato dei pin 1=accesi 0=spenti


// REGISTRI GPIO PORTA 1
#define FIO1DIR *(volatile unsigned long*) 0x2009C020		//imposta i pin in output (1) input(0)
#define FIO1MASK *(volatile unsigned long*) 0x2009C030		//imposta una maschera sui pin desiderati
#define FIO1SET *(volatile unsigned long*) 0x2009C038		//imposta i pin a livello logico alto (1)
#define FIO1CLR *(volatile unsigned long*) 0x2009C03C		//imposta i pin a livello logico basso (1)
#define FIO1PIN *(volatile unsigned long*) 0x2009C034		//imposta i pin a livello logico alto (1) e a livello logico basso (0)   viene usato per verificare lo stato dei pin 1=accesi 0=spenti

// REGISTRI GPIO PORTA 2
#define FIO2DIR *(volatile unsigned long*) 0x2009C040		//imposta i pin in output (1) input(0)
#define FIO2MASK *(volatile unsigned long*) 0x2009C050		//imposta una maschera sui pin desiderati
#define FIO2SET *(volatile unsigned long*) 0x2009C058		//imposta i pin a livello logico alto (1)
#define FIO2CLR *(volatile unsigned long*) 0x2009C05C		//imposta i pin a livello logico basso (1)
#define FIO2PIN *(volatile unsigned long*) 0x2009C054		//imposta i pin a livello logico alto (1) e a livello logico basso (0)   viene usato per verificare lo stato dei pin 1=accesi 0=spenti


// REGISTRI GPIO PORTA 3
#define FIO3DIR *(volatile unsigned long*) 0x2009C060		//imposta i pin in output (1) input(0)
#define FIO3SET *(volatile unsigned long*) 0x2009C078		//imposta i pin a livello logico alto (1)
#define FIO3CLR *(volatile unsigned long*) 0x2009C07C		//imposta i pin a livello logico basso (1)
#define FIO3PIN *(volatile unsigned long*) 0x2009C074		//imposta i pin a livello logico alto (1) e a livello logico basso (0)   viene usato per verificare lo stato dei pin 1=accesi 0=spenti


// REGISTRI GPIO PORTA 4
#define FIO4DIR *(volatile unsigned long*) 0x2009C080		//imposta i pin in output (1) input(0)
#define FIO4SET *(volatile unsigned long*) 0x2009C098		//imposta i pin a livello logico alto (1)
#define FIO4CLR *(volatile unsigned long*) 0x2009C09C		//imposta i pin a livello logico basso (1)
#define FIO4PIN *(volatile unsigned long*) 0x2009C094		//imposta i pin a livello logico alto (1) e a livello logico basso (0)   viene usato per verificare lo stato dei pin 1=accesi 0=spenti


// REGISTRI SELEZIONE FUNZIONE DEI PIN
#define PINSEL0 *(volatile unsigned long*) 0x4002C000		// registro di selezione funzioni sui porta 0 pin 0-15
#define PINSEL1 *(volatile unsigned long*) 0x4002C004		// registro di selezione funzioni sui porta 0 pin 16-31
#define PINSEL2 *(volatile unsigned long*) 0x4002C00		// registro di selezione funzioni porta 1 sui pin 0-15
#define PINSEL3 *(volatile unsigned long*) 0x4002C00C		// registro di selezione funzioni porta 1 sui pin 16-31
#define PINSEL4 *(volatile unsigned long*) 0x4002C010		// registro di selezione funzioni porta 2 sui pin 0-15
#define PINSEL7 *(volatile unsigned long*) 0x4002C01C		// registro di selezione funzioni porta 3 sui pin 16-31
#define PINSEL8 *(volatile unsigned long*) 0x4002C020		// registro di selezione funzioni porta 3 sui pin 16-31
#define PINSEL9 *(volatile unsigned long*) 0x4002C024		// registro di selezione funzioni porta 4 sui pin 16-31
#define PINSEL10 *(volatile unsigned long*) 0x4002C028		// registro di selezione funzioni trace port enable



// REGISTRI System Tick Timer
#define STCTRL *(volatile unsigned long*) 0xE000E010		//abilita il conteggio del System Tick Timer, abilita l'interrupt del System Tick Timer, possiamo impostare un clock esterno   (di base scrivere STCTRL |= 7;)
#define STRELOAD *(volatile unsigned long*) 0xE000E014		//dentro questo registro va caricato il numero da far contare al System Tick Timer
#define STCURR *(volatile unsigned long*) 0xE000E018		//ci restituisce in tempo reale il valore corrente del System Tick Timer
#define STCALIB *(volatile unsigned long*) 0xE000E01C


//REGISTRI INTERRUPT ESTERNO
#define IO0IntEnF *(volatile unsigned long*) 0x40028094		//imposta i pin porta 0 come sorgente di interrupt sulfronte di discesa
#define IO2IntEnF *(volatile unsigned long*) 0x400280B4		//imposta i pin porta 2 come sorgente di interrupt sulfronte di discesa

#define IO0IntEnR *(volatile unsigned long*) 0x40028090		//imposta i pin porta 0 come sorgente di interrupt sulfronte di salita
#define IO2IntEnR *(volatile unsigned long*) 0x400280B0		//imposta i pin porta 2 come sorgente di interrupt sulfronte di salita

#define IO0IntClr *(volatile unsigned long*) 0x4002808C		//cancella l'interrupt dei pin porta 0
#define IO2IntClr *(volatile unsigned long*) 0x400280AC		//cancella l'interrupt dei pin porta 2

#define IO0IntStatF *(volatile unsigned long*) 0x40028088	//ci restituisce i bit dei pin porta 0 da cui è arrivato l'interrupt sul fronte di discesa
#define IO2IntStatF *(volatile unsigned long*) 0x400280A8	//ci restituisce i bit dei pin porta 2 da cui è arrivato l'interrupt sul fronte di discesa

#define IO0IntStatR *(volatile unsigned long*) 0x40028084	//ci restituisce i bit dei pin porta 0 da cui è arrivato l'interrupt sul fronte di salita
#define IO2IntStatR *(volatile unsigned long*) 0x400280A4	//ci restituisce i bit dei pin porta 0 da cui è arrivato l'interrupt sul fronte di salita


// REGISTRI CONFIGURAZIONE NVIC
#define ISER0 *(volatile unsigned long*) 0xE000E100			//abilita interrupt periferiche
#define ISER1 *(volatile unsigned long*) 0xE000E104			//abilita interrupt periferiche
#define ICER0 *(volatile unsigned long*) 0xE000E180
#define ICER1 *(volatile unsigned long*) 0xE000E184
#define ISPR0 *(volatile unsigned long*) 0xE000E200
#define ISPR1 *(volatile unsigned long*) 0xE000E204
#define ICPR0 *(volatile unsigned long*) 0xE000E280
#define ICPR1 *(volatile unsigned long*) 0xE000E284
#define IABR0 *(volatile unsigned long*) 0xE000E300
#define IABR1 *(volatile unsigned long*) 0xE000E304
#define IPR0 *(volatile unsigned long*) 0xE000E400			//registro per impostare la priorità alle diverse periferiche
#define IPR1 *(volatile unsigned long*) 0xE000E404			//registro per impostare la priorità alle diverse periferiche
#define IPR2 *(volatile unsigned long*) 0xE000E408			//registro per impostare la priorità alle diverse periferiche
#define IPR3 *(volatile unsigned long*) 0xE000E40C			//registro per impostare la priorità alle diverse periferiche
#define IPR4 *(volatile unsigned long*) 0xE000E410			//registro per impostare la priorità alle diverse periferiche
#define IPR5 *(volatile unsigned long*) 0xE000E414			//registro per impostare la priorità alle diverse periferiche
#define IPR6 *(volatile unsigned long*) 0xE000E418			//registro per impostare la priorità alle diverse periferiche
#define IPR7 *(volatile unsigned long*) 0xE000E41C			//registro per impostare la priorità alle diverse periferiche
#define IPR8 *(volatile unsigned long*) 0xE000E420			//registro per impostare la priorità alle diverse periferiche
//#define STIR *(volatile unsigned long*) 0xE000EF00


// REGISTRO ALIMENTAZIONE PERIFERICHE
#define PCONP *(volatile unsigned long*) 0x400FC0C4			//ricorda che in molti casi le periferiche hanno a loro volta un interruttore interno


// REGISTRI CONVERITORE ANALOGICO-DIGITALE (ADC)
#define AD0CR *(volatile unsigned long*) 0x40034000
#define ADGDR *(volatile unsigned long*) 0x40034004
#define AD0INTEN *(volatile unsigned long*) 0x4003400C
#define AD0DR0 *(volatile unsigned long*) 0x40034010
#define AD0DR1 *(volatile unsigned long*) 0x40034014
#define AD0DR2 *(volatile unsigned long*) 0x40034018
#define AD0DR3 *(volatile unsigned long*) 0x4003401C
#define AD0DR4 *(volatile unsigned long*) 0x40034020
#define AD0DR5 *(volatile unsigned long*) 0x40034024
#define AD0DR6 *(volatile unsigned long*) 0x40034028
#define AD0DR7 *(volatile unsigned long*) 0x4003402C
#define ADSTAT *(volatile unsigned long*) 0x40034030
//#define ADTRM *(volatile unsigned long*) 0x40034034


//REGISTRI RITimer
#define RICOMPVAL *(volatile unsigned long*) 0x400B0000		//valore da caricare a 32 bit
#define RIMASK *(volatile unsigned long*) 0x400B0004		//forza l'interrupt
#define RICTRL *(volatile unsigned long*) 0x400B0008		//settaggi
#define RICOUNTER *(volatile unsigned long*) 0x400B000C		//conteggio in tempo reale

//REGISTRI PWM(Pulse Width Modulation)
#define PWM1IR *(volatile unsigned long*) 0x40018000
#define PWM1TCR *(volatile unsigned long*) 0x40018004
#define PWM1TC *(volatile unsigned long*) 0x40018008
#define PWM1PR *(volatile unsigned long*) 0x4001800C
#define PWM1PC *(volatile unsigned long*) 0x40018010
#define PWM1MCR *(volatile unsigned long*) 0x40018014
#define PWM1MR0 *(volatile unsigned long*) 0x40018018
#define PWM1MR1 *(volatile unsigned long*) 0x4001801C
#define PWM1MR2 *(volatile unsigned long*) 0x40018020
#define PWM1MR3 *(volatile unsigned long*) 0x40018024
#define PWM1CCR *(volatile unsigned long*) 0x40018028
#define PWM1CR0 *(volatile unsigned long*) 0x4001802C
#define PWM1CR1 *(volatile unsigned long*) 0x40018030
#define PWM1CR2 *(volatile unsigned long*) 0x40018034
#define PWM1CR3 *(volatile unsigned long*) 0x40018038
#define PWM1MR4 *(volatile unsigned long*) 0x40018040
#define PWM1MR5 *(volatile unsigned long*) 0x40018044
#define PWM1MR6 *(volatile unsigned long*) 0x40018048
#define PWM1PCR *(volatile unsigned long*) 0x4001804C
#define PWM1LER *(volatile unsigned long*) 0x40018050
#define PWM1CTCR *(volatile unsigned long*) 0x40018070

//Peripheral Clock Selection
#define PCLKSEL0 *(volatile unsigned long*) 0x400FC1A8
#define PCLKSEL1 *(volatile unsigned long*) 0x400FC1AC

//REGISTRI TIMER1
#define TIMER1_TCR *(volatile unsigned long*) 0x40008004
#define TIMER1_PR  *(volatile unsigned long*) 0x4000800C
#define TIMER1_MR0 *(volatile unsigned long*) 0x40008018
#define TIMER1_IR *(volatile unsigned long*) 0x40008000
#define TIMER1_MCR *(volatile unsigned long*) 0x40008014

//REGISTRI TIMER2
#define T2CCR *(volatile unsigned long*) 0x40090028
#define T2TCR *(volatile unsigned long*) 0x40090004
#define T2CR0 *(volatile unsigned long*) 0x4009002C
#define T2IR *(volatile unsigned long*) 0x40090000

//REGISTRI TIMER3
#define T3TCR *(volatile unsigned long*) 0x40094004
#define T3MCR *(volatile unsigned long*) 0x40094014
#define T3EMR *(volatile unsigned long*) 0x4009403C
#define T3MR0 *(volatile unsigned long*) 0x40094018

#endif /* LPC_REGISTRI_H_ */
