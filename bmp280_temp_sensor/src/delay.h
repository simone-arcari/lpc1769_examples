/*
 *  Created on: 29 mag 2020
 *  Author: Simone Arcari
 *
 *	This program uses the TIMER1 of the microCONTROLLER lpc1769[ARM Cortex M-3, f_clock=96MHz].
 *  When using this library do not use TIMER1 for other purposes.
 *  User Manual: 	https://www.nxp.com/docs/en/user-guide/UM10360.pdf
 *
 * @file	delay.h
 * @date	2020-05-29
 * @version	v1.0
 *
 */
#ifndef DELAY_H_
#define DELAY_H_

/* TIMER1 registers */
#define TIMER1_TCR *(volatile unsigned long*) 0x40008004
#define TIMER1_PR  *(volatile unsigned long*) 0x4000800C
#define TIMER1_MR0 *(volatile unsigned long*) 0x40008018
#define TIMER1_IR *(volatile unsigned long*) 0x40008000
#define TIMER1_MCR *(volatile unsigned long*) 0x40008014

/*!
 * @brief This internal API is used to stop program
 * execution for a specified time[in microseconds].
 *
 * @param[in] delayInUs.
 *
 */
void delayUs(unsigned short delayInUs);

/*!
 * @brief This internal API is used to stop program
 * execution for a specified time[in milliseconds].
 *
 * @param[in] delayInMs.
 *
 */
void delayMs(unsigned short delayInMs);

#endif /* DELAY_H_ */
