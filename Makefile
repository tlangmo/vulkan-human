.PHONY:	fmt validate_fmt buildfiles


error:
	@echo "Please choose one of the following target: create, validate_fmt, fmt"

validate_fmt:
	find src -iname *.h -o -iname *.cpp | xargs clang-format -i -verbose --style=file --dry-run

fmt:
	find src -iname *.h -o -iname *.cpp | xargs clang-format -i -verbose --style=file

create:
	@echo "Creating project Makefiles using cmake"
	rm -rf build_make
	mkdir build_make
	cmake -S . -B build_make -DCMAKE_TOOLCHAIN_FILE="thirdparty/vcpkg/scripts/buildsystems/vcpkg.cmake"
