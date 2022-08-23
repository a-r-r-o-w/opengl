add_executable(
  sierpinski-triangle-2d
    sierpinski-triangle-2d.cpp
)

target_link_libraries(
  sierpinski-triangle-2d
  PUBLIC
    glut
    GL
    GLU
)
