################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../openbeacon-sensor/src/3d_acceleration.c \
../openbeacon-sensor/src/main.c \
../openbeacon-sensor/src/nRF_API.c \
../openbeacon-sensor/src/nRF_CMD.c \
../openbeacon-sensor/src/pmu.c \
../openbeacon-sensor/src/sound.c 

OBJS += \
./openbeacon-sensor/src/3d_acceleration.o \
./openbeacon-sensor/src/main.o \
./openbeacon-sensor/src/nRF_API.o \
./openbeacon-sensor/src/nRF_CMD.o \
./openbeacon-sensor/src/pmu.o \
./openbeacon-sensor/src/sound.o 

C_DEPS += \
./openbeacon-sensor/src/3d_acceleration.d \
./openbeacon-sensor/src/main.d \
./openbeacon-sensor/src/nRF_API.d \
./openbeacon-sensor/src/nRF_CMD.d \
./openbeacon-sensor/src/pmu.d \
./openbeacon-sensor/src/sound.d 


# Each subdirectory must supply rules for building sources it contributes
openbeacon-sensor/src/%.o: ../openbeacon-sensor/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	/home/damendola/openbeacon/arm-2011.03/bin/arm-none-eabi-gcc -I/home/damendola/openbeacon/arm-2011.03/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


