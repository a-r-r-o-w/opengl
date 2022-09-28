#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.hpp"

namespace gl {

  shader::shader (const shader_type& type, const std::string& filepath)
    : m_type (type),
      m_id (0),
      m_filepath (filepath) {
    std::ifstream file (m_filepath.c_str());

    if (not file.is_open()) {
      std::cerr << "file (" << filepath << ") could not be opened" << std::endl;
      return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string source_code = buffer.str();
    int success;
    char infolog [512];

    const char *c = source_code.c_str();
    const char* const* x = &c;
    
    m_id = glCreateShader(static_cast <u32> (m_type));
    glShaderSource(m_id, 1, x, (const i32*)0);
    glCompileShader(m_id);
    glValidateProgram(m_id);
    
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);

    if (!success) {
      glGetShaderInfoLog(m_id, 512, nullptr, infolog);
      std::cerr << "Error compiling " << (m_type == gl::shader_type::vertex ? "vertex" : "fragment") << " shader\n" << infolog << std::endl;
    }
  }

  shader::~shader () {
    glDeleteShader(m_id);
  }

  const shader_type& shader::get_type () const {
    return m_type;
  }

  u32 shader::get_id () const {
    return m_id;
  }

  shader_program::shader_program ()
    : m_id (0) {
    m_id = glCreateProgram();
  }

  shader_program::~shader_program () {
    glDeleteProgram(m_id);
  }

  void shader_program::bind () const {
    glUseProgram(m_id);
  }

  void shader_program::unbind () const {
    glUseProgram(0);
  }

  void shader_program::add_shader (const shader& s) const {
    glAttachShader(m_id, s.get_id());
  }

  void shader_program::link () const {
    glLinkProgram(m_id);
  }

  i32 shader_program::get_uniform_location (const std::string& name) {
    auto location = m_uniform_location.find(name);

    if (location != m_uniform_location.end())
      return location->second;
    
    m_uniform_location[name] = glGetUniformLocation(m_id, name.c_str());
    return m_uniform_location[name];
  }

  template <>
  void shader_program::set_uniform <glm::vec4> (const std::string &name, const glm::vec4& value) {
    glUniform4f(get_uniform_location(name), value.x, value.y, value.z, value.w);
  }

  template <>
  void shader_program::set_uniform <glm::mat4> (const std::string &name, const glm::mat4& value) {
    glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, &value[0][0]);
  }

} // namespace gl
