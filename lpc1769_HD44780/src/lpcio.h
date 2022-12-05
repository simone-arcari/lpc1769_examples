/*
 * lpcio.h
 *
 *  Created on: 19 lug 2019
 *      Author: Simone Arcari
 */

#ifndef LPCIO_H_
#define LPCIO_H_

typedef enum {OUTPUT, INPUT} pin_status;
typedef enum {HIGH, LOW} pin_value;
typedef enum {zero, one, two, three} pin_port;

void pinMode(int pin, pin_status status, pin_port port);		//---- Function to set the pin state
void digitalWrite(int pin, pin_value value,  pin_port port);	//---- Function to set the pin value(0-1)
void delayMs(unsigned short delayInMs);							//----Delay function in milliseconds
void delayUs(unsigned short delayInUs);							//----Delay function in microseconds


#endif /* LPCIO_H_ */
