#ifndef HEADER_APPLICATION_H
#define HEADER_APPLICATION_H

#include <string>
#include <functional>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "types.hpp"
#include "renderer.hpp"
#include "object.hpp"

namespace gl {

  using namespace types;

  class application : public renderer {
    private:
      u32 m_width;
      u32 m_height;
      std::string m_name;
      GLFWwindow* m_window;
      bool m_running;

      std::unique_ptr <shader_program> m_shader_program;
    
    public:
      std::vector <std::unique_ptr <object>> m_objects;

    public:
      application (u32, u32, const std::string&);
      ~application ();

      void run ();

      void on_resize (i32, i32);
      void on_keypress (i32, i32, i32, i32);
      void on_update ();

      // void add_object (const object&);
      // void add_object (std::unique_ptr <object>&&);

      u32 get_width () const;
      u32 get_height () const;
    
    private:
      void set_callbacks ();
  };

} // namespace gl

#endif // HEADER_APPLICATION_H
