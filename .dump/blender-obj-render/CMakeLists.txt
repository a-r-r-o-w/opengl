cmake_minimum_required(VERSION 3.0)

project(
  simple-glut-template
  DESCRIPTION
    "Blender OBJ Render"
  LANGUAGES
    C
    CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

if (CMAKE_BUILD_TYPE STREQUAL "")
  message(STATUS "No build type was specified using -DCMAKE_BUILD_TYPE=[Debug|Release]. Defaulting to Release build")
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Debug flags
#   - remove "-fsanitize=address,undefined" if you do not have
#     [ASan](https://github.com/google/sanitizers/wiki/AddressSanitizer) installed
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -g -DDEBUG_MODE -D_GLIBCXX_DEBUG -fsanitize=address,undefined")

# Release build compile flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -O3")

include_directories(deps)
add_subdirectory(src)

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
