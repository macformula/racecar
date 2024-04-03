PLATFORM =
PROJECT = 

BUILD = build
BUILD_DIR = $(BUILD)/$(PROJECT)/$(PLATFORM)
CUBEMX_DIR := projects/$(PROJECT)/platforms/$(PLATFORM)/cubemx

COMPILE_COMMANDS_DEST = $(BUILD)/compile_commands.json

build: FORCE
	cmake -B $(BUILD_DIR) -S. -G"Unix Makefiles" -DPROJECT=$(PROJECT) -DPLATFORM=$(PLATFORM) -D CMAKE_EXPORT_COMPILE_COMMANDS=1
	cmake --build $(BUILD_DIR)
	cp $(BUILD_DIR)/compile_commands.json $(COMPILE_COMMANDS_DEST)

clean: FORCE
	rm -rf $(BUILD_DIR)
	rm $(COMPILE_COMMANDS_DEST)

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
