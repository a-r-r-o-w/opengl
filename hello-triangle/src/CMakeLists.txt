add_executable(
  hello-triangle
    hello-triangle.cpp
    ../../deps/glad/src/glad.c
)

target_link_libraries(
  hello-triangle
  PUBLIC
    glfw
    dl
)
