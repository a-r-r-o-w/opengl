#include "GL/freeglut.h"
#include "glm/glm.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <random>

// global and state variables
namespace globals {
  const int screen_width = 720;
  const int screen_height = 720;
  const int total_points = 1'000'000;

  const glm::vec3 vertices[3] = {
    {-1.0f, -1.0f, 0.0f},
    {+1.0f, -1.0f, 0.0f},
    { 0.0f, +1.0f, 0.0f}
  };

  glm::vec3 vertex_colors[3] = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1}
  };

  glm::vec3 vertex_color_deltas[3] = {
    {0.001f * 4, 0.004f * 4, 0.002f * 4},
    {0.002f * 4, 0.005f * 4, 0.010f * 4},
    {0.015f * 4, 0.004f * 4, 0.001f * 4}
  };

  unsigned int animation_tick = 0;

  std::random_device device;
  std::mt19937 rng (device());
  std::uniform_real_distribution <> real_distribution (-1.0f, 1.0f);
  std::uniform_int_distribution <> int_distribution (0, 2);

  std::vector <glm::vec3> points;
}

void display ();
void timer (int);
void generate_points ();
glm::vec3 midway_point (const glm::vec3&, const glm::vec3&);
void draw_simple ();
void draw_random_colored ();
void draw_gradual_change ();
void draw_animated ();

int main (int argc, char** argv) {
  // generate points for sierpinski triangle
  generate_points();

  // inititalise GLUT
  glutInit(&argc, argv);
  glutInitWindowSize(globals::screen_width, globals::screen_height);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

  // create window and initialise callbacks
  glutCreateWindow("Sierpinski Triangle");
  glutTimerFunc(0, timer, 0);
  glutDisplayFunc(display);

  // run main event loop
  glutMainLoop();

  return 0;
}

// display callback called by GLUT for every render
void display () {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // draw_simple();
  // draw_random_colored();
  // draw_gradual_change();
  draw_animated();

  glutSwapBuffers();
}

// re-render scene after a fixed timestep
void timer (int value) {
  glutPostRedisplay();
  glutTimerFunc(16, timer, 0);
}

// generate points for the sierpinski triangle
void generate_points () {
  using namespace globals;

  glm::vec3 p = vertices[0];

  points.reserve(total_points);
  for (int i = 0; i < total_points; ++i) {
    auto q = midway_point(p, vertices[int_distribution(rng)]);
    points.push_back(q);
    p = q;
  }

  std::sort(points.begin(), points.end(), [] (auto &l, auto &r) {
    if (l.y < r.y)
      return true;
    if (l.y > r.y)
      return false;
    return l.x < r.x;
  });
}

// find the midpoint of two points
glm::vec3 midway_point (const glm::vec3 &p1, const glm::vec3 &p2) {
  return glm::vec3((p1.x + p2.x) / 2, (p1.y + p2.y) / 2, (p1.z + p2.z) / 2);
}

void draw_simple () {
  using namespace globals;

  glBegin(GL_POINTS);

  for (int i = 0; i < total_points; ++i)
    glVertex3f(points[i].x, points[i].y, points[i].z);

  glEnd();
}

void draw_random_colored () {
  using namespace globals;

  glBegin(GL_POINTS);

  float r = real_distribution(rng), g = real_distribution(rng), b = real_distribution(rng);
  r = (r + 1) / 2;
  g = (g + 1) / 2;
  b = (b + 1) / 2;

  for (int i = 0; i < total_points; ++i) {
    if (i % 100000 == 0) {
      r = real_distribution(rng);
      g = real_distribution(rng);
      b = real_distribution(rng);
      r = (r + 1) / 2;
      g = (g + 1) / 2;
      b = (b + 1) / 2;
    }
    glColor3f(r, g, b);
    glVertex3f(points[i].x, points[i].y, points[i].z);
  }

  glEnd();
}

void draw_gradual_change () {
  using namespace globals;

  glBegin(GL_POINTS);

  int factor = 10;
  float r = 0.0f, g = 1.0f, b = 0.5f, delta = 1.0f * factor / (total_points);

  for (int i = 0; i < total_points; ++i) {
    if (i % factor == 0) {
      r += delta;
      g -= delta;
      b -= delta / 2;
    }
    glColor3f(r, g, b);
    glVertex3f(points[i].x, points[i].y, points[i].z);
  }

  glEnd();
}

void draw_animated () {
  using namespace globals;

  glBegin(GL_POINTS);

  for (int i = 0; i < total_points; ++i) {
    float d0 = glm::distance(points[i], vertices[0]);
    float d1 = glm::distance(points[i], vertices[1]);
    float d2 = glm::distance(points[i], vertices[2]);

    // denominator has been approximated. actual value should be 2 * (1 + sqrt(5))
    float r = d0 * (vertex_colors[0].x + vertex_colors[1].x + vertex_colors[2].x) / 6;
    float g = d1 * (vertex_colors[0].y + vertex_colors[1].y + vertex_colors[2].y) / 6;
    float b = d2 * (vertex_colors[0].z + vertex_colors[1].z + vertex_colors[2].z) / 6;

    glColor3f(r, g, b);
    glVertex3f(points[i].x, points[i].y, points[i].z);
  }

  glEnd();

  ++animation_tick;

  if (animation_tick == 2) {
    animation_tick = 0;

    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        if ((vertex_colors[i][j] + vertex_color_deltas[i][j] <= 0.0f) or
            (vertex_colors[i][j] + vertex_color_deltas[i][j] >= 1.0f))
          vertex_color_deltas[i][j] = -vertex_color_deltas[i][j];
      }
      vertex_colors[i] += vertex_color_deltas[i];
    }
  }
}
