ifeq ($(OS),Windows_NT)
    CUBEMX_PATH := $(shell where STM32CubeMX)
    # Convert path to Windows-style and add .exe extension
    CUBEMX_PATH := $(subst \,\\,$(CUBEMX_PATH))
else
    CUBEMX_PATH := $(shell which STM32CubeMX)
endif

IOC_FILE = board_config.ioc
AUTOGEN_CUBEMX = ON

CUBEMX_GEN_SCRIPT = cubemx_script.txt

# Get the directory of this file since it is called from other locations
BUILD_SYS_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
CUSTOM_TARGETS_FILE = $(BUILD_SYS_DIR)/custom_cubemx_targets.mk

# Copy the CubeMX makefile and custom targets into a new makefile
CustomMakefile.mk: Makefile $(CUSTOM_TARGETS_FILE)
	@echo "Creating $@ from [$^]"
	cat Makefile > $@
	cat $(CUSTOM_TARGETS_FILE) >> $@ 

# This recipe will execute whenever IOC_FILE has a newer timestamp than the
# Makefile, i.e. whenever IOC_FILE has been updated but new code has not been
# generated.
Makefile: $(IOC_FILE)
	@echo "Autogenerating from CubeMX. If you don't want to do this, you must manually 'Generate Code' before building."
# Create an file containing commands to generate the cubemx code.
	@printf 'config load "%s"\n' $(IOC_FILE) > $(CUBEMX_GEN_SCRIPT)
	@printf 'project generate ./\n' >> $(CUBEMX_GEN_SCRIPT)
	@printf 'exit\n' >> $(CUBEMX_GEN_SCRIPT)

# Run the cubemx program to generate code.
	java -jar "$(CUBEMX_PATH)" -q "$(CUBEMX_GEN_SCRIPT)"

	rm $(CUBEMX_GEN_SCRIPT)