#ifndef HEADER_UTILS_HPP
#define HEADER_UTILS_HPP

#include <fstream>
#include <string>
#include <sstream>
#include <memory>

#include <glm/glm.hpp>

#include "types.hpp"
#include "object.hpp"

using namespace gl::types;

std::unique_ptr <gl::object> load_blender_obj (const std::string &filepath, const std::string &name) {
  std::ifstream file (filepath);
  std::string line;
  std::stringstream stream;

  auto object = std::make_unique <gl::object> (name.c_str());

  while (not file.eof()) {
    std::getline(file, line);
    
    if (line.starts_with("v ")) {
      stream << line.substr(2);
      
      glm::vec3 v;
      stream >> v.x >> v.y >> v.z;
      
      object->add_vertex(v);
    }
    else if (line.starts_with("f ")) {
      i32 x, y, z;

      stream << line.substr(2);
      
      stream >> line;
      x = std::stoi(line.substr(0, line.find_first_of('/'))) - 1;

      stream >> line;
      y = std::stoi(line.substr(0, line.find_first_of('/'))) - 1;

      stream >> line;
      z = std::stoi(line.substr(0, line.find_first_of('/'))) - 1;

      (*object)
        .add_index(x)
        .add_index(y)
        .add_index(z);
    }

    stream.clear();
  }

  file.close();
  object->load();

  return object;
}

#endif // HEADER_UTILS_HPP
