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

struct point {
  int x;
  int y;
  int z;

  point (int x = 0, int y = 0, int z = 0)
    : x (x), y (y), z (z)
  { }
};

namespace globals {
  int screen_width = 720;
  int screen_height = 720;

  int total_points = 20;
  int grid_size = 10;
  int animation_delay = 200;

  std::vector <point> points;
  
  glm::vec3 clear_color (0.1f, 0.1f, 0.1f);
  glm::vec3 point_color (1.0f, 0.0f, 0.0f);
  glm::vec3 invalid_hull_color (1.0f, 0.8f, 0.2f);
  glm::vec3 valid_hull_color (0.0f, 1.0f, 0.0f);
  glm::vec3 grid_color (0.3f, 0.3f, 0.3f);

  std::random_device device;
  std::mt19937 rng (device());
  std::uniform_int_distribution <> width_distribution (0 + grid_size, screen_width - grid_size);
  std::uniform_int_distribution <> height_distribution (0 + grid_size, screen_height - grid_size);
}

// function declarations

void handle_resize (int, int);

void generate_points ();

void display ();
void display_grid ();
void display_points ();
void display_graham_scan_hull (const std::vector <std::pair <point, int>>&);
void display_graham_scan_hull_partial (const std::vector <std::pair <point, int>>&, std::vector <bool>&);
void display_graham_scan_hull_creation (const std::vector <std::pair <point, int>>&);

int orientation (const point&, const point&, const point&);

std::vector <std::pair <point, int>> graham_scan_convex_hull ();

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

  glutMainLoop();

  return 0;
}

void handle_resize (int width, int height) {
  using namespace globals;

  screen_width = width;
  screen_height = height;

  glViewport(0, 0, width, height);
  glutPostRedisplay();
}

void generate_points () {
  using namespace globals;

  auto generate_point = [&] () {
    int x = width_distribution(rng) / grid_size * grid_size;
    int y = height_distribution(rng) / grid_size * grid_size;
    return point(x, y, 0);
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
  
  double width_dr = (double)screen_width / 2;
  double height_dr = (double)screen_height / 2;

  for (int i = 0; i < screen_height; i += grid_size) {
    for (int j = 0; j < screen_width; j += grid_size) {
      double x = (double)j / width_dr - 1.0;
      double y = (double)i / height_dr - 1.0;
      
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

  double width_dr = (double)screen_width / 2;
  double height_dr = (double)screen_height / 2;

  glBegin(GL_POINTS);
  glColor3f(point_color.x, point_color.y, point_color.z);

  for (auto &point: points) {
    glm::vec3 p (point.x, point.y, point.z);
    p.x = p.x / width_dr - 1.0f;
    p.y = p.y / height_dr - 1.0f;
    glVertex3f(p.x, p.y, p.z);
  }

  glEnd();
}

void display_graham_scan_hull (const std::vector <std::pair <point, int>> &hull) {
  using namespace globals;

  glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  display_grid();
  display_points();

  int n = hull.size();
  double width_dr = (double)screen_width / 2;
  double height_dr = (double)screen_height / 2;

  glBegin(GL_LINES);
  
  glColor3f(valid_hull_color.x, valid_hull_color.y, valid_hull_color.z);

  for (int i = 0; i < n; ++i) {
    auto &p1 = hull[i].first;
    auto &p2 = hull[(i + 1) % n].first;
    glm::vec3 n1 (p1.x, p1.y, p1.z);
    glm::vec3 n2 (p2.x, p2.y, p2.z);

    n1.x = n1.x / width_dr - 1.0;
    n2.x = n2.x / width_dr - 1.0;
    n1.y = n1.y / height_dr - 1.0;
    n2.y = n2.y / height_dr - 1.0;

    glVertex3f(n1.x, n1.y, n1.z);
    glVertex3f(n2.x, n2.y, n2.z);
  }

  glEnd();
  glFlush();
  glutSwapBuffers();
}

void display_graham_scan_hull_partial (const std::vector <std::pair <point, int>> &hull, std::vector <bool> &status) {
  using namespace globals;

  glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  display_grid();
  display_points();

  int n = hull.size();
  double width_dr = (double)screen_width / 2;
  double height_dr = (double)screen_height / 2;

  for (int j = 1; j < n; ++j) {
    auto &[p1, _] = hull[j - 1];
    auto &[p2, index] = hull[j];
    glm::vec3 n1 (p1.x, p1.y, p1.z);
    glm::vec3 n2 (p2.x, p2.y, p2.z);

    n1.x = n1.x / width_dr - 1.0;
    n2.x = n2.x / width_dr - 1.0;
    n1.y = n1.y / height_dr - 1.0;
    n2.y = n2.y / height_dr - 1.0;

    glBegin(GL_LINES);

    if (status[index])
      glColor3f(valid_hull_color.x, valid_hull_color.y, valid_hull_color.z);
    else
      glColor3f(invalid_hull_color.x, invalid_hull_color.y, invalid_hull_color.z);
    
    glVertex3f(n1.x, n1.y, n1.z);
    glVertex3f(n2.x, n2.y, n2.z);
    
    glEnd();
  }

  if (n > 2) {
    auto &p1 = hull.front().first;
    auto &p2 = hull.back().first;
    glm::vec3 n1 (p1.x, p1.y, p1.z);
    glm::vec3 n2 (p2.x, p2.y, p2.z);

    n1.x = n1.x / width_dr - 1.0;
    n2.x = n2.x / width_dr - 1.0;
    n1.y = n1.y / height_dr - 1.0;
    n2.y = n2.y / height_dr - 1.0;

    glPushAttrib(GL_ENABLE_BIT);
    glLineStipple(5, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);

    glColor3f(invalid_hull_color.x, invalid_hull_color.y, invalid_hull_color.z);
    glVertex3f(n1.x, n1.y, n1.z);
    glVertex3f(n2.x, n2.y, n2.z);
    
    glEnd();
    glPopAttrib();
  }

  glFlush();
  glutSwapBuffers();
}

void display_graham_scan_hull_creation (const std::vector <std::pair <point, int>> &hull) {
  using namespace globals;

  std::vector <std::pair <point, int>> current_hull;
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

int orientation (const point& a, const point& b, const point& c) {
  int value = a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);
  if (value < 0)
    return -1; // clockwise
  else if (value > 0)
    return +1; // counter clockwise
  return 0; // collinear
}

std::vector <std::pair <point, int>> graham_scan_convex_hull () {
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

  std::vector <std::pair <point, int>> stack;

  for (int i = 0; i < total_points; ++i) {
    int k = stack.size();

    while (k > 1 and orientation(stack[k - 2].first, stack[k - 1].first, points[i]) >= 0) {
      stack.pop_back();
      --k;
    }

    stack.push_back({points[i], i});
  }

  return stack;
}
