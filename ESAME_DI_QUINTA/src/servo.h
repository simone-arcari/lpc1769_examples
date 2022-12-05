/*
 * servo.h
 *
 *  Created on: 13 ago 2019
 *      Author: simon
 */

#ifndef SERVO_H_
#define SERVO_H_

// valori estremi dutycycle per PWM e periodo base
#define PWM_dc_MAX	3000		// espresso in microsecondi
#define PWM_dc_MIN	600		// espresso in microsecondi
#define PWM_period  20000		// espresso in microsecondi


void servo_pin_init(int pin);
void servo_write(int PWM_dc, int pin);


#endif /* SERVO_H_ */
