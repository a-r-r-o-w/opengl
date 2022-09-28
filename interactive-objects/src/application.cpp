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

  static std::unique_ptr <gl::object> create_none ();
  static std::unique_ptr <gl::object> create_triangle (u32, u32, u32);
  static std::unique_ptr <gl::object> create_rectangle (u32, u32, u32);
  static std::unique_ptr <gl::object> create_cuboid (u32, u32, u32);

  application::application (u32 width, u32 height, u32 depth, const std::string &name)
    : m_width (width),
      m_height (height),
      m_depth (depth),
      m_name (name),
      m_window (nullptr),
      m_running (true),
      m_shader_program (nullptr),
      m_camera (glm::vec3((f32)m_width / 2, (f32)m_height / 2, (f32)m_depth)),
      m_delta_time (0.0f),
      m_last_frame (0.0f),
      m_first_mouse (true),
      m_last_mouse_x (m_width / 2),
      m_last_mouse_y (m_height / 2),
      m_objects (),
      m_selected_object_index (1)
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

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    set_callbacks();
    set_shaders();
    set_clear_color({0.2f, 0.2f, 0.2f, 1.0f});

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
  }

  application::~application () {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
  }

  void application::clear () const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    if (action == GLFW_PRESS or action == GLFW_REPEAT) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_window, GL_TRUE);
          break;
        
        case GLFW_KEY_W:
          m_camera.on_keypress(camera_movement::front, m_delta_time);
          break;
        
        case GLFW_KEY_S:
          m_camera.on_keypress(camera_movement::back, m_delta_time);
          break;
        
        case GLFW_KEY_A:
          m_camera.on_keypress(camera_movement::left, m_delta_time);
          break;
        
        case GLFW_KEY_D:
          m_camera.on_keypress(camera_movement::right, m_delta_time);
          break;
      }
    }
  }

  void application::on_mousemove (f64 x, f64 y) {
    f32 x_pos = static_cast <f32> (x);
    f32 y_pos = static_cast <f32> (y);

    if (m_first_mouse) {
      m_last_mouse_x = x_pos;
      m_last_mouse_y = y_pos;
      m_first_mouse = false;
    }

    f32 x_offset = x_pos - m_last_mouse_x;
    f32 y_offset = m_last_mouse_y - y_pos;

    m_last_mouse_x = x_pos;
    m_last_mouse_y = y_pos;
    
    m_camera.on_mousemove(x_offset, y_offset);
  }

  void application::on_mousescroll (f64 x, f64 y) {
    f32 x_offset = static_cast <float> (x);
    f32 y_offset = static_cast <float> (y);

    m_camera.on_mousescroll(x_offset, y_offset);
  }

  void application::on_update () {
    f32 current_frame = static_cast <float> (glfwGetTime());
    m_delta_time = current_frame - m_last_frame;
    m_last_frame = current_frame;

    auto clear_color = get_clear_color();
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    clear();

    glm::mat4 projection = glm::perspective(glm::radians(m_camera.get_fov()), (f32)m_width / (f32)m_height, (f32)m_depth / 4, (f32)-m_depth * 2);
    // glm::mat4 projection = glm::ortho(0.0f, (f32)m_width, 0.0f, (f32)m_height, 0.0f, (f32)m_depth);
    glm::mat4 model (1.0f);

    m_shader_program->bind();
    m_shader_program->set_uniform("u_model", model);
    m_shader_program->set_uniform("u_view", m_camera.get_view());
    m_shader_program->set_uniform("u_projection", projection);
    m_shader_program->set_uniform("u_color", glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));

    if (m_selected_object_index >= 0 and m_selected_object_index < (i32)m_objects.size())
      draw_elements(
        m_objects[m_selected_object_index]->get_vertex_array(),
        m_objects[m_selected_object_index]->get_index_buffer(),
        *m_shader_program
      );

    imgui_update();

    m_running = !glfwWindowShouldClose(m_window);
  }

  u32 application::get_width () const {
    return m_width;
  }
  
  u32 application::get_height () const {
    return m_height;
  }

  u32 application::get_depth () const {
    return m_depth;
  }

  void application::imgui_update () {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("Objects")) {
        for (i32 i = 0; i < (i32)m_objects.size(); ++i) {
          if (ImGui::MenuItem(m_objects[i]->get_name().c_str()))
            m_selected_object_index = i;
        }
        
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }

    std::string shape_name = "None";

    if (m_selected_object_index >= 0 and m_selected_object_index < (i32)m_objects.size())
      shape_name = m_objects[m_selected_object_index]->get_name();

    ImGui::Text("Shape rendered: %s", shape_name.c_str());

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Press ESC to exit");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

    glfwSetCursorPosCallback(m_window, [] (GLFWwindow* window, f64 x, f64 y) {
      application& app = *static_cast <application*> (glfwGetWindowUserPointer(window));
      app.on_mousemove(x, y);
    });

    glfwSetScrollCallback (m_window, [] (GLFWwindow* window, f64 x, f64 y) {
      application& app = *static_cast <application*> (glfwGetWindowUserPointer(window));
      app.on_mousescroll(x, y);
    });
  }

  void application::set_shaders () {
    gl::shader vertex_shader (gl::shader_type::vertex, "../src/shaders/vertex.shader.glsl");
    gl::shader fragment_shader (gl::shader_type::fragment, "../src/shaders/fragment.shader.glsl");

    m_shader_program = std::make_unique <shader_program> ();
    m_shader_program->add_shader(vertex_shader);
    m_shader_program->add_shader(fragment_shader);
    m_shader_program->link();
  }

  void application::initialise_demo () {
    m_objects.emplace_back(create_none());
    m_objects.emplace_back(create_triangle(m_width, m_height, m_depth));
    m_objects.emplace_back(create_rectangle(m_width, m_height, m_depth));
    m_objects.emplace_back(create_cuboid(m_width, m_height, m_depth));
  }

  static std::unique_ptr <gl::object> create_none () {
    auto object = std::make_unique <gl::object> ("None");
    
    (*object)
      .load();

    return object;
  }

  static std::unique_ptr <gl::object> create_triangle (u32 width, u32 height, u32 depth) {
    f32 t_width  = (f32)width / 4;
    f32 t_height = (f32)height / 4;

    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;

    f32 left   = w_mid - t_width;
    f32 right  = w_mid + t_width;
    f32 bottom = h_mid - t_height;
    f32 top    = h_mid + t_height;

    auto object = std::make_unique <gl::object> ("Triangle");

    (*object)
      .add_vertex({left, bottom, -d_mid})
      .add_vertex({right, bottom, -d_mid})
      .add_vertex({w_mid, top, -d_mid})
      .add_index(0)
      .add_index(1)
      .add_index(2)
      .load();

    return object;
  }

  static std::unique_ptr <gl::object> create_rectangle (u32 width, u32 height, u32 depth) {
    f32 t_width  = (f32)width / 4;
    f32 t_height = (f32)height / 4;

    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;

    f32 left   = w_mid - t_width;
    f32 right  = w_mid + t_width;
    f32 bottom = h_mid - t_height;
    f32 top    = h_mid + t_height;

    auto object = std::make_unique <gl::object> ("Rectangle");

    (*object)
      .add_vertex({left, bottom, -d_mid})
      .add_vertex({right, bottom, -d_mid})
      .add_vertex({right, top, -d_mid})
      .add_vertex({left, top, -d_mid})
      .add_index(0).add_index(1).add_index(2)
      .add_index(2).add_index(3).add_index(0)
      .load();

    return object;
  }

  static std::unique_ptr <gl::object> create_cuboid (u32 width, u32 height, u32 depth) {
    f32 t_width  = (f32)width / 4;
    f32 t_height = (f32)height / 4;

    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;

    f32 left   = w_mid - t_width;
    f32 right  = w_mid + t_width;
    f32 bottom = h_mid - t_height;
    f32 top    = h_mid + t_height;

    auto object = std::make_unique <gl::object> ("Cuboid");

    (*object)
      .add_vertex({left, bottom, 0.0f})
      .add_vertex({right, bottom, 0.0f})
      .add_vertex({right, top, 0.0f})
      .add_vertex({left, top, 0.0f})
      .add_vertex({left, bottom, -d_mid})
      .add_vertex({right, bottom, -d_mid})
      .add_vertex({right, top, -d_mid})
      .add_vertex({left, top, -d_mid})
      .add_index(0).add_index(1).add_index(2)
      .add_index(2).add_index(3).add_index(0)
      .add_index(1).add_index(5).add_index(6)
      .add_index(6).add_index(2).add_index(1)
      .add_index(4).add_index(5).add_index(6)
      .add_index(6).add_index(7).add_index(4)
      .add_index(0).add_index(4).add_index(7)
      .add_index(7).add_index(3).add_index(0)
      .add_index(0).add_index(1).add_index(5)
      .add_index(5).add_index(4).add_index(0)
      .add_index(3).add_index(2).add_index(6)
      .add_index(6).add_index(7).add_index(3)
      .load();

    return object;
  }

} // namespace gl
