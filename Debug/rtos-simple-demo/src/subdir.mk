################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rtos-simple-demo/src/main.c 

OBJS += \
./rtos-simple-demo/src/main.o 

C_DEPS += \
./rtos-simple-demo/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
rtos-simple-demo/src/%.o: ../rtos-simple-demo/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


