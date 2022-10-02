#include "scene.hpp"

namespace gl {

  scene::scene (const std::string &name)
    : m_name (name) {

  }

  scene::~scene () {

  }

  void scene::add_object (std::unique_ptr <object> &&o) {
    m_objects.emplace_back(std::move(o));
  }

  const std::vector <std::unique_ptr <object>>& scene::get_objects () const {
    return m_objects;
  }

  const std::string& scene::get_name () const {
    return m_name;
  }

} // namespace gl
