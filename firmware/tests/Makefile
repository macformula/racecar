build: FORCE
	cmake -Bbuild -S. -Wno-dev
	cmake --build build

test:
	ctest --test-dir build

clean: FORCE
	rm build -rf

FORCE: