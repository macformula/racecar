TARGET = stm32f767

ARM_PATH = "C:/Program Files (x86)/GNU Arm Embedded Toolchain/10 2021.10/bin/"
ARM_PREFIX = $(ARM_PATH)arm-none-eabi-

GPP = $(ARM_PREFIX)g++

INFRA_INC = infra/

TARGET_INC = infra/mcal/$(TARGET)
TARGET_SRC = $(wildcard infra/mcal/$(TARGET)/*.cc)

DRIVER_INC = drivers/$(TARGET)/STM32F7xx_HAL_Driver/Inc
DRIVER_SRC = $(wildcard drivers/$(TARGET)/STM32F7xx_HAL_Driver/Src/*.c)

MCAL_INC = drivers/$(TARGET)/mcal
MCAL_SRC = $(wildcard drivers/${TARGET}/mcal/*.cc)

# CMSIS
CMSIS_DEVICE_INC = drivers/$(TARGET)/CMSIS/Device/ST/STM32F7xx/Include
CMSIS_CORE_INC = drivers/$(TARGET)/CMSIS/Core/Include

#Project
PROJ_INC = TMS/mcal/$(TARGET)
PROJ_INC2 = $(PROJ_INC)/Core/Inc

# Defines
DEFINES = -D STM32F767xx 

FLAGS = -mcpu=cortex-m7 -mthumb

build: TMS/main.cc
	$(GPP) TMS/main.cc -I$(TARGET_INC) $(TARGET_SRC) -I$(DRIVER_INC) $(DRIVER_SRC) -I$(CMSIS_CORE_INC) -I$(CMSIS_DEVICE_INC) -I$(MCAL_INC) $(MCAL_SRC) -I$(PROJ_INC) -I$(PROJ_INC2) -I$(INFRA_INC) $(DEFINES) $(FLAGS)

help:
	$(GPP) --version