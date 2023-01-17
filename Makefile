.PHONY:	fmt validate_fmt buildfiles


error:
	@echo "Please choose one of the following target: create, validate_fmt, fmt"

validate_fmt:
	find src  -regex '.*\.\(h\|cpp\|vert\|frag\|comp\)'  | xargs clang-format -i --verbose  --style=file --dry-run

fmt:
	find src -regex '.*\.\(h\|cpp\|vert\|frag\|comp\)'  | xargs clang-format -i  --verbose --style=file

create:
	@echo "Creating project Makefiles using cmake"
	rm -rf build_make
	mkdir build_make
	cmake -S . -B build_make -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="thirdparty/vcpkg/scripts/buildsystems/vcpkg.cmake"
