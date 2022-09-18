add_executable(
  blender-obj-render
    blender-obj-render.cpp
)

target_link_libraries(
  blender-obj-render
    PUBLIC
      glut
      GL
      GLU
)
