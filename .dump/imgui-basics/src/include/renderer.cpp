#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer.hpp"

namespace gl {

  renderer::renderer ()
    : m_clear_color (0.0f, 0.0f, 0.0f, 1.0f) {

  }

  renderer::~renderer () {

  }

  void renderer::clear () const {
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void renderer::draw (const vertex_array& va, const index_buffer& ib, const shader_program& s) const {
    s.bind();
    va.bind();
    ib.bind();

    glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, static_cast <const void*> (0));
  }

  const glm::vec4& renderer::get_clear_color () const {
    return m_clear_color;
  }
  
  void renderer::set_clear_color (const glm::vec4& clear_color) {
    m_clear_color = clear_color;
  }

  void renderer::set_view_port (i32 x, i32 y, i32 width, i32 height) const {
    glViewport(x, y, width, height);
  }

} // namespace gl
