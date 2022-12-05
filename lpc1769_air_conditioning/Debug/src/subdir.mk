################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/HD44780_PCF8574_i2c.c \
../src/bmp280.c \
../src/cr_startup_lpc175x_6x.c \
../src/crp.c \
../src/delay.c \
../src/i2c_bit_banging.c \
../src/lpc1769_air_conditioning.c \
../src/sysinit.c 

OBJS += \
./src/HD44780_PCF8574_i2c.o \
./src/bmp280.o \
./src/cr_startup_lpc175x_6x.o \
./src/crp.o \
./src/delay.o \
./src/i2c_bit_banging.o \
./src/lpc1769_air_conditioning.o \
./src/sysinit.o 

C_DEPS += \
./src/HD44780_PCF8574_i2c.d \
./src/bmp280.d \
./src/cr_startup_lpc175x_6x.d \
./src/crp.d \
./src/delay.d \
./src/i2c_bit_banging.d \
./src/lpc1769_air_conditioning.d \
./src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_LPCOPEN -DNO_BOARD_LIB -D__LPC17XX__ -D__REDLIB__ -I"C:\Users\simon\Documents\MCUXpressoIDE_11.0.1_2563\workspace\lpc_chip_175x_6x\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


