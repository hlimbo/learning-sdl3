cmake_minimum_required(VERSION 3.16)

set(APP_NAME)
set(EXECUTABLE_TARGET)

if(APPLE)
  set(APP_NAME "hello-mac")
  set(EXECUTABLE_TARGET "MACOSX_BUNDLE")
elseif(WIN32)
  set(APP_NAME "hello-windows")
  set(EXECUTABLE_TARGET "WIN32")
endif()


project("${APP_NAME}")

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(
  "vendored/SDL"
  EXCLUDE_FROM_ALL
)

# create executable
add_executable(${APP_NAME} ${EXECUTABLE_TARGET} hello.c)

# Link to the actual SDL3 library.
target_link_libraries(${APP_NAME} PRIVATE SDL3::SDL3)