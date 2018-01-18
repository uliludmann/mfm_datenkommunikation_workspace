################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32f072xb.s 

OBJS += \
./startup/startup_stm32f072xb.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -I"/Users/ulrichludmann/Documents/HM Mechatronik/datenkommunikation/workspace_praktikum/ABGABE_LN/inc" -I"/Users/ulrichludmann/Documents/HM Mechatronik/datenkommunikation/workspace_praktikum/ABGABE_LN/CMSIS/core" -I"/Users/ulrichludmann/Documents/HM Mechatronik/datenkommunikation/workspace_praktikum/ABGABE_LN/CMSIS/device" -I"/Users/ulrichludmann/Documents/HM Mechatronik/datenkommunikation/workspace_praktikum/ABGABE_LN/HAL_Driver/Inc/Legacy" -I"/Users/ulrichludmann/Documents/HM Mechatronik/datenkommunikation/workspace_praktikum/ABGABE_LN/HAL_Driver/Inc" -I"/Users/ulrichludmann/Documents/HM Mechatronik/datenkommunikation/workspace_praktikum/ABGABE_LN/Utilities/STM32F0xx-Nucleo" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


