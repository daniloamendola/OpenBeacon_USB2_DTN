################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../core/peripherals/src/usb/cdcusbdesc.c \
../core/peripherals/src/usb/cdcuser.c \
../core/peripherals/src/usb/usbcore.c \
../core/peripherals/src/usb/usbhw.c \
../core/peripherals/src/usb/usbuser.c 

OBJS += \
./core/peripherals/src/usb/cdcusbdesc.o \
./core/peripherals/src/usb/cdcuser.o \
./core/peripherals/src/usb/usbcore.o \
./core/peripherals/src/usb/usbhw.o \
./core/peripherals/src/usb/usbuser.o 

C_DEPS += \
./core/peripherals/src/usb/cdcusbdesc.d \
./core/peripherals/src/usb/cdcuser.d \
./core/peripherals/src/usb/usbcore.d \
./core/peripherals/src/usb/usbhw.d \
./core/peripherals/src/usb/usbuser.d 


# Each subdirectory must supply rules for building sources it contributes
core/peripherals/src/usb/%.o: ../core/peripherals/src/usb/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


