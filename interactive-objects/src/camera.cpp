#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"

namespace gl {

  camera::camera (
    const glm::vec3 &position, const glm::vec3 &world_up,
    f32 yaw, f32 pitch
  )
    : m_position (position),
      m_front (glm::vec3(0.0f, 0.0f, -1.0f)),
      m_world_up (world_up),
      m_yaw (yaw),
      m_pitch (pitch),
      m_speed (camera_defaults::speed),
      m_sensitivity (camera_defaults::sensitivity),
      m_zoom (camera_defaults::zoom) {
    update_camera();
  }

  camera::~camera () {

  }

  void camera::on_keypress (const camera_movement& movement, f32 deltatime) {
    f32 velocity = m_speed * deltatime;

    switch (movement) {
      case camera_movement::front:
        m_position += m_front * velocity;
        break;
      
      case camera_movement::back:
        m_position -= m_front * velocity;
        break;
      
      case camera_movement::left:
        m_position += m_right * velocity;
        break;

      case camera_movement::right:
        m_position -= m_right * velocity;
        break;
      
      default:
        std::cerr << "Invalid camera direction" << std::endl;
        break;
    }
  }

  void camera::on_mousemove (f32 x_offset, f32 y_offset) {
    x_offset *= m_sensitivity;
    y_offset *= m_sensitivity;

    m_yaw += x_offset;
    m_pitch += y_offset;

    if (m_pitch > 89.0f)
      m_pitch = 89.0f;
    if (m_pitch < -89.0f)
      m_pitch = -89.0f;
    
    update_camera();
  }

  void camera::on_mousescroll (f32 x_offset, f32 y_offset) {
    m_zoom -= y_offset;
    
    if (m_zoom < 1.0f)
      m_zoom = 1.0f;
    if (m_zoom > 45.0f)
      m_zoom = 45.0f;
  }

  f32 camera::get_fov () const {
    return m_zoom;
  }

  glm::mat4 camera::get_view () const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
  }

  void camera::update_camera () {
    m_front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    m_front.y = glm::sin(glm::radians(m_pitch));
    m_front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));

    m_front = glm::normalize(m_front);
    m_right = glm::normalize(glm::cross(m_front, m_world_up));
    m_up = glm::normalize(glm::cross(m_right, m_front));
  }

} // namespace gl
