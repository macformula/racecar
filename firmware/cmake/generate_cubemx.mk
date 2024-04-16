ifeq ($(OS),Windows_NT)
    CUBEMX_PATH := $(shell where STM32CubeMX)
    # Convert path to Windows-style and add .exe extension
    CUBEMX_PATH := $(subst \,\\,$(CUBEMX_PATH))
else
    CUBEMX_PATH := $(shell which STM32CubeMX)
endif

IOC_FILE = board_config.ioc

CUBEMX_GEN_SCRIPT = cubemx_script.txt

# This recipe will execute whenever IOC_FILE has a newer timestamp than the
# Makefile, i.e. whenever IOC_FILE has been updated but new code has not been
# generated.

.PHONY: Makefile

# GenerateCubeMx: $(IOC_FILE)
# # Create an file containing commands to generate the cubemx code.
# 	echo "config load \"$(IOC_FILE)\"" > $(CUBEMX_GEN_SCRIPT)
# 	echo "project generate ./" >> $(CUBEMX_GEN_SCRIPT)
# 	echo "exit" >> $(CUBEMX_GEN_SCRIPT)

# # Run the cubemx program to generate code.
# 	java -jar "$(CUBEMX_PATH)" -q "$(CUBEMX_GEN_SCRIPT)"

# 	rm $(CUBEMX_GEN_SCRIPT)

Makefile: $(IOC_FILE)
# Add a recipe for building the sources to objects without linking them.
# This is used by build_cubemx.cmake
	echo "" >> Makefile
	echo 'objects: $$(OBJECTS)' >> Makefile

# Add a recipe for printing any Makefile variable to stdout
# This is used by build_cubemx.cmake
	echo "" >> Makefile
	echo "%.value:" >> Makefile
	echo "\t@echo \$$(\$$*)" >> Makefile