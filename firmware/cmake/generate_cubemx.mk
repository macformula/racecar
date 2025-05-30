# Find the STM32CubeMX executable
ifeq ($(OS), Windows_NT)
# Convert Windows backslash to regular slash
	CUBEMX_PATH := $(subst \,/,$(shell where STM32CubeMX))
else # Linux / MacOS
    CUBEMX_PATH := $(shell which STM32CubeMX)
endif

# Find the JAVA which is installed with CubeMX. Spaces in path are escaped.
space := $(subst ,, )

# Known bug: Expanding CUBEMX_JAVA twice does not work.
ifeq ($(shell uname), Darwin)
# MacOS
	CUBEMX_JAVA := $(dir $(subst $(space),\$(space),$(CUBEMX_PATH)))jre/Contents/Home/bin/java
else
# Windows / Linux
	CUBEMX_JAVA := $(dir $(subst $(space),\$(space),$(CUBEMX_PATH)))jre/bin/java
endif

IOC_FILE = board_config.ioc

CUBEMX_GEN_SCRIPT = cubemx_script.txt

# Get the directory of this file since it is called from other locations
BUILD_SYS_DIR:=$(dir $(MAKEFILE_LIST))
CUSTOM_TARGETS_FILE = $(BUILD_SYS_DIR)custom_cubemx_targets.mk

CUBEMX_GEN_SCRIPT_TEMPLATE := $(BUILD_SYS_DIR)generate_cubemx_script.txt.template

cmake/racecar-toolchain.cmake: cmake/gcc-arm-none-eabi.cmake
	@echo "Modifying autogen toolchain file."
	cp $< $@
	. $(BUILD_SYS_DIR)custom_toolchain.sh

# This recipe will execute whenever IOC_FILE has a newer timestamp than the
# generated toolchain file, i.e. whenever IOC_FILE has been updated but new code
# has not been generated.
cmake/gcc-arm-none-eabi.cmake: $(IOC_FILE) $(CUBEMX_GEN_SCRIPT_TEMPLATE)
	@echo "Autogenerating from CubeMX. If you don't want to do this, you must manually 'Generate Code' before building."
# Create a file containing commands to generate the cubemx code.
	sed -e 's/IOC_FILE/$(IOC_FILE)/g' $(CUBEMX_GEN_SCRIPT_TEMPLATE) > $(CUBEMX_GEN_SCRIPT)
	
# Run the cubemx program to generate code.
	$(CUBEMX_JAVA) -jar "$(CUBEMX_PATH)" -q "$(CUBEMX_GEN_SCRIPT)"
