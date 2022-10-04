#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "application.hpp"
#include "scene.hpp"

namespace gl {

  static std::random_device rd;
  static std::mt19937 mt (rd());
  static std::uniform_real_distribution <> rng (-1, 1);

  static std::unique_ptr <gl::scene> create_scene_none ();
  static std::unique_ptr <gl::scene> create_scene_triangle (u32, u32, u32);
  static std::unique_ptr <gl::scene> create_scene_rectangle (u32, u32, u32);
  static std::unique_ptr <gl::scene> create_scene_cuboid (u32, u32, u32);
  static std::unique_ptr <gl::scene> create_scene_circle (u32, u32, u32);
  static std::unique_ptr <gl::scene> create_scene_sphere (u32, u32, u32);
  static std::unique_ptr <gl::scene> create_scene_torus (u32, u32, u32);
  static std::unique_ptr <gl::scene> create_scene_suzanne (u32, u32, u32);
  static std::unique_ptr <gl::scene> create_scene_klein_bottle (u32, u32, u32);
  static std::unique_ptr <gl::scene> create_scene_planetary_gear (u32, u32, u32);
  static std::unique_ptr <gl::scene> create_scene_assignment (u32, u32, u32);

  static void circle_generate (object&, u32, u32, u32, u32);

  static std::unique_ptr <gl::object> load_blender_obj (const std::string&, const std::string&, const std::vector <glm::vec3>&);

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
      m_should_camera_move (false),
      m_first_mouse (true),
      m_last_mouse_x (m_width / 2),
      m_last_mouse_y (m_height / 2),
      m_grid (nullptr),
      m_grid_spacing (25.0f),
      m_display_grid (true),
      m_display_outline (false),
      m_display_wireframe (false),
      m_display_depth_test (true),
      m_display_smooth_lines (true),
      m_key_pressed (m_key_count),
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    set_callbacks();
    set_shaders();
    set_clear_color({0.2f, 0.2f, 0.2f, 1.0f});

    create_grid();

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
    
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      if (action == GLFW_PRESS) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_should_camera_move = true;
      }
      else {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_should_camera_move = false;
        m_first_mouse = true;
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
    
    if (m_should_camera_move)
      m_camera.on_mousemove(-x_offset, -y_offset);
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
    m_shader_program->set_uniform("u_color", glm::vec4 {0, 0, 0, 1});
    m_shader_program->set_uniform("u_use_vertex_color", true);

    (m_display_depth_test ? glEnable : glDisable)(GL_DEPTH_TEST);
    (m_display_smooth_lines ? glEnable: glDisable)(GL_LINE_SMOOTH);

    if (m_display_grid) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      
      set_draw_mode(draw_mode::line);
      m_shader_program->set_uniform("u_blend", m_grid->get_blend());
      draw_elements(
        m_grid->get_vertex_array(),
        m_grid->get_index_buffer(),
        *m_shader_program
      );
    }

    set_draw_mode(draw_mode::triangle);

    if (m_scene_index >= 0 and m_scene_index < (i32)m_scenes.size()) {
      auto &scene = *m_scenes[m_scene_index];

      if (m_display_wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        for (auto &o: scene.get_objects()) {
          model = o->get_model();
          m_shader_program->set_uniform("u_model", model);
          draw_elements(
            o->get_vertex_array(),
            o->get_index_buffer(),
            *m_shader_program
          );
        }
      }
      else {
        if (m_display_outline) {
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

          m_shader_program->set_uniform("u_use_vertex_color", false);
          glLineWidth(2.5f);

          for (auto &o: scene.get_objects()) {
            if (!o->m_should_render)
              continue;
            
            model = o->get_model();
            m_shader_program->set_uniform("u_model", model);

            draw_elements(
              o->get_vertex_array(),
              o->get_index_buffer(),
              *m_shader_program
            );
          }

          glLineWidth(1.0f);
          m_shader_program->set_uniform("u_use_vertex_color", true);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        for (auto &o: scene.get_objects()) {
          if (!o->m_should_render)
            continue;
          
          model = o->get_model();
          m_shader_program->set_uniform("u_model", model);
          m_shader_program->set_uniform("u_blend", o->get_blend());

          draw_elements(
            o->get_vertex_array(),
            o->get_index_buffer(),
            *m_shader_program
          );
        }
      }

      scene.on_update(m_delta_time);
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

    if (ImGui::CollapsingHeader("Application", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::SetNextItemOpen(true, ImGuiCond_Once);

      if (ImGui::TreeNode("Scene")) {
      
        std::vector <const char*> names;
        
        for (i32 i = 0; i < (i32)m_scenes.size(); ++i)
          names.push_back(m_scenes[i]->get_name().c_str());
        
        ImGui::Combo("##", &m_scene_index, names.data(), names.size());

        ImGui::TreePop();
      }

      ImGui::Separator();
      ImGui::SetNextItemOpen(true, ImGuiCond_Once);

      if (ImGui::TreeNode("Rendered Objects")) {
        if (m_scene_index >= 0 and m_scene_index < (i32)m_scenes.size()) {
          auto &scene = *m_scenes[m_scene_index];

          for (auto &o: scene.get_objects())
            ImGui::Checkbox(o->get_name().c_str(), &o->m_should_render);
        }

        ImGui::TreePop();
      }

      ImGui::Separator();
      ImGui::SetNextItemOpen(true, ImGuiCond_Once);

      if (ImGui::TreeNode("General Settings")) {
        ImGui::Checkbox("Grid", &m_display_grid);
        ImGui::Checkbox("Outline", &m_display_outline);
        ImGui::Checkbox("Wireframe", &m_display_wireframe);
        ImGui::Checkbox("Depth Test", &m_display_depth_test);

        ImGui::TreePop();
      }

      ImGui::Separator();
      ImGui::SetNextItemOpen(true, ImGuiCond_Once);
      
      if (ImGui::TreeNode("Camera")) {
        auto &position = m_camera.get_position();

        ImGui::Text("  Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
        ImGui::Text("       FOV: %.2f", m_camera.get_fov());
        ImGui::Text("       Yaw: %.2f", m_camera.get_yaw());
        ImGui::Text("     Pitch: %.2f", m_camera.get_pitch());
        ImGui::Text("      Roll: %.2f", m_camera.get_roll());

        ImGui::TreePop();
      }
    }

    ImGui::Separator();

    ImGui::Text("Last render %.3f ms", m_delta_time * 1000);
    ImGui::Text("FPS %.3f", ImGui::GetIO().Framerate);
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

    if (m_key_pressed[GLFW_KEY_TAB]) {
      ++m_scene_index;
      m_scene_index %= m_scenes.size();
      m_key_pressed[GLFW_KEY_TAB] = false;
      return;
    }

    if (m_key_pressed[GLFW_KEY_W])
      m_camera.on_keypress(camera_movement::front, m_delta_time);

    if (m_key_pressed[GLFW_KEY_A] or m_key_pressed[GLFW_KEY_LEFT])
      m_camera.on_keypress(camera_movement::right, m_delta_time);

    if (m_key_pressed[GLFW_KEY_S])
      m_camera.on_keypress(camera_movement::back, m_delta_time);

    if (m_key_pressed[GLFW_KEY_D] or m_key_pressed[GLFW_KEY_RIGHT])
      m_camera.on_keypress(camera_movement::left, m_delta_time);
    
    if (m_key_pressed[GLFW_KEY_UP])
      m_camera.on_keypress(camera_movement::up, m_delta_time);
    
    if (m_key_pressed[GLFW_KEY_DOWN])
      m_camera.on_keypress(camera_movement::down, m_delta_time);

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

  void application::create_grid () {
    m_grid.reset(new object("Grid"));

    static glm::vec3 grid_color (0.8f, 0.8f, 0.8f);

    i32 len = std::max(m_width, m_depth);
    f32 h_mid = (f32)m_height / 2;

    f32 left = -len * m_grid_expanse_factor;
    f32 right = len * m_grid_expanse_factor;
    f32 back = -len * m_grid_expanse_factor;
    f32 front = len * m_grid_expanse_factor;

    u32 index = 0;

    for (f32 row = back; row < front; row += m_grid_spacing) {
      m_grid->add_vertex({left, h_mid, row}, grid_color);
      m_grid->add_vertex({right, h_mid, row}, grid_color);
      m_grid->add_index(index++);
      m_grid->add_index(index++);
    }

    for (f32 col = left; col < right; col += m_grid_spacing) {
      m_grid->add_vertex({col, h_mid, back}, grid_color);
      m_grid->add_vertex({col, h_mid, front}, grid_color);
      m_grid->add_index(index++);
      m_grid->add_index(index++);
    }

    m_grid->load();
  }

  void application::initialise_demo () {
    m_scenes.emplace_back(create_scene_none());
    m_scenes.emplace_back(create_scene_triangle(m_width, m_height, m_depth));
    m_scenes.emplace_back(create_scene_rectangle(m_width, m_height, m_depth));
    m_scenes.emplace_back(create_scene_cuboid(m_width, m_height, m_depth));
    m_scenes.emplace_back(create_scene_circle(m_width, m_height, m_depth));
    m_scenes.emplace_back(create_scene_sphere(m_width, m_height, m_depth));
    m_scenes.emplace_back(create_scene_torus(m_width, m_height, m_depth));
    m_scenes.emplace_back(create_scene_suzanne(m_width, m_height, m_depth));
    m_scenes.emplace_back(create_scene_klein_bottle(m_width, m_height, m_depth));
    m_scenes.emplace_back(create_scene_planetary_gear(m_width, m_height, m_depth));
    // m_scenes.emplace_back(create_scene_assignment(m_width, m_height, m_depth));
  }

  static std::unique_ptr <gl::scene> create_scene_none () {
    auto scene = std::make_unique <gl::scene> ("Empty", scene_properties());
    return scene;
  }

  static std::unique_ptr <gl::scene> create_scene_triangle (u32 width, u32 height, u32 depth) {
    f32 t_width  = (f32)width / 10;
    f32 t_height = (f32)height / 10;

    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;

    f32 left   = w_mid - t_width;
    f32 right  = w_mid + t_width;
    f32 bottom = h_mid - t_height;
    f32 top    = h_mid + t_height;

    auto scene = std::make_unique <gl::scene> ("Triangle", scene_properties());
    auto object = std::make_unique <gl::object> ("Triangle");

    (*object)
      .add_vertex({left, bottom, -d_mid}, {1, 0, 0})
      .add_vertex({right, bottom, -d_mid}, {0, 1, 0})
      .add_vertex({w_mid, top, -d_mid}, {0, 0, 1})
      .add_index(0)
      .add_index(1)
      .add_index(2)
      .load();
    
    scene->add_object(std::move(object));

    return scene;
  }

  static std::unique_ptr <gl::scene> create_scene_rectangle (u32 width, u32 height, u32 depth) {
    f32 t_width  = (f32)width / 10;
    f32 t_height = (f32)height / 10;

    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;

    f32 left   = w_mid - t_width;
    f32 right  = w_mid + t_width;
    f32 bottom = h_mid - t_height;
    f32 top    = h_mid + t_height;

    auto scene = std::make_unique <gl::scene> ("Rectangle", scene_properties());
    auto object = std::make_unique <gl::object> ("Rectangle");

    (*object)
      .add_vertex({left, bottom, -d_mid}, {0, 0, 0})
      .add_vertex({right, bottom, -d_mid}, {1, 1, 1})
      .add_vertex({right, top, -d_mid}, {0, 0, 0})
      .add_vertex({left, top, -d_mid}, {1, 1, 1})
      .add_index(0).add_index(1).add_index(2)
      .add_index(2).add_index(3).add_index(0)
      .load();
    
    scene->add_object(std::move(object));

    return scene;
  }

  static std::unique_ptr <gl::scene> create_scene_cuboid (u32 width, u32 height, u32 depth) {
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

    auto scene = std::make_unique <gl::scene> ("Cuboid", scene_properties());
    auto object = std::make_unique <gl::object> ("Cuboid");

    glm::vec3 colors[] = {
      {0.2, 0.8, 0.8},
      {0.8, 0.2, 0.8}
    };

    (*object)
      .add_vertex({left, bottom, -d_mid}, colors[0])
      .add_vertex({right, bottom, -d_mid}, colors[1])
      .add_vertex({right, top, -d_mid}, colors[0])
      .add_vertex({left, top, -d_mid}, colors[1])
      .add_vertex({left, bottom, back}, colors[0])
      .add_vertex({right, bottom, back}, colors[1])
      .add_vertex({right, top, back}, colors[0])
      .add_vertex({left, top, back}, colors[1])
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
    
    scene->add_object(std::move(object));

    return scene;
  }

  static std::unique_ptr <gl::scene> create_scene_circle (u32 width, u32 height, u32 depth) {
    auto scene = std::make_unique <gl::scene> ("Circle", scene_properties());
    auto object = std::make_unique <gl::object> ("Circle");

    circle_generate(*object, 20, width, height, depth);
    object->load();
    
    scene->add_object(std::move(object));

    return scene;
  }

  static std::unique_ptr <gl::scene> create_scene_sphere (u32 width, u32 height, u32 depth) {
    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;

    static std::vector <glm::vec3> colors = {
      {0.8f, 0.4f, 0.2f},
      {0.8f, 0.5f, 0.2f},
      {0.9f, 0.6f, 0.2f},
      {1.0f, 0.7f, 0.3f},
      {1.0f, 0.8f, 0.4f}
    };

    auto scene = std::make_unique <gl::scene> ("Sphere", scene_properties());
    auto object = load_blender_obj("../res/sphere.obj", "Sphere", colors);

    (*object)
      .scale(glm::vec3(100.0f))
      .translate({w_mid, h_mid, -d_mid});

    scene->add_object(std::move(object));
    
    return scene;
  }

  static std::unique_ptr <gl::scene> create_scene_torus (u32 width, u32 height, u32 depth) {
    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;

    std::vector <glm::vec3> colors;

    for (i32 i = 0; i < 10; ++i) {
      glm::vec3 color (198.0f + 2 * i, 137.0f + 2 * i, 88.0f - 2 * i);
      color /= 256.0f;
      colors.push_back(color);
    }

    auto scene = std::make_unique <gl::scene> ("Torus", scene_properties());
    auto object = load_blender_obj("../res/torus.obj", "Torus", colors);

    (*object)
      .scale(glm::vec3(100.0f))
      .rotate(90.f, {1, 0, 0})
      .translate({w_mid, h_mid, -d_mid});

    scene->add_object(std::move(object));
    
    return scene;
  }

  static std::unique_ptr <gl::scene> create_scene_suzanne (u32 width, u32 height, u32 depth) {
    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;

    std::vector <glm::vec3> colors;

    for (i32 i = 0; i < 256; ++i) {
      glm::vec3 color (rng(mt), rng(mt), rng(mt));
      color += 1.0f;
      color /= 2.0f;
      colors.push_back(color);
    }

    auto scene = std::make_unique <gl::scene> ("Suzanne", scene_properties());
    auto object = load_blender_obj("../res/suzanne.obj", "Suzanne", colors);
    
    (*object)
      .scale(glm::vec3(100.0f))
      .translate({w_mid, h_mid, -d_mid});

    scene->add_object(std::move(object));
    
    return scene;
  }

  static std::unique_ptr <gl::scene> create_scene_klein_bottle (u32 width, u32 height, u32 depth) {
    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;

    std::vector <glm::vec3> colors;

    for (i32 i = 0; i < 20; ++i) {
      glm::vec3 color (200.0f - 2 * i, 200.0f - 2 * i, 210.0f + 2 * i);
      color /= 256.0f;
      colors.push_back(color);
    }

    auto scene = std::make_unique <gl::scene> ("Klein Bottle", scene_properties());
    auto object = load_blender_obj("../res/klein-bottle.obj", "Klein Bottle", colors);
    
    (*object)
      .scale(glm::vec3(100.0f))
      .translate({w_mid, h_mid, -d_mid})
      .set_blend(0.5f);

    scene->add_object(std::move(object));
    
    return scene;
  }

  static std::unique_ptr <gl::scene> create_scene_planetary_gear (u32 width, u32 height, u32 depth) {
    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;

    std::vector <glm::vec3> colors;

    for (i32 i = 0; i < 20; ++i) {
      glm::vec3 color (200.0f - i, 200.0f - i, 200.0f - i);
      color /= 256.0f;
      colors.push_back(color);
    }

    auto scene = std::make_unique <gl::scene> ("Planetary Gear", scene_properties());

    auto object1 = load_blender_obj("../res/planetary-big.obj", "Big Gear", colors);
    auto object21 = load_blender_obj("../res/planetary-medium.obj", "Medium Gear 1", colors);
    auto object22 = load_blender_obj("../res/planetary-medium.obj", "Medium Gear 2", colors);
    auto object23 = load_blender_obj("../res/planetary-medium.obj", "Medium Gear 3", colors);
    auto object3 = load_blender_obj("../res/planetary-small.obj", "Small Gear", colors);

    for (auto i: object1->get_vertices())
      std::cout << i.x << ' ' << i.y << '\n';

    (*object1)
      .scale(glm::vec3(2.0f))
      .rotate(90.0f, {1, 0, 0})
      .translate({w_mid, h_mid, -d_mid});
    (*object21)
      .scale(glm::vec3(2.0f))
      .rotate(90.0f, {1, 0, 0})
      .translate({w_mid, h_mid, -d_mid});
    (*object22)
      .scale(glm::vec3(2.0f))
      .rotate(90.0f, {1, 0, 0})
      .translate({w_mid, h_mid, -d_mid});
    (*object23)
      .scale(glm::vec3(2.0f))
      .rotate(90.0f, {1, 0, 0})
      .translate({w_mid, h_mid, -d_mid});
    (*object3)
      .scale(glm::vec3(2.0f))
      .rotate(90.0f, {1, 0, 0})
      .translate({w_mid, h_mid, -d_mid});

    scene->add_object(std::move(object1));
    scene->add_object(std::move(object21));
    scene->add_object(std::move(object22));
    scene->add_object(std::move(object23));
    scene->add_object(std::move(object3));
    
    return scene;
  }

  // static std::unique_ptr <gl::scene> create_scene_assignment (u32 width, u32 height, u32 depth) {
  //   f32 w_mid = (f32)width / 2;
  //   f32 h_mid = (f32)height / 2;
  //   f32 d_mid = (f32)depth / 2;
    
  //   const f32 sphere_radius = 200.0f;
  //   const f32 velocity_factor = 500.0f;

  //   static auto random_point_inside_sphere = [&] () -> glm::vec3 {
  //     auto u = std::cbrt(rng(mt));
  //     auto r = sphere_radius * u;
  //     auto point = glm::vec3(rng(mt), rng(mt), rng(mt));
  //     point = glm::normalize(point);
  //     point *= r;
  //     return point;
  //   };

  //   static auto random_velocity = [&] () -> glm::vec3 {
  //     auto velocity = glm::vec3(rng(mt), rng(mt), rng(mt));
  //     velocity *= velocity_factor;
  //     return velocity;
  //   };

  //   auto scene = std::make_unique <gl::scene> (
  //     "Assignment",
  //     scene_properties(
  //       w_mid - sphere_radius, w_mid + sphere_radius,
  //       h_mid + sphere_radius, h_mid - sphere_radius,
  //       d_mid + sphere_radius, d_mid - sphere_radius
  //     )
  //   );

  //   auto object1 = load_blender_obj("../res/sphere.obj", "Sphere");
  //   auto object2 = load_blender_obj("../res/klein-bottle.obj", "Klein Bottle");
  //   auto object3 = load_blender_obj("../res/planetary-gear.obj", "Planetary Gear");

  //   auto properties1 = gl::object_properties({0, 0, 0});
  //   auto properties2 = gl::object_properties(random_velocity());
  //   auto properties3 = gl::object_properties(random_velocity());

  //   (*object1)
  //     .scale(glm::vec3(sphere_radius))
  //     .translate({w_mid, h_mid, -d_mid});

  //   (*object2)
  //     .scale(glm::vec3(10.0f))
  //     .translate(random_point_inside_sphere())
  //     .translate({w_mid, h_mid, -d_mid});

  //   (*object3)
  //     .scale(glm::vec3(0.5f))
  //     .rotate(90.0f, {1, 0, 0})
  //     .translate(random_point_inside_sphere())
  //     .translate({w_mid, h_mid, -d_mid});

  //   scene->add_object(std::move(object1), properties1);
  //   scene->add_object(std::move(object2), properties2);
  //   scene->add_object(std::move(object3), properties3);
    
  //   return scene;
  // }

  void circle_generate (object& object, u32 points, u32 width, u32 height, u32 depth) {
    f32 w_mid = (f32)width / 2;
    f32 h_mid = (f32)height / 2;
    f32 d_mid = (f32)depth / 2;
    f32 angle = glm::radians(360.0f / points);

    static auto rotate_point = [] (glm::vec3 center, glm::vec3 point, f32 angle) -> glm::vec3 {
      f32 s = glm::sin(angle);
      f32 c = glm::cos(angle);

      point.x -= center.x;
      point.y -= center.y;

      f32 xnew = point.x * c - point.y * s;
      f32 ynew = point.x * s + point.y * c;

      point.x = xnew + center.x;
      point.y = ynew + center.y;

      return point;
    };

    object.clear();

    static glm::vec3 colors[] = {
      {0.8, 0.2, 0.2},
      {0.8, 0.4, 0.1}
    };

    object.add_vertex({0, 0, 0}, {1, 1, 1});
    object.add_vertex({1, 0, 0}, colors[1]);

    for (i32 i = 0; i < (i32)points; ++i) {
      object.add_vertex(rotate_point({0, 0, 0}, object.get_vertices()[2 * i + 2], angle), colors[i % 2]);
      object.add_index(0);
      object.add_index(i + 1);
      object.add_index(i + 2);
    }

    object
      .scale(glm::vec3(100.0f))
      .translate({w_mid, h_mid, -d_mid});
  }

  std::unique_ptr <gl::object> load_blender_obj (
    const std::string &filepath, const std::string &name,
    const std::vector <glm::vec3>& colors
  ) {
    std::ifstream file (filepath);
    std::string line;
    std::stringstream stream;

    auto object = std::make_unique <gl::object> (name.c_str());
    u32 size = colors.size();

    while (not file.eof()) {
      std::getline(file, line);
      
      if (line.starts_with("v ")) {
        stream << line.substr(2);
        
        glm::vec3 v;
        stream >> v.x >> v.y >> v.z;
        
        object->add_vertex(v, colors[std::floor((rng(mt) + 1) * size / 2)]);
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

} // namespace gl
