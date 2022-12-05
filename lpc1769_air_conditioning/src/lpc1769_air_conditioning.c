/*
===============================================================================
 Name        : lpc1769_air_conditioning.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "bmp280.h"
#include "delay.h"
#include "i2c_bit_banging.h"
#include "HD44780_PCF8574_i2c.h"

/* over temperature value */
#define T_threshold 28

/* GPIO registers */
#define FIO2DIR *(volatile unsigned long*) 0x2009C040
#define FIO2SET *(volatile unsigned long*) 0x2009C058
#define FIO2CLR *(volatile unsigned long*) 0x2009C05C

/* extern variables */
extern int status;
extern int i2c_anable;
extern int I2CbufferCnt;
extern unsigned char i2c_buffer[64];
extern unsigned char i2c_data_rec[64];

void delay_ms(uint32_t period_ms);
int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);

int main(void) {

    int8_t rslt;
    struct bmp280_dev bmp;
    struct bmp280_config conf;
    struct bmp280_uncomp_data ucomp_data;
    int32_t temp32;
    double temp;
    uint32_t pres32, pres64;
    double pres;

    /* Set the pin P2.0 for transistor control as output */
    FIO2DIR = 1;

	/* Initializing i2c communication */
	i2c_BB_init();

	/* Initializing i2c LCD display */
	InitLCD_i2c();

    /* Map the delay function pointer with the function responsible for implementing the delay */
    bmp.delay_ms = delay_ms;

    /* Assign device I2C address based on the status of SDO pin (GND for PRIMARY(0x76) & VDD for SECONDARY(0x77)) */
    bmp.dev_id = BMP280_I2C_ADDR_PRIM;

    /* Select the interface mode as I2C */
    bmp.intf = BMP280_I2C_INTF;

    /* Map the I2C read & write function pointer with the functions responsible for I2C bus transfer */
    bmp.read = i2c_reg_read;
    bmp.write = i2c_reg_write;

    /* Initializing bmp280 sensor */
    rslt = bmp280_init(&bmp);

    /* Always read the current settings before writing, especially when
     * all the configuration is not modified
     */
    rslt = bmp280_get_config(&conf, &bmp);

    /* configuring the temperature oversampling, filter coefficient and output data rate */
    /* Overwrite the desired settings */
    conf.filter = BMP280_FILTER_COEFF_2;

    /* Temperature oversampling set at 4x */
    conf.os_temp = BMP280_OS_4X;

    /* Pressure oversampling set at 4x */
    conf.os_pres = BMP280_OS_4X;

    /* Setting the output data rate as 1HZ(1000ms) */
    conf.odr = BMP280_ODR_1000_MS;
    rslt = bmp280_set_config(&conf, &bmp);

    /* Always set the power mode after setting the configuration */
    rslt = bmp280_set_power_mode(BMP280_NORMAL_MODE, &bmp);

    /* I write on the display what is necessary to clearly visualize the data from the sensor */
	PutCommand_i2c(DISPLAY_CLEAR_CMD);
	GoToLine_i2c(0,0);
	WriteString_i2c((unsigned char*)"temp:", false);
	GoToLine_i2c(0,8);
	WriteString_i2c((unsigned char*)"C", false);
	GoToLine_i2c(1,0);
	WriteString_i2c((unsigned char*)"press:", false);
	GoToLine_i2c(1,13);
	WriteString_i2c((unsigned char*)"Pa", false);

    while(1) {

    	/* Reading the raw data from sensor */
        rslt = bmp280_get_uncomp_data(&ucomp_data, &bmp);


        /* Getting the 32 bit compensated temperature */
        rslt = bmp280_get_comp_temp_32bit(&temp32, ucomp_data.uncomp_temp, &bmp);
        /* Getting the compensated temperature as floating point value */
        rslt = bmp280_get_comp_temp_double(&temp, ucomp_data.uncomp_temp, &bmp);

		GoToLine_i2c(0,6);
		WriteString_i2c((unsigned char*)"  ", false);
		GoToLine_i2c(0,6);
		WriteNumber_i2c(temp);


        /* Getting the compensated pressure using 32 bit precision */
        rslt = bmp280_get_comp_pres_32bit(&pres32, ucomp_data.uncomp_press, &bmp);
        /* Getting the compensated pressure as floating point value */
        rslt = bmp280_get_comp_pres_double(&pres, ucomp_data.uncomp_press, &bmp);

		GoToLine_i2c(1,7);
		WriteString_i2c((unsigned char*)"      ", false);
		GoToLine_i2c(1,7);
		WriteNumber_i2c(pres);

		/* check for the over temperature */
		if(temp >= T_threshold)			//turn on cooling fan
			FIO2SET = 1;
		else
			FIO2CLR = 1;				//turn off cooling fan

        /* Sleep time between measurements = BMP280_ODR_1000_MS */
        bmp.delay_ms(1000);
    }
    return 0 ;
}

/*!
 *  @brief Function that creates a mandatory delay required in some of the APIs such as "bmg250_soft_reset",
 *      "bmg250_set_foc", "bmg250_perform_self_test"  and so on.
 *
 *  @param[in] period_ms  : the required wait time in milliseconds.
 *  @return void.
 *
 */
void delay_ms(uint32_t period_ms)
{
    /* Implement the delay routine according to the target machine */
	delayMs(period_ms);
}

/*!
 *  @brief Function for writing the sensor's registers through I2C bus.
 *
 *  @param[in] i2c_addr : sensor I2C address.
 *  @param[in] reg_addr : Register address.
 *  @param[in] reg_data : Pointer to the data buffer whose value is to be written.
 *  @param[in] length   : No of bytes to write.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

    /* Implement the I2C write routine according to the target machine. */
	i2c_anable = 1;
	status = 0;

	i2c_buffer[0] = i2c_addr << 1;				// bmp280 i2c address for write operations
	i2c_buffer[1] = reg_addr;					// bmp280 internal register address

	for(int i = 2; i < length + 2; i++)
	{
		i2c_buffer[i] = reg_data[i-2];
	}

	I2CbufferCnt = length + 1;

	while(status <= 4){							// the cycle is interrupted before reaching case 5
		__asm volatile ("wfi");					// wait for any interrupt
	}

	i2c_anable = 0;

	return BMP280_OK;
}

/*!
 *  @brief Function for reading the sensor's registers through I2C bus.
 *
 *  @param[in] i2c_addr : Sensor I2C address.
 *  @param[in] reg_addr : Register address.
 *  @param[out] reg_data    : Pointer to the data buffer to store the read data.
 *  @param[in] length   : No of bytes to read.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{

    /* Implement the I2C read routine according to the target machine. */

	i2c_anable = 1;
	status = 0;

	i2c_buffer[0] = i2c_addr << 1;				// bmp280 i2c address for write operations
	i2c_buffer[1] = reg_addr;					// bmp280 internal register address
	I2CbufferCnt = 1;

    while(status <= 3){							// the cycle stops before reaching case 4
		__asm volatile ("wfi");					// wait for any interrupt
    }

    status = 0;
	i2c_buffer[0] = (i2c_addr << 1) | 0x1;		// bmp280 i2c address for read operations
	I2CbufferCnt = length-1;					// Total buffer size: only data

	while(status <= 4){							// the cycle is interrupted before reaching case 5
		__asm volatile ("wfi");					// wait for any interrupt
	}


	for(int i = 0; i < length; i++)
	{
		reg_data[i] = i2c_data_rec[i];
		i2c_data_rec[i] = 0;
	}

	i2c_anable = 0;

	return BMP280_OK;
}
