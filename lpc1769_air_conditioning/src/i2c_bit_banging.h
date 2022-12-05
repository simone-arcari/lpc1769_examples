/*
 *  Created on: 29 mag 2020
 *  Author: Simone Arcari
 *
 *
 *  This program emulates the i2c standard through the bit banging technique and uses the
 *  GPIO pins of the microcontroller lpc1769[ARM Cortex M-3, f_clock=96MHz].
 *  Repetitive Interrupt Timer [RIT] is used to generate the i2c communication clock.
 *	When using this library do not use P0.19 and P0.20 and Repetitive Interrupt Timer [RIT]
 *	for other purposes.
 *  User Manual: 	https://www.nxp.com/docs/en/user-guide/UM10360.pdf
 *
 *  NOTA: The RIT_IRQHandler(void) function is not declared because it is already
 *  declared within the cr_startup_lpc175_6x.c file.
 *
 * @file	i2c_bit_banging.h.h
 * @date	2020-05-29
 * @version	v1.0
 *
 */

#ifndef I2C_BIT_BANGING_H_
#define I2C_BIT_BANGING_H_

/* NVIC peripherals interrupt */
#define ISER0 *(volatile unsigned long*) 0xE000E100

/* Power Control for Peripherals register */
#define PCONP *(volatile unsigned long*) 0x400FC0C4

/* Interrupt priority registers */
#define IPR7 *(volatile unsigned long*) 0xE000E41C

/* GPIO registers[port0] */
#define FIO0DIR *(volatile unsigned long*) 0x2009C000
#define FIO0SET *(volatile unsigned long*) 0x2009C018
#define FIO0CLR *(volatile unsigned long*) 0x2009C01C
#define FIO0MASK *(volatile unsigned long*) 0x2009C010
#define FIO0PIN *(volatile unsigned long*) 0x2009C014

/* Repetitive Interrupt Timer[RIT] registers */
#define RICOMPVAL *(volatile unsigned long*) 0x400B0000
#define RIMASK *(volatile unsigned long*) 0x400B0004
#define RICTRL *(volatile unsigned long*) 0x400B0008
#define RICOUNTER *(volatile unsigned long*) 0x400B000C

/*!
 * @brief This internal API is used to initialize the GPIO pins and
 * i2c communication clock using the Repetitive Interrupt Timer [RIT].
 *
 * @param[in] Null.
 *
 */
void i2c_BB_init();

/*!
 * @brief This internal API is used to start i2c communication.
 *
 * @param[in] Null.
 *
 */
void start_i2c();

/*!
 * @brief This internal API is used to stop i2c communication.
 *
 * @param[in] Null.
 *
 */
void stop_i2c();

/*!
 * @brief This internal API is used to check the ACK bit
 * provided by the slave device.
 *
 * @param[in] Null.
 *
 */
void ACK();

#endif /* I2C_BIT_BANGING_H_ */
