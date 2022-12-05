/*
 * lpc_joystick.h
 *
 *  Created on: 20 lug 2019
 *      Author: simon
 *
 *     i pin del joystick sono gli stessi dell'ADC quindi devono essere due dei seguenti pin e devono essere diversi:
 *     P0.23	P0.24	P0.25	P0.26
 *     P1.30	P1.31
 *     P0.2	P0.3
 */

#ifndef LPC_JOYSTICK_H_
#define LPC_JOYSTICK_H_

void joystick_pin(int x_pin, int y_pin);
void joystick_init();
void set_channel(int pin);
int pin_correct_test(int pin);
int value_x();
int value_y();

#endif /* LPC_JOYSTICK_H_ */
