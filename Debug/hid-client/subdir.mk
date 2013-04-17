################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hid-client/hid_listen.c \
../hid-client/rawhid.c 

OBJS += \
./hid-client/hid_listen.o \
./hid-client/rawhid.o 

C_DEPS += \
./hid-client/hid_listen.d \
./hid-client/rawhid.d 


# Each subdirectory must supply rules for building sources it contributes
hid-client/%.o: ../hid-client/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


