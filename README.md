# OpenGL Starter Project

A clean, minimal OpenGL 3.3 Core Profile starter template configured with GLFW, GLAD, GLM, and STB Image.

## 📁 Project Structure

```text
├── include/       # GLFW, GLAD, GLM, stb_image header files
├── lib/           # GLFW static and dynamic libraries
├── shaders/       # (Optional) Directory for vertex and fragment shaders
├── src/
│   ├── main.cpp       # Minimal starter application (window, render loop, event handling)
│   ├── glad.c         # GLAD OpenGL loader implementation
│   └── stb_image.cpp  # stb_image implementation
├── CMakeLists.txt # CMake configuration
├── Makefile       # Make configuration
├── run.bat        # Quick one-click build and run script for Windows (MinGW/g++)
└── glfw3.dll      # GLFW 64-bit Windows dynamic link library
```

## 🚀 How to Run

### Option 1: Quick Batch Script (Windows MinGW)
Double-click or run from terminal:
```cmd
run.bat
```

### Option 2: Using Make
```cmd
make run
```

### Option 3: Using CMake
```cmd
mkdir build
cd build
cmake ..
cmake --build .
```
