#include "GL/freeglut.h"
#include "glm/glm.hpp"

// global and state variables
namespace globals {
  const int screen_width = 720;
  const int screen_height = 720;
  const int total_points = 1'000'000;

  glm::vec3 shape_color (0.6f, 0.6f, 0.6f);
  glm::vec3 background_color (0.2f, 0.2f, 0.2f);

  glm::vec3 camera (0, 0, 1);
  glm::vec3 up (0, 1, 0);
}

void display ();
void keypress_handler (unsigned char, int, int);

int main (int argc, char** argv) {
  // initialize GLUT
  glutInit(&argc, argv);
  glutInitWindowSize(globals::screen_width, globals::screen_height);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow("gluLookAt");

  // add callbacks for GLUT to call for different events
  glutDisplayFunc(display);
  glutKeyboardFunc(keypress_handler);
  
  // set background clear color
  glClearColor(globals::background_color.x, globals::background_color.y, globals::background_color.z, 0.0f);

  glEnable(GL_DEPTH_TEST);

  // start main loop
  glutMainLoop();

  return 0;
}

// handle keyboard input
void keypress_handler (unsigned char key, int x, int y) {
  using namespace globals;

  switch (key) {
    case 'w': {
      // sets camera on the negative z axis with up vector pointing to positive y direction
      camera.x = 0; camera.y = 0; camera.z = -1;
      up.x = 0; up.y = 1; up.z = 0;
    }
    break;
    
    case 's': {
      // sets camera on the positive z axis with up vector pointing to positive y direction
      camera.x = 0; camera.y = 0; camera.z = +1;
      up.x = 0; up.y = 1; up.z = 0;
    }
    break;
    
    case 'a': {
      // sets camera on the negative x axis with up vector pointing to positive y direction
      camera.x = -1; camera.y = 0; camera.z = 0;
      up.x = 0; up.y = 1; up.z = 0;
    }
    break;

    case 'd': {
      // sets camera on the positive x axis with up vector pointing to positive y direction
      camera.x = +1; camera.y = 0; camera.z = 0;
      up.x = 0; up.y = 1; up.z = 0;
    }
    break;

    case 'q': {
      // sets camera on the positive y axis with up vector pointing to positive x direction
      camera.x = 0; camera.y = +1; camera.z = 0;
      up.x = 1; up.y = 0; up.z = 0;
    }
    break;

    case 'e': {
      // sets camera on the negative y axis with up vector pointing to positive x direction
      camera.x = 0; camera.y = -1; camera.z = 0;
      up.x = 1; up.y = 0; up.z = 0;
    }
    break;
  }

  // re-render scene
  glutPostRedisplay();
}

// display callback called by GLUT for every render
void display () {
  using namespace globals;

  // clear color and depth buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // load modelview matrix and place camera at correct location
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(camera.x, camera.y, camera.z, 0, 0, 0, up.x, up.y, up.z);

  glPushMatrix();
    glColor3f(shape_color.x, shape_color.y, shape_color.z);
    glutWireTeapot(0.6f);
  glPopMatrix();

  // swap front and back buffers used for rendering
  glutSwapBuffers();
}
