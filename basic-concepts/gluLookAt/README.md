# gluLookAt

This program demonstrates how the gluLookAt function can be used to change the camera position, angle and orientation.

Keyboard inputs as documented in the code can be used to change camera position.

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
cd opengl/basic-concepts/gluLookAt
mkdir build
cd build
cmake ..
make
./gluLookAt
```

**Front-view of a Wire Teapot**

![./images/front-view.png](./images/front-view.png)

**Top-view of a Wire Teapot**

![./images/top-view.png](./images/top-view.png)