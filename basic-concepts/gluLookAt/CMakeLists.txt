cmake_minimum_required(VERSION 3.0)

project(
  hello-triangle
  DESCRIPTION
    "gluLookAt"
  LANGUAGES
    C
    CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
# set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -g -DDEBUG_MODE -D_GLIBCXX_DEBUG -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -O3")

include_directories(deps)

add_subdirectory(src)
