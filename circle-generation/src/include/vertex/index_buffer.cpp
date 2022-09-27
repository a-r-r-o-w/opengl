#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "index_buffer.hpp"

namespace gl {

  index_buffer::index_buffer (const u32 *data, u32 count)
    : m_id (0),
      m_count (count) {
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(u32), data, GL_STATIC_DRAW);
  }

  index_buffer::~index_buffer () {
    glDeleteBuffers(1, &m_id);
  }

  void index_buffer::bind () const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
  }

  void index_buffer::unbind () const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  u32 index_buffer::get_count () const {
    return m_count;
  }

} // namespace gl
