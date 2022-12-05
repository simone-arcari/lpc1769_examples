#include "lpcio.h"
#include "lpc1769_registri.h"

void pinMode(int pin, pin_status status, pin_port port)
{
	int number = 1<<pin;

	switch(status)
    {
    	case OUTPUT:
    		switch(port)
    		{
    			case zero:
    				FIO0DIR |= number;
    			break;

    			case one:
    			    FIO1DIR |= number;
    			break;

    			case two:
    			    FIO2DIR |= number;
    			break;

    			case three:
    			    FIO3DIR |= number;
    			break;
    		}
    	break;

    	case INPUT:
    		switch(port)
    		{
    			case zero:
    				FIO0DIR &= ~number;
    			break;

    			case one:
    			    FIO1DIR &= ~number;
    			break;

    			case two:
    			    FIO2DIR &= ~number;
    			break;

    			case three:
    			    FIO3DIR &= ~number;
    			break;
    		}
    	break;
    }
}
void digitalWrite(int pin, pin_value value,  pin_port port)
{
	int number = 1<<pin;

    switch(value)
    {
    	case HIGH:
    		switch(port)
    		{
    			case zero:
    				FIO0SET |= number;
    			break;

    			case one:
    				FIO1SET |= number;
    			break;

    			case two:
    				FIO2SET |= number;
    			break;

    			case three:
    				FIO3SET |= number;
    			break;
    		}
    	break;

    	case LOW:
    		switch(port)
    		{
    			case zero:
    				FIO0CLR |= number;
    			break;

    			case one:
    				FIO1CLR |= number;
    			break;

    			case two:
    				FIO2CLR |= number;
    			break;

    			case three:
    				FIO3CLR |= number;
    			break;
    		}
    	break;
    }
}

/*****************************************************************************
 *
 * Description:
 *    Delay execution by a specified number of milliseconds by using
 *    timer #1. A polled implementation.
 *
 * Params:
 *    [in] delayInMs - the number of milliseconds to delay.
 *
 ****************************************************************************/
void delayMs(unsigned short delayInMs)
{
  /*
   * setup timer #1 for delay
   */
  TIMER1_TCR = 0x02;          //stop and reset timer
  TIMER1_PR  = 0x00;          //set prescaler to zero
  TIMER1_MR0 = (CORE_CLK/4) / 1000 * delayInMs;
  TIMER1_IR  = 0xff;          //reset all interrrupt flags
  TIMER1_MCR = 0x04;          //stop timer on match
  TIMER1_TCR = 0x01;          //start timer

  //wait until delay time has elapsed
  while (TIMER1_TCR & 0x01);
}

/*****************************************************************************
 *
 * Description:
 *    Delay execution by a specified number of microseconds by using
 *    timer #1. A polled implementation.
 *
 * Params:
 *    [in] delayInUs - the number of microseconds to delay.
 *
 ****************************************************************************/
void delayUs(unsigned short delayInUs)
{
  /*
   * setup timer #1 for delay
   */
  TIMER1_TCR = 0x02;          //stop and reset timer
  TIMER1_PR  = 0x00;          //set prescaler to zero
  TIMER1_MR0 = (CORE_CLK/4) / 1000000 * delayInUs;
  TIMER1_IR  = 0xFF;          //reset all interrrupt flags
  TIMER1_MCR = 0x04;          //stop timer on match
  TIMER1_TCR = 0x01;          //start timer

  //wait until delay time has elapsed
  while (TIMER1_TCR & 0x01);
}
