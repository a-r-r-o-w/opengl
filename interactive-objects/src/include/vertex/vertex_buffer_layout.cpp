#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "vertex_buffer_layout.hpp"

namespace gl {

  vertex_buffer_element::vertex_buffer_element (u32 type, u32 count, bool normalised)
    : m_type (type),
      m_count (count),
      m_normalised (normalised)
  { }

  vertex_buffer_element::~vertex_buffer_element ()
  { }

  u32 vertex_buffer_element::get_size_of_type (u32 type) {
    switch (type) {
      case GL_FLOAT:         return sizeof(f32);
      case GL_UNSIGNED_INT:  return sizeof(u32);
      case GL_UNSIGNED_BYTE: return sizeof(byte);
    }

    // TODO: add exception if not these types
    return 0;
  }

  u32 vertex_buffer_element::get_type () const {
    return m_type;
  }

  u32 vertex_buffer_element::get_count () const {
    return m_count;
  }

  u32 vertex_buffer_element::get_normalised () const {
    return m_normalised;
  }

  vertex_buffer_layout::vertex_buffer_layout () 
    : m_elements (),
      m_stride (0)
  { }

  vertex_buffer_layout::~vertex_buffer_layout ()
  { }

  template <typename T>
  void vertex_buffer_layout::push (u32 count) {
    // TODO: add exception for undefined implementation
  }

  template <>
  void vertex_buffer_layout::push <f32> (u32 count) {
    m_elements.push_back({GL_FLOAT, count, GL_FALSE});
    m_stride += count * vertex_buffer_element::get_size_of_type(GL_FLOAT);
  }

  template <>
  void vertex_buffer_layout::push <u32> (u32 count) {
    m_elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    m_stride += count * vertex_buffer_element::get_size_of_type(GL_UNSIGNED_INT);
  }

  template <>
  void vertex_buffer_layout::push <byte> (u32 count) {
    m_elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    m_stride += count * vertex_buffer_element::get_size_of_type(GL_UNSIGNED_BYTE);
  }

  const std::vector <vertex_buffer_element>& vertex_buffer_layout::get_elements () const {
    return m_elements;
  }

  u32 vertex_buffer_layout::get_stride () const {
    return m_stride;
  }

} // namespace gl
