#ifndef HEADER_OBJECT_H
#define HEADER_OBJECT_H

#include <string>
#include <memory>

#include "vertex/vertex.hpp"

namespace gl {

  class object {
    private:
      std::string m_name;
      std::vector <f32> m_vertices;
      std::vector <u32> m_indices;
      
      vertex_array m_vertex_array;
      vertex_buffer_layout m_vertex_buffer_layout;
      std::unique_ptr <index_buffer> m_index_buffer;
      std::unique_ptr <vertex_buffer> m_vertex_buffer;
    
    public:
      object (const std::string&);
      ~object ();

      object& add_vertex (const glm::vec3&);
      object& add_index  (u32);

      void load ();

      const vertex_array& get_vertex_array () const;
      const index_buffer& get_index_buffer () const;

      const std::string& get_name () const;
  };

} // namespace gl

#endif // HEADER_OBJECT_H
