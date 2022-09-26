#include <iostream>
#include <thread>

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

namespace globals {
  const int width = 800;
  const int height = 600;

  glm::vec4 draw_color (0.8f, 0.2f, 0.2f, 1.0f);
}

using namespace gl::types;

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

  glfwSetFramebufferSizeCallback(window, [] ([[maybe_unused]] GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
  });

  glfwSwapInterval(1);

  gl::renderer renderer;

  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  {
    const f32 length = 200.0f;

    f32 vertices[] = {
        0.0f,   0.0f, 0.0f,
        0.0f, length, 0.0f,
      length, length, 0.0f,
      length,   0.0f, 0.0f
    };

    u32 indices[] = {
      0, 1, 3,
      1, 2, 3
    };

    gl::vertex_array va;
    gl::vertex_buffer_layout layout;
    gl::vertex_buffer vb (vertices, sizeof(vertices));
    gl::index_buffer ib (indices, sizeof(indices) / sizeof(u32));

    layout.push <f32> (3);
    va.add_buffer(vb, layout);

    gl::shader_program shader_program;
    gl::shader vertex_shader (GL_VERTEX_SHADER, "../src/shaders/vertex.shader.glsl");
    gl::shader fragment_shader (GL_FRAGMENT_SHADER, "../src/shaders/fragment.shader.glsl");

    shader_program.add_shader(vertex_shader);
    shader_program.add_shader(fragment_shader);
    shader_program.link();
    
    glm::mat4 projection = glm::ortho(0.0f, (f32)globals::width, 0.0f, (f32)globals::height, -5.0f, 5.0f);
    glm::mat4 view (1.0f);
    glm::vec3 translation (0, 0, 0);
    
    shader_program.bind();
    shader_program.set_uniform("u_color", globals::draw_color);

    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      process_input(window);

      renderer.clear();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
      glm::mat4 mvp = projection * view * model;

      shader_program.set_uniform("u_MVP", mvp);

      {
        ImGui::Begin("Controls");
        ImGui::SliderFloat("translation_x", &translation.x, 0.0f, globals::width - length);
        ImGui::SliderFloat("translation_y", &translation.y, 0.0f, globals::height - length);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
      }

      renderer.draw(va, ib, shader_program);

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      glfwSwapBuffers(window);
    }

    ib.unbind();
    vb.unbind();
    va.unbind();
    shader_program.unbind();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);

  glfwTerminate();

  return 0;
}
