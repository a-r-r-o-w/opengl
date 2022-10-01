#ifndef HEADER_SCENE_HPP
#define HEADER_SCENE_HPP

#include "types.hpp"
#include "camera.hpp"
#include "object.hpp"

namespace gl {

  using namespace gl::types;

  class scene {
    private:
      std::vector <object> m_objects;
      camera m_camera;

    public:
      scene ();
      ~scene ();
  };

} // namespace gl

#endif // HEADER_SCENE_HPP
