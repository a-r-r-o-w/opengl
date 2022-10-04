#ifndef HEADER_VERTEX_ARRAY_H
#define HEADER_VERTEX_ARRAY_H

#include "types.hpp"
#include "vertex_buffer.hpp"
#include "vertex_buffer_layout.hpp"

namespace gl {

  using namespace gl::types;

  class vertex_array {
    private:
      u32 m_id;

    public:
      vertex_array ();
      ~vertex_array ();

      void bind () const;
      void unbind () const;

      void add_buffer (const vertex_buffer&, const vertex_buffer_layout&);
  };

} // namespace gl

#endif // HEADER_VERTEX_ARRAY_H
