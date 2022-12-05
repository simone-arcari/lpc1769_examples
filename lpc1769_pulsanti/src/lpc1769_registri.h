/*
 * lpc1769_registri.h
 *
 *  Created on: 28 set 2019
 *      Author: simon
 */

#ifndef LPC1769_REGISTRI_H_
#define LPC1769_REGISTRI_H_


// REGISTRI GPIO PORTA 2
#define FIO2DIR *(volatile unsigned long*) 0x2009C040		//imposta i pin in output (1) input(0)
#define FIO2MASK *(volatile unsigned long*) 0x2009C050		//imposta una maschera sui pin desiderati
#define FIO2SET *(volatile unsigned long*) 0x2009C058		//imposta i pin a livello logico alto (1)
#define FIO2CLR *(volatile unsigned long*) 0x2009C05C		//imposta i pin a livello logico basso (1)
#define FIO2PIN *(volatile unsigned long*) 0x2009C054		//imposta i pin a livello logico alto (1) e a livello logico basso (0)   viene usato per verificare lo stato dei pin 1=accesi 0=spenti

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
#define STIR *(volatile unsigned long*) 0xE000EF00


#endif /* LPC1769_REGISTRI_H_ */
