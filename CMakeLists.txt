cmake_minimum_required(VERSION 3.16)

set(APP_NAME)
set(EXECUTABLE_TARGET)
set(DEPENDENCIES_DIR)
set(SDL3_LIB)

# to turn on these options, pass in the -D option when running cmake
# e.g. cmake -S . -B platform_builds/windows -D COMPILE_FROM_SDL3_SRC=ON
option(COMPILE_FROM_SDL3_SRC "compiles SDL3 project from vendored/SDL3 folder" OFF)

if(APPLE)
  set(APP_NAME "hello-mac")
  set(EXECUTABLE_TARGET "MACOSX_BUNDLE")
  set(DEPENDENCIES_DIR "${CMAKE_SOURCE_DIR}/dependencies/mac")
elseif(WIN32)
  set(APP_NAME "hello-windows")
  set(EXECUTABLE_TARGET "WIN32")
  set(DEPENDENCIES_DIR "${CMAKE_SOURCE_DIR}/dependencies/windows")
endif()


project("${APP_NAME}")

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

if(COMPILE_FROM_SDL3_SRC)
  # This assumes the SDL source is available in vendored/SDL
  add_subdirectory(
    "vendored/SDL"
    EXCLUDE_FROM_ALL
  )

  # create executable
  add_executable(${APP_NAME} ${EXECUTABLE_TARGET} hello.c)
  # Link to the actual SDL3 library.
  target_link_libraries(${APP_NAME} PRIVATE SDL3::SDL3)
else()
  # Use dependencies to build project directly
  set(SDL3_PATH "${DEPENDENCIES_DIR}/SDL3")

  if(NOT EXISTS "${SDL3_PATH}")
      message(FATAL_ERROR 
          "\n[DEPENDENCY ERROR]: SDL3 was not found at:\n  ${SDL3_PATH}\n"
          "Please ensure you have placed the pre-compiled library in the correct "
          "platform subfolder under 'dependencies/'."
      )
  endif()

  add_executable(${APP_NAME} ${EXECUTABLE_TARGET} hello.c)
  target_include_directories(${APP_NAME} PRIVATE "${SDL3_PATH}/include")

  find_library(SDL3_LIB
    NAMES SDL3
    PATHS ${SDL3_PATH}
    NO_DEFAULT_PATH
  )

  if (SDL3_LIB)
    target_link_libraries(${APP_NAME} PRIVATE "${SDL3_LIB}")
  else()
    message(FATAL_ERROR "Could not find SDL3 library file in ${SDL3_PATH}")
  endif()

  # Add SDL3.dll file next to windows executable after successful linking
  if(WIN32)
    # 1. Define where the DLL currently lives
    set(SDL3_DLL_PATH "${SDL3_PATH}/SDL3.dll")

    # 2. Tell CMake to copy the DLL after the build finishes
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_DLL_PATH}"
        "$<TARGET_FILE_DIR:${APP_NAME}>"
        COMMENT "Copying SDL3.dll to executable directory..."
    )
  endif()

endif()

#######################################################
#                                                     #
#            Shareable Builds Section                 #    
#                                                     #
#######################################################
# on command line can enter commands such as:
# cmake --build platform_builds/windows --config Release

install(TARGETS ${APP_NAME}
        RUNTIME DESTINATION .
        BUNDLE DESTINATION .)

if(WIN32)
  # install DLL next to EXE
  install(FILES "${SDL3_PATH}/SDL3.dll" DESTINATION .)
elseif(APPLE)
  # For Mac, we usually put dylibs in a Frameworks folder, 
  # but for a simple setup, next to the binary works too.
  install(FILES "${SDL3_PATH}/lib/libSDL3.dylib" DESTINATION .) 
endif()