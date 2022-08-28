#include "GL/glut.h"
#include "glm/glm.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stack>
#include <thread>

namespace globals {
  int screen_width = 720;
  int screen_height = 720;

  int total_points = 20;
  int grid_size = 10;
  int animation_delay = 200;

  std::vector <glm::vec3> points;
  
  glm::vec3 clear_color (0.1f, 0.1f, 0.1f);
  glm::vec3 point_color (1.0f, 0.0f, 0.0f);
  glm::vec3 invalid_hull_color (1.0f, 0.8f, 0.2f);
  glm::vec3 valid_hull_color (0.0f, 1.0f, 0.0f);
  glm::vec3 grid_color (0.3f, 0.3f, 0.3f);

  std::random_device device;
  std::mt19937 rng (device());
  std::uniform_int_distribution <> distribution (0, 720);
}

// function declarations

void handle_resize (int, int);

void generate_points ();

void display ();
void display_grid ();
void display_points ();
void display_graham_scan_hull (const std::vector <std::pair <glm::vec3, int>>&);
void display_graham_scan_hull_partial (const std::vector <std::pair <glm::vec3, int>>&, std::vector <bool>&);
void display_graham_scan_hull_creation (const std::vector <std::pair <glm::vec3, int>>&);

int orientation (const glm::vec3&, const glm::vec3&, const glm::vec3&);

std::vector <std::pair <glm::vec3, int>> graham_scan_convex_hull (bool = false);

int main (int argc, char** argv) {
  // generate random set of points
  generate_points();

  // initialise GLUT
  glutInit(&argc, argv);

  glutInitWindowSize(globals::screen_width, globals::screen_height);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_RGB);

  glutCreateWindow("Convex Hull");
  glutDisplayFunc(display);
  glutReshapeFunc(handle_resize);
  
  // increase point size for better display
  glEnable(GL_POINT_SMOOTH);
  glPointSize(8.0f);

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  glutMainLoop();

  return 0;
}

void handle_resize (int width, int height) {
  globals::screen_width = width;
  globals::screen_height = height;
  glViewport(0, 0, width, height);
  glutPostRedisplay();
}

void generate_points () {
  using namespace globals;

  auto generate_point = [] () {
    glm::vec3 point;
    point.x = (double)distribution(rng) / ((double)screen_width / 2) - 1.0;
    point.y = (double)distribution(rng) / ((double)screen_height / 2) - 1.0;
    point.z = 0.0f;
    return point;
  };

  for (int i = 0; i < total_points; ++i)
    points.push_back(generate_point());
}

void display () {
  auto hull = graham_scan_convex_hull();
  
  // display_graham_scan_hull(hull);
  display_graham_scan_hull_creation(hull);
}

void display_grid () {
  using namespace globals;

  glBegin(GL_LINES);
  glColor3f(grid_color.x, grid_color.y, grid_color.z);

  for (int i = 0; i < screen_height; i += grid_size) {
    for (int j = 0; j < screen_width; j += grid_size) {
      double x = (double)i / ((double)screen_height / 2) - 1.0;
      double y = (double)i / ((double)screen_width / 2) - 1.0;
      glVertex3f(x, -1, 0.0f);
      glVertex3f(x, +1, 0.0f);
      glVertex3f(-1, y, 0.0f);
      glVertex3f(+1, y, 0.0f);
    }
  }

  glEnd();
}

void display_points () {
  using namespace globals;

  glBegin(GL_POINTS);
  glColor3f(point_color.x, point_color.y, point_color.z);

  for (auto &point: points)
    glVertex3f(point.x, point.y, point.z);

  glEnd();
}

void display_graham_scan_hull (const std::vector <std::pair <glm::vec3, int>> &hull) {
  using namespace globals;

  glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  display_grid();
  display_points();

  int n = hull.size();

  glBegin(GL_LINES);
  
  glColor3f(valid_hull_color.x, valid_hull_color.y, valid_hull_color.z);

  for (int i = 0; i < n; ++i) {
    auto &p1 = hull[i].first;
    auto &p2 = hull[(i + 1) % n].first;

    glVertex3f(p1.x, p1.y, p1.z);
    glVertex3f(p2.x, p2.y, p2.z);
  }

  glEnd();
  glFlush();
  glutSwapBuffers();
}

void display_graham_scan_hull_partial (const std::vector <std::pair <glm::vec3, int>> &hull, std::vector <bool> &status) {
  using namespace globals;

  glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  display_grid();
  display_points();

  int n = hull.size();

  for (int j = 1; j < n; ++j) {
    auto &[p1, _] = hull[j - 1];
    auto &[p2, index] = hull[j];

    glBegin(GL_LINES);

    if (status[index])
      glColor3f(valid_hull_color.x, valid_hull_color.y, valid_hull_color.z);
    else
      glColor3f(invalid_hull_color.x, invalid_hull_color.y, invalid_hull_color.z);
    
    glVertex3f(p1.x, p1.y, p1.z);
    glVertex3f(p2.x, p2.y, p2.z);
    
    glEnd();
  }

  if (n > 2) {
    glPushAttrib(GL_ENABLE_BIT);
    glLineStipple(5, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    
    glColor3f(invalid_hull_color.x, invalid_hull_color.y, invalid_hull_color.z);
    glVertex3f(hull.front().first.x, hull.front().first.y, hull.front().first.z);
    glVertex3f(hull.back().first.x, hull.back().first.y, hull.back().first.z);
    
    glEnd();
    glPopAttrib();
  }

  glFlush();
  glutSwapBuffers();
}

void display_graham_scan_hull_creation (const std::vector <std::pair <glm::vec3, int>> &hull) {
  using namespace globals;

  std::vector <std::pair <glm::vec3, int>> current_hull;
  std::vector <bool> status (total_points);

  for (auto &[p, index]: hull)
    status[index] = true;
  
  for (int i = 0; i < total_points; ++i) {
    current_hull.push_back({points[i], i});
    display_graham_scan_hull_partial(current_hull, status);
    
    if (not status[i])
      current_hull.pop_back();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(animation_delay));
  }

  display_graham_scan_hull(hull);
}

int orientation (const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
  double value = a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);

  if (value < 0)
    return -1; // clockwise
  else if (value > 0)
    return +1; // counter clockwise
  return 0; // collinear
}

std::vector <std::pair <glm::vec3, int>> graham_scan_convex_hull (bool include_collinear) {
  using namespace globals;

  auto p0 = *std::min_element(points.begin(), points.end(), [] (auto &l, auto &r) {
    return std::make_pair(l.y, l.x) < std::make_pair(r.y, r.x);
  });

  std::sort(points.begin(), points.end(), [&] (auto &l, auto &r) {
    int o = orientation(p0, l, r);

    if (o == 0) {
      auto lhs = (p0.x - l.x) * (p0.x - l.x) + (p0.y - l.y) * (p0.y - l.y);
      auto rhs = (p0.x - r.x) * (p0.x - r.x) + (p0.y - r.y) * (p0.y - r.y);
      return lhs < rhs;
    }

    return o < 0;
  });

  if (include_collinear) {
    int i = total_points - 1;
    while (i >= 0 and orientation(p0, points[i], points.back()) == 0)
      --i;
    std::reverse(points.begin() + i + 1, points.end());
  }

  std::vector <std::pair <glm::vec3, int>> stack;

  for (int i = 0; i < total_points; ++i) {
    int k = stack.size();

    while (k > 1) {
      int o = orientation(stack[k - 2].first, stack[k - 1].first, points[i]);

      if (!(o < 0 or (include_collinear and o == 0))) {
        stack.pop_back();
        --k;
      }
      else
        break;
    }

    stack.push_back({points[i], i});
  }

  return stack;
}
