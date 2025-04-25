# Modify the CubeMX-generated toolchain file to work with out build system.
# These commands are not in the Makefile since Make messes with the ${} characters
# in the `sed` replacement string.

# CubeMX forces CMake to skip its automatic compiler detection, which messes
# with our clangd setup. Comment out the lines that do this.
sed -E '/set\(CMAKE.*COMPILER_FORCED.*$/s/^/# /g' cmake/racecar-toolchain.cmake > tmp.txt
mv tmp.txt cmake/racecar-toolchain.cmake

# CubeMX incorrectly assumes that our top-level CMakeLists.txt is in the same
# directory as the .ioc file. This means that the linker script directory is wrong.
# Find the "-T ** .ld" line and replace the CMAKE variable with the current directory.
sed -E "/-T.*\.ld/s|\\\$\{CMAKE_SOURCE_DIR\}|${PWD}|g" cmake/racecar-toolchain.cmake > tmp.txt
mv tmp.txt cmake/racecar-toolchain.cmake
