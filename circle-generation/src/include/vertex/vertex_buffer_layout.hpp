#ifndef HEADER_VERTEX_BUFFER_LAYOUT_H
#define HEADER_VERTEX_BUFFER_LAYOUT_H

#include <vector>

#include "types.hpp"
#include "vertex_buffer.hpp"

namespace gl {

  using namespace gl::types;

  class vertex_buffer_element {
    private:
      u32 m_type;
      u32 m_count;
      u32 m_normalised;
    
    public:
      vertex_buffer_element (u32, u32, bool);
      ~vertex_buffer_element ();
    
      static u32 get_size_of_type (u32);

      u32 get_type () const;
      u32 get_count () const;
      u32 get_normalised () const;
  };

  class vertex_buffer_layout {
    private:
      std::vector <vertex_buffer_element> m_elements;
      u32 m_stride;

    public:
      vertex_buffer_layout ();
      ~vertex_buffer_layout ();

      template <typename T>
      void push (u32);

      const std::vector <vertex_buffer_element>& get_elements () const;
      u32 get_stride () const;
  };

} // namespace gl

#endif // HEADER_VERTEX_BUFFER_LAYOUT_H
