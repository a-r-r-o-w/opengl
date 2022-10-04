#ifndef HEADER_OBJECT_H
#define HEADER_OBJECT_H

#include <string>
#include <memory>

#include "vertex/vertex.hpp"

namespace gl {

  class object {
    private:
      std::string m_name;
      std::vector <glm::vec3> m_vertices;
      std::vector <u32> m_indices;

      glm::vec3 m_velocity;
      glm::vec3 m_rotation_angles;
      f32 m_blend;

      glm::mat4 m_translate;
      glm::mat4 m_rotate;
      glm::mat4 m_scale;
      
      vertex_array m_vertex_array;
      vertex_buffer_layout m_vertex_buffer_layout;
      std::unique_ptr <index_buffer> m_index_buffer;
      std::unique_ptr <vertex_buffer> m_vertex_buffer;

    public:
      bool m_should_render;
    
    public:
      object (const std::string&);
      ~object ();

      object& add_vertex (const glm::vec3&, const glm::vec3&);
      object& add_index  (u32);

      object& clear ();
      object& load ();

      object& translate (const glm::vec3&);
      object& rotate (f32, const glm::vec3&);
      object& scale (const glm::vec3&);

      object& set_velocity (const glm::vec3&);
      object& set_rotation_angles (const glm::vec3&);
      object& set_render (bool);
      object& set_blend (f32);

      const glm::vec3& get_velocity () const;
      const glm::vec3& get_rotation_angles () const;

      const glm::mat4& get_translate () const;
      const glm::mat4& get_rotate () const;
      const glm::mat4& get_scale () const;

      glm::mat4 get_model () const;
      f32 get_blend () const;
      const std::vector <glm::vec3>& get_vertices () const;
      std::vector <glm::vec3>& get_vertices ();
      const std::vector <u32>& get_indices () const;

      const vertex_array& get_vertex_array () const;
      const index_buffer& get_index_buffer () const;

      const std::string& get_name () const;
  };

} // namespace gl

#endif // HEADER_OBJECT_H
