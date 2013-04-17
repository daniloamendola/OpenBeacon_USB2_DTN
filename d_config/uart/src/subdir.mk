################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../uart/src/uarttest.c 

OBJS += \
./uart/src/uarttest.o 

C_DEPS += \
./uart/src/uarttest.d 


# Each subdirectory must supply rules for building sources it contributes
uart/src/%.o: ../uart/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	/home/damendola/openbeacon/arm-2011.03/bin/arm-none-eabi-gcc -I/home/damendola/openbeacon/arm-2011.03/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


