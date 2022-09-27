#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer.hpp"

namespace gl {

  renderer::renderer ()
    : m_clear_color (0.0f, 0.0f, 0.0f, 1.0f),
      m_draw_mode (draw_mode::triangle) {

  }

  renderer::~renderer () {

  }

  void renderer::clear () const {
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void renderer::draw_elements (const vertex_array &va, const index_buffer &ib, const shader_program &s) const {
    s.bind();
    va.bind();
    ib.bind();

    glDrawElements(static_cast <u32> (m_draw_mode), ib.get_count(), GL_UNSIGNED_INT, static_cast <const void*> (0));
  }

  const glm::vec4& renderer::get_clear_color () const {
    return m_clear_color;
  }

  const draw_mode& renderer::get_draw_mode () const {
    return m_draw_mode;
  }
  
  void renderer::set_clear_color (const glm::vec4 &clear_color) {
    m_clear_color = clear_color;
  }

  void renderer::set_draw_mode (const draw_mode &draw_mode) {
    m_draw_mode = draw_mode;
  }

  void renderer::set_view_port (i32 x, i32 y, i32 width, i32 height) const {
    glViewport(x, y, width, height);
  }

} // namespace gl
