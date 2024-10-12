ifndef PROJECT
$(error PROJECT not defined. Add the `PROJECT=<project-name>` argument.)
endif

ifndef PLATFORM
$(error PLATFORM not defined. Add the `PLATFORM=<platform-name>` argument.)
endif

BUILD := build
BUILD_DIR = $(BUILD)/$(PROJECT)/$(PLATFORM)
PLATFORM_DIR := projects/$(PROJECT)/platforms/$(PLATFORM)
GENERATOR := -G"Unix Makefiles"
LOG_LEVEL := STATUS

#Extract the project folder name from the path, removing any parent directories and trailing slashes.
PROJECT_NAME := $(notdir $(patsubst %/,%,$(PROJECT)))

COMPILE_COMMANDS_DEST := $(BUILD)/compile_commands.json

.PHONY: build config clean deepclean st-flash

build: config
	cmake --build $(BUILD_DIR)

config:
	cmake -B $(BUILD_DIR) -S. $(GENERATOR) -D PROJECT=$(PROJECT) -D PLATFORM=$(PLATFORM) -D PROJECT_NAME=$(PROJECT_NAME) --log-level $(LOG_LEVEL)

	@echo Copying compile_commands.json to $(BUILD) for clangd.
	cp $(BUILD_DIR)/compile_commands.json $(COMPILE_COMMANDS_DEST)
	@echo Ensure the .clangd file exists.
	touch .clangd 

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(COMPILE_COMMANDS_DEST)

deepclean: clean
ifeq ($(findstring stm32,$(PLATFORM)),stm32)
	@echo "Deleting files and directories in $(PLATFORM_DIR)/cubemx that are ignored by Git"
	find $(PLATFORM_DIR)/cubemx | xargs git check-ignore | xargs rm -rf
else
	@echo "Skipping deepclean for platform $(PLATFORM)"
endif

st-flash: build
	st-flash --reset write $(BUILD_DIR)/$(PROJECT_NAME).bin 0x08000000
