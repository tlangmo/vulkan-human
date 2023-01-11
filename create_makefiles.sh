rm -rf build_make
mkdir build_make
cmake -S . -B build_make -DCMAKE_TOOLCHAIN_FILE="thirdparty/vcpkg/scripts/buildsystems/vcpkg.cmake"
