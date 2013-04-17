################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../usbcdc/src/main.c 

OBJS += \
./usbcdc/src/main.o 

C_DEPS += \
./usbcdc/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
usbcdc/src/%.o: ../usbcdc/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


