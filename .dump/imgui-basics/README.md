# ImGUI Basics

Interactive objects

### Installation

For Windows and Mac users, you must download/compile GLFW and name the library `libglfw.so` and place it in a directory where CMake can find it. Linux users can run `apt update && apt install libglfw3-dev` to install all requirements (tested on Kali, Ubuntu, Arch).

```
git clone --recursive https://github.com/a-r-r-o-w/opengl
cd opengl/.dump/imgui-basics
mkdir build
cd build
cmake ..
make
./imgui-basics
```

If everything goes well, you should get something that looks like:

![imgui-basics.png](./images/imgui-basics.png)
