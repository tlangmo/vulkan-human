export VCPKG_BIN="thirdparty/vcpkg/vcpkg"

export VULKAN_SDK="/usr/"

#sudo apt install libx11-dev libxft-dev libxext-dev libwayland-dev libxkbcommon-dev libegl1-mesa-dev

$VCPKG_BIN install --recurse \
    catch2 \
    sdl2 \
    glm \
    glfw3 \
    vulkan-memory-allocator \
    tinygltf