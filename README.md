# Horror House at Night — 3D Computer Graphics Project

**Developer:** MD Jahid Hasan Jim  
**Roll Number:** 2107054  
**Course:** Computer Graphics Sessional (CSE 4-1)  
**Framework:** C++ / Legacy OpenGL (Fixed-Function Pipeline) with FreeGLUT / GLUT  

---

## 🎬 Project Overview

An eerie, presentation-ready 3D **"Horror House at Night"** scene built from scratch in C++ and OpenGL. The project visually demonstrates all **FOUR core computer graphics light models** operating concurrently with **Texture Mapping (`GL_MODULATE`)** within a single cohesive, atmospheric environment inspired by classic gothic horror aesthetics.

### 🌟 Key Visual & Technical Highlights
- **4 Distinct Light Types** working simultaneously, individually toggleable with live status UI.
- **Texture Mapping with `GL_MODULATE`**: Weathered wood planks, slate roof shingles, wet dirt/mud ground, cobblestone pathway, gnarly bark, peeling paint & rust on the car, and celestial moon.
- **Robust Fallback**: Uses `stb_image` to load PNG/JPG/BMP textures from `textures/`. If an image file is missing, it dynamically synthesizes procedural textures in memory so it never crashes!
- **Realistic Undulating Terrain & 3 Reflective Puddles**: Smooth heightmap elevation with finite-difference normals and mirror-like puddles catching moon and bulb reflections.
- **Irregular Mossy Rocks & Boulders**: Perturbed low-poly faceted rock generator with top-facing moss tinting, partially embedded into the mud.
- **Abandoned Environmental Clutter**: Smashed wooden crate with spilling planks, upright & mud-tilted barrels, fallen fence pickets, rusty metal bucket, scattered bricks, tangled dead bushes, and an antique leaning wrought-iron lamppost with glowing carriage lantern.
- **Atmospheric Fog (`GL_FOG` - `GL_EXP2`)** with dark blue midnight horizon matching.
- **Dynamic Planar Projected Shadows** driven by low-angle moonlight projection matrices for house, car, trees, and clutter props.
- **Sinister Carved Jack-o'-Lanterns** with glowing eyes, toothy mouths, and internal candlelight halos.
- **Swaying & Flickering Porch Bulb** with physical harmonic pendulum motion and filament flicker.
- **Detailed Abandoned Rusted Car**: Beveled hood and trunk contours, sloped windshield with spiderweb glass cracks, ajar driver's door showing steering wheel & bench seat, dangling side mirror, dual headlights, chrome slotted grille, heavy bumpers with overriders, and deflated flat tyre listing the car into the mud with high metallic specular glints.
- **First-Person Camera** (WASD + Mouse Look) and **Cinematic Auto-Tour Mode (`C`)**.
- **2D HUD Overlay & Title Screen** with glowing borders, live FPS counter, reticle, and vignette.

---

## 📁 Where to Put Texture Files

Image files belong in the **`textures/`** directory in your project root:
```text
opengl-project/
├── textures/
│   ├── wall.png (or .jpg / .bmp)   # Weathered wood planks for house walls & doors
│   ├── roof.png (or .jpg / .bmp)   # Dark roof shingles / tiles
│   ├── ground.png (or .jpg / .bmp) # Wet dirt / mud terrain
│   ├── stone.png (or .jpg / .bmp)  # Cobblestone pathway & stone foundation
│   ├── bark.png (or .jpg / .bmp)   # Tree trunks & branches
│   ├── rust.png (or .jpg / .bmp)   # Peeling paint & rusted car metal
│   └── moon.png (or .jpg / .bmp)   # Lunar surface texture
├── include/
├── lib/
├── src/
│   └── main.cpp
├── run.bat
└── run.ps1
```
*Note: You can drop any standard `.png`, `.jpg`, or `.bmp` file into `textures/` with these names. If any file is omitted, procedural textures are synthesized automatically.*

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
| **`1`** | Toggle Point Light (Porch Bulb with Moving Shadows & Light Pool) |
| **`2`** | Toggle Directional Light (Moonlight with Planar Shadows) |
| **`3` / `F`** | Toggle Spot Light (Flashlight with Volumetric Fog Beam) |
| **`4`** | Toggle Area Light (Window Interior Glow) |
| **`0`** | Master Switch (Toggle ALL Lights) |
| **`T`** | Toggle Texture Mapping ON / OFF (`GL_MODULATE`) |
| **`G`** | Toggle Fog (`GL_FOG`) |
| **`B`** | Toggle Bulb Pendulum Sway & Random Flicker |
| **`L`** | Trigger Distant Lightning Strike (Multi-Pulse Flash & Sky Burst) |
| **`M`** | Toggle Procedural Ambient Audio & Thunder (WinMM) |
| **`C`** | Toggle Hands-Free Cinematic Auto-Tour Presentation Mode |
| **`H`** | Toggle In-Game HUD & Controls Overlay |
| **`P`** | Take Screenshot (Saves as uncompressed 24-bit `.bmp`) |
| **`R`** | Reset Camera to Default Reference Image View |
| **`Enter` / `Space`** | Start Simulation from Title Screen |
| **`ESC`** | Exit Application |

---

## 🛠️ How to Build and Run

### Option 1: Quick Batch Script (Windows)
Double-click `run.bat` or run in terminal:
```cmd
.\run.bat
```

### Option 2: PowerShell Script
```powershell
.\run.ps1
```

### Option 3: Direct GCC/G++ Command Line
```cmd
g++ -std=c++17 src/main.cpp -Iinclude -Llib -lfreeglut -lopengl32 -lglu32 -lgdi32 -lwinmm -o main.exe
.\main.exe
```

---

## 🎓 Viva Defense Quick-Reference

- **Q: Why use `GL_MODULATE` instead of `GL_REPLACE` for texture mapping?**  
  *A:* `GL_REPLACE` overwrites polygon colors completely, discarding lighting calculations. `GL_MODULATE` multiplies the texture color ($C_t$) with the computed Phong lighting ($C_l$) such that $C = C_t \times C_l$, preserving all ambient, diffuse, specular highlights, and shadows on textured surfaces.

- **Q: How does `GL_REPEAT` prevent texture stretching?**  
  *A:* When UV texture coordinates exceed $1.0$, `GL_REPEAT` tiles the texture periodically based on the fractional part of $U$ and $V$, maintaining uniform resolution across large walls, roofs, and terrain.

- **Q: How does OpenGL differentiate between a Point Light and a Directional Light?**  
  *A:* By the 4th element ($w$) of the position array passed to `glLightfv(..., GL_POSITION, pos)`:
  - $w = 1.0f \implies$ **Point / Positional Light** at coordinates $(x, y, z)$.
  - $w = 0.0f \implies$ **Directional Light** whose parallel rays travel along direction vector $(-x, -y, -z)$.

- **Q: How do `GL_SPOT_CUTOFF` and `GL_SPOT_EXPONENT` control a Spotlight?**  
  *A:* `GL_SPOT_CUTOFF` specifies the half-angle of the light cone ($0^\circ$ to $90^\circ$). `GL_SPOT_EXPONENT` defines the power factor modulating radial falloff concentration from the central axis $\cos^\alpha(\theta)$ towards the cone edge.

- **Q: How do Dynamic Planar Projected Shadows and the Swinging Light Pool work?**  
  *A:* We compute a $4 \times 4$ projection matrix that flattens geometry onto planes ($y = 0.725$ for porch floor, $y = 0$ for terrain) along rays cast from the light position. For the swaying porch bulb, the light position $(x(t), y(t), z(t), 1.0)$ swings dynamically, causing the cast shadows of porch balusters and furniture to move across the floor in exact counter-motion with the swinging amber light pool.

- **Q: Why does Legacy OpenGL require Area Light Emulation?**  
  *A:* The fixed-function pipeline natively only supports mathematical delta point, directional, and spot lights. True area lights with soft penumbras require surface integral evaluation or Monte Carlo ray-tracing. We emulate it using an offset point source with high ambient dispersion ($k_a$) and soft distance attenuation.
