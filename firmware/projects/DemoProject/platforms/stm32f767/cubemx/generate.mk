ifeq ($(OS),Windows_NT)
    CUBEMX_PATH := $(shell where STM32CubeMX)
    # Convert path to Windows-style and add .exe extension
    CUBEMX_PATH := $(subst \,\\,$(CUBEMX_PATH))
else
    CUBEMX_PATH := $(shell which STM32CubeMX)
endif

IOC_FILE = board_config.ioc

CUBEMX_GEN_SCRIPT = cubemx_script.txt

GenerateCode: Makefile

Makefile: $(IOC_FILE)
	echo "config load \"$(IOC_FILE)\"" > $(CUBEMX_GEN_SCRIPT)
	echo "project generate ./" >> $(CUBEMX_GEN_SCRIPT)
	echo "exit" >> $(CUBEMX_GEN_SCRIPT)

	java -jar "$(CUBEMX_PATH)" -q "$(CUBEMX_GEN_SCRIPT)"

	rm $(CUBEMX_GEN_SCRIPT)

	echo "" >> Makefile
	echo 'objects: $$(OBJECTS)' >> Makefile