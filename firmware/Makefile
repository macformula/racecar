PLATFORM =
PROJECT = 

BUILD = build
BUILD_DIR = $(BUILD)/$(PROJECT)/$(PLATFORM)
CUBEMX_DIR := projects/$(PROJECT)/platforms/$(PLATFORM)/cubemx
GENERATOR = -G"Unix Makefiles"

COMPILE_COMMANDS_DEST = $(BUILD)/compile_commands.json

build: config
	cmake --build $(BUILD_DIR)

config: FORCE
	cmake -B $(BUILD_DIR) -S. $(GENERATOR) -DPROJECT=$(PROJECT) -DPLATFORM=$(PLATFORM) $(COMPILE_COMMANDS_FLAGS)

	@echo Copying compile_commands.json to $(BUILD) for clangd.
	cp $(BUILD_DIR)/compile_commands.json $(COMPILE_COMMANDS_DEST)
	@# Ensure the .clangd file exists
	touch .clangd 

clean: FORCE
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
	st-flash --reset write $(BUILD_DIR)/main.bin 0x08000000

FORCE:
