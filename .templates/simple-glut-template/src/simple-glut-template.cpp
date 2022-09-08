#include "GL/freeglut.h"
#include "glm/glm.hpp"

#define usable [[maybe_unused]]

// global and state variables
namespace globals {
  int screen_width = 720;
  int screen_height = 720;
  
  const int window_start_position_x = 100;
  const int window_start_position_y = 100;

  const glm::vec4 clear_color (0.2f, 0.2f, 0.2f, 1.0f);
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

int main (int argc, char** argv) {
  // initialize GLUT window size, position, display mode
  glutInit(&argc, argv);
  glutInitWindowSize(globals::screen_width, globals::screen_height);
  glutInitWindowPosition(globals::window_start_position_x, globals::window_start_position_y);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

  // continue execution after window is closed by user
  // glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

  // create a window
  glutCreateWindow("Simple GLUT Template");

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

  // start main loop
  glutMainLoop();

  return 0;
}

namespace callback {

  void display () {
    using namespace globals;

    // clear color buffer
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT);

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
