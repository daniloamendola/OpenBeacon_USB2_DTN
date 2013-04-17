################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../core/startup/LPC13xx.c 

OBJS += \
./core/startup/LPC13xx.o 

C_DEPS += \
./core/startup/LPC13xx.d 


# Each subdirectory must supply rules for building sources it contributes
core/startup/%.o: ../core/startup/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


