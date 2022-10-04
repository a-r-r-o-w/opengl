#ifndef HEADER_SCENE_HPP
#define HEADER_SCENE_HPP

#include <memory>

#include "types.hpp"
#include "camera.hpp"
#include "object.hpp"

namespace gl {

  using namespace gl::types;

  struct scene_properties {
    f32 m_left_bound;
    f32 m_right_bound;
    f32 m_up_bound;
    f32 m_down_bound;
    f32 m_front_bound;
    f32 m_back_bound;

    scene_properties (f32 = 0, f32 = 0, f32 = 0, f32 = 0, f32 = 0, f32 = 0);
    ~scene_properties ();
  };

  class scene {
    private:
      std::string m_name;
      scene_properties m_scene_properties;
      u32 m_object_count;
      std::vector <std::unique_ptr <object>> m_objects;

    public:
      scene (const std::string&, const scene_properties&);
      ~scene ();

      void add_object (std::unique_ptr <object>&&);

      void on_update (f32);

      const scene_properties& get_properties () const;
      const std::vector <std::unique_ptr <object>>& get_objects () const;
      const std::string& get_name () const;
  };

} // namespace gl

#endif // HEADER_SCENE_HPP
