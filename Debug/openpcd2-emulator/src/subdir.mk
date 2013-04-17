################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../openpcd2-emulator/src/main.c \
../openpcd2-emulator/src/pmu.c \
../openpcd2-emulator/src/usbserial.c 

OBJS += \
./openpcd2-emulator/src/main.o \
./openpcd2-emulator/src/pmu.o \
./openpcd2-emulator/src/usbserial.o 

C_DEPS += \
./openpcd2-emulator/src/main.d \
./openpcd2-emulator/src/pmu.d \
./openpcd2-emulator/src/usbserial.d 


# Each subdirectory must supply rules for building sources it contributes
openpcd2-emulator/src/%.o: ../openpcd2-emulator/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


