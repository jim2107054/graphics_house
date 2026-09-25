# Horror House at Night — 3D Computer Graphics Project

**Developer:** MD Jahid Hasan Jim  
**Roll Number:** 2107054  
**Course:** Computer Graphics Sessional (CSE 4-1)  
**Framework:** C++ / Legacy OpenGL (Fixed-Function Pipeline) with FreeGLUT / GLUT  

---

## 🎬 Project Overview

An eerie, presentation-ready 3D **"Horror House at Night"** scene built from scratch in C++ and OpenGL. The project visually demonstrates all **FOUR core computer graphics light models** operating concurrently within a single cohesive, atmospheric environment inspired by classic gothic horror aesthetics.

### 🌟 Key Visual & Technical Highlights
- **4 Distinct Light Types** working simultaneously, individually toggleable with live status UI.
- **Atmospheric Fog (`GL_FOG` - `GL_EXP2`)** with dark blue midnight horizon matching.
- **Dynamic Planar Projected Shadows** driven by low-angle moonlight projection matrices.
- **Sinister Carved Jack-o'-Lanterns** with glowing eyes, toothy mouths, and internal candlelight halos.
- **Swaying & Flickering Porch Bulb** with physical harmonic pendulum motion and filament flicker.
- **Full Gothic Mansion Architecture**: Multi-pitch gabled roofs, octagonal spire tower, stone base, porch, tilted hanging door, window muntins, and broken furniture.
- **Abandoned Environment**: Rusted metal car catching specular highlights, bare twisted trees, rocks, tombstones, and crooked picket fence.
- **First-Person Camera** (WASD + Mouse Look) and **Cinematic Auto-Tour Mode (`C`)**.
- **2D HUD Overlay & Title Screen** with glowing borders, live FPS counter, reticle, and vignette.

---

## 💡 The Four Lighting Types

| Light # | OpenGL Identifier | Light Model | World Object | Key Characteristics |
|---|---|---|---|---|
| **Light 0** | `GL_LIGHT0` | **Point Light** | Porch Hanging Bulb | Positional ($w=1.0$), warm amber, distance attenuation, harmonic $\sin(t)$ pendulum sway & random filament flicker. |
| **Light 1** | `GL_LIGHT1` | **Directional Light** | Full Moon Sky | Directional ($w=0.0$), parallel cool blue rays from infinity, casts dramatic planar ground shadows. |
| **Light 2** | `GL_LIGHT2` | **Spot Light** | First-Person Flashlight | Positional ($w=1.0$), $18.5^\circ$ cone cutoff (`GL_SPOT_CUTOFF`), exponent $28.0$, follows camera eye and view vector every frame. |
| **Light 3** | `GL_LIGHT3` | **Area Light Emulation** | Parlor Window Glow | Point light source placed at window opening with elevated ambient ($k_a$) dispersion to simulate soft radiant window illumination. |

---

## 🎮 Controls & Shortcuts

| Key / Input | Action |
|---|---|
| **`W` / `A` / `S` / `D`** | Move Forward / Strafe Left / Move Backward / Strafe Right |
| **Mouse Motion** | Look Around (Yaw & Clamped Pitch) |
| **`Space` / `Ctrl` (or `X`)** | Fly Up / Fly Down |
| **`1`** | Toggle Point Light (Porch Bulb) |
| **`2`** | Toggle Directional Light (Moonlight) |
| **`3` / `F`** | Toggle Spot Light (Flashlight) |
| **`4`** | Toggle Area Light (Window Interior Glow) |
| **`0`** | Master Switch (Toggle ALL Lights) |
| **`G`** | Toggle Fog (`GL_FOG`) |
| **`B`** | Toggle Bulb Pendulum Sway & Random Flicker |
| **`C`** | Toggle Hands-Free Cinematic Auto-Tour Presentation Mode |
| **`H`** | Toggle In-Game HUD & Controls Overlay |
| **`P`** | Take Screenshot (Saves as uncompressed 24-bit `.bmp`) |
| **`R`** | Reset Camera to Starting Position |
| **`Enter` / `Space`** | Start Simulation from Title Screen |
| **`ESC`** | Exit Application |

---

## 🛠️ How to Build and Run

All required header files and FreeGLUT libraries are pre-packaged directly in `include/` and `lib/` for 100% standalone portability.

### Option 1: Quick Batch Script (Windows)
Double-click `run.bat` or run in terminal:
```cmd
run.bat
```

### Option 2: Direct GCC/G++ Command Line (MinGW / MSYS2)
```cmd
g++ -std=c++17 src/main.cpp -Iinclude -Llib -lfreeglut -lopengl32 -lglu32 -lgdi32 -lwinmm -o main.exe
main.exe
```

### Option 3: Using Make
```cmd
make run
```

### Option 4: Using CMake
```cmd
mkdir build
cd build
cmake ..
cmake --build .
./HorrorHouseAtNight.exe
```

---

## 🎓 Viva Defense Quick-Reference

- **Q: How does OpenGL differentiate between a Point Light and a Directional Light?**  
  *A:* By the 4th element ($w$) of the position array passed to `glLightfv(..., GL_POSITION, pos)`:
  - $w = 1.0f \implies$ **Point / Positional Light** at coordinates $(x, y, z)$.
  - $w = 0.0f \implies$ **Directional Light** whose parallel rays travel along direction vector $(-x, -y, -z)$.

- **Q: How do `GL_SPOT_CUTOFF` and `GL_SPOT_EXPONENT` control a Spotlight?**  
  *A:* `GL_SPOT_CUTOFF` specifies the half-angle of the light cone ($0^\circ$ to $90^\circ$). `GL_SPOT_EXPONENT` defines the power factor modulating radial falloff concentration from the central axis $\cos^\alpha(\theta)$ towards the cone edge.

- **Q: Why does Legacy OpenGL require Area Light Emulation?**  
  *A:* The fixed-function pipeline natively only supports mathematical point, directional, and spot lights (delta Dirac distributions). True area lights with soft penumbras require surface integral evaluation or Monte Carlo ray-tracing shaders. We emulate it using an offset point source with high ambient dispersion ($k_a$) and soft distance attenuation.

- **Q: How do Planar Projected Shadows work?**  
  *A:* We multiply the model matrix by a $4 \times 4$ planar projection matrix derived from the plane equation $Ax+By+Cz+D=0$ and the light vector $L$, flattening object vertices onto $Y=0$, rendered in a dark semi-transparent blending pass with lighting disabled.
