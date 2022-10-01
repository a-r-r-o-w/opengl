#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "application.hpp"
#include "utils.hpp"

namespace gl {

  static std::unique_ptr <gl::object> create_scene_none ();
  static std::unique_ptr <gl::object> create_scene_triangle (u32, u32, u32);
  static std::unique_ptr <gl::object> create_scene_rectangle (u32, u32, u32);
  static std::unique_ptr <gl::object> create_scene_cuboid (u32, u32, u32);
  static std::unique_ptr <gl::object> create_scene_torus (u32, u32, u32);
  static std::unique_ptr <gl::object> create_scene_suzanne (u32, u32, u32);

  application::application (u32 width, u32 height, u32 depth, const std::string &name)
    : m_width (width),
      m_height (height),
      m_depth (depth),
      m_name (name),
      m_window (nullptr),
      m_running (true),
      m_shader_program (nullptr),
      m_camera (glm::vec3((f32)m_width / 2, (f32)m_height / 2, 0.0f)),
      m_delta_time (0.0f),
      m_last_frame (0.0f),
      m_first_mouse (true),
      m_last_mouse_x (m_width / 2),
      m_last_mouse_y (m_height / 2),
      m_scenes (),
      m_scene_index (1)
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
    glfwSwapInterval(1);

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
    // Don't handle unknown keys
    // GLFW_KEY_UNKNOWN is defined as -1 and will cause out of bounds access
    if (key == GLFW_KEY_UNKNOWN)
      return;
    
    if (action == GLFW_RELEASE)
      m_key_pressed[key] = false;
    else
      m_key_pressed[key] = true;
  }

  void application::on_mouseclick (
    [[maybe_unused]] i32 button,
    [[maybe_unused]] i32 action,
    [[maybe_unused]] i32 mods
  ) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, action == GLFW_PRESS);

    if (io.WantCaptureMouse)
      return;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      if (action == GLFW_PRESS) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      }
      else {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      }
    }
  }

  void application::on_mousemove (
    [[maybe_unused]] f64 x,
    [[maybe_unused]] f64 y
  ) {
    f32 x_pos = static_cast <f32> (x);
    f32 y_pos = static_cast <f32> (y);

    ImGuiIO &io = ImGui::GetIO();
    io.AddMousePosEvent(x_pos, y_pos);

    if (io.WantCaptureMouse)
      return;

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

    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseWheelEvent(x_offset, y_offset);

    if (io.WantCaptureMouse)
      return;

    m_camera.on_mousescroll(x_offset, y_offset);
  }

  void application::on_update () {
    keypress_update();

    f32 current_frame = static_cast <float> (glfwGetTime());
    m_delta_time = current_frame - m_last_frame;
    m_last_frame = current_frame;

    auto clear_color = get_clear_color();
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    clear();

    auto projection = m_camera.get_projection((f32)m_width / (f32)m_height, 1.0f, (f32)m_depth);
    auto view = m_camera.get_view();
    glm::mat4 model (1.0f);

    m_shader_program->bind();
    m_shader_program->set_uniform("u_model", model);
    m_shader_program->set_uniform("u_view", view);
    m_shader_program->set_uniform("u_projection", projection);
    m_shader_program->set_uniform("u_color", glm::vec4(0.8f, 0.2f, 0.2f, 1.0f));

    if (m_scene_index >= 0 and m_scene_index < (i32)m_scenes.size()) {
      model = m_scenes[m_scene_index]->get_model();
      m_shader_program->set_uniform("u_model", model);
      draw_elements(
        m_scenes[m_scene_index]->get_vertex_array(),
        m_scenes[m_scene_index]->get_index_buffer(),
        *m_shader_program
      );
    }

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

    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_MenuBar);

    ImGui::ShowDemoWindow();

    std::string shape_name = "None";

    if (m_scene_index >= 0 and m_scene_index < (i32)m_scenes.size())
      shape_name = m_scenes[m_scene_index]->get_name();

    if (ImGui::CollapsingHeader("Application", ImGuiTreeNodeFlags_DefaultOpen)) {
      {
        ImGui::Text("Scene");
        
        std::vector <const char*> names;
        
        for (i32 i = 0; i < (i32)m_scenes.size(); ++i)
          names.push_back(m_scenes[i]->get_name().c_str());
        
        ImGui::Combo("##", &m_scene_index, names.data(), names.size());
      }

      ImGui::Separator();

      {
        ImGui::Text("Camera");

        auto &position = m_camera.get_position();

        ImGui::Text("  Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
        ImGui::Text("       FOV: %.2f", m_camera.get_fov());
        ImGui::Text("       Yaw: %.2f", m_camera.get_yaw());
        ImGui::Text("     Pitch: %.2f", m_camera.get_pitch());
        ImGui::Text("      Roll: %.2f", m_camera.get_roll());
      }
    }
    
    ImGui::Separator();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Press ESC to exit");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  void application::keypress_update () {
    if (m_key_pressed[GLFW_KEY_ESCAPE]) {
      glfwSetWindowShouldClose(m_window, GL_TRUE);
      return;
    }

    if (m_key_pressed[GLFW_KEY_W])
      m_camera.on_keypress(camera_movement::front, m_delta_time);

    if (m_key_pressed[GLFW_KEY_A])
      m_camera.on_keypress(camera_movement::right, m_delta_time);

    if (m_key_pressed[GLFW_KEY_S])
      m_camera.on_keypress(camera_movement::back, m_delta_time);

    if (m_key_pressed[GLFW_KEY_D])
      m_camera.on_keypress(camera_movement::left, m_delta_time);

    if (m_key_pressed[GLFW_KEY_Q])
      m_camera.on_keypress(camera_movement::left_roll, m_delta_time);

    if (m_key_pressed[GLFW_KEY_E])
      m_camera.on_keypress(camera_movement::right_roll, m_delta_time);
  }

  void application::set_callbacks () {
    glfwSetErrorCallback([] (i32 error_code, const char *description) {
      std::cerr << "GLFW Error (" << error_code << "): " << description << std::endl;
    });

    glfwSetFramebufferSizeCallback(m_window, [] (GLFWwindow* window, i32 width, i32 height) {
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

    glfwSetMouseButtonCallback(m_window, [] (GLFWwindow* window, i32 button, i32 action, i32 mods) {
      application& app = *static_cast <application*> (glfwGetWindowUserPointer(window));
      app.on_mouseclick(button, action, mods);
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
    m_scenes.emplace_back(create_scene_none());
    m_scenes.emplace_back(create_scene_triangle(m_width, m_height, m_depth));
    m_scenes.emplace_back(create_scene_rectangle(m_width, m_height, m_depth));
    m_scenes.emplace_back(create_scene_cuboid(m_width, m_height, m_depth));
    m_scenes.emplace_back(create_scene_torus(m_width, m_height, m_depth));
    m_scenes.emplace_back(create_scene_suzanne(m_width, m_height, m_depth));
  }

  static std::unique_ptr <gl::object> create_scene_none () {
    auto object = std::make_unique <gl::object> ("None");
    
    (*object)
      .load();

    return object;
  }

  static std::unique_ptr <gl::object> create_scene_triangle (u32 width, u32 height, u32 depth) {
    f32 t_width  = (f32)width / 10;
    f32 t_height = (f32)height / 10;

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

  static std::unique_ptr <gl::object> create_scene_rectangle (u32 width, u32 height, u32 depth) {
    f32 t_width  = (f32)width / 10;
    f32 t_height = (f32)height / 10;

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

  static std::unique_ptr <gl::object> create_scene_cuboid (u32 width, u32 height, u32 depth) {
    f32 t_width  = (f32)width / 10;
    f32 t_height = (f32)height / 10;
    f32 t_depth = (f32)depth / 10;

    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;

    f32 left   = w_mid - t_width;
    f32 right  = w_mid + t_width;
    f32 bottom = h_mid - t_height;
    f32 top    = h_mid + t_height;
    f32 back   = -d_mid - t_depth;

    auto object = std::make_unique <gl::object> ("Cuboid");

    (*object)
      .add_vertex({left, bottom, -d_mid})
      .add_vertex({right, bottom, -d_mid})
      .add_vertex({right, top, -d_mid})
      .add_vertex({left, top, -d_mid})
      .add_vertex({left, bottom, back})
      .add_vertex({right, bottom, back})
      .add_vertex({right, top, back})
      .add_vertex({left, top, back})
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

  static std::unique_ptr <gl::object> create_scene_torus (u32 width, u32 height, u32 depth) {
    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;

    auto object = load_blender_obj("../res/torus.obj", "Torus");
    object->scale(glm::vec3(100.0f));
    object->translate({w_mid, h_mid, -d_mid});
    return object;
  }

  static std::unique_ptr <gl::object> create_scene_suzanne (u32 width, u32 height, u32 depth) {
    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;

    auto object = load_blender_obj("../res/suzanne.obj", "Suzanne");
    object->scale(glm::vec3(100.0f));
    object->translate({w_mid, h_mid, -d_mid});
    return object;
  }

} // namespace gl
