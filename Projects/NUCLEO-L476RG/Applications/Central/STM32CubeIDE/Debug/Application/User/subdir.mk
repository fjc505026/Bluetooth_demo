################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/app_bluenrg_2.c \
C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/central.c \
C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/console.c \
C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/hci_tl_interface.c \
C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/main.c \
C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/st_data_parser.c \
C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/stm32l4xx_hal_msp.c \
C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/stm32l4xx_it.c \
C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/stm32l4xx_nucleo.c \
C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/stm32l4xx_nucleo_bus.c \
../Application/User/syscalls.c \
../Application/User/sysmem.c 

OBJS += \
./Application/User/app_bluenrg_2.o \
./Application/User/central.o \
./Application/User/console.o \
./Application/User/hci_tl_interface.o \
./Application/User/main.o \
./Application/User/st_data_parser.o \
./Application/User/stm32l4xx_hal_msp.o \
./Application/User/stm32l4xx_it.o \
./Application/User/stm32l4xx_nucleo.o \
./Application/User/stm32l4xx_nucleo_bus.o \
./Application/User/syscalls.o \
./Application/User/sysmem.o 

C_DEPS += \
./Application/User/app_bluenrg_2.d \
./Application/User/central.d \
./Application/User/console.d \
./Application/User/hci_tl_interface.d \
./Application/User/main.d \
./Application/User/st_data_parser.d \
./Application/User/stm32l4xx_hal_msp.d \
./Application/User/stm32l4xx_it.d \
./Application/User/stm32l4xx_nucleo.d \
./Application/User/stm32l4xx_nucleo_bus.d \
./Application/User/syscalls.d \
./Application/User/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/app_bluenrg_2.o: C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/app_bluenrg_2.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L476xx -DDEBUG -c -I../../Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../../../../../Middlewares/ST/BlueNRG-2/utils -I../../../../../../Middlewares/ST/BlueNRG-2/includes -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/central.o: C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/central.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L476xx -DDEBUG -c -I../../Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../../../../../Middlewares/ST/BlueNRG-2/utils -I../../../../../../Middlewares/ST/BlueNRG-2/includes -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/console.o: C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/console.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L476xx -DDEBUG -c -I../../Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../../../../../Middlewares/ST/BlueNRG-2/utils -I../../../../../../Middlewares/ST/BlueNRG-2/includes -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/hci_tl_interface.o: C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/hci_tl_interface.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L476xx -DDEBUG -c -I../../Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../../../../../Middlewares/ST/BlueNRG-2/utils -I../../../../../../Middlewares/ST/BlueNRG-2/includes -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/main.o: C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/main.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L476xx -DDEBUG -c -I../../Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../../../../../Middlewares/ST/BlueNRG-2/utils -I../../../../../../Middlewares/ST/BlueNRG-2/includes -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/st_data_parser.o: C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/st_data_parser.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L476xx -DDEBUG -c -I../../Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../../../../../Middlewares/ST/BlueNRG-2/utils -I../../../../../../Middlewares/ST/BlueNRG-2/includes -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/stm32l4xx_hal_msp.o: C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/stm32l4xx_hal_msp.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L476xx -DDEBUG -c -I../../Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../../../../../Middlewares/ST/BlueNRG-2/utils -I../../../../../../Middlewares/ST/BlueNRG-2/includes -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/stm32l4xx_it.o: C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/stm32l4xx_it.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L476xx -DDEBUG -c -I../../Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../../../../../Middlewares/ST/BlueNRG-2/utils -I../../../../../../Middlewares/ST/BlueNRG-2/includes -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/stm32l4xx_nucleo.o: C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/stm32l4xx_nucleo.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L476xx -DDEBUG -c -I../../Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../../../../../Middlewares/ST/BlueNRG-2/utils -I../../../../../../Middlewares/ST/BlueNRG-2/includes -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/stm32l4xx_nucleo_bus.o: C:/Users/jingchen.fan/demo/Bluetooth_demo/Projects/NUCLEO-L476RG/Applications/Central/Src/stm32l4xx_nucleo_bus.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L476xx -DDEBUG -c -I../../Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../../../../../Middlewares/ST/BlueNRG-2/utils -I../../../../../../Middlewares/ST/BlueNRG-2/includes -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/%.o Application/User/%.su Application/User/%.cyclo: ../Application/User/%.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L476xx -DDEBUG -c -I../../Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Middlewares/ST/BlueNRG-2/hci/hci_tl_patterns/Basic -I../../../../../../Middlewares/ST/BlueNRG-2/utils -I../../../../../../Middlewares/ST/BlueNRG-2/includes -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User

clean-Application-2f-User:
	-$(RM) ./Application/User/app_bluenrg_2.cyclo ./Application/User/app_bluenrg_2.d ./Application/User/app_bluenrg_2.o ./Application/User/app_bluenrg_2.su ./Application/User/central.cyclo ./Application/User/central.d ./Application/User/central.o ./Application/User/central.su ./Application/User/console.cyclo ./Application/User/console.d ./Application/User/console.o ./Application/User/console.su ./Application/User/hci_tl_interface.cyclo ./Application/User/hci_tl_interface.d ./Application/User/hci_tl_interface.o ./Application/User/hci_tl_interface.su ./Application/User/main.cyclo ./Application/User/main.d ./Application/User/main.o ./Application/User/main.su ./Application/User/st_data_parser.cyclo ./Application/User/st_data_parser.d ./Application/User/st_data_parser.o ./Application/User/st_data_parser.su ./Application/User/stm32l4xx_hal_msp.cyclo ./Application/User/stm32l4xx_hal_msp.d ./Application/User/stm32l4xx_hal_msp.o ./Application/User/stm32l4xx_hal_msp.su ./Application/User/stm32l4xx_it.cyclo ./Application/User/stm32l4xx_it.d ./Application/User/stm32l4xx_it.o ./Application/User/stm32l4xx_it.su ./Application/User/stm32l4xx_nucleo.cyclo ./Application/User/stm32l4xx_nucleo.d ./Application/User/stm32l4xx_nucleo.o ./Application/User/stm32l4xx_nucleo.su ./Application/User/stm32l4xx_nucleo_bus.cyclo ./Application/User/stm32l4xx_nucleo_bus.d ./Application/User/stm32l4xx_nucleo_bus.o ./Application/User/stm32l4xx_nucleo_bus.su ./Application/User/syscalls.cyclo ./Application/User/syscalls.d ./Application/User/syscalls.o ./Application/User/syscalls.su ./Application/User/sysmem.cyclo ./Application/User/sysmem.d ./Application/User/sysmem.o ./Application/User/sysmem.su

.PHONY: clean-Application-2f-User

