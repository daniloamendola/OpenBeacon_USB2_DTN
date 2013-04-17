################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lpc-flash/src/lpc-flash.c 

OBJS += \
./lpc-flash/src/lpc-flash.o 

C_DEPS += \
./lpc-flash/src/lpc-flash.d 


# Each subdirectory must supply rules for building sources it contributes
lpc-flash/src/%.o: ../lpc-flash/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


