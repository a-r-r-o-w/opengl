#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace globals {
  const int width = 800;
  const int height = 600;

  // vertex shader
  const char *vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec3 pos;\n"
    "void main () {\n"
    "  gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
    "}\n";
  
  // fragment shader
  const char *fragment_shader_source =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main () {\n"
    "  FragColor = vec4(1.0f, 0.3f, 0.3f, 1.0f);\n"
    "}\n";
}

int main () {
  // initialise GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // create a GLFW window
  GLFWwindow *window = glfwCreateWindow(globals::width, globals::height, "Element Buffer Objects", nullptr, nullptr);

  // check if window was created successfully
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window!" << std::endl;
    glfwTerminate();
    return -1;
  }

  // make the newly created window the current OpenGL context
  glfwMakeContextCurrent(window);
  
  // handle resizing
  glfwSetFramebufferSizeCallback(window, [] ([[maybe_unused]] GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
  });
  
  // initialise GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD!" << std::endl;
    return -1;
  }

  // basic keyboard input processing to check if ESC key was pressed
  // and close the window if so
  auto process_input = [] (GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
  };

  // error handling
  int success;
  char infolog [512];

  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  // compile the vertex shader
  glShaderSource(vertex_shader, 1, &globals::vertex_shader_source, nullptr);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, nullptr, infolog);
    std::cerr << "Error compiling vertex shader\n" << infolog << std::endl;
  }

  // compile the fragment shader
  glShaderSource(fragment_shader, 1, &globals::fragment_shader_source, nullptr);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, nullptr, infolog);
    std::cerr << "Error compiling fragment shader\n" << infolog << std::endl;
  }

  // create a shader program
  unsigned int shader_program = glCreateProgram();

  // attach vertex and fragment shaders to shader program and perform linking
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, nullptr, infolog);
    std::cerr << "Error in linking shader\n" << infolog << std::endl;
  }

  // free up resources allocated for shaders
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  // vertices for our object
  float vertices[] = {
     0.5f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f
  };

  // order of indices that will be used to render our object with repeated vertices
  unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
  };

  unsigned int vbo, vao, ebo;

  // generate required buffers
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glGenVertexArrays(1, &vao);

  // bind buffers to generated IDs
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  
  // initialise buffers with data
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  
  // let OpenGL know how the data in the buffer has been organised
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  // unbind buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // uncomment below line to enable wireframe mode
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // main loop
  while (!glfwWindowShouldClose(window)) {
    process_input(window);

    // clear the screen every render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // use the shader program and draw our object
    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)0);

    // swap front and back buffers for our window
    glfwSwapBuffers(window);

    // handle events such as keypress
    glfwPollEvents();
  }

  // free up resources allocated for buffers
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(shader_program);

  // cleanup
  glfwTerminate();

  return 0;
}
