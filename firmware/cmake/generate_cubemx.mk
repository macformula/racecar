ifeq ($(OS),Windows_NT)
# Convert windows backslash to regular slash
	CUBEMX_PATH := $(subst \,/,$(shell where STM32CubeMX))
else # Linux / MacOS
    CUBEMX_PATH := $(shell which STM32CubeMX)
endif

# Find the JAVA which is installed with CubeMX. Spaces in path are escaped.
space := $(subst ,, )
CUBEMX_JAVA := $(dir $(subst $(space),\$(space),$(CUBEMX_PATH)))jre/bin/java
# Known bug: Expanding CUBEMX_JAVA twice does not work.

IOC_FILE = board_config.ioc

CUBEMX_GEN_SCRIPT = cubemx_script.txt

# Get the directory of this file since it is called from other locations
BUILD_SYS_DIR:=$(dir $(MAKEFILE_LIST))
CUSTOM_TARGETS_FILE = $(BUILD_SYS_DIR)custom_cubemx_targets.mk

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
	$(CUBEMX_JAVA) -jar "$(CUBEMX_PATH)" -q "$(CUBEMX_GEN_SCRIPT)"