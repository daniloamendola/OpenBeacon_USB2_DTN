################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../openpcd2/src/main.c \
../openpcd2/src/pmu.c \
../openpcd2/src/usbserial.c 

OBJS += \
./openpcd2/src/main.o \
./openpcd2/src/pmu.o \
./openpcd2/src/usbserial.o 

C_DEPS += \
./openpcd2/src/main.d \
./openpcd2/src/pmu.d \
./openpcd2/src/usbserial.d 


# Each subdirectory must supply rules for building sources it contributes
openpcd2/src/%.o: ../openpcd2/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	/home/damendola/openbeacon/arm-2011.03/bin/arm-none-eabi-gcc -I/home/damendola/openbeacon/arm-2011.03/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


