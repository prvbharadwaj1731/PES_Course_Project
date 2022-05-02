################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/ADC.c \
../source/DAC.c \
../source/PES_Assignment_7.c \
../source/autocorrelate.c \
../source/delay.c \
../source/i2c.c \
../source/mma8451q.c \
../source/mtb.c \
../source/sample_generator.c \
../source/semihost_hardfault.c \
../source/sine_generator.c \
../source/sine_test.c 

OBJS += \
./source/ADC.o \
./source/DAC.o \
./source/PES_Assignment_7.o \
./source/autocorrelate.o \
./source/delay.o \
./source/i2c.o \
./source/mma8451q.o \
./source/mtb.o \
./source/sample_generator.o \
./source/semihost_hardfault.o \
./source/sine_generator.o \
./source/sine_test.o 

C_DEPS += \
./source/ADC.d \
./source/DAC.d \
./source/PES_Assignment_7.d \
./source/autocorrelate.d \
./source/delay.d \
./source/i2c.d \
./source/mma8451q.d \
./source/mtb.d \
./source/sample_generator.d \
./source/semihost_hardfault.d \
./source/sine_generator.d \
./source/sine_test.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\prvbh\Documents\MCUXpressoIDE_11.2.0_4120\workspace\PES_Assignment_7\board" -I"C:\Users\prvbh\Documents\MCUXpressoIDE_11.2.0_4120\workspace\PES_Assignment_7\source" -I"C:\Users\prvbh\Documents\MCUXpressoIDE_11.2.0_4120\workspace\PES_Assignment_7" -I"C:\Users\prvbh\Documents\MCUXpressoIDE_11.2.0_4120\workspace\PES_Assignment_7\drivers" -I"C:\Users\prvbh\Documents\MCUXpressoIDE_11.2.0_4120\workspace\PES_Assignment_7\CMSIS" -I"C:\Users\prvbh\Documents\MCUXpressoIDE_11.2.0_4120\workspace\PES_Assignment_7\utilities" -I"C:\Users\prvbh\Documents\MCUXpressoIDE_11.2.0_4120\workspace\PES_Assignment_7\startup" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


