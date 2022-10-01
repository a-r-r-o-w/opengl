#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "object.hpp"

namespace gl {

  object::object (const std::string &name)
    : m_name (name),
      m_vertices (),
      m_indices (),
      m_translate(glm::mat4(1.0f)),
      m_rotate(glm::mat4(1.0f)),
      m_scale(glm::mat4(1.0f)),
      m_vertex_array (),
      m_vertex_buffer_layout (),
      m_index_buffer (nullptr),
      m_vertex_buffer (nullptr)
  { }

  object::~object () {

  }

  object& object::add_vertex (const glm::vec3 &v) {
    m_vertices.push_back(v);
    return *this;
  }

  object& object::add_index (u32 index) {
    m_indices.push_back(index);
    return *this;
  }

  void object::load () {
    m_index_buffer = std::make_unique <index_buffer> (m_indices.data(), m_indices.size());
    m_vertex_buffer = std::make_unique <vertex_buffer> (m_vertices.data(), 3 * sizeof(f32) * m_vertices.size());
    m_vertex_buffer_layout.push <f32> (3);
    m_vertex_array.add_buffer(*m_vertex_buffer, m_vertex_buffer_layout);
  }

  void object::translate (const glm::vec3& t) {
    m_translate = glm::translate(m_translate, t);
  }

  void object::scale (const glm::vec3& s) {
    m_scale = glm::scale(m_scale, s);
  }

  glm::mat4 object::get_model () const {
    return m_translate * m_rotate * m_scale;
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
