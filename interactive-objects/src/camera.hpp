#ifndef HEADER_CAMERA_HPP
#define HEADER_CAMERA_HPP

#include <glm/glm.hpp>

#include "types.hpp"

namespace gl {

  using namespace gl::types;

  namespace camera_defaults {

    const f32 yaw = -90.0f;
    const f32 pitch = 0.0f;
    const f32 speed = 1000.0f;
    const f32 sensitivity = 0.01f;
    const f32 zoom = 45.0f;

  } // namespace camera_defaults

  enum class camera_movement {
    front,
    back,
    left,
    right
  };

  class camera {
    private:
      glm::vec3 m_position;
      glm::vec3 m_front;
      glm::vec3 m_up;
      glm::vec3 m_right;
      glm::vec3 m_world_up;

      f32 m_yaw;
      f32 m_pitch;
      f32 m_speed;
      f32 m_sensitivity;
      f32 m_zoom;

    public:
      camera (
        const glm::vec3& = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& = glm::vec3(0.0f, 1.0f, 0.0f),
        f32 = camera_defaults::yaw, f32 = camera_defaults::pitch
      );
      ~camera();

      void on_keypress (const camera_movement&, f32);
      void on_mousemove (f32, f32);
      void on_mousescroll (f32, f32);

      f32 get_fov () const;
      glm::mat4 get_view () const;
    
    private:
      void update_camera ();
  };

} // namespace gl

#endif // HEADER_CAMERA_HPP
