################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/cr_startup_lpc175x_6x.c \
../example/src/httpd.c \
../example/src/lwip_fs.c \
../example/src/sysinit.c \
../example/src/webserver.c 

OBJS += \
./example/src/cr_startup_lpc175x_6x.o \
./example/src/httpd.o \
./example/src/lwip_fs.o \
./example/src/sysinit.o \
./example/src/webserver.o 

C_DEPS += \
./example/src/cr_startup_lpc175x_6x.d \
./example/src/httpd.d \
./example/src/lwip_fs.d \
./example/src/sysinit.d \
./example/src/webserver.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M3 -I"C:\Users\simon\Documents\MCUXpressoIDE_11.0.1_2563\workspace\lpc_chip_175x_6x\inc" -I"C:\Users\simon\Documents\MCUXpressoIDE_11.0.1_2563\workspace\lpc_board_nxp_lpcxpresso_1769\inc" -I"C:\Users\simon\Documents\MCUXpressoIDE_11.0.1_2563\workspace\webserver\example\inc" -I"C:\Users\simon\Documents\MCUXpressoIDE_11.0.1_2563\workspace\webserver\lwip\inc" -I"C:\Users\simon\Documents\MCUXpressoIDE_11.0.1_2563\workspace\webserver\lwip\inc\ipv4" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


