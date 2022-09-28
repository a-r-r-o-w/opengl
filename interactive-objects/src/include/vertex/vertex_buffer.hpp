#ifndef HEADER_VERTEX_BUFFER_H
#define HEADER_VERTEX_BUFFER_H

#include "types.hpp"

namespace gl {

  using namespace gl::types;

  class vertex_buffer {
    private:
      u32 m_id;

    public:
      vertex_buffer (const void*, u32);
      ~vertex_buffer ();

      void bind () const;
      void unbind () const;
  };

} // namespace gl

#endif // HEADER_VERTEX_BUFFER_H
