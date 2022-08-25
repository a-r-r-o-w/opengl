#include "GL/glut.h"
#include "glm/glm.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <random>

namespace globals {
  const int screen_width = 720;
  const int screen_height = 720;
  const int total_points = 2'000'000;

  const glm::vec3 vertices[] = {
    { 0.0f, +1.0f,  0.0f},
    {-1.0f, -1.0f, -1.0f},
    {+1.0f, -1.0f, -1.0f},
    { 0.0f, -1.0f, +1.0f}
  };

  const std::vector <int> triangle_indices[] = {
    {0, 1, 2},
    {0, 1, 3},
    {0, 2, 3},
    {1, 2, 3}
  };

  glm::vec3 vertex_colors[] = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
    {1, 1, 1}
  };

  glm::vec3 vertex_color_deltas[] = {
    {0.001f * 4, 0.004f * 4, 0.002f * 4},
    {0.002f * 4, 0.005f * 4, 0.010f * 4},
    {0.015f * 4, 0.004f * 4, 0.001f * 4},
    {0.005f * 4, 0.010f * 4, 0.002f * 4}
  };

  unsigned int animation_tick = 0;

  std::random_device device;
  std::mt19937 rng (device());
  std::uniform_real_distribution <> real_distribution (-1.0f, 1.0f);
  std::uniform_int_distribution <> int_distribution (0, 3);

  std::vector <glm::vec3> points;
}

/* function declarations */

void timer (int);
void display ();
void generate_points ();
glm::vec3 midway_point (const glm::vec3&, const glm::vec3&);
glm::vec3 generate_point_inside_pyramid ();
void handle_keypress (unsigned char, int, int);
void draw_simple ();
void draw_random_colored ();
void draw_gradual_change ();
void draw_animated ();

int main (int argc, char** argv) {
  // generate points using randomised algorithm to create
  // the Sierpinski pyramid
  generate_points();

  // basic GLUT stuff
  glutInit(&argc, argv);

  glutInitWindowSize(globals::screen_width, globals::screen_height);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);

  glutCreateWindow("Sierpinski Triangle");
  glutDisplayFunc(display);
  glutTimerFunc(0, timer, 0);
  glutKeyboardFunc(handle_keypress);
  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glOrtho(-2, 2, -2, 2, 2, -2);

  glutMainLoop();

  return 0;
}

/* re-render every few milliseconds */
void timer (int value) {
  glutPostRedisplay();
  glutTimerFunc(20, timer, 0);
}

/* display function that handles what's currently being rendered */
void display () {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw_simple();
  // draw_random_colored();
  // draw_gradual_change();
  draw_animated();

  glFlush();
}

/* helper function to generate points required to render Sierpinski Pyramid using
   randomised algorithm */
void generate_points () {
  using namespace globals;

  glm::vec3 p = generate_point_inside_pyramid();

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

/* helper function to find midpoint of two points */
glm::vec3 midway_point (const glm::vec3 &p1, const glm::vec3 &p2) {
  return glm::vec3((p1.x + p2.x) / 2, (p1.y + p2.y) / 2, (p1.z + p2.z) / 2);
}

/* helper function to generate a point inside the specified Pyramid */
glm::vec3 generate_point_inside_pyramid () {
  using namespace globals;
  
  static auto abc = glm::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]);
  static auto abd = glm::cross(vertices[1] - vertices[0], vertices[3] - vertices[0]);
  static auto acd = glm::cross(vertices[2] - vertices[0], vertices[3] - vertices[0]);
  static auto bcd = glm::cross(vertices[2] - vertices[1], vertices[3] - vertices[1]);

  auto is_point_inside_pyramid = [&] (const glm::vec3 &point) {
    auto d1 = glm::dot(point, abc);
    auto d2 = glm::dot(point, abd);
    auto d3 = glm::dot(point, acd);
    auto d4 = glm::dot(point, bcd);

    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0) || (d4 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0) || (d4 > 0);

    return !(has_neg and has_pos);
  };

  glm::vec3 point;

  while (true) {
    point = {real_distribution(rng), real_distribution(rng), real_distribution(rng)};

    if (is_point_inside_pyramid(point))
      return point;
  }
}

/* keypress handler */
void handle_keypress (unsigned char key, int x, int y) {
  if (key == 'w')
    glRotatef(1, 1, 0, 0);
  else if (key == 's')
    glRotatef(-1, 1, 0, 0);
  else if (key == 'a')
    glRotatef(1, 0, 1, 0);
  else if (key == 'd')
    glRotatef(-1, 0, 1, 0);
  else if (key == 'q')
    glRotatef(1, 0, 0, 1);
  else if (key == 'e')
    glRotatef(-1, 0, 0, 1);
  glutPostRedisplay();
}

/* draws a simple white Sierpinski Pyramid */
void draw_simple () {
  using namespace globals;

  glBegin(GL_POINTS);

  glColor3f(1, 1, 1);

  for (auto &point: points)
    glVertex3f(point.x, point.y, point.z);

  glEnd();
}

/* draws a randomly colored Sierpinski Pyramid which looks terrible */
void draw_random_colored () {
  using namespace globals;

  float r, g, b;

  glBegin(GL_POINTS);

  for (int i = 0; i < total_points; ++i) {
    if (i % 100000 == 0) {
      r = real_distribution(rng);
      g = real_distribution(rng);
      b = real_distribution(rng);
      r = (r + 1) / 2;
      g = (g + 1) / 2;
      b = (b + 1) / 2;
      glColor3f(r, g, b);
    }
    glVertex3f(points[i].x, points[i].y, points[i].z);
  }

  glEnd();
}

/* draws a Sierpinski pyramid with a gradually changing color gradient */
void draw_gradual_change () {
  using namespace globals;

  int factor = 10;
  float r = 0.0f, g = 1.0f, b = 0.5f, delta = 1.0f * factor / (total_points);

  glBegin(GL_POINTS);

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

/* draws a Sierpinski pyramid with color changing animation */
void draw_animated () {
  using namespace globals;

  glBegin(GL_POINTS);

  for (int i = 0; i < total_points; ++i) {
    float d0 = glm::distance(points[i], vertices[0]);
    float d1 = glm::distance(points[i], vertices[1]);
    float d2 = glm::distance(points[i], vertices[2]);
    float d3 = glm::distance(points[i], vertices[3]);

    // denominator has been manually chosen (opengl will clamp values if out of bounds)
    // actual value should be 2 * (1 + sqrt(5)) + sqrt(6)?
    float r = (d0 * vertex_colors[0].x + d1 * vertex_colors[1].x + d2 * vertex_colors[2].x + d3 * vertex_colors[3].x) / 6;
    float g = (d0 * vertex_colors[0].y + d1 * vertex_colors[1].y + d2 * vertex_colors[2].y + d3 * vertex_colors[3].y) / 6;
    float b = (d0 * vertex_colors[0].z + d1 * vertex_colors[1].z + d2 * vertex_colors[2].z + d3 * vertex_colors[3].z) / 6;

    glColor3f(r, g, b);
    glVertex3f(points[i].x, points[i].y, points[i].z);
  }

  glEnd();

  ++animation_tick;

  if (animation_tick == 2) {
    animation_tick = 0;

    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 3; ++j) {
        if ((vertex_colors[i][j] + vertex_color_deltas[i][j] <= 0.0f) or
            (vertex_colors[i][j] + vertex_color_deltas[i][j] >= 1.0f))
          vertex_color_deltas[i][j] = -vertex_color_deltas[i][j];
      }
      vertex_colors[i] += vertex_color_deltas[i];
    }
  }
}
