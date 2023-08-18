.PHONY: toml
toml: build/Makefile
	cmake --build build


build:
	mkdir -p build


test:
	mkdir -p test


build/Makefile: Makefile CMakeLists.txt test/CMakeLists.txt | build
	cmake -DCMAKE_CXX_STANDARD=11 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF -B build .


test/CMakeLists.txt: tool/gentests | test
	tool/gentests


.PHONY: runtests
runtests: build/Makefile
	cmake --build build --target run_toml_tests


.PHONY: debugtests
debugtests: build/Makefile
	cmake --build build --target test_toml
	gdb build/test/test_toml


.PHONY: gentests
gentests:
	tool/gentests


.PHONY: clean
clean:
	rm -rf build
