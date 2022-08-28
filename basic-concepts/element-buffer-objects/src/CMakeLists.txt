add_executable(
  element-buffer-object
    element-buffer-object.cpp
    ../../../.deps/glad/glad.c
)

target_link_libraries(
  element-buffer-object
  PUBLIC
    glfw
    dl
)
