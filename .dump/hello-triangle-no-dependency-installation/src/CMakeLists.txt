add_executable(
  hello-triangle
    hello-triangle.cpp
)

target_link_libraries(
  hello-triangle
  PUBLIC
    glad
    glfw
    dl
)
