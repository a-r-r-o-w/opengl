#ifndef HEADER_APPLICATION_H
#define HEADER_APPLICATION_H

#include <string>
#include <functional>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "types.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "object.hpp"
#include "camera.hpp"

namespace gl {

  using namespace types;

  class application : public renderer {
    private:
      u32 m_width;
      u32 m_height;
      u32 m_depth;
      std::string m_name;
      GLFWwindow* m_window;
      bool m_running;

      std::unique_ptr <shader_program> m_shader_program;

      camera m_camera;
      f32 m_delta_time;
      f32 m_last_frame;

      bool m_should_camera_move;
      bool m_first_mouse;
      f32 m_last_mouse_x;
      f32 m_last_mouse_y;

      static constexpr f32 m_grid_expanse_factor = 5.0f;
      std::unique_ptr <object> m_grid;
      f32 m_grid_spacing;
      
      bool m_display_grid;
      bool m_display_outline;
      bool m_display_wireframe;
      bool m_display_depth_test;
      bool m_display_smooth_lines;

      // 348 is the maximum value of a GLFW_KEY_<XXXX>
      static constexpr u32 m_key_count = 349;
      std::vector <bool> m_key_pressed;
    
    public:
      std::vector <std::unique_ptr <scene>> m_scenes;
      i32 m_scene_index;
      i32 m_selected_object_index;
      glm::vec3 m_cached_velocity;
      glm::vec3 m_cached_rotation_angles;

    public:
      application (u32, u32, u32, const std::string&);
      ~application ();

      void clear () const;

      void run ();

      void on_resize (i32, i32);
      void on_keypress (i32, i32, i32, i32);
      void on_mouseclick (i32, i32, i32);
      void on_mousemove (f64, f64);
      void on_mousescroll (f64, f64);
      void on_update ();

      u32 get_width () const;
      u32 get_height () const;
      u32 get_depth () const;
    
    private:
      void imgui_update ();
      void keypress_update ();

      void set_callbacks ();
      void set_shaders ();

      void create_grid ();
    
    public:
      void initialise_demo ();
  };

} // namespace gl

#endif // HEADER_APPLICATION_H
