add_executable(
  glut-menus
    glut-menus.cpp
)

target_link_libraries(
  glut-menus
    PUBLIC
      glut
      GL
      GLU
)
