#ifndef HEADER_INDEX_BUFFER_H
#define HEADER_INDEX_BUFFER_H

#include "types.hpp"

namespace gl {

  using namespace gl::types;

  class index_buffer {
    private:
      u32 m_id;
      u32 m_count;

    public:
      index_buffer (const u32*, u32);
      ~index_buffer ();

      void bind () const;
      void unbind () const;

      u32 get_count () const;
  };

} // namespace gl

#endif // HEADER_INDEX_BUFFER_H
