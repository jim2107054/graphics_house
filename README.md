# Cinematic Halloween Haunted House at Night (Modern OpenGL 3.3 Core)

A modern, photorealistic-ready 3D rendering project in C++ and OpenGL (Core Profile 3.3).

---

## 📁 Project Architecture

```text
opengl-project/
├── CMakeLists.txt             # Cross-platform CMake build configuration
├── run.bat                    # One-click Windows compile & launch script
├── include/                   # Third-party library headers
│   ├── glad/gl.h              # Modern OpenGL 3.3 function loader
│   ├── GLFW/glfw3.h           # Windowing, context & input system
│   ├── glm/                   # Header-only OpenGL Mathematics (vec3, mat4, transforms)
│   ├── stb_image.h            # Lightweight image/texture loader
│   └── KHR/
├── lib/                       # Precompiled binaries (GLFW for MinGW/MSVC)
├── src/                       # C++ Source Code
│   ├── main.cpp               # 1600x900 viewport, render loop, scene assembly & lighting
│   ├── glad.c                 # GLAD runtime loader implementation
│   ├── Shader.h               # Modern GLSL program compiler & uniform manager
│   └── Camera.h               # Cinematic FPS/Flythrough camera (Pitch/Yaw/FOV)
├── shaders/                   # GLSL 330 core shaders
│   ├── scene.vert             # Vertex transformation & Normal Matrix calculations
│   ├── scene.frag             # Multi-light Blinn-Phong & exponential Halloween fog
│   ├── sky.vert               # Sky backdrop vertex shader
│   └── sky.frag               # Gradient midnight sky & glowing Halloween moon disc
└── assets/                    # Textures and 3D Models
    ├── textures/
    └── models/
```

---

## 🚀 Build & Run Instructions (Windows)

### Option 1: One-Click Run (Fastest)
Double-click `run.bat` or run in Command Prompt / PowerShell:
```cmd
.\run.bat
```

### Option 2: CMake + MinGW / Ninja
```powershell
# 1. Generate build files
cmake -B build -G "MinGW Makefiles"

# 2. Compile executable
cmake --build build

# 3. Launch application
.\build\HalloweenHauntedHouse.exe
```

### Option 3: Visual Studio / MSVC
```powershell
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
.\build\Release\HalloweenHauntedHouse.exe
```

---

## 🎮 Interactive Controls
- **W / A / S / D**: Cinematic Camera Movement (Forward / Left / Backward / Right)
- **SPACE / Left CTRL**: Fly Up / Down
- **Mouse**: First-person look around (Euler angles Pitch / Yaw)
- **Mouse Scroll**: Zoom in / out (Dynamic Field of View)
- **TAB**: Toggle Wireframe Mode (`glPolygonMode`)
- **ESC**: Exit Application

---

## 🎓 Viva Voce & Technical Defense Notes

| Technical Concept | Implementation in Project | Viva Explanation |
| :--- | :--- | :--- |
| **Core Profile 3.3** | `glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE)` | Strips out deprecated immediate mode (`glBegin/glEnd`). All rendering is strictly programmable via GLSL shaders and GPU buffers. |
| **VBO / VAO Pipeline** | `glGenVertexArrays`, `glGenBuffers`, `glVertexAttribPointer` | `VBO` stores raw geometry vertices in VRAM. `VAO` binds and caches vertex attribute layouts (Pos: 0, Normal: 1, UV: 2) for zero CPU overhead per draw call. |
| **Delta-Time (`deltaTime`)** | `float current = glfwGetTime(); deltaTime = current - last;` | Decouples animation and camera movement from framerate fluctuations (e.g. 60 FPS vs 144 FPS). |
| **sRGB / Linear Color** | `glEnable(GL_FRAMEBUFFER_SRGB)` | Renders lighting math in physical linear space, then automatically applies hardware gamma correction ($2.2$) before displaying on screen. |
| **Z-Buffer Depth Test** | `glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);` | Evaluates fragment depth in normalized device coordinates; discards occluded geometry behind closer fragments. |
| **Blinn-Phong Lighting** | `vec3 H = normalize(L + V); pow(max(dot(N, H), 0.0), shininess);` | Uses the halfway vector $H$ between light direction $L$ and view direction $V$, eliminating specular distortion at acute view angles. |
| **Distance Attenuation** | $1.0 / (k_c + k_l \cdot d + k_q \cdot d^2)$ | Physics-inspired quadratic light falloff for the glowing pumpkin lantern. |
| **Atmospheric Night Fog**| $\text{mix}(\text{fogColor}, \text{shadedColor}, e^{-(d \cdot \text{density})^2})$ | Exponential squared fog that dynamically submerges distant geometry into dark midnight blue. |
