################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include usbcdc/src/subdir.mk
-include usbcdc-storage/src/subdir.mk
-include uart/src/subdir.mk
-include sound/src/subdir.mk
-include rtos-simple-demo/src/subdir.mk
-include openpcd2/src/subdir.mk
-include openpcd2-emulator/src/subdir.mk
-include openbeacon-usb2/src/subdir.mk
-include openbeacon-tag/src/subdir.mk
-include openbeacon-sensor/src/subdir.mk
-include lpc-flash/src/subdir.mk
-include hid-client/subdir.mk
-include core/startup/subdir.mk
-include core/peripherals/src/usb/subdir.mk
-include core/peripherals/src/subdir.mk
-include core/openbeacon/src/subdir.mk
-include core/freertos/src/subdir.mk
-include core/cmsis/src/subdir.mk
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
all: openbeacon-usb2

# Tool invocations
openbeacon-usb2: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	/home/damendola/openbeacon/arm-2011.03/bin/arm-none-eabi-gcc  -o "openbeacon-usb2" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) openbeacon-usb2
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
