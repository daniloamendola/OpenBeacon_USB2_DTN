################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../core/cmsis/src/core_cm3.c \
../core/cmsis/src/system_LPC13xx.c 

OBJS += \
./core/cmsis/src/core_cm3.o \
./core/cmsis/src/system_LPC13xx.o 

C_DEPS += \
./core/cmsis/src/core_cm3.d \
./core/cmsis/src/system_LPC13xx.d 


# Each subdirectory must supply rules for building sources it contributes
core/cmsis/src/%.o: ../core/cmsis/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	/home/damendola/openbeacon/arm-2011.03/bin/arm-none-eabi-gcc -I/home/damendola/openbeacon/arm-2011.03/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


