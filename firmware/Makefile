PLATFORM =
PROJECT = 

BUILD_DIR = build/$(PROJECT)/$(PLATFORM)
CUBEMX_DIR := projects/$(PROJECT)/platforms/$(PLATFORM)/cubemx

KEEP_FILES := .gitignore CMakeLists.txt README board_config.ioc generate.mk

build: FORCE
	cmake -B $(BUILD_DIR) -S. -G"Unix Makefiles" -DPROJECT=$(PROJECT) -DPLATFORM=$(PLATFORM) 
	cmake --build $(BUILD_DIR)

clean: FORCE
	rm $(BUILD_DIR) -rf

deepclean: clean
	@echo "Deleting files and directories in $(CUBEMX_DIR) except $(KEEP_FILES)"
	@cd $(CUBEMX_DIR) && for file in *; do \
		if ! echo "$(KEEP_FILES)" | grep -q "\\b$$file\\b"; then \
			if [ -d "$$file" ]; then \
				echo "Deleting directory $$file"; \
				rm -rf "$$file"; \
			else \
				echo "Deleting file $$file"; \
				rm -f "$$file"; \
			fi; \
		fi; \
	done

st-flash: build
	st-flash --reset write $(BUILD_DIR)/main.bin 0x08000000

FORCE:
