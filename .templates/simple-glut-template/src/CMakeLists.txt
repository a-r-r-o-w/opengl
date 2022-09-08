add_executable(
  simple-glut-template
    simple-glut-template.cpp
)

target_link_libraries(
  simple-glut-template
    PUBLIC
      glut
      GL
      GLU
)
