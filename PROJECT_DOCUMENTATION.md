# 🏚️ 3D Horror House at Night — Comprehensive Project & Viva Defense Report

**Course:** Computer Graphics Laboratory (CSE 4-1)  
**Developer:** MD Jahid Hasan Jim  
**Roll / Student ID:** 2107054  
**Technologies:** C++17, OpenGL Fixed-Function Pipeline, FreeGLUT, WinMM Audio, GLU  
**Generated PDF Document:** [Horror_House_Project_Report_Jim_2107054.pdf](file:///y:/4-1/Computer%20Graphics/oengl/opengl-project/opengl-project/Horror_House_Project_Report_Jim_2107054.pdf)

---

## 1. Executive Summary & Architecture

This project is a real-time 3D graphical haunted house environment implemented in C++ using OpenGL and FreeGLUT. The application demonstrates fundamental and advanced computer graphics techniques including hierarchical geometric modeling, multi-source dynamic lighting (point, directional, spot, area, and emissive), procedural audio synthesis, physics-based simulations, particle systems, anti-aliased depth management, and collision detection.

### Global Scene Transformation & Hierarchy
- **Coordinate Space:** Right-handed 3D Cartesian coordinates ($+X$ = Right, $+Y$ = Up, $+Z$ = Out of screen / Toward camera).
- **House Root Transformation:** 
  $$\mathbf{M}_{\text{house}} = \mathbf{T}(-2.80, 0.0, -11.50) \cdot \mathbf{R}_Y(-18.0^\circ)$$
  All house wings, tower spires, porch, windows, and interior furniture are modeled in house-local space and transformed through the OpenGL matrix stack (`glPushMatrix` / `glPopMatrix`).

---

## 2. 4-Light Illumination Model & Shading

The scene incorporates four primary dynamic OpenGL light sources:

| Light Index | Type | Position / Direction | Color & Tint | Special Behavior |
| :--- | :--- | :--- | :--- | :--- |
| **`GL_LIGHT0`** | **Point Light** | Porch fixture $(-3.37, 2.76, 3.42)$ | Warm Amber $(1.0, 0.72, 0.28)$ | Dynamic pendulum swing & voltage flicker |
| **`GL_LIGHT1`** | **Directional** | Moon Vector $(0.28, 0.85, 0.44)$ | Pale Cyan $(0.16, 0.22, 0.35)$ | Constant global moonlight casting dark shadows |
| **`GL_LIGHT2`** | **Spotlight** | Camera Position $(X_{\text{cam}}, Y_{\text{cam}}, Z_{\text{cam}})$ | White $(0.95, 0.95, 0.90)$ | Cutoff $24^\circ$, Exponent $18.0$, moves with player head |
| **`GL_LIGHT3`** | **Area Light** | House Interior | Golden $(0.45, 0.30, 0.08)$ | Interior glow streaming through windows |
| **Candles** | **Emissive** | Pumpkin Lanterns $(14\times)$ | Orange $(1.0, 0.55, 0.05)$ | Localized noise-based flame flickering |

---

## 3. Object-by-Object Transformation, Coordinate & Primitive Breakdown

*(Use this section for instant answers during Viva & Defense questions)*

### 🏠 Main Haunted House Structure
- **Left Wing (Living Area):**
  - *Base Primitive:* Unit Cube (`drawBox`).
  - *Transformed Dimensions:* Width = $6.2\text{m}$, Height = $4.8\text{m}$, Depth = $8.2\text{m}$.
  - *Local Position:* $(-6.0, 2.4, 0.0)$ | *World Position:* $(-8.8, 2.4, -11.5)$.
  - *Function:* `drawHouseWing()` (`src/main.cpp:2840`).
- **Central Gothic Tower & Spire:**
  - *Base Primitive:* Unit Cube base + 4-Sided Pyramid (`drawPrismRoof`).
  - *Transformed Dimensions:* Base $3.8\text{m} \times 8.8\text{m}$, Spire Height = $6.5\text{m}$.
  - *Local Position:* $(-1.8, 4.4, 0.5)$.
  - *Function:* `drawHouse()` (`src/main.cpp:2880`).
- **Right Wing (Extension):**
  - *Base Primitive:* Unit Cube (`drawBox`) with intersecting gabled roof.
  - *Transformed Dimensions:* Width = $4.8\text{m}$, Height = $4.2\text{m}$, Depth = $6.5\text{m}$.
  - *Local Position:* $(3.2, 2.1, -0.8)$.

### 🪟 Windows (Front, Sides, and Back — 12 Total)
- *Base Primitive:* Recessed Wall Quad + Outer Trim Frame Box + Glowing Glass Quad + Crossbar Grids.
- *Transformation Chain:* `glTranslatef(x, y, z) -> glRotatef(rotY, 0, 1, 0) -> glScalef(w, h, depth)`.
  - **Left Wall Windows:** $X = -9.00\text{m}$, $\text{RotY} = -90^\circ$.
  - **Right Wall Windows:** $X = 6.05\text{m}$, $\text{RotY} = 90^\circ$.
  - **Back Wall Windows:** $Z = -3.85\text{m} / -3.38\text{m}$, $\text{RotY} = 180^\circ$.
  - **Front Wall Windows:** $Z = 4.12\text{m}$, $\text{RotY} = 0^\circ$.
- *Anti-Glitch Fix:* Depth offset of $+0.02\text{m}$ for frame and $+0.006\text{m}$ for glass core to eliminate all Z-buffer contention.
- *Function:* `drawHouseWindow()` (`src/main.cpp:2780`).

### 🚪 Front Door & Entrance Passage
- *Base Primitive:* Compound Archway Box (`drawBox`) + Recessed Dark Plank Door.
- *Local Position:* $(-4.6, 1.2, 4.12)$ with opening span $X \in [-5.4, -3.8]$.
- *Collision Handling:* The doorway is the **only portal** where `isHouseLocationFree()` returns `true`, allowing player entry into the interior.

### 🚗 Abandoned Vintage Car
- *Base Primitive:* Lower Chassis Box ($4.4\text{m} \times 0.6\text{m} \times 2.1\text{m}$) + Slanted Cabin Box ($2.4\text{m} \times 0.9\text{m} \times 1.9\text{m}$) + 4 Cylinder Wheels ($R=0.42\text{m}$) + Shattered Windshield (White line-strip spiderweb).
- *World Position:* $(X = 7.4\text{m}, Z = -5.2\text{m})$, Yaw = $-32.0^\circ$, Wheel tilt = $3.5^\circ$ embedded in mud.
- *Function:* `drawHauntedCar()` (`src/main.cpp:2500`).

### 🪵 Leaning Historic Telephone Pole
- *Base Primitive:* Tapered Cylinder ($R_{\text{base}}=0.14\text{m}, R_{\text{top}}=0.08\text{m}, H=8.5\text{m}$) + 2 Horizontal Crossarm Boxes + 4 Cylinder Insulators + Sagging Wire Line Strips.
- *World Position:* $(X = 9.2\text{m}, Z = -4.5\text{m})$, Soil Lean = $6.5^\circ$.
- *Function:* `drawTelephonePole()` (`src/main.cpp:2580`).

### 🎃 Jack-o'-Lantern Pumpkins (14 Total)
- *Base Primitive:* 12 Latitudinal Ribbed Slices using `drawSphere` + Arched Cylinder Stem + Subtracted Triangle Eye/Mouth Vertices.
- *Positions:* Placed along both sides of the stone pathway from $Z = -8.5\text{m}$ to $Z = 14.0\text{m}$.
- *Dynamic Behavior:* Emissive light flickering modulated by continuous sinusoidal noise function.
- *Function:* `drawCarvedPumpkin()` (`src/main.cpp:2150`).

### 🏚️ Dilapidated Interior Furniture
- **Collapsed Dining Table:** Two split tabletop boxes (`drawBox`), with one side dropped to the floorboards and the other propped on clay bricks.
- **Overturned Wooden Chairs:** Broken leg spindles ($0.25\text{m}$ vs $0.45\text{m}$) and slanted backrests tilted at $45^\circ$ on the floor.
- **Rotted Floorboards & Rafters:** Gapped, angled floor planks with dark void spaces beneath.
- **Cobwebs:** Translucent alpha-blended radial fan polygons (`GL_TRIANGLE_FAN`) across ceiling corners.
- *Function:* `drawDilapidatedInterior()` (`src/main.cpp:3200`).

### 👻 Summonable Ghost Apparition
- *Base Primitive:* Procedurally deformed tapered sphere mesh with flowing trailing hem vertices and two dark spherical hollow eye sockets.
- *Spawning & Motion:* Spawns at house doorway $(-4.6, 1.45, -3.2)$ and swoops outward to $(+1.5\text{m})$ with pulsing scale and alpha transparency fade.
- *Audio:* Accompanied by procedural spooky audio synthesis (`PlaySoundA`).
- *Function:* `drawGhostApparition()` (`src/main.cpp:3320`), triggered by **`[E]`** or **`[J]`**.

---

## 4. Dynamic Simulations & Mathematical Modeling

1. **Pendulum Porch Light Simulation:**
   $$\theta(t) = \theta_{\text{amp}} \cdot \sin(\omega t + \phi)$$
   The light position in `GL_LIGHT0` is updated continuously in world space to match the swinging fixture tip.

2. **6-DOF Airborne Falling Leaves Engine:**
   - 45 active particle instances.
   - Wind velocity vector: $\vec{v}_{\text{wind}} = (-1.0, -0.4, -0.15)\text{ m/s}$.
   - Individual 3D tumbling rotation rates $(\omega_x, \omega_y, \omega_z)$.
   - Continuous ground-level collision respawn into the natural tree canopy.

3. **Camera & Wall Collision Detection:**
   Transforms world coordinates $(x, z)$ into house-local space via inverse rotation:
   $$\begin{pmatrix} x_{\text{loc}} \\ z_{\text{loc}} \end{pmatrix} = \begin{pmatrix} \cos(18^\circ) & -\sin(18^\circ) \\ \sin(18^\circ) & \cos(18^\circ) \end{pmatrix} \begin{pmatrix} x - x_{\text{orig}} \\ z - z_{\text{orig}} \end{pmatrix}$$
   Bounds testing evaluates bounding boxes for exterior walls and permits passage only through the door aperture $([-5.4, -3.8], Z \approx 4.78)$.

---

## 5. Keyboard & Demonstration Cheat-Sheet

| Key | Action / Feature Demonstrated |
| :---: | :--- |
| **`[E]` / `[J]`** | **Summon Ghost Apparition with Spooky Sound Effect** |
| **`[1]`** | Toggle Porch Bulb Point Light (`GL_LIGHT0`) |
| **`[2]`** | Toggle Directional Moonlight (`GL_LIGHT1`) |
| **`[3]` / `[F]`** | Toggle First-Person Camera Flashlight (`GL_LIGHT2`) |
| **`[4]`** | Toggle Window Interior Area Glow (`GL_LIGHT3`) |
| **`[5]` / `[K]`** | Toggle Pumpkin Candle Flicker |
| **`[0]`** | Master Toggle (All Lights ON / OFF) |
| **`[B]`** | Toggle Porch Bulb Harmonic Sway & Flicker |
| **`[C]`** | Toggle Cinematic Camera Auto-Tour |
| **`[L]`** | Trigger Manual Lightning Strike with Thunder |
| **`[T]`** | Toggle Texture Mapping ON / OFF |
| **`[G]`** | Toggle Atmospheric Distance Fog |
| **`[M]`** | Toggle Procedural Ambient Audio Loop |
| **`[P]`** | Capture Screenshot (`.bmp`) |
| **`[R]`** | Reset Camera to Vantage Point |
| **`[W][A][S][D] + Mouse`** | First-Person Walk Navigation |

---

## 6. Live Viva Code-Modification Guide

If the examiner asks you to modify any scene property on the spot:

1. **Reposition the Vintage Car:**  
   Go to `src/main.cpp` line ~7610, find:
   ```cpp
   drawHauntedCar(7.4f, -5.2f, -32.0f);
   ```
   Modify `7.4f` ($X$) or `-5.2f` ($Z$) to place the car anywhere on the terrain.

2. **Resize the House Wings:**  
   Go to `drawHouse()` in `src/main.cpp` line ~2840:
   ```cpp
   drawHouseWing(-6.0f, 0.0f, 0.0f, 6.2f, 4.8f, 8.2f);
   ```
   Edit `6.2f` (width) or `4.8f` (height).

3. **Change Window Light Colors:**  
   Go to `drawHouseWindow()` in `src/main.cpp` line ~2780:
   ```cpp
   glColor4f(1.0f, 0.72f, 0.25f, 1.0f); // Amber glow
   ```
   Change to `glColor4f(0.2f, 1.0f, 0.3f, 1.0f)` for an eerie haunted green glow.

4. **Change Ghost Spawn Speed / Duration:**  
   In `GhostEntity g_ghost` (`src/main.cpp:530`), change `g_ghost.duration` or target coordinates.
