################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../core/peripherals/src/gpio.c \
../core/peripherals/src/uart.c 

OBJS += \
./core/peripherals/src/gpio.o \
./core/peripherals/src/uart.o 

C_DEPS += \
./core/peripherals/src/gpio.d \
./core/peripherals/src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
core/peripherals/src/%.o: ../core/peripherals/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	/home/damendola/openbeacon/arm-2011.03/bin/arm-none-eabi-gcc -I/home/damendola/openbeacon/arm-2011.03/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


