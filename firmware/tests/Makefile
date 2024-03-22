test: build
	ctest --test-dir build --output-on-failure

build: FORCE
	cmake -Bbuild -S. -Wno-dev
	cmake --build build

clean: FORCE
	rm build -rf

FORCE: