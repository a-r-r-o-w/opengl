#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "application.hpp"

namespace gl {

  application::application (u32 width, u32 height, const std::string &name)
    : m_width (width),
      m_height (height),
      m_name (name),
      m_window (nullptr),
      m_running (true),
      m_shader_program (nullptr),
      m_objects ()
  {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    m_window = glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);

    if (m_window == nullptr) {
      std::cerr << "Failed to create GLFW window!" << std::endl;
      return;
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cerr << "Failed to initialise GLAD!" << std::endl;
      return;
    }

    glfwSetWindowUserPointer(m_window, this);

    gl::shader vertex_shader (gl::shader_type::vertex, "../src/shaders/vertex.shader.glsl");
    gl::shader fragment_shader (gl::shader_type::fragment, "../src/shaders/fragment.shader.glsl");

    m_shader_program = std::make_unique <shader_program> ();
    m_shader_program->add_shader(vertex_shader);
    m_shader_program->add_shader(fragment_shader);
    m_shader_program->link();

    set_callbacks();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
  }

  application::~application () {
    glfwTerminate();
  }

  void application::run () {
    while (m_running) {
      glfwPollEvents();
      on_update();
      glfwSwapBuffers(m_window);
    }
  }

  void application::on_resize (i32 width, i32 height) {
    m_width = width;
    m_height = height;
    set_view_port(0, 0, m_width, m_height);
  }

  void application::on_keypress (
    [[maybe_unused]] i32 key,
    [[maybe_unused]] i32 scancode,
    [[maybe_unused]] i32 action,
    [[maybe_unused]] i32 mods
  ) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(m_window, GL_TRUE);
  }

  void application::on_update () {
    clear();

    glm::mat4 projection = glm::ortho(0.0f, (f32)m_width, 0.0f, (f32)m_height, -5.0f, +5.0f);
    glm::mat4 view (1.0f);
    glm::mat4 model (1.0f);

    m_shader_program->bind();
    m_shader_program->set_uniform("u_model", model);
    m_shader_program->set_uniform("u_view", view);
    m_shader_program->set_uniform("u_projection", projection);
    m_shader_program->set_uniform("u_color", glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("Objects")) {
        if (ImGui::MenuItem("Triangle")) {}
        if (ImGui::MenuItem("Square")) {}
        if (ImGui::MenuItem("Rectangle")) {}
        if (ImGui::MenuItem("Sphere")) {}
        if (ImGui::MenuItem("Klein Bottle")) {}
        
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Press ESC to exit");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    draw_elements(m_objects[0]->get_vertex_array(), m_objects[0]->get_index_buffer(), *m_shader_program);

    m_running = !glfwWindowShouldClose(m_window);
  }

  u32 application::get_width () const {
    return m_width;
  }
  
  u32 application::get_height () const {
    return m_height;
  }

  void application::set_callbacks () {
    glfwSetErrorCallback([] (i32 error_code, const char *description) {
      std::cerr << "GLFW Error (" << error_code << "): " << description << std::endl;
    });

    glfwSetWindowSizeCallback(m_window, [] (GLFWwindow* window, i32 width, i32 height) {
      application& app = *static_cast <application*> (glfwGetWindowUserPointer(window));
      app.on_resize(width, height);
    });

    glfwSetKeyCallback(m_window, [] (GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
      application& app = *static_cast <application*> (glfwGetWindowUserPointer(window));
      app.on_keypress(key, scancode, action, mods);
    });
  }

} // namespace gl
