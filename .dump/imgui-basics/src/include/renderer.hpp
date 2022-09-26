#ifndef HEADER_RENDERER_H
#define HEADER_RENDERER_H

#include <glm/glm.hpp>

#include "vertex/vertex_array.hpp"
#include "vertex/index_buffer.hpp"
#include "shader/shader.hpp"

namespace gl {

  class renderer {
    private:
      glm::vec4 m_clear_color;

    public:
      renderer ();
      ~renderer ();

      void clear () const;
      void draw (const vertex_array&, const index_buffer&, const shader_program&) const;

      const glm::vec4& get_clear_color () const;
      void set_clear_color (const glm::vec4&);
      void set_view_port (i32, i32, i32, i32) const;
  };

} // namespace gl

#endif // HEADER_RENDERER_H
