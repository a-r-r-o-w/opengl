add_executable(
  glulookat
    glulookat.cpp
)

target_link_libraries(
  glulookat
  PUBLIC
    glut
    GL
    GLU
)
