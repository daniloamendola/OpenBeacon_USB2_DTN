################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../core/freertos/src/heap_2.c \
../core/freertos/src/list.c \
../core/freertos/src/port.c \
../core/freertos/src/queue.c \
../core/freertos/src/tasks.c 

OBJS += \
./core/freertos/src/heap_2.o \
./core/freertos/src/list.o \
./core/freertos/src/port.o \
./core/freertos/src/queue.o \
./core/freertos/src/tasks.o 

C_DEPS += \
./core/freertos/src/heap_2.d \
./core/freertos/src/list.d \
./core/freertos/src/port.d \
./core/freertos/src/queue.d \
./core/freertos/src/tasks.d 


# Each subdirectory must supply rules for building sources it contributes
core/freertos/src/%.o: ../core/freertos/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	/home/damendola/openbeacon/arm-2011.03/bin/arm-none-eabi-gcc -I/home/damendola/openbeacon/arm-2011.03/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


