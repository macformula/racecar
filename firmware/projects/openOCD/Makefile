# Toolchain
TOOLCHAIN = arm-none-eabi-

# Compiler and flags
CC = $(TOOLCHAIN)gcc
CFLAGS = -mcpu=cortex-m4 -mthumb -O2 -ggdb -Wall
INCLUDES = -IInc -IDrivers/STM32F7xx_HAL_Driver/Inc -IDrivers/STM32F7xx_HAL_Driver/Inc/Legacy -IDrivers/CMSIS/Device/ST/STM32F7xx/Include -IDrivers/CMSIS/Include

# Linker and flags
LD = $(TOOLCHAIN)ld
LDFLAGS = -TSTM32F7508-NUCLEO.ld

# Source files
SRCS = \
  Src/main.c \
  Src/stm32f7xx_it.c \
  Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_msp.c \
  Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_timebase_tim.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable
EXECUTABLE = openocdTest.elf 

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
    $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
    $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
    rm -f $(OBJS) $(EXECUTABLE)

.PHONY: all clean