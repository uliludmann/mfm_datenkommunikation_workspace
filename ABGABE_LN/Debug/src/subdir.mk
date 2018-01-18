################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/stm32f0xx_it.c \
../src/syscalls.c \
../src/system_stm32f0xx.c 

OBJS += \
./src/main.o \
./src/stm32f0xx_it.o \
./src/syscalls.o \
./src/system_stm32f0xx.o 

C_DEPS += \
./src/main.d \
./src/stm32f0xx_it.d \
./src/syscalls.d \
./src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32F072RBTx -DSTM32F0 -DNUCLEO_F072RB -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F072xB -I"/Users/ulrichludmann/Documents/HM Mechatronik/datenkommunikation/workspace_praktikum/ABGABE_LN/inc" -I"/Users/ulrichludmann/Documents/HM Mechatronik/datenkommunikation/workspace_praktikum/ABGABE_LN/CMSIS/core" -I"/Users/ulrichludmann/Documents/HM Mechatronik/datenkommunikation/workspace_praktikum/ABGABE_LN/CMSIS/device" -I"/Users/ulrichludmann/Documents/HM Mechatronik/datenkommunikation/workspace_praktikum/ABGABE_LN/HAL_Driver/Inc/Legacy" -I"/Users/ulrichludmann/Documents/HM Mechatronik/datenkommunikation/workspace_praktikum/ABGABE_LN/HAL_Driver/Inc" -I"/Users/ulrichludmann/Documents/HM Mechatronik/datenkommunikation/workspace_praktikum/ABGABE_LN/Utilities/STM32F0xx-Nucleo" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


