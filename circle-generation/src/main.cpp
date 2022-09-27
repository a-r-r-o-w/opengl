#include <iostream>
#include <thread>
#include <numeric>
#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "vertex/vertex.hpp"
#include "shader/shader.hpp"
#include "renderer.hpp"

using namespace gl::types;

namespace globals {
  i32 width = 800;
  i32 height = 600;

  glm::vec4 draw_color (0.8f, 0.2f, 0.2f, 1.0f);

  std::vector <glm::vec3> vertices;
  std::vector <u32> indices;

  glm::mat4 projection = glm::ortho((f32)-width, (f32)width, (f32)-height, (f32)height, -5.0f, 5.0f);
  glm::mat4 view (1.0f);
  glm::vec3 translation (0, 0, 0);
}

glm::vec3 rotate_point (glm::vec3 center, glm::vec3 point, f32 angle) {
  f32 s = glm::sin(angle);
  f32 c = glm::cos(angle);

  point.x -= center.x;
  point.y -= center.y;

  f32 xnew = point.x * c - point.y * s;
  f32 ynew = point.x * s + point.y * c;

  point.x = xnew + center.x;
  point.y = ynew + center.y;

  return point;
}

void generate_verticies (i32 points) {
  using namespace globals;

  vertices.clear();
  indices.clear();

  f32 angle = (360.0f * glm::pi <float> ()) / (180.0f * points);
  
  vertices.push_back({0, 0, 0});
  vertices.push_back({(f32)globals::width / 2, 0, 0});

  for (int i = 0; i < points; ++i) {
    vertices.push_back(rotate_point({0, 0, 0}, vertices.back(), angle));
    indices.push_back(i);
    indices.push_back(i + 1);
    indices.push_back(0);
  }

  indices.push_back(vertices.size() - 2);
  indices.push_back(vertices.size() - 1);
  indices.push_back(0);
}

void framebuffer_size_callback ([[maybe_unused]] GLFWwindow *window, i32 width, i32 height) {
  globals::width = width;
  globals::height = height;
  globals::projection = glm::ortho((f32)-globals::width, (f32)globals::width, (f32)-globals::height, (f32)globals::height, -5.0f, 5.0f);
  glViewport(0, 0, width, height);
};

int main () {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window = glfwCreateWindow(globals::width, globals::height, "ImGUI Basics", nullptr, nullptr);
  
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window!" << std::endl;
    glfwTerminate();
    return -1;
  }
  
  glfwMakeContextCurrent(window);
  
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD!" << std::endl;
    return -1;
  }
  
  auto process_input = [] (GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
  };

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwSwapInterval(1);

  gl::renderer renderer;

  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  {
    int n = 5;

    gl::shader_program shader_program;
    gl::shader vertex_shader (gl::shader_type::vertex, "../src/shaders/vertex.shader.glsl");
    gl::shader fragment_shader (gl::shader_type::fragment, "../src/shaders/fragment.shader.glsl");

    shader_program.add_shader(vertex_shader);
    shader_program.add_shader(fragment_shader);
    shader_program.link();
    
    shader_program.bind();
    shader_program.set_uniform("u_color", globals::draw_color);

    renderer.set_draw_mode(gl::draw_mode::triangle);
    
    glLineWidth(3.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      process_input(window);

      renderer.clear();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      generate_verticies(n);

      gl::vertex_array va;
      gl::vertex_buffer_layout layout;
      gl::vertex_buffer vb (globals::vertices.data(), 3 * sizeof(f32) * globals::vertices.size());
      gl::index_buffer ib (globals::indices.data(), globals::indices.size());

      layout.push <f32> (3);
      va.add_buffer(vb, layout);

      glm::mat4 model = glm::translate(glm::mat4(1.0f), globals::translation);
      glm::mat4 mvp = globals::projection * globals::view * model;

      shader_program.set_uniform("u_MVP", mvp);

      {
        ImGui::Begin("Controls");
        ImGui::SliderFloat("translation_x", &globals::translation.x, -1.0f, 1.0f);
        ImGui::SliderFloat("translation_y", &globals::translation.y, -1.0f, 1.0f);
        ImGui::SliderInt("count", &n, 1, 100);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
      }

      renderer.draw_elements(va, ib, shader_program);
      ib.unbind();
      vb.unbind();
      va.unbind();

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      glfwSwapBuffers(window);
    }

    shader_program.unbind();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);

  glfwTerminate();

  return 0;
}
