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
all: lpc1769_display_16x2_Gabriele_Nocera.axf

# Tool invocations
lpc1769_display_16x2_Gabriele_Nocera.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\simon\Documents\MCUXpressoIDE_11.0.1_2563\workspace\lpc_chip_175x_6x\Debug" -Xlinker -Map="lpc1769_display_16x2_Gabriele_Nocera.map" -Xlinker --cref -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m3 -mthumb -T "lpc1769_display_16x2_Gabriele_Nocera_Debug.ld" -o "lpc1769_display_16x2_Gabriele_Nocera.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) lpc1769_display_16x2_Gabriele_Nocera.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "lpc1769_display_16x2_Gabriele_Nocera.axf"; # arm-none-eabi-objcopy -v -O binary "lpc1769_display_16x2_Gabriele_Nocera.axf" "lpc1769_display_16x2_Gabriele_Nocera.bin" ; # checksum -p LPC1769 -d "lpc1769_display_16x2_Gabriele_Nocera.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
