/*
 *  Created on: 29 mag 2020
 *  Author: simon
 *
 *	This program uses the TIMER1 of the microCONTROLLER lpc1769[ARM Cortex M-3, f_clock=96MHz].
 *  When using this library do not use TIMER1 for other purposes.
 *  User Manual: 	https://www.nxp.com/docs/en/user-guide/UM10360.pdf
 *
 * @file	delay.c
 * @date	2020-05-29
 * @version	v1
 *
 */
#include "delay.h"

/*!
 * @brief This internal API is used to stop program
 * execution for a specified time[in microseconds].
 *
 * @param[in] delayInUs.
 *
 */
void delayUs(unsigned short delayInUs)
{
  /*
   * setup TIMER1 for delay
   */
  TIMER1_TCR = 0x02;          			//stop and reset timer
  TIMER1_PR  = 0x00;          			//set prescaler to zero

  TIMER1_MR0 = (96000000/4) / 1000000 * delayInUs;

  TIMER1_IR  = 0xFF;          			//reset all interrrupt flags
  TIMER1_MCR = 0x04;          			//stop timer on match
  TIMER1_TCR = 0x01;          			//start timer


  while (TIMER1_TCR & 0x01);			//wait until delay time has elapsed
}

/*!
 * @brief This internal API is used to stop program
 * execution for a specified time[in milliseconds].
 *
 * @param[in] delayInMs.
 *
 */
void delayMs(unsigned short delayInMs)
{
  /*
   * setup timer #1 for delay
   */
  TIMER1_TCR = 0x02;          			//stop and reset timer
  TIMER1_PR  = 0x00;          			//set prescaler to zero
  TIMER1_MR0 = (96000000/4) / 1000 * delayInMs;
  TIMER1_IR  = 0xff;          			//reset all interrupt flags
  TIMER1_MCR = 0x04;          			//stop timer on match
  TIMER1_TCR = 0x01;          			//start timer


  while (TIMER1_TCR & 0x01);    		//wait until delay time has elapsed
}
