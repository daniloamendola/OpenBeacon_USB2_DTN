################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../usbcdc-storage/src/DiskImg.c \
../usbcdc-storage/src/cdcuser.c \
../usbcdc-storage/src/clkconfig.c \
../usbcdc-storage/src/compusbdesc.c \
../usbcdc-storage/src/main.c \
../usbcdc-storage/src/mscuser.c \
../usbcdc-storage/src/usbcore.c \
../usbcdc-storage/src/usbhw.c \
../usbcdc-storage/src/usbuser.c 

OBJS += \
./usbcdc-storage/src/DiskImg.o \
./usbcdc-storage/src/cdcuser.o \
./usbcdc-storage/src/clkconfig.o \
./usbcdc-storage/src/compusbdesc.o \
./usbcdc-storage/src/main.o \
./usbcdc-storage/src/mscuser.o \
./usbcdc-storage/src/usbcore.o \
./usbcdc-storage/src/usbhw.o \
./usbcdc-storage/src/usbuser.o 

C_DEPS += \
./usbcdc-storage/src/DiskImg.d \
./usbcdc-storage/src/cdcuser.d \
./usbcdc-storage/src/clkconfig.d \
./usbcdc-storage/src/compusbdesc.d \
./usbcdc-storage/src/main.d \
./usbcdc-storage/src/mscuser.d \
./usbcdc-storage/src/usbcore.d \
./usbcdc-storage/src/usbhw.d \
./usbcdc-storage/src/usbuser.d 


# Each subdirectory must supply rules for building sources it contributes
usbcdc-storage/src/%.o: ../usbcdc-storage/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	/home/damendola/openbeacon/arm-2011.03/bin/arm-none-eabi-gcc -I/home/damendola/openbeacon/arm-2011.03/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


