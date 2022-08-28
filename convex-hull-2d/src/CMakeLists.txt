add_executable(
  convex-hull-2d
    convex-hull-2d.cpp
)

target_link_libraries(
  convex-hull-2d
  PUBLIC
    glut
    GL
    GLU
)
