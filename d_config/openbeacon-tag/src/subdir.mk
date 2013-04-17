################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../openbeacon-tag/src/3d_acceleration.c \
../openbeacon-tag/src/main.c \
../openbeacon-tag/src/nRF_API.c \
../openbeacon-tag/src/nRF_CMD.c \
../openbeacon-tag/src/pmu.c \
../openbeacon-tag/src/sound.c 

OBJS += \
./openbeacon-tag/src/3d_acceleration.o \
./openbeacon-tag/src/main.o \
./openbeacon-tag/src/nRF_API.o \
./openbeacon-tag/src/nRF_CMD.o \
./openbeacon-tag/src/pmu.o \
./openbeacon-tag/src/sound.o 

C_DEPS += \
./openbeacon-tag/src/3d_acceleration.d \
./openbeacon-tag/src/main.d \
./openbeacon-tag/src/nRF_API.d \
./openbeacon-tag/src/nRF_CMD.d \
./openbeacon-tag/src/pmu.d \
./openbeacon-tag/src/sound.d 


# Each subdirectory must supply rules for building sources it contributes
openbeacon-tag/src/%.o: ../openbeacon-tag/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	/home/damendola/openbeacon/arm-2011.03/bin/arm-none-eabi-gcc -I/home/damendola/openbeacon/arm-2011.03/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


