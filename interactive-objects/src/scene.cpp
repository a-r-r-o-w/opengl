#include "scene.hpp"
#include <iostream>
namespace gl {

  scene_properties::scene_properties (f32 l, f32 r, f32 u, f32 d, f32 f, f32 b)
    : m_left_bound (l),
      m_right_bound (r),
      m_up_bound (u),
      m_down_bound (d),
      m_front_bound (f),
      m_back_bound (b) {

  }

  scene_properties::~scene_properties () {
    
  }

  scene::scene (const std::string &name, const scene_properties& properties)
    : m_name (name),
      m_scene_properties (properties),
      m_object_count (0),
      m_objects () {

  }

  scene::~scene () {

  }

  void scene::add_object (std::unique_ptr <object> &&o) {
    m_objects.emplace_back(std::move(o));
    ++m_object_count;
  }

  void scene::on_update (f32 deltatime) {
    for (u32 i = 0; i < m_object_count; ++i) {
      auto &object = *m_objects[i];
      auto translation = glm::vec3(object.get_translate()[3]);
      auto new_velocity = object.get_velocity();
      auto &rotation_angles = object.get_rotation_angles();

      if ((translation.x > m_scene_properties.m_right_bound) or
          (translation.x < m_scene_properties.m_left_bound)) {
        new_velocity.x = -new_velocity.x;
        
        if (translation.x > m_scene_properties.m_right_bound)
          translation.x = m_scene_properties.m_right_bound;
        else if (translation.x < m_scene_properties.m_left_bound)
          translation.x = m_scene_properties.m_left_bound;
      }
      
      if ((translation.y > m_scene_properties.m_up_bound) or
          (translation.y < m_scene_properties.m_down_bound)) {
        new_velocity.y = -new_velocity.y;

        if (translation.y > m_scene_properties.m_up_bound)
          translation.y = m_scene_properties.m_up_bound;
        else if (translation.y < m_scene_properties.m_down_bound)
          translation.y = m_scene_properties.m_down_bound;
      }
      
      if ((translation.z > m_scene_properties.m_front_bound) or
          (translation.z < m_scene_properties.m_back_bound)) {
        new_velocity.z = -new_velocity.z;

        if (translation.y > m_scene_properties.m_front_bound)
          translation.z = m_scene_properties.m_front_bound;
        else if (translation.y < m_scene_properties.m_back_bound)
          translation.z = m_scene_properties.m_back_bound;
      }
      
      object.set_velocity(new_velocity);
      object.translate(new_velocity * deltatime);
      object.rotate(rotation_angles.x, {1, 0, 0});
      object.rotate(rotation_angles.y, {0, 1, 0});
      object.rotate(rotation_angles.z, {0, 0, 1});
    }
  }
  
  const scene_properties& scene::get_properties () const {
    return m_scene_properties;  
  }

  const std::vector <std::unique_ptr <object>>& scene::get_objects () const {
    return m_objects;
  }

  const std::string& scene::get_name () const {
    return m_name;
  }

} // namespace gl
