add_executable(
  sierpinski-triangle-3d
    sierpinski-triangle-3d.cpp
)

target_link_libraries(
  sierpinski-triangle-3d
  PUBLIC
    glut
    GL
    GLU
)
