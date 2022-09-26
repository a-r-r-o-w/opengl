#ifndef HEADER_SHADER_H
#define HEADER_SHADER_H

#include <string>
#include <unordered_map>

#include "types.hpp"

namespace gl {

  using namespace gl::types;

  class shader {
    private:
      u32 m_type;
      u32 m_id;
      std::string m_filepath;
    
    public:
      shader (u32, const std::string&);
      ~shader ();

      u32 get_type () const;
      u32 get_id () const;
  };

  class shader_program {
    private:
      u32 m_id;
      std::unordered_map <std::string, int> m_uniform_location;
    
    public:
      shader_program ();
      ~shader_program ();

      void bind () const;
      void unbind () const;

      void add_shader (const shader&) const;
      void link () const;

      i32 get_uniform_location (const std::string&);

      template <typename T>
      void set_uniform (const std::string&, const T&);
  };

} // namespace gl

#endif // HEADER_SHADER_H
