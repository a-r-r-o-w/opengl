#ifndef HEADER_SCENE_HPP
#define HEADER_SCENE_HPP

#include <memory>

#include "types.hpp"
#include "camera.hpp"
#include "object.hpp"

namespace gl {

  using namespace gl::types;

  class scene {
    private:
      std::string m_name;
      std::vector <std::unique_ptr <object>> m_objects;

    public:
      scene (const std::string&);
      ~scene ();

      void add_object (std::unique_ptr <object>&&);

      const std::vector <std::unique_ptr <object>>& get_objects () const;
      const std::string& get_name () const;
  };

} // namespace gl

#endif // HEADER_SCENE_HPP
