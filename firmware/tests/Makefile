test: build
	ctest --test-dir build

build: FORCE
	cmake -Bbuild -S. -Wno-dev
	cmake --build build

clean: FORCE
	rm build -rf

FORCE: