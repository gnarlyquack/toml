.PHONY: toml
toml: build/Makefile
	cmake --build build


build:
	mkdir build


build/Makefile: Makefile CMakeLists.txt | build
	cmake -DCMAKE_CXX_STANDARD=11 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF -B build .


.PHONY: test
test: build/Makefile
	cmake --build build --target run_toml_tests


.PHONY: debug
debug: build/Makefile
	cmake --build build --target test_toml
	gdb build/test_toml


.PHONY: clean
clean:
	rm -rf build
