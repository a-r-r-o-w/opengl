#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "vertex_buffer.hpp"

namespace gl {

  vertex_buffer::vertex_buffer (const void *data, u32 size)
    : m_id (0) {
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
  }

  vertex_buffer::~vertex_buffer () {
    glDeleteBuffers(1, &m_id);
  }

  void vertex_buffer::bind () const {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
  }

  void vertex_buffer::unbind () const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

} // namespace gl
