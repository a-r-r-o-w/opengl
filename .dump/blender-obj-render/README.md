# Blender OBJ Render

This program renders a model exported in OBJ format from Blender.

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
cd opengl/.dump/blender-obj-render
mkdir build
cd build
cmake ..
make
./blender-obj-render
```
