#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "application.hpp"

static void ShowDemoWindowInputs();

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
      m_camera (glm::vec3((f32)m_width / 2, (f32)m_height / 2, 0.0f)),
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
    // Don't handle unknown keys
    // GLFW_KEY_UNKNOWN is defined as -1 and will cause out of bounds access
    if (key == GLFW_KEY_UNKNOWN)
      return;
    
    if (action == GLFW_RELEASE)
      m_key_pressed[key] = false;
    else
      m_key_pressed[key] = true;
  }

  void application::on_mouseclick (i32 button, i32 action, i32 mods) {
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
    keypress_update();

    f32 current_frame = static_cast <float> (glfwGetTime());
    m_delta_time = current_frame - m_last_frame;
    m_last_frame = current_frame;

    auto clear_color = get_clear_color();
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    clear();

    f32 camera_z = m_camera.get_position().z;

    // glm::mat4 projection = glm::perspective(glm::radians(m_camera.get_fov()), (f32)m_width / (f32)m_height, (f32)camera_z, (f32)camera_z - m_depth * 2);
    // glm::mat4 projection = glm::perspective(glm::radians(m_camera.get_fov()), (f32)m_width / (f32)m_height, 1.0f, (f32)m_depth);
    // glm::mat4 projection = glm::ortho(0.0f, (f32)m_width, 0.0f, (f32)m_height, 0.0f, (f32)m_depth);
    glm::mat4 projection = m_camera.get_projection((f32)m_width / (f32)m_height, 1.0f, (f32)m_depth);
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

    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_MenuBar);

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

    ShowDemoWindowInputs();

    ImGui::Text("Shape rendered: %s", shape_name.c_str());
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

  static std::unique_ptr <gl::object> create_rectangle (u32 width, u32 height, u32 depth) {
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

  static std::unique_ptr <gl::object> create_cuboid (u32 width, u32 height, u32 depth) {
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

} // namespace gl

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

// Helper to wire demo markers located in code to an interactive browser
typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiDemoMarkerCallback      GImGuiDemoMarkerCallback;
extern void*                        GImGuiDemoMarkerCallbackUserData;
// ImGuiDemoMarkerCallback             GImGuiDemoMarkerCallback = NULL;
// void*                               GImGuiDemoMarkerCallbackUserData = NULL;
#define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback(__FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)

void ShowDemoWindowInputs()
{
    IMGUI_DEMO_MARKER("Inputs, Navigation & Focus");
    if (ImGui::CollapsingHeader("Inputs, Navigation & Focus"))
    {
        ImGuiIO& io = ImGui::GetIO();

        // Display ImGuiIO output flags
        IMGUI_DEMO_MARKER("Inputs, Navigation & Focus/Output");
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        if (ImGui::TreeNode("Output"))
        {
            ImGui::Text("io.WantCaptureMouse: %d", io.WantCaptureMouse);
            ImGui::Text("io.WantCaptureMouseUnlessPopupClose: %d", io.WantCaptureMouseUnlessPopupClose);
            ImGui::Text("io.WantCaptureKeyboard: %d", io.WantCaptureKeyboard);
            ImGui::Text("io.WantTextInput: %d", io.WantTextInput);
            ImGui::Text("io.WantSetMousePos: %d", io.WantSetMousePos);
            ImGui::Text("io.NavActive: %d, io.NavVisible: %d", io.NavActive, io.NavVisible);
            ImGui::TreePop();
        }

        // Display Mouse state
        IMGUI_DEMO_MARKER("Inputs, Navigation & Focus/Mouse State");
        if (ImGui::TreeNode("Mouse State"))
        {
            if (ImGui::IsMousePosValid())
                ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
            else
                ImGui::Text("Mouse pos: <INVALID>");
            ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);

            int count = IM_ARRAYSIZE(io.MouseDown);
            ImGui::Text("Mouse down:");         for (int i = 0; i < count; i++) if (ImGui::IsMouseDown(i))      { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
            ImGui::Text("Mouse clicked:");      for (int i = 0; i < count; i++) if (ImGui::IsMouseClicked(i))   { ImGui::SameLine(); ImGui::Text("b%d (%d)", i, ImGui::GetMouseClickedCount(i)); }
            ImGui::Text("Mouse released:");     for (int i = 0; i < count; i++) if (ImGui::IsMouseReleased(i))  { ImGui::SameLine(); ImGui::Text("b%d", i); }
            ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);
            ImGui::Text("Pen Pressure: %.1f", io.PenPressure); // Note: currently unused
            ImGui::TreePop();
        }

        // Display mouse cursors
        IMGUI_DEMO_MARKER("Inputs, Navigation & Focus/Mouse Cursors");
        if (ImGui::TreeNode("Mouse Cursors"))
        {
            const char* mouse_cursors_names[] = { "Arrow", "TextInput", "ResizeAll", "ResizeNS", "ResizeEW", "ResizeNESW", "ResizeNWSE", "Hand", "NotAllowed" };
            IM_ASSERT(IM_ARRAYSIZE(mouse_cursors_names) == ImGuiMouseCursor_COUNT);

            ImGuiMouseCursor current = ImGui::GetMouseCursor();
            ImGui::Text("Current mouse cursor = %d: %s", current, mouse_cursors_names[current]);
            ImGui::BeginDisabled(true);
            ImGui::CheckboxFlags("io.BackendFlags: HasMouseCursors", &io.BackendFlags, ImGuiBackendFlags_HasMouseCursors);
            ImGui::EndDisabled();

            ImGui::Text("Hover to see mouse cursors:");
            ImGui::SameLine(); HelpMarker(
                "Your application can render a different mouse cursor based on what ImGui::GetMouseCursor() returns. "
                "If software cursor rendering (io.MouseDrawCursor) is set ImGui will draw the right cursor for you, "
                "otherwise your backend needs to handle it.");
            for (int i = 0; i < ImGuiMouseCursor_COUNT; i++)
            {
                char label[32];
                sprintf(label, "Mouse cursor %d: %s", i, mouse_cursors_names[i]);
                ImGui::Bullet(); ImGui::Selectable(label, false);
                if (ImGui::IsItemHovered())
                    ImGui::SetMouseCursor(i);
            }
            ImGui::TreePop();
        }

        // Display Keyboard/Mouse state
        IMGUI_DEMO_MARKER("Inputs, Navigation & Focus/Keyboard, Gamepad & Navigation State");
        if (ImGui::TreeNode("Keyboard, Gamepad & Navigation State"))
        {
            // We iterate both legacy native range and named ImGuiKey ranges, which is a little odd but this allows displaying the data for old/new backends.
            // User code should never have to go through such hoops: old code may use native keycodes, new code may use ImGuiKey codes.
#ifdef IMGUI_DISABLE_OBSOLETE_KEYIO
            struct funcs { static bool IsLegacyNativeDupe(ImGuiKey) { return false; } };
            const ImGuiKey key_first = (ImGuiKey)ImGuiKey_NamedKey_BEGIN;
#else
            struct funcs { static bool IsLegacyNativeDupe(ImGuiKey key) { return key < 512 && ImGui::GetIO().KeyMap[key] != -1; } }; // Hide Native<>ImGuiKey duplicates when both exists in the array
            const ImGuiKey key_first = (ImGuiKey)0;
            //ImGui::Text("Legacy raw:");       for (ImGuiKey key = key_first; key < ImGuiKey_COUNT; key++) { if (io.KeysDown[key]) { ImGui::SameLine(); ImGui::Text("\"%s\" %d", ImGui::GetKeyName(key), key); } }
#endif
            ImGui::Text("Keys down:");          for (ImGuiKey key = key_first; key < ImGuiKey_COUNT; key = (ImGuiKey)(key + 1)) { if (funcs::IsLegacyNativeDupe(key)) continue; if (ImGui::IsKeyDown(key)) { ImGui::SameLine(); ImGui::Text("\"%s\" %d (%.02f secs)", ImGui::GetKeyName(key), key, ImGui::GetKeyData(key)->DownDuration); } }
            ImGui::Text("Keys pressed:");       for (ImGuiKey key = key_first; key < ImGuiKey_COUNT; key = (ImGuiKey)(key + 1)) { if (funcs::IsLegacyNativeDupe(key)) continue; if (ImGui::IsKeyPressed(key)) { ImGui::SameLine(); ImGui::Text("\"%s\" %d", ImGui::GetKeyName(key), key); } }
            ImGui::Text("Keys released:");      for (ImGuiKey key = key_first; key < ImGuiKey_COUNT; key = (ImGuiKey)(key + 1)) { if (funcs::IsLegacyNativeDupe(key)) continue; if (ImGui::IsKeyReleased(key)) { ImGui::SameLine(); ImGui::Text("\"%s\" %d", ImGui::GetKeyName(key), key); } }
            ImGui::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
            ImGui::Text("Chars queue:");        for (int i = 0; i < io.InputQueueCharacters.Size; i++) { ImWchar c = io.InputQueueCharacters[i]; ImGui::SameLine();  ImGui::Text("\'%c\' (0x%04X)", (c > ' ' && c <= 255) ? (char)c : '?', c); } // FIXME: We should convert 'c' to UTF-8 here but the functions are not public.

            // Draw an arbitrary US keyboard layout to visualize translated keys
            {
                const ImVec2 key_size = ImVec2(35.0f, 35.0f);
                const float  key_rounding = 3.0f;
                const ImVec2 key_face_size = ImVec2(25.0f, 25.0f);
                const ImVec2 key_face_pos = ImVec2(5.0f, 3.0f);
                const float  key_face_rounding = 2.0f;
                const ImVec2 key_label_pos = ImVec2(7.0f, 4.0f);
                const ImVec2 key_step = ImVec2(key_size.x - 1.0f, key_size.y - 1.0f);
                const float  key_row_offset = 9.0f;

                ImVec2 board_min = ImGui::GetCursorScreenPos();
                ImVec2 board_max = ImVec2(board_min.x + 3 * key_step.x + 2 * key_row_offset + 10.0f, board_min.y + 3 * key_step.y + 10.0f);
                ImVec2 start_pos = ImVec2(board_min.x + 5.0f - key_step.x, board_min.y);

                struct KeyLayoutData { int Row, Col; const char* Label; ImGuiKey Key; };
                const KeyLayoutData keys_to_display[] =
                {
                    { 0, 0, "", ImGuiKey_Tab },      { 0, 1, "Q", ImGuiKey_Q }, { 0, 2, "W", ImGuiKey_W }, { 0, 3, "E", ImGuiKey_E }, { 0, 4, "R", ImGuiKey_R },
                    { 1, 0, "", ImGuiKey_CapsLock }, { 1, 1, "A", ImGuiKey_A }, { 1, 2, "S", ImGuiKey_S }, { 1, 3, "D", ImGuiKey_D }, { 1, 4, "F", ImGuiKey_F },
                    { 2, 0, "", ImGuiKey_LeftShift },{ 2, 1, "Z", ImGuiKey_Z }, { 2, 2, "X", ImGuiKey_X }, { 2, 3, "C", ImGuiKey_C }, { 2, 4, "V", ImGuiKey_V }
                };

                // Elements rendered manually via ImDrawList API are not clipped automatically.
                // While not strictly necessary, here IsItemVisible() is used to avoid rendering these shapes when they are out of view.
                ImGui::Dummy(ImVec2(board_max.x - board_min.x, board_max.y - board_min.y));
                if (ImGui::IsItemVisible())
                {
                    ImDrawList* draw_list = ImGui::GetWindowDrawList();
                    draw_list->PushClipRect(board_min, board_max, true);
                    for (int n = 0; n < IM_ARRAYSIZE(keys_to_display); n++)
                    {
                        const KeyLayoutData* key_data = &keys_to_display[n];
                        ImVec2 key_min = ImVec2(start_pos.x + key_data->Col * key_step.x + key_data->Row * key_row_offset, start_pos.y + key_data->Row * key_step.y);
                        ImVec2 key_max = ImVec2(key_min.x + key_size.x, key_min.y + key_size.y);
                        draw_list->AddRectFilled(key_min, key_max, IM_COL32(204, 204, 204, 255), key_rounding);
                        draw_list->AddRect(key_min, key_max, IM_COL32(24, 24, 24, 255), key_rounding);
                        ImVec2 face_min = ImVec2(key_min.x + key_face_pos.x, key_min.y + key_face_pos.y);
                        ImVec2 face_max = ImVec2(face_min.x + key_face_size.x, face_min.y + key_face_size.y);
                        draw_list->AddRect(face_min, face_max, IM_COL32(193, 193, 193, 255), key_face_rounding, ImDrawFlags_None, 2.0f);
                        draw_list->AddRectFilled(face_min, face_max, IM_COL32(252, 252, 252, 255), key_face_rounding);
                        ImVec2 label_min = ImVec2(key_min.x + key_label_pos.x, key_min.y + key_label_pos.y);
                        draw_list->AddText(label_min, IM_COL32(64, 64, 64, 255), key_data->Label);
                        if (ImGui::IsKeyDown(key_data->Key))
                            draw_list->AddRectFilled(key_min, key_max, IM_COL32(255, 0, 0, 128), key_rounding);
                    }
                    draw_list->PopClipRect();
                }
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Capture override"))
        {
            HelpMarker(
                "The value of io.WantCaptureMouse and io.WantCaptureKeyboard are normally set by Dear ImGui "
                "to instruct your application of how to route inputs. Typically, when a value is true, it means "
                "Dear ImGui wants the corresponding inputs and we expect the underlying application to ignore them.\n\n"
                "The most typical case is: when hovering a window, Dear ImGui set io.WantCaptureMouse to true, "
                "and underlying application should ignore mouse inputs (in practice there are many and more subtle "
                "rules leading to how those flags are set).");

            ImGui::Text("io.WantCaptureMouse: %d", io.WantCaptureMouse);
            ImGui::Text("io.WantCaptureMouseUnlessPopupClose: %d", io.WantCaptureMouseUnlessPopupClose);
            ImGui::Text("io.WantCaptureKeyboard: %d", io.WantCaptureKeyboard);

            HelpMarker(
                "Hovering the colored canvas will override io.WantCaptureXXX fields.\n"
                "Notice how normally (when set to none), the value of io.WantCaptureKeyboard would be false when hovering and true when clicking.");
            static int capture_override_mouse = -1;
            static int capture_override_keyboard = -1;
            const char* capture_override_desc[] = { "None", "Set to false", "Set to true" };
            ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
            ImGui::SliderInt("SetNextFrameWantCaptureMouse()", &capture_override_mouse, -1, +1, capture_override_desc[capture_override_mouse + 1], ImGuiSliderFlags_AlwaysClamp);
            ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
            ImGui::SliderInt("SetNextFrameWantCaptureKeyboard()", &capture_override_keyboard, -1, +1, capture_override_desc[capture_override_keyboard + 1], ImGuiSliderFlags_AlwaysClamp);

            ImGui::ColorButton("##panel", ImVec4(0.7f, 0.1f, 0.7f, 1.0f), ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(256.0f, 192.0f)); // Dummy item
            if (ImGui::IsItemHovered() && capture_override_mouse != -1)
                ImGui::SetNextFrameWantCaptureMouse(capture_override_mouse == 1);
            if (ImGui::IsItemHovered() && capture_override_keyboard != -1)
                ImGui::SetNextFrameWantCaptureKeyboard(capture_override_keyboard == 1);

            ImGui::TreePop();
        }

        IMGUI_DEMO_MARKER("Inputs, Navigation & Focus/Tabbing");
        if (ImGui::TreeNode("Tabbing"))
        {
            ImGui::Text("Use TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            static char buf[32] = "hello";
            ImGui::InputText("1", buf, IM_ARRAYSIZE(buf));
            ImGui::InputText("2", buf, IM_ARRAYSIZE(buf));
            ImGui::InputText("3", buf, IM_ARRAYSIZE(buf));
            ImGui::PushAllowKeyboardFocus(false);
            ImGui::InputText("4 (tab skip)", buf, IM_ARRAYSIZE(buf));
            ImGui::SameLine(); HelpMarker("Item won't be cycled through when using TAB or Shift+Tab.");
            ImGui::PopAllowKeyboardFocus();
            ImGui::InputText("5", buf, IM_ARRAYSIZE(buf));
            ImGui::TreePop();
        }

        IMGUI_DEMO_MARKER("Inputs, Navigation & Focus/Focus from code");
        if (ImGui::TreeNode("Focus from code"))
        {
            bool focus_1 = ImGui::Button("Focus on 1"); ImGui::SameLine();
            bool focus_2 = ImGui::Button("Focus on 2"); ImGui::SameLine();
            bool focus_3 = ImGui::Button("Focus on 3");
            int has_focus = 0;
            static char buf[128] = "click on a button to set focus";

            if (focus_1) ImGui::SetKeyboardFocusHere();
            ImGui::InputText("1", buf, IM_ARRAYSIZE(buf));
            if (ImGui::IsItemActive()) has_focus = 1;

            if (focus_2) ImGui::SetKeyboardFocusHere();
            ImGui::InputText("2", buf, IM_ARRAYSIZE(buf));
            if (ImGui::IsItemActive()) has_focus = 2;

            ImGui::PushAllowKeyboardFocus(false);
            if (focus_3) ImGui::SetKeyboardFocusHere();
            ImGui::InputText("3 (tab skip)", buf, IM_ARRAYSIZE(buf));
            if (ImGui::IsItemActive()) has_focus = 3;
            ImGui::SameLine(); HelpMarker("Item won't be cycled through when using TAB or Shift+Tab.");
            ImGui::PopAllowKeyboardFocus();

            if (has_focus)
                ImGui::Text("Item with focus: %d", has_focus);
            else
                ImGui::Text("Item with focus: <none>");

            // Use >= 0 parameter to SetKeyboardFocusHere() to focus an upcoming item
            static float f3[3] = { 0.0f, 0.0f, 0.0f };
            int focus_ahead = -1;
            if (ImGui::Button("Focus on X")) { focus_ahead = 0; } ImGui::SameLine();
            if (ImGui::Button("Focus on Y")) { focus_ahead = 1; } ImGui::SameLine();
            if (ImGui::Button("Focus on Z")) { focus_ahead = 2; }
            if (focus_ahead != -1) ImGui::SetKeyboardFocusHere(focus_ahead);
            ImGui::SliderFloat3("Float3", &f3[0], 0.0f, 1.0f);

            ImGui::TextWrapped("NB: Cursor & selection are preserved when refocusing last used item in code.");
            ImGui::TreePop();
        }

        IMGUI_DEMO_MARKER("Inputs, Navigation & Focus/Dragging");
        if (ImGui::TreeNode("Dragging"))
        {
            ImGui::TextWrapped("You can use ImGui::GetMouseDragDelta(0) to query for the dragged amount on any widget.");
            for (int button = 0; button < 3; button++)
            {
                ImGui::Text("IsMouseDragging(%d):", button);
                ImGui::Text("  w/ default threshold: %d,", ImGui::IsMouseDragging(button));
                ImGui::Text("  w/ zero threshold: %d,", ImGui::IsMouseDragging(button, 0.0f));
                ImGui::Text("  w/ large threshold: %d,", ImGui::IsMouseDragging(button, 20.0f));
            }

            ImGui::Button("Drag Me");
            if (ImGui::IsItemActive())
                ImGui::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImGuiCol_Button), 4.0f); // Draw a line between the button and the mouse cursor

            // Drag operations gets "unlocked" when the mouse has moved past a certain threshold
            // (the default threshold is stored in io.MouseDragThreshold). You can request a lower or higher
            // threshold using the second parameter of IsMouseDragging() and GetMouseDragDelta().
            ImVec2 value_raw = ImGui::GetMouseDragDelta(0, 0.0f);
            ImVec2 value_with_lock_threshold = ImGui::GetMouseDragDelta(0);
            ImVec2 mouse_delta = io.MouseDelta;
            ImGui::Text("GetMouseDragDelta(0):");
            ImGui::Text("  w/ default threshold: (%.1f, %.1f)", value_with_lock_threshold.x, value_with_lock_threshold.y);
            ImGui::Text("  w/ zero threshold: (%.1f, %.1f)", value_raw.x, value_raw.y);
            ImGui::Text("io.MouseDelta: (%.1f, %.1f)", mouse_delta.x, mouse_delta.y);
            ImGui::TreePop();
        }
    }
}
