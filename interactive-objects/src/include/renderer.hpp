#ifndef HEADER_RENDERER_H
#define HEADER_RENDERER_H

#include <glm/glm.hpp>

#include "vertex/vertex_array.hpp"
#include "vertex/index_buffer.hpp"
#include "shader/shader.hpp"

namespace gl {

  enum class draw_mode {
    point = GL_POINTS,

    line = GL_LINES,
    line_adjacency = GL_LINES_ADJACENCY,
    line_loop = GL_LINE_LOOP,
    line_strip = GL_LINE_STRIP,
    line_strip_adjacency = GL_LINE_STRIP_ADJACENCY,
    
    triangle = GL_TRIANGLES,
    triangle_adjacency = GL_TRIANGLES_ADJACENCY,
    triangle_fan = GL_TRIANGLE_FAN,
    triangle_strip = GL_TRIANGLE_STRIP,
    triangle_strip_adjacency = GL_TRIANGLE_STRIP_ADJACENCY
  };

  class renderer {
    private:
      glm::vec4 m_clear_color;
      draw_mode m_draw_mode;

    public:
      renderer ();
      ~renderer ();

      void clear () const;
      void draw_elements (const vertex_array&, const index_buffer&, const shader_program&) const;

      const glm::vec4& get_clear_color () const;
      const draw_mode& get_draw_mode () const;
      
      void set_clear_color (const glm::vec4&);
      void set_draw_mode (const draw_mode&);
      void set_view_port (i32, i32, i32, i32) const;
  };

} // namespace gl

#endif // HEADER_RENDERER_H
