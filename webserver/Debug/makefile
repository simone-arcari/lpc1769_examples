################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include lwip/src/netif/subdir.mk
-include lwip/src/core/ipv4/subdir.mk
-include lwip/src/core/subdir.mk
-include lwip/src/arch/subdir.mk
-include lwip/src/api/subdir.mk
-include example/src/subdir.mk
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
all: webserver.axf

# Tool invocations
webserver.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\simon\Documents\MCUXpressoIDE_11.0.1_2563\workspace\lpc_chip_175x_6x\Debug" -L"C:\Users\simon\Documents\MCUXpressoIDE_11.0.1_2563\workspace\lpc_board_nxp_lpcxpresso_1769\Debug" -Xlinker -Map="webserver.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "webserver_Debug.ld" -o "webserver.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) webserver.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "webserver.axf"; # arm-none-eabi-objcopy -v -O binary "webserver.axf" "webserver.bin" ; # checksum -p LPC1769 -d "webserver.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
