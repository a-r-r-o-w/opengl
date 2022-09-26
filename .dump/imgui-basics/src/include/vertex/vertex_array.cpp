#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "vertex_array.hpp"

namespace gl {

  vertex_array::vertex_array ()
    : m_id (0) {
    glGenVertexArrays(1, &m_id);
  }

  vertex_array::~vertex_array () {
    glDeleteVertexArrays(1, &m_id);
  }

  void vertex_array::bind () const {
    glBindVertexArray(m_id);
  }

  void vertex_array::unbind () const {
    glBindVertexArray(0);
  }

  void vertex_array::add_buffer (const vertex_buffer& vb, const vertex_buffer_layout& layout) {
    bind();
    vb.bind();

    const auto& elements = layout.get_elements();

    for (u32 i = 0, offset = 0; i < elements.size(); ++i) {
      const auto &element = elements[i];
      
      glEnableVertexAttribArray(i);
      glVertexAttribPointer(
        i, element.get_count(), element.get_type(), element.get_normalised(),
        layout.get_stride(), (const void*)static_cast <std::uintptr_t> (offset)
      );
      
      offset += vertex_buffer_element::get_size_of_type(element.get_type()) * element.get_count();
    }
  }

} // namespace gl
