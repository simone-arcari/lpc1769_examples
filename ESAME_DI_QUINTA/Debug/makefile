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
all: ESAME_DI_QUINTA.axf

# Tool invocations
ESAME_DI_QUINTA.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\simon\Documents\MCUXpressoIDE_11.0.1_2563\workspace\lpc_chip_175x_6x\Debug" -Xlinker -Map="ESAME_DI_QUINTA.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m3 -mthumb -T "ESAME_DI_QUINTA_Debug.ld" -o "ESAME_DI_QUINTA.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) ESAME_DI_QUINTA.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "ESAME_DI_QUINTA.axf"; # arm-none-eabi-objcopy -v -O binary "ESAME_DI_QUINTA.axf" "ESAME_DI_QUINTA.bin" ; # checksum -p LPC1769 -d "ESAME_DI_QUINTA.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
