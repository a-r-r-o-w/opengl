#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"

namespace gl {

  camera::camera (
    const glm::vec3 &position, const glm::vec3 &world_up,
    f32 yaw, f32 pitch, f32 roll
  )
    : m_position (position),
      m_front (glm::vec3(0.0f, 0.0f, -1.0f)),
      m_world_up (world_up),
      m_yaw (yaw),
      m_pitch (pitch),
      m_roll (roll),
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
      
      case camera_movement::up:
        m_position += m_up * velocity;
        break;
      
      case camera_movement::down:
        m_position -= m_up * velocity;
        break;
      
      case camera_movement::left_roll:
        m_roll += 1.0f;
        if (m_roll >= 360.0f)
          m_roll -= 360.0f;
        m_world_up = glm::normalize(glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(m_roll), m_front)) * glm::vec3(0, 1, 0));
        break;

      case camera_movement::right_roll:
        m_roll -= 1.0f;
        if (m_roll <= -360.0f)
          m_roll += 360.0f;
        m_world_up = glm::normalize(glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(m_roll), m_front)) * glm::vec3(0, 1, 0));
        break;
      
      default:
        std::cerr << "Invalid camera direction" << std::endl;
        break;
    }

    update_camera();
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
    
    if (m_zoom < 10.0f)
      m_zoom = 10.0f;
    if (m_zoom > 60.0f)
      m_zoom = 60.0f;
  }

  f32 camera::get_fov () const {
    return m_zoom;
  }

  const glm::vec3& camera::get_position () const {
    return m_position;
  }
  
  glm::vec3& camera::get_position () {
    return m_position;
  }

  f32 camera::get_yaw () const {
    return m_yaw;
  }

  f32 camera::get_pitch () const {
    return m_pitch;
  }

  f32 camera::get_roll () const {
    return m_roll;
  }

  glm::mat4 camera::get_projection (f32 aspect_ratio, f32 z_near, f32 z_far) const {
    return glm::perspective(glm::radians(m_zoom), aspect_ratio, z_near, z_far);
  }

  glm::mat4 camera::get_view () const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
  }

  void camera::set_fov (f32 zoom) {
    m_zoom = zoom;
  }

  void camera::set_position (const glm::vec3 &position) {
    m_position = position;
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
