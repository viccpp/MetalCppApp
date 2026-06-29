cmake_minimum_required(VERSION 3.22)
project(MetalCppApp LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Metal-cpp headers
# Download from https://developer.apple.com/metal/cpp/ and unzip into
# the project root as "metal-cpp/".
# ---------------------------------------------------------------------------
set(METAL_CPP_DIR "${CMAKE_SOURCE_DIR}/metal-cpp"
    CACHE PATH "Path to the metal-cpp header directory")

if(NOT EXISTS "${METAL_CPP_DIR}/Metal/Metal.hpp")
    message(FATAL_ERROR
        "metal-cpp headers not found at ${METAL_CPP_DIR}.\n"
        "Download from https://developer.apple.com/metal/cpp/ and unzip "
        "into <project_root>/metal-cpp/")
endif()

# ---------------------------------------------------------------------------
# Frameworks
# ---------------------------------------------------------------------------
find_library(METAL_LIB      Metal)
find_library(METALKIT_LIB   MetalKit)
find_library(COCOA_LIB      Cocoa)
find_library(QUARTZCORE_LIB QuartzCore)
find_library(FOUNDATION_LIB Foundation)

# ---------------------------------------------------------------------------
# Compile Metal shaders → default.metallib
# ---------------------------------------------------------------------------
set(METALLIB_OUTPUT "${CMAKE_BINARY_DIR}/default.metallib")

add_custom_command(
    OUTPUT  "${METALLIB_OUTPUT}"
    COMMAND xcrun -sdk macosx metal
                  -c "${CMAKE_SOURCE_DIR}/src/shaders.metal"
                  -o "${CMAKE_BINARY_DIR}/shaders.air"
    COMMAND xcrun -sdk macosx metallib
                  "${CMAKE_BINARY_DIR}/shaders.air"
                  -o "${METALLIB_OUTPUT}"
    DEPENDS "${CMAKE_SOURCE_DIR}/src/shaders.metal"
    COMMENT "Compiling Metal shaders"
)
add_custom_target(Shaders DEPENDS "${METALLIB_OUTPUT}")

# Copy metallib next to the executable after build
add_custom_command(
    OUTPUT  "${CMAKE_BINARY_DIR}/MetalCppApp.app/Contents/MacOS/default.metallib"
    COMMAND ${CMAKE_COMMAND} -E copy
            "${METALLIB_OUTPUT}"
            "${CMAKE_BINARY_DIR}/default.metallib"
    DEPENDS Shaders
)

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
add_executable(MetalCppApp MACOSX_BUNDLE
    src/main.cpp
    src/metal_impl.cpp
    src/renderer.cpp
    src/platform.mm
)

add_dependencies(MetalCppApp Shaders)

target_include_directories(MetalCppApp PRIVATE
    "${METAL_CPP_DIR}"
    src
)

target_link_libraries(MetalCppApp
    "${METAL_LIB}"
    "${METALKIT_LIB}"
    "${COCOA_LIB}"
    "${QUARTZCORE_LIB}"
    "${FOUNDATION_LIB}"
)

# Copy metallib into the build directory so the app can find it at runtime
add_custom_command(TARGET MetalCppApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${METALLIB_OUTPUT}"
            "$<TARGET_FILE_DIR:MetalCppApp>/default.metallib"
    COMMENT "Copying default.metallib next to executable"
)
