/*
 * lpc1769_registri.h
 *
 *  Created on: 27 set 2019
 *      Author: simon
 */

#ifndef LPC1769_REGISTRI_H_
#define LPC1769_REGISTRI_H_


#define CORE_CLK 96000000				// ARM core clock


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


//REGISTRI TIMER1
#define TIMER1_TCR *(volatile unsigned long*) 0x40008004
#define TIMER1_PR  *(volatile unsigned long*) 0x4000800C
#define TIMER1_MR0 *(volatile unsigned long*) 0x40008018
#define TIMER1_IR *(volatile unsigned long*) 0x40008000
#define TIMER1_MCR *(volatile unsigned long*) 0x40008014


#define PCONP *(volatile unsigned long*) 0x400FC0C4
#define PCLKSEL1 *(volatile unsigned long*) 0x400FC1AC
#define PINSEL0 *(volatile unsigned long*) 0x4002C000
#define PINMODE0 *(volatile unsigned long*) 0x4002C040
#define PINMODE_OD0 *(volatile unsigned long*) 0x4002C068
#define ISER0 *(volatile unsigned long*) 0xE000E100

#define T2CCR *(volatile unsigned long*) 0x40090028
#define T2TCR *(volatile unsigned long*) 0x40090004
#define T2CR0 *(volatile unsigned long*) 0x4009002C
#define T2IR *(volatile unsigned long*) 0x40090000

#define T3TCR *(volatile unsigned long*) 0x40094004
#define T3MCR *(volatile unsigned long*) 0x40094014
#define T3EMR *(volatile unsigned long*) 0x4009403C
#define T3MR0 *(volatile unsigned long*) 0x40094018

// frequenza di clock del processore ARM
#define CORE_CLK 96000000

#define FIO0DIR *(volatile unsigned long*) 0x2009C000
#define FIO0SET *(volatile unsigned long*) 0x2009C018
#define FIO0CLR *(volatile unsigned long*) 0x2009C01C


#define FIO0DIR *(volatile unsigned long*) 0x2009C000
#define FIO0PIN *(volatile unsigned long*) 0x2009C014
#define FIO0MASK *(volatile unsigned long*) 0x2009C010

#define FIO1DIR *(volatile unsigned long*) 0x2009C020
#define FIO1PIN *(volatile unsigned long*) 0x2009C034
#define FIO1MASK *(volatile unsigned long*) 0x2009C030

#define FIO2DIR *(volatile unsigned long*) 0x2009C040
#define FIO2PIN *(volatile unsigned long*) 0x2009C054
#define FIO2MASK *(volatile unsigned long*) 0x2009C050

// registri System Tick Timer
#define STCTRL *(volatile unsigned long*) 0xE000E010
#define STRELOAD *(volatile unsigned long*) 0xE000E014



#endif /* LPC1769_REGISTRI_H_ */
