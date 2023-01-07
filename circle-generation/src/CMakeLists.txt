add_executable(
  circle-generation
    ./main.cpp
)

target_link_libraries(
  circle-generation
  PUBLIC
    glad
    glfw
    dl
    glcore
    imgui
)
