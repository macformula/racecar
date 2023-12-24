PLATFORM = stm32f767
PROJECT = TMS

BUILD_DIR = build/$(PROJECT)/$(PLATFORM)

build: clean
	cmake -B $(BUILD_DIR) -S. -G"Unix Makefiles" -DPROJECT=$(PROJECT) -DPLATFORM=$(PLATFORM) 
	cmake --build $(BUILD_DIR)

clean: FORCE
	rm $(BUILD_DIR) -rf

FORCE: