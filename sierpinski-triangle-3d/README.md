# Sierpinski Triangle in 3D (Sierpinski Pyramid)

This program generates the famous Sierpinski triangle/gasket/sieve fractal pattern in 3D. You can read more about it [here](https://en.wikipedia.org/wiki/Sierpi%C5%84ski_triangle).

The program uses the following algorithm:

```
1) create a pyramid with 4 arbitrary starting points
2) assign p as a random point within the pyramid
3) select a random vertex v of the pyramid
4) assign q as the midpoint of p and v
5) draw q
6) repeat 3-6 for many iterations
```

### Requirements

This program requires the following libraries/tools:

- [GLUT](https://www.opengl.org/resources/libraries/glut/glut_downloads.php)
- [GLM](https://github.com/g-truc/glm)
- [CMake](https://cmake.org/)

Installation instructions for Windows/Mac can be found online. Linux users can install all relevant requirements using `sudo apt-get freeglut3 freeglut3-dev libglm-dev cmake`.

### Build

If all libraries have been setup correctly, building the program is really simple.

```
git clone https://github.com/a-r-r-o-w/opengl
cd opengl/sierpinski-triangle-3d
mkdir build
cd build
cmake ..
make
./sierpinski-triangle-3d
```

If everything worked well, you have a Sierpinski Triangle in 3D!
