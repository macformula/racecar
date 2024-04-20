PLATFORM =
PROJECT = 

# Pass `AUTOGEN_CUBEMX=OFF` to `make` to disable cubmx generation
AUTOGEN_CUBEMX = ON

BUILD = build
BUILD_DIR = $(BUILD)/$(PROJECT)/$(PLATFORM)
CUBEMX_DIR := projects/$(PROJECT)/platforms/$(PLATFORM)/cubemx
GENERATOR = -G"Unix Makefiles"

COMPILE_COMMANDS_DEST = $(BUILD)/compile_commands.json

.PHONY: build config clean deepclean st-flash

build: config
	cmake --build $(BUILD_DIR)

config:
	cmake -B $(BUILD_DIR) -S. $(GENERATOR) -D PROJECT=$(PROJECT) -D PLATFORM=$(PLATFORM) -D AUTOGEN_CUBEMX=$(AUTOGEN_CUBEMX)

	@echo Copying compile_commands.json to $(BUILD) for clangd.
	cp $(BUILD_DIR)/compile_commands.json $(COMPILE_COMMANDS_DEST)
	@# Ensure the .clangd file exists.
	touch .clangd 

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(COMPILE_COMMANDS_DEST)

deepclean: clean
ifeq ($(findstring stm32,$(PLATFORM)),stm32)
	@echo "Deleting files and directories in $(CUBEMX_DIR) that are ignored by Git"
	find $(CUBEMX_DIR) | xargs git check-ignore | xargs rm -rf
else
	@echo "Skipping deepclean for platform $(PLATFORM)"
endif

st-flash: build
	st-flash --reset write $(BUILD_DIR)/$(PROJECT).bin 0x08000000
