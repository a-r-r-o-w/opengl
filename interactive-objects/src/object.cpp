#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "object.hpp"
#include <iostream>
namespace gl {

  object::object (const std::string &name)
    : m_name (name),
      m_vertices (),
      m_indices (),
      m_velocity (0.0f),
      m_rotation_angles (0.0f, 0.0f, 0.0f),
      m_blend (1.0f),
      m_translate(glm::mat4(1.0f)),
      m_rotate(glm::mat4(1.0f)),
      m_scale(glm::mat4(1.0f)),
      m_vertex_array (),
      m_vertex_buffer_layout (),
      m_index_buffer (nullptr),
      m_vertex_buffer (nullptr),
      m_should_render (true)
  {
    m_vertex_buffer_layout.push <f32> (3); // vertex
    m_vertex_buffer_layout.push <f32> (3); // color
  }

  object::~object () {

  }

  object& object::add_vertex (const glm::vec3 &vertex, const glm::vec3 &color) {
    m_vertices.push_back(vertex);
    m_vertices.push_back(color);
    return *this;
  }

  object& object::add_index (u32 index) {
    m_indices.push_back(index);
    return *this;
  }

  object& object::clear () {
    m_vertices.clear();
    m_indices.clear();
    m_translate = glm::mat4(1.0f);
    m_rotate = glm::mat4(1.0f);
    m_scale = glm::mat4(1.0f);
    m_index_buffer.reset(nullptr);
    m_vertex_buffer.reset(nullptr);
    return *this;
  }

  object& object::load () {
    m_index_buffer.reset(new index_buffer(m_indices.data(), m_indices.size()));
    m_vertex_buffer.reset(new vertex_buffer(m_vertices.data(), 3 * sizeof(f32) * m_vertices.size()));
    m_vertex_array.add_buffer(*m_vertex_buffer, m_vertex_buffer_layout);
    return *this;
  }

  object& object::translate (const glm::vec3& t) {
    m_translate = glm::translate(m_translate, t);
    return *this;
  }

  object& object::rotate (f32 angle, const glm::vec3& r) {
    m_rotate = glm::rotate(m_rotate, glm::radians(angle), r);
    return *this;
  }

  object& object::scale (const glm::vec3& s) {
    m_scale = glm::scale(m_scale, s);
    return *this;
  }

  object& object::set_velocity (const glm::vec3& v) {
    m_velocity = v;
    return *this;
  }

  object& object::set_rotation_angles (const glm::vec3& v) {
    m_rotation_angles = v;
    return *this;
  }

  object& object::set_render (bool should_render) {
    m_should_render = should_render;
    return *this;
  }

  object& object::set_blend (f32 blend) {
    m_blend = blend;
    return *this;
  }

  const glm::vec3& object::get_velocity () const {
    return m_velocity;
  }

  const glm::vec3& object::get_rotation_angles () const {
    return m_rotation_angles;
  }

  const glm::mat4& object::get_translate () const {
    return m_translate;
  }

  const glm::mat4& object::get_rotate () const {
    return m_rotate;
  }

  const glm::mat4& object::get_scale () const {
    return m_scale;
  }

  glm::mat4 object::get_model () const {
    return m_translate * m_rotate * m_scale;
  }

  f32 object::get_blend () const {
    return m_blend;
  }

  const std::vector <glm::vec3>& object::get_vertices () const {
    return m_vertices;
  }

  std::vector <glm::vec3>& object::get_vertices () {
    return m_vertices;
  }

  const std::vector <u32>& object::get_indices () const {
    return m_indices;
  }

  const vertex_array& object::get_vertex_array () const {
    return m_vertex_array;
  }

  const index_buffer& object::get_index_buffer () const {
    return *m_index_buffer;
  }

  const std::string& object::get_name () const {
    return m_name;
  }

} // namespace gl
