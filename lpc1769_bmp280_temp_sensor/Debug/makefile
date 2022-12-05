################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: lpc1769_bmp280_temp_sensor.axf

# Tool invocations
lpc1769_bmp280_temp_sensor.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="lpc1769_bmp280_temp_sensor.map" -Xlinker --cref -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m3 -mthumb -T "lpc1769_bmp280_temp_sensor_Debug.ld" -o "lpc1769_bmp280_temp_sensor.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) lpc1769_bmp280_temp_sensor.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "lpc1769_bmp280_temp_sensor.axf"; # arm-none-eabi-objcopy -v -O binary "lpc1769_bmp280_temp_sensor.axf" "lpc1769_bmp280_temp_sensor.bin" ; # checksum -p LPC1769 -d "lpc1769_bmp280_temp_sensor.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
