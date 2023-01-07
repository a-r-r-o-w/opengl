add_executable(
  interactive-objects
    ./application.cpp
    ./scene.cpp
    ./object.cpp
    ./camera.cpp
    ./main.cpp
)

target_link_libraries(
  interactive-objects
  PUBLIC
    glad
    glfw
    dl
    glcore
    imgui
)
