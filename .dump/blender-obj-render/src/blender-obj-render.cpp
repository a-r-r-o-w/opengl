#include "GL/freeglut.h"
#include "glm/glm.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <vector>

#define usable [[maybe_unused]]

// global and state variables
namespace globals {
  int screen_width = 720;
  int screen_height = 720;
  
  const int window_start_position_x = 100;
  const int window_start_position_y = 100;

  const glm::vec4 clear_color (0.2f, 0.2f, 0.2f, 1.0f);

  std::vector <glm::vec3> vertices;
  std::vector <std::tuple <int, int, int>> faces;
}

namespace callback {
  void display         ();
  void overlay_display ();

  void reshape (int, int);

  void keyboard    (unsigned char, int, int);
  void keyboard_up (unsigned char, int, int);

  void mouse       (int, int, int, int);
  void mouse_wheel (int, int, int, int);

  void motion         (int, int);
  void passive_motion (int, int);

  void idle ();
}

void read_mesh ();

int main (int argc, char** argv) {
  // initialize GLUT window size, position, display mode
  glutInit(&argc, argv);
  glutInitWindowSize(globals::screen_width, globals::screen_height);
  glutInitWindowPosition(globals::window_start_position_x, globals::window_start_position_y);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  // continue execution after window is closed by user
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
  // glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

  // create a window
  glutCreateWindow("Blender OBJ Render");

  // setup callbacks that GLUT calls depending on what event has to be handled
  glutDisplayFunc(callback::display);
  glutOverlayDisplayFunc(callback::overlay_display);
  glutReshapeFunc(callback::reshape);
  glutKeyboardFunc(callback::keyboard);
  glutKeyboardUpFunc(callback::keyboard_up);
  glutMouseFunc(callback::mouse);
  glutMouseWheelFunc(callback::mouse_wheel);
  glutMotionFunc(callback::motion);
  glutPassiveMotionFunc(callback::passive_motion);
  glutIdleFunc(callback::idle);

  // read mesh data
  read_mesh();

  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // start main loop
  gluLookAt(0, 1, 0, 0, 0, 0, 0, 0, 1); 
  glutMainLoop();

  return 0;
}

void read_mesh () {
  using globals::vertices;
  using globals::faces;

  // std::ifstream file ("../res/sphere.obj");
  std::ifstream file ("../res/torus.obj");
  // std::ifstream file ("../res/suzanne.obj");
  std::string line;
  std::stringstream ss;
  glm::vec3 v;

  while (not file.eof()) {
    std::getline(file, line);
    std::string type = line.substr(0, 2);

    if (type == "v ") {
      ss << line.substr(2);
      ss >> v.x >> v.y >> v.z;
      v.x *= 0.8;
      v.y *= 0.8;
      v.z *= 0.8;
      vertices.push_back(v);
      ss.clear();
    }
    else if (type == "f ") {
      int x, y, z;
      ss << line.substr(2);
      
      ss >> line;
      x = std::stoi(line.substr(0, line.find_first_of('/'))) - 1;
      ss >> line;
      y = std::stoi(line.substr(0, line.find_first_of('/'))) - 1;
      ss >> line;
      z = std::stoi(line.substr(0, line.find_first_of('/'))) - 1;

      faces.push_back({x, y, z});
      ss.clear();
    }
  }

  file.close();
}

namespace callback {

  void display () {
    using namespace globals;

    // clear color buffer
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
    for (auto &[x, y, z]: faces) {
      auto &v1 = vertices[x];
      auto &v2 = vertices[y];
      auto &v3 = vertices[z];
      glVertex3f(v1.x, v1.y, v1.z);
      glVertex3f(v2.x, v2.y, v2.z);
      glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();

    // swap back and front buffers used in rendering
    glutSwapBuffers();
  }

  void overlay_display () {

  }

  // 
  void reshape (int width, int height) {
    // set screen width and height to new values after reshape
    globals::screen_width = width;
    globals::screen_height = height;
    glViewport(0, 0, width, height);
    glutPostRedisplay();
  }

  void keyboard (usable unsigned char key, usable int x, usable int y) {

  }

  void keyboard_up (usable unsigned char key, usable int x, usable int y) {

  }

  void mouse (usable int button, usable int state, usable int x, usable int y) {

  }

  void mouse_wheel (usable int button, usable int dir, usable int x, usable int y) {

  }

  void motion (usable int x, usable int y) {

  }

  void passive_motion (usable int x, usable int y) {

  }

  void idle () {
    display();
  }

} // callback
