cmake_minimum_required(VERSION 3.0)

project(
  circle-generation
  DESCRIPTION
    "Interactive Objects"
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

# Debug build compile flags
#   - remove "-fsanitize=address,undefined" if you do not have
#     [ASan](https://github.com/google/sanitizers/wiki/AddressSanitizer) installed
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -g -DDEBUG_MODE -D_GLIBCXX_DEBUG -fsanitize=address,undefined")

# Release build compile flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -O3")

set(CMAKE_PREFIX_PATH "../deps")
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

include_directories(../deps/)
include_directories(../deps/glad/include/)
include_directories(../deps/imgui/)
include_directories(../deps/imgui/backends/)
include_directories(src/include/)

add_library(
  glad
    ../deps/glad/src/glad.c
)

add_library(
  glcore
    ./src/include/vertex/index_buffer.cpp
    ./src/include/vertex/vertex_array.cpp
    ./src/include/vertex/vertex_buffer.cpp
    ./src/include/vertex/vertex_buffer_layout.cpp
    ./src/include/shader/shader.cpp
    ./src/include/renderer.cpp
    ./src/application.cpp
)

add_library (
  imgui
    ../deps/imgui/imgui.cpp
    ../deps/imgui/imgui_demo.cpp
    ../deps/imgui/imgui_draw.cpp
    ../deps/imgui/imgui_tables.cpp
    ../deps/imgui/imgui_widgets.cpp
    ../deps/imgui/backends/imgui_impl_glfw.cpp
    ../deps/imgui/backends/imgui_impl_opengl3.cpp
)

add_subdirectory(src)

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
