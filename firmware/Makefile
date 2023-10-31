# Run 'make build' or 'make clean' from Git Bash. May not work in other shells

build: FORCE
	cmake -B TMS/build -S . -G "Unix Makefiles"
	cmake --build TMS/build

clean: FORCE
	rm -r TMS/build

# empty rule to force others to occur
FORCE:
