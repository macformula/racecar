PLATFORM =
PROJECT = 

BUILD_DIR = build/$(PROJECT)/$(PLATFORM)

build: FORCE
	cmake -B $(BUILD_DIR) -S. -G"Unix Makefiles" -DPROJECT=$(PROJECT) -DPLATFORM=$(PLATFORM) 
	cmake --build $(BUILD_DIR)

clean: FORCE
	rm $(BUILD_DIR) -rf

st-flash: build
	st-flash --reset write $(BUILD_DIR)/main.bin 0x08000000

FORCE:
