.PHONY: toml
toml: build/Makefile
	cmake --build build


build:
	mkdir build


build/Makefile: Makefile CMakeLists.txt | build
	cmake -B build .


.PHONY: test
test: build/Makefile
	cmake --build build --target run_toml_tests


.PHONY: debug
debug: build/Makefile
	cmake --build build --target test_toml
	lldb build/test_toml


.PHONY: clean
clean:
	rm -rf build
