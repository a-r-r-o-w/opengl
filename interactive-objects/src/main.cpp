#include <iostream>

#include "types.hpp"
#include "application.hpp"
#include "object.hpp"

using namespace gl::types;

void initialise_demo (gl::application&);

std::unique_ptr <gl::object> create_triangle (u32, u32);

int main () {
  {
    gl::application* application = new gl::application(800, 600, "Interactive Objects");
    
    initialise_demo(*application);
    application->run();

    delete application;
  }

  return 0;
}

void initialise_demo (gl::application& application) {
  application.m_objects.emplace_back(create_triangle(application.get_width(), application.get_height()));
}

std::unique_ptr <gl::object> create_triangle (u32 width, u32 height) {
  f32 t_width  = (f32)width / 4;
  f32 t_height = (f32)height / 4;

  f32 w_mid = (f32)width / 2;
  f32 h_mid = (f32)height / 2;

  f32 left   = w_mid - t_width;
  f32 right  = w_mid + t_width;
  f32 bottom = h_mid - t_height;
  f32 top    = h_mid + t_height;

  auto object = std::make_unique <gl::object> ("Triangle");

  (*object)
    .add_vertex({left, bottom, 0.0f})
    .add_vertex({right, bottom, 0.0f})
    .add_vertex({w_mid, top, 0.0f})
    .add_index(0)
    .add_index(1)
    .add_index(2)
    .load();

  return object;
}
