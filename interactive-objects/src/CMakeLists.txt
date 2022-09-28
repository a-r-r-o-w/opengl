include_directories(./include)

add_library(
  glad
    ../../.deps/glad/glad.c
)

add_library(
  glcore
    ./include/vertex/index_buffer.cpp
    ./include/vertex/vertex_array.cpp
    ./include/vertex/vertex_buffer.cpp
    ./include/vertex/vertex_buffer_layout.cpp
    ./include/shader/shader.cpp
    ./include/renderer.cpp
    ./application.cpp
)

add_library (
  imgui
    ../../.deps/imgui/imgui.cpp
    ../../.deps/imgui/imgui_demo.cpp
    ../../.deps/imgui/imgui_draw.cpp
    ../../.deps/imgui/imgui_tables.cpp
    ../../.deps/imgui/imgui_widgets.cpp
    ../../.deps/imgui/backends/imgui_impl_glfw.cpp
    ../../.deps/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(
  interactive-objects
    ./application.cpp
    ./object.cpp
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
