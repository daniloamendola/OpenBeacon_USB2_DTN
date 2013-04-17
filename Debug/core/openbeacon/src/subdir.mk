################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../core/openbeacon/src/crc16.c \
../core/openbeacon/src/crc8.c \
../core/openbeacon/src/debug_printf.c \
../core/openbeacon/src/hid.c \
../core/openbeacon/src/iap.c \
../core/openbeacon/src/msd.c \
../core/openbeacon/src/persistent.c \
../core/openbeacon/src/rfid.c \
../core/openbeacon/src/spi.c \
../core/openbeacon/src/vfs.c \
../core/openbeacon/src/xxtea.c 

OBJS += \
./core/openbeacon/src/crc16.o \
./core/openbeacon/src/crc8.o \
./core/openbeacon/src/debug_printf.o \
./core/openbeacon/src/hid.o \
./core/openbeacon/src/iap.o \
./core/openbeacon/src/msd.o \
./core/openbeacon/src/persistent.o \
./core/openbeacon/src/rfid.o \
./core/openbeacon/src/spi.o \
./core/openbeacon/src/vfs.o \
./core/openbeacon/src/xxtea.o 

C_DEPS += \
./core/openbeacon/src/crc16.d \
./core/openbeacon/src/crc8.d \
./core/openbeacon/src/debug_printf.d \
./core/openbeacon/src/hid.d \
./core/openbeacon/src/iap.d \
./core/openbeacon/src/msd.d \
./core/openbeacon/src/persistent.d \
./core/openbeacon/src/rfid.d \
./core/openbeacon/src/spi.d \
./core/openbeacon/src/vfs.d \
./core/openbeacon/src/xxtea.d 


# Each subdirectory must supply rules for building sources it contributes
core/openbeacon/src/%.o: ../core/openbeacon/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


