# Run 'make build' or 'make clean' from Git Bash. May not work in other shells

BINARY = TMS/build/TMS.bin

build: FORCE
	cmake -B TMS/build -S . -G "Unix Makefiles"
	cmake --build TMS/build

clean: FORCE
	rm -r TMS/build

flash: FORCE
	st-flash --reset write $(BINARY) 0x08000000

# empty rule to force others to occur
FORCE:

windows: FORCE
	mkdir TMS/build/windows -p
	g++ TMS/main.cc TMS/mcal/windows/bindings.cc -I TMS/mcal -I. --std=c++20 -o "TMS/build/windows/main.exe"