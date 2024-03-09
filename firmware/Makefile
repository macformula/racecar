PLATFORM =
PROJECT = 

BUILD_DIR = build/$(PROJECT)/$(PLATFORM)
CUBEMX_DIR := projects/$(PROJECT)/platforms/$(PLATFORM)/cubemx

build: FORCE
	cmake -B $(BUILD_DIR) -S. -G"Unix Makefiles" -DPROJECT=$(PROJECT) -DPLATFORM=$(PLATFORM) 
	cmake --build $(BUILD_DIR)

clean: FORCE
	rm $(BUILD_DIR) -rf

deepclean: clean
ifeq ($(findstring stm32,$(PLATFORM)),stm32)
	@echo "Deleting files and directories in $(CUBEMX_DIR) that are ignored by Git"
	@cd $(CUBEMX_DIR) && \
		for file in $$(ls -R); do \
			if git check-ignore "$$file" >/dev/null; then \
				echo "Deleting $$file"; \
				rm -rf "$$file"; \
			fi; \
		done
else
	@echo "Skipping deepclean for platform $(PLATFORM)"
endif

st-flash: build
	st-flash --reset write $(BUILD_DIR)/main.bin 0x08000000

FORCE:
