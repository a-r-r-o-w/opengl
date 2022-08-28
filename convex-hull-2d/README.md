# Convex Hull

This program finds the Convex Hull of a set of points and visualizes what the different algorithms "see" when computing it. The Convex Hull of a set of points is defined as the smallest convex polygon that contains all the points in it. You can read more about it [here](https://en.wikipedia.org/wiki/Convex_hull).

The following algorithms have been implemented (a brief description of how the algorithms work is present towards the bottom of this page):

- Graham Scan

### Requirements

This program requires the following libraries/tools:

- [GLUT](https://www.opengl.org/resources/libraries/glut/glut_downloads.php)
- [GLM](https://github.com/g-truc/glm)
- [CMake](https://cmake.org/)

Installation instructions for Windows/Mac can be found online. Linux users can install all relevant requirements using `sudo apt install freeglut3 freeglut3-dev libglm-dev cmake`.

### Build

If all libraries have been setup correctly, building the program is really simple.

```
git clone https://github.com/a-r-r-o-w/opengl
cd opengl/convex-hull-2d
mkdir build
cd build
cmake ..
make
./convex-hull-2d
```

### Visualization

**Graham Scan**

https://user-images.githubusercontent.com/72266394/187084087-8ddd964d-d2c2-4cba-9763-fe99986a93a2.mp4
