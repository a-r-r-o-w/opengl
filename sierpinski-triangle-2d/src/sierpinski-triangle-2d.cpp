#include "GL/glut.h"
#include "glm/vec3.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <random>

namespace globals {
  const int screen_width = 720;
  const int screen_height = 720;
  const int total_points = 1'000'000;

  const glm::vec3 vertices[3] = {
    {-1.0f, -1.0f, 0.0f},
    {+1.0f, -1.0f, 0.0f},
    { 0.0f, +1.0f, 0.0f}
  };

  std::random_device device;
  std::mt19937 rng (device());
  std::uniform_real_distribution <> real_distribution (-1.0f, 1.0f);
  std::uniform_int_distribution <> int_distribution (0, 2);

  std::vector <glm::vec3> points;
}

void display ();
void generate_points ();
glm::vec3 midway_point (const glm::vec3&, const glm::vec3&);
glm::vec3 generate_point_inside_triangle ();
void draw_simple ();
void draw_random_colored ();
void draw_gradual_change ();

int main (int argc, char** argv) {
  generate_points();

  glutInit(&argc, argv);

  glutInitWindowSize(globals::screen_width, globals::screen_height);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_RGB);

  glutCreateWindow("Sierpinski Triangle");
  glutDisplayFunc(display);
  glutMainLoop();

  return 0;
}

void display () {
  using namespace globals;
  
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // draw_simple();
  // draw_random_colored();
  draw_gradual_change();

  glutSwapBuffers();
}

void generate_points () {
  using namespace globals;

  glm::vec3 p = generate_point_inside_triangle();

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

glm::vec3 midway_point (const glm::vec3 &p1, const glm::vec3 &p2) {
  return glm::vec3((p1.x + p2.x) / 2, (p1.y + p2.y) / 2, (p1.z + p2.z) / 2);
}

glm::vec3 generate_point_inside_triangle () {
  using namespace globals;

  auto sign = [] (auto &p1, auto &p2, auto &p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
  };

  auto is_point_inside_triangle = [&] (const glm::vec3 &point) {
    auto d1 = sign(point, vertices[0], vertices[1]);
    auto d2 = sign(point, vertices[1], vertices[2]);
    auto d3 = sign(point, vertices[2], vertices[0]);

    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg and has_pos);
  };

  glm::vec3 point;

  while (true) {
    point = {real_distribution(rng), real_distribution(rng), 0.0f};

    if (is_point_inside_triangle(point))
      return point;
  }
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
