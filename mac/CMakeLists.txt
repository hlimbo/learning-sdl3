cmake_minimum_required(VERSION 3.16)
project(hello-mac)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(
  "${CMAKE_CURRENT_SOURCE_DIR}/../vendored/SDL"
  "${CMAKE_CURRENT_BINARY_DIR}/build"
  EXCLUDE_FROM_ALL
)

# create mac executable
add_executable(hello-mac MACOSX_BUNDLE hello.c)

# Link to the actual SDL3 library.
target_link_libraries(hello-mac PRIVATE SDL3::SDL3)