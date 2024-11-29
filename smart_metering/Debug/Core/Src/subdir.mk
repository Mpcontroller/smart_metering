################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/MB.c \
../Core/Src/Usart.c \
../Core/Src/call_back_functions.c \
../Core/Src/main.c \
../Core/Src/smart_metering.c \
../Core/Src/stm32l4xx_hal_msp.c \
../Core/Src/stm32l4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l4xx.c \
../Core/Src/vmm.c 

OBJS += \
./Core/Src/MB.o \
./Core/Src/Usart.o \
./Core/Src/call_back_functions.o \
./Core/Src/main.o \
./Core/Src/smart_metering.o \
./Core/Src/stm32l4xx_hal_msp.o \
./Core/Src/stm32l4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l4xx.o \
./Core/Src/vmm.o 

C_DEPS += \
./Core/Src/MB.d \
./Core/Src/Usart.d \
./Core/Src/call_back_functions.d \
./Core/Src/main.d \
./Core/Src/smart_metering.d \
./Core/Src/stm32l4xx_hal_msp.d \
./Core/Src/stm32l4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l4xx.d \
./Core/Src/vmm.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DARM_MATH_CM4 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I"E:/STM32 workspace/smart_metering/Drivers/CMSIS/DSP/Include" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/MB.cyclo ./Core/Src/MB.d ./Core/Src/MB.o ./Core/Src/MB.su ./Core/Src/Usart.cyclo ./Core/Src/Usart.d ./Core/Src/Usart.o ./Core/Src/Usart.su ./Core/Src/call_back_functions.cyclo ./Core/Src/call_back_functions.d ./Core/Src/call_back_functions.o ./Core/Src/call_back_functions.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/smart_metering.cyclo ./Core/Src/smart_metering.d ./Core/Src/smart_metering.o ./Core/Src/smart_metering.su ./Core/Src/stm32l4xx_hal_msp.cyclo ./Core/Src/stm32l4xx_hal_msp.d ./Core/Src/stm32l4xx_hal_msp.o ./Core/Src/stm32l4xx_hal_msp.su ./Core/Src/stm32l4xx_it.cyclo ./Core/Src/stm32l4xx_it.d ./Core/Src/stm32l4xx_it.o ./Core/Src/stm32l4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32l4xx.cyclo ./Core/Src/system_stm32l4xx.d ./Core/Src/system_stm32l4xx.o ./Core/Src/system_stm32l4xx.su ./Core/Src/vmm.cyclo ./Core/Src/vmm.d ./Core/Src/vmm.o ./Core/Src/vmm.su

.PHONY: clean-Core-2f-Src

