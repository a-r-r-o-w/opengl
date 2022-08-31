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

  int total_points = 50;
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
void display_hull (const std::vector <std::pair <point, int>>&);
void display_graham_scan_hull_partial (const std::vector <std::pair <point, int>>&, std::vector <bool>&);
void display_graham_scan_hull_creation (const std::vector <std::pair <point, int>>&);
void display_andrew_monotone_chain_hull_partial (
  const std::vector <std::pair <point, int>>&,
  const std::vector <std::pair <point, int>>&,
  const std::vector <bool>&
);
void display_andrew_monotone_chain_hull_creation ();

int orientation (const point&, const point&, const point&);

std::vector <std::pair <point, int>> graham_scan_convex_hull ();
std::vector <std::pair <point, int>> andrew_monotone_chain_convex_hull ();

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
  /* (1) graham scan convex hull visualization */
  // auto hull = graham_scan_convex_hull();
  // display_hull(hull);

  /* (2) graham scan convex hull step-by-step creation visualization */
  // auto hull = graham_scan_convex_hull();
  // display_graham_scan_hull_creation(hull);

  /* (3) andrew monotone chain convex hull visualization */
  // auto hull = andrew_monotone_chain_convex_hull();
  // display_hull(hull);

  /* (4) andrew monotone chain convex hull creation visualization */
  display_andrew_monotone_chain_hull_creation();
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

void display_hull (const std::vector <std::pair <point, int>> &hull) {
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

  for (int i = 1; i < n; ++i) {
    auto &[p1, _] = hull[i - 1];
    auto &[p2, index] = hull[i];
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

  display_hull(hull);
}

void display_andrew_monotone_chain_hull_partial (
  const std::vector <std::pair <point, int>>& upper_hull,
  const std::vector <std::pair <point, int>>& lower_hull,
  const std::vector <bool>& status
) {
  using namespace globals;

  glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  display_grid();
  display_points();

  int upper_size = upper_hull.size();
  int lower_size = lower_hull.size();
  double width_dr = (double)screen_width / 2;
  double height_dr = (double)screen_height / 2;

  for (int i = 1; i < upper_size; ++i) {
    auto &[p1, _] = upper_hull[i - 1];
    auto &[p2, index] = upper_hull[i];
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

  if (upper_size > 2) {
    auto &p1 = upper_hull.front().first;
    auto &p2 = upper_hull.back().first;
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

  for (int i = 1; i < lower_size; ++i) {
    auto &[p1, _] = lower_hull[i - 1];
    auto &[p2, index] = lower_hull[i];
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

  if (lower_size > 2) {
    auto &p1 = lower_hull.front().first;
    auto &p2 = lower_hull.back().first;
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

void display_andrew_monotone_chain_hull_creation () {
  using namespace globals;

  std::sort(points.begin(), points.end(), [] (auto &l, auto &r) {
    return std::make_pair(l.x, l.y) < std::make_pair(r.x, r.y);
  });

  point p1 = points.front(), p2 = points.back();
  std::vector <std::pair <point, int>> upper_hull, lower_hull, hull;
  std::vector <bool> status (total_points, true);

  upper_hull.push_back({p1, 0});
  lower_hull.push_back({p1, 0});

  for (int i = 1; i < total_points; ++i) {
    if (i == total_points - 1 or orientation(p1, points[i], p2) < 0) {
      int k = upper_hull.size();

      while (k > 1 and orientation(upper_hull[k - 2].first, upper_hull[k - 1].first, points[i]) >= 0) {
        status[upper_hull.back().second] = false;
        upper_hull.pop_back();
        --k;
      }
      
      upper_hull.push_back({points[i], i});
    }

    if (i == total_points - 1 or orientation(p1, points[i], p2) > 0) {
      int k = lower_hull.size();

      while (k > 1 and orientation(lower_hull[k - 2].first, lower_hull[k - 1].first, points[i]) <= 0) {
        status[lower_hull.back().second] = false;
        lower_hull.pop_back();
        --k;
      }

      lower_hull.push_back({points[i], i});
    }

    display_andrew_monotone_chain_hull_partial(upper_hull, lower_hull, status);
    std::this_thread::sleep_for(std::chrono::milliseconds(animation_delay));
  }

  for (int i = 0; i < (int)upper_hull.size(); ++i)
    hull.push_back(upper_hull[i]);
  
  for (int i = lower_hull.size() - 2; i >= 0; --i)
    hull.push_back(lower_hull[i]);
  
  display_hull(hull);
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

std::vector <std::pair <point, int>> andrew_monotone_chain_convex_hull () {
  using namespace globals;

  std::sort(points.begin(), points.end(), [] (auto &l, auto &r) {
    return std::make_pair(l.x, l.y) < std::make_pair(r.x, r.y);
  });

  point p1 = points.front(), p2 = points.back();
  std::vector <std::pair <point, int>> upper_hull, lower_hull, hull;

  upper_hull.push_back({p1, 0});
  lower_hull.push_back({p1, 0});

  for (int i = 1; i < total_points; ++i) {
    if (i == total_points - 1 or orientation(p1, points[i], p2) < 0) {
      int k = upper_hull.size();

      while (k > 1 and orientation(upper_hull[k - 2].first, upper_hull[k - 1].first, points[i]) >= 0) {
        upper_hull.pop_back();
        --k;
      }
      
      upper_hull.push_back({points[i], i});
    }

    if (i == total_points - 1 or orientation(p1, points[i], p2) > 0) {
      int k = lower_hull.size();

      while (k > 1 and orientation(lower_hull[k - 2].first, lower_hull[k - 1].first, points[i]) <= 0) {
        lower_hull.pop_back();
        --k;
      }

      lower_hull.push_back({points[i], i});
    }
  }

  for (int i = 0; i < (int)upper_hull.size(); ++i)
    hull.push_back(upper_hull[i]);
  
  for (int i = lower_hull.size() - 2; i >= 0; --i)
    hull.push_back(lower_hull[i]);
  
  return hull;
}
