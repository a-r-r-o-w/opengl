#include <glm/glm.hpp>

#include "object.hpp"

namespace gl {

  object::object (const std::string &name)
    : m_name (name),
      m_vertices (),
      m_indices (),
      m_vertex_array (),
      m_vertex_buffer_layout (),
      m_index_buffer (nullptr),
      m_vertex_buffer (nullptr)
  { }

  object::~object () {

  }

  object& object::add_vertex (const glm::vec3 &v) {
    m_vertices.push_back(v.x);
    m_vertices.push_back(v.y);
    m_vertices.push_back(v.z);
    return *this;
  }

  object& object::add_index (u32 index) {
    m_indices.push_back(index);
    return *this;
  }

  void object::load () {
    m_index_buffer = std::make_unique <index_buffer> (m_indices.data(), m_indices.size());
    m_vertex_buffer = std::make_unique <vertex_buffer> (m_vertices.data(), 3 * sizeof(f32) * m_indices.size());
    m_vertex_buffer_layout.push <f32> (3);
    m_vertex_array.add_buffer(*m_vertex_buffer, m_vertex_buffer_layout);
  }

  const vertex_array& object::get_vertex_array () const {
    return m_vertex_array;
  }

  const index_buffer& object::get_index_buffer () const {
    return *m_index_buffer;
  }

} // namespace gl
