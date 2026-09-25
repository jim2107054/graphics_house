/*
================================================================================
  COMPUTER GRAPHICS FINAL PROJECT: HORROR HOUSE AT NIGHT
  Developer : MD Jahid Hasan Jim
  Roll No   : 2107054
  Framework : C++ / Legacy OpenGL (Fixed-Function Pipeline) with GLUT / FreeGLUT
================================================================================

================================================================================
  THE FOUR LIGHT TYPES IN THIS SCENE (VIVA & DEMONSTRATION REFERENCE)
================================================================================

  1. POINT LIGHT (GL_LIGHT0) - Porch Hanging Bulb
     * Definition   : A positional light emitting equally in all 360-degree directions 
                      from a single point in space (homogenous coordinate w = 1.0).
     * Implementation: Position set via glLightfv(GL_LIGHT0, GL_POSITION, pos) where
                      pos = (x, y, z, 1.0f).
     * Attenuation  : Decays with distance via 1 / (kc + kl*d + kq*d^2) using
                      GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION, GL_QUADRATIC_ATTENUATION.
     * Animation    : Sways dynamically like a pendulum using sin(t) / cos(t) harmonic 
                      functions; flickers randomly to simulate an aging incandescent filament.

  2. DIRECTIONAL LIGHT (GL_LIGHT1) - Moonlit Sky
     * Definition   : Light rays arrive parallel from an infinite distance (such as the 
                      sun or moon). There is no specific position, only a direction vector 
                      (homogenous coordinate w = 0.0).
     * Implementation: Position set via glLightfv(GL_LIGHT1, GL_POSITION, dir) where
                      dir = (-dx, -dy, -dz, 0.0f).
     * Attenuation  : Distance attenuation does NOT apply to directional lights.
     * Visual Role  : Low-angle cool blue-white illumination casting long, dramatic planar 
                      shadows across the yard and over the ground.

  3. SPOT LIGHT (GL_LIGHT2) - First-Person Flashlight
     * Definition   : A positional light source constrained to emit cone-shaped light 
                      in a specific direction vector.
     * Implementation: 
         - Position : GL_POSITION with w = 1.0f (placed at the camera/player eye position).
         - Direction: GL_SPOT_DIRECTION matches the camera's forward view vector.
         - Cone Cutoff: GL_SPOT_CUTOFF set to ~18.5 degrees (half-angle of cone spread).
         - Falloff  : GL_SPOT_EXPONENT set to ~28.0 (controls edge concentration/smoothness).
     * Visual Role  : Moves and rotates synchronously with the player's first-person view.

  4. AREA LIGHT EMULATION (GL_LIGHT3) - Warm Glowing House Window
     * Viva Note    : Legacy OpenGL fixed-function pipeline natively only supports idealized 
                      point, directional, and spot light sources (delta light distributions). 
                      True area/geometric lights require surface integral formulations or 
                      modern physically-based ray tracing / Monte Carlo sampling.
     * Emulation    : We emulate an area light by placing a warm amber point light at the 
                      geometric center of the window opening with moderate attenuation, 
                      coupled with an elevated ambient reflection term (ka) and warm emissive 
                      window geometry. This produces a soft, diffuse radiant spread illuminating 
                      the porch floor, walls, and interior.

================================================================================
  VIVA QUICK-ANSWER CHEAT-SHEET
================================================================================
  Q1: How does OpenGL distinguish between a Point Light and a Directional Light?
  A1: Through the 4th parameter (w) of GL_POSITION: w = 1.0 denotes a positional Point Light
      at (x, y, z); w = 0.0 denotes a Directional Light whose rays travel along (-x, -y, -z).

  Q2: How do GL_SPOT_CUTOFF and GL_SPOT_EXPONENT shape a spotlight?
  A2: GL_SPOT_CUTOFF defines the half-angle of the emission cone (0° to 90°, or 180° for uniform).
      GL_SPOT_EXPONENT controls the intensity falloff from the central axis to the cone boundary
      (higher values create a sharper center beam with soft edges).

  Q3: What purpose do Surface Normals (glNormal3f) serve in OpenGL lighting?
  A3: Normals define the perpendicular orientation of each surface polygon. OpenGL calculates 
      the cosine of the angle between the normal and the light vector (N · L for diffuse) and 
      the reflection/halfway vector (N · H for specular) according to the Phong reflection model.

  Q4: How do Planar Projected Shadows work mathematically?
  A4: We construct a 4x4 projection matrix that flattens 3D object vertices onto the ground 
      plane (y = 0) along the ray originating from the light source. The geometry is rendered 
      in a dark semi-transparent shadow pass with lighting disabled and blending enabled.

  Q5: How does GL_NORMALIZE prevent lighting errors during scaling?
  A5: When glScalef() is applied to an object, normal vectors get scaled proportionally, 
      distorting the dot products in Phong calculations. GL_NORMALIZE forces OpenGL to unit-
      normalize all transformed normal vectors before lighting calculations.

================================================================================
  CONTROLS & SHORTCUTS
================================================================================
  - [W] / [S]           : Move Camera Forward / Backward
  - [A] / [D]           : Strafe Camera Left / Right
  - [Space] / [Ctrl]    : Fly Up / Fly Down
  - [Mouse Move]        : Look Around (Yaw & Pitch, clamped to avoid gimbal flip)
  - [1]                 : Toggle Point Light (Porch Bulb)
  - [2]                 : Toggle Directional Light (Moonlight)
  - [3] / [F]           : Toggle Spot Light (Flashlight)
  - [4]                 : Toggle Area Light (Window Interior Glow)
  - [0]                 : Toggle ALL Lights (Master Switch)
  - [G]                 : Toggle Atmospheric Fog (GL_FOG)
  - [B]                 : Toggle Bulb Pendulum Sway & Random Flicker
  - [C]                 : Toggle Hands-Free Cinematic Auto-Tour Presentation Mode
  - [H]                 : Toggle HUD & Controls Overlay
  - [R]                 : Reset Camera to Starting Vantage Point
  - [Enter] / [Space]   : Start Simulation from Title Screen
  - [ESC]               : Exit Application
================================================================================
*/

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// CONSTANTS & CONFIGURATION
// ============================================================================
const int WINDOW_INIT_WIDTH  = 1280;
const int WINDOW_INIT_HEIGHT = 720;

// Application States
enum AppState {
    STATE_TITLE,
    STATE_SCENE
};

AppState g_appState = STATE_TITLE;

// Window & Screen
int g_windowWidth  = WINDOW_INIT_WIDTH;
int g_windowHeight = WINDOW_INIT_HEIGHT;

// Time & Animation
float g_time = 0.0f;
float g_deltaTime = 0.016f;
int   g_prevTimeMs = 0;
int   g_frameCount = 0;
float g_fps = 60.0f;
float g_fpsTimer = 0.0f;

// Camera State (First Person)
struct Camera {
    float x, y, z;
    float yaw;    // in degrees
    float pitch;  // in degrees
    float speed;
    float sens;
} g_cam = {
    0.0f, 2.2f, 28.0f,   // Initial Position
    -90.0f, -2.0f,       // Initial Yaw, Pitch (looking towards -Z where house is)
    12.0f,               // Movement Speed
    0.15f                // Mouse Sensitivity
};

// Keyboard state for smooth WASD movement
bool g_keyState[256] = { false };
bool g_specialKeyState[256] = { false };
bool g_isCtrlPressed = false;

// Mouse tracking
int  g_lastMouseX = -1;
int  g_lastMouseY = -1;
bool g_mouseCaptured = false;

// Light Switches & Settings
bool g_light0PointOn       = true;  // Porch Bulb
bool g_light1DirectionalOn = true;  // Moonlight
bool g_light2SpotOn        = true;  // Flashlight
bool g_light3AreaOn        = true;  // Window Glow

bool g_fogEnabled          = true;
bool g_bulbAnimEnabled     = true;
bool g_showHUD             = true;
bool g_cinematicMode       = false;
float g_cinematicTime      = 0.0f;

// Porch bulb dynamic state
float g_bulbBaseX = -2.8f;
float g_bulbBaseY = 4.2f;
float g_bulbBaseZ = 4.2f;
float g_bulbCordLength = 1.2f;
float g_bulbCurX = -2.8f;
float g_bulbCurY = 3.0f;
float g_bulbCurZ = 4.2f;
float g_bulbFlickerFactor = 1.0f;

// Directional Moonlight Position/Vector
float g_moonDir[4] = { -0.45f, 0.75f, 0.48f, 0.0f }; // w = 0.0f denotes directional

// Stars in the night sky
struct Star {
    float x, y, z;
    float size;
    float brightness;
};
std::vector<Star> g_stars;

// ============================================================================
// MATERIAL DEFINITIONS & HELPER STRUCTS
// ============================================================================
struct Material {
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float emission[4];
    float shininess;
};

void applyMaterial(const Material& m) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   m.ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   m.diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m.specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  m.emission);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, m.shininess);
}

// Preset Materials
const Material MAT_DARK_WOOD = {
    { 0.12f, 0.08f, 0.05f, 1.0f },
    { 0.28f, 0.18f, 0.12f, 1.0f },
    { 0.08f, 0.08f, 0.08f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    8.0f
};

const Material MAT_WEATHERED_WALL = {
    { 0.15f, 0.15f, 0.18f, 1.0f },
    { 0.32f, 0.33f, 0.38f, 1.0f },
    { 0.05f, 0.05f, 0.06f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    5.0f
};

const Material MAT_ROOF_SHINGLE = {
    { 0.08f, 0.07f, 0.09f, 1.0f },
    { 0.18f, 0.16f, 0.22f, 1.0f },
    { 0.10f, 0.10f, 0.14f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    12.0f
};

const Material MAT_STONE = {
    { 0.12f, 0.13f, 0.15f, 1.0f },
    { 0.25f, 0.28f, 0.30f, 1.0f },
    { 0.20f, 0.20f, 0.22f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    24.0f
};

const Material MAT_WET_GROUND = {
    { 0.05f, 0.06f, 0.07f, 1.0f },
    { 0.12f, 0.14f, 0.16f, 1.0f },
    { 0.35f, 0.40f, 0.45f, 1.0f }, // High specular for wet moonlit gleam
    { 0.00f, 0.00f, 0.00f, 1.0f },
    55.0f
};

const Material MAT_RUSTY_METAL = {
    { 0.18f, 0.08f, 0.05f, 1.0f },
    { 0.45f, 0.22f, 0.15f, 1.0f },
    { 0.50f, 0.35f, 0.25f, 1.0f }, // Shiny specular highlights
    { 0.00f, 0.00f, 0.00f, 1.0f },
    48.0f
};

const Material MAT_CAR_GLASS = {
    { 0.10f, 0.15f, 0.20f, 0.8f },
    { 0.20f, 0.35f, 0.45f, 0.8f },
    { 0.80f, 0.90f, 1.00f, 0.8f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    90.0f
};

const Material MAT_PUMPKIN_SKIN = {
    { 0.25f, 0.08f, 0.01f, 1.0f },
    { 0.85f, 0.38f, 0.05f, 1.0f },
    { 0.30f, 0.18f, 0.05f, 1.0f },
    { 0.02f, 0.01f, 0.00f, 1.0f },
    18.0f
};

const Material MAT_PUMPKIN_GLOW = {
    { 0.60f, 0.30f, 0.05f, 1.0f },
    { 1.00f, 0.65f, 0.10f, 1.0f },
    { 1.00f, 0.90f, 0.40f, 1.0f },
    { 1.00f, 0.60f, 0.10f, 1.0f }, // Self-luminous interior
    30.0f
};

const Material MAT_WINDOW_GLOW = {
    { 0.50f, 0.30f, 0.10f, 1.0f },
    { 0.95f, 0.70f, 0.25f, 1.0f },
    { 0.80f, 0.60f, 0.20f, 1.0f },
    { 0.95f, 0.65f, 0.15f, 1.0f }, // Glowing window glass
    20.0f
};

const Material MAT_BULB_EMISSIVE = {
    { 0.80f, 0.70f, 0.20f, 1.0f },
    { 1.00f, 0.90f, 0.40f, 1.0f },
    { 1.00f, 1.00f, 0.80f, 1.0f },
    { 1.00f, 0.85f, 0.30f, 1.0f },
    60.0f
};

const Material MAT_MOON = {
    { 0.60f, 0.70f, 0.85f, 1.0f },
    { 0.90f, 0.95f, 1.00f, 1.0f },
    { 0.50f, 0.60f, 0.80f, 1.0f },
    { 0.85f, 0.90f, 1.00f, 1.0f }, // Luminous moon
    30.0f
};

const Material MAT_BARK = {
    { 0.08f, 0.06f, 0.05f, 1.0f },
    { 0.18f, 0.14f, 0.11f, 1.0f },
    { 0.03f, 0.03f, 0.03f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    4.0f
};

// ============================================================================
// INITIALIZATION FUNCTIONS
// ============================================================================
void initStars() {
    g_stars.clear();
    srand(1337); // deterministic seed for consistent, beautiful starfield
    for (int i = 0; i < 300; ++i) {
        float theta = ((float)rand() / RAND_MAX) * 2.0f * (float)M_PI;
        float phi   = ((float)rand() / RAND_MAX) * 0.45f * (float)M_PI; // upper hemisphere
        float r     = 180.0f;
        Star s;
        s.x = r * std::cos(phi) * std::cos(theta);
        s.y = r * std::sin(phi) + 10.0f;
        s.z = r * std::cos(phi) * std::sin(theta);
        s.size = 1.0f + ((float)rand() / RAND_MAX) * 2.5f;
        s.brightness = 0.4f + ((float)rand() / RAND_MAX) * 0.6f;
        g_stars.push_back(s);
    }
}

void initOpenGL() {
    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Enable Smooth Shading & Normal Vector Normalization
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    // Enable Backface Culling for solid meshes
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Enable Blending for transparency & glowing billboards
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Clear Color: Deep eerie midnight slate blue
    glClearColor(0.025f, 0.035f, 0.065f, 1.0f);

    // Global Ambient Light: Dim nocturnal ambience so unlit areas are not pure pitch black
    float globalAmbient[] = { 0.06f, 0.08f, 0.12f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // ------------------------------------------------------------------------
    // SETUP FOG (GL_FOG)
    // ------------------------------------------------------------------------
    float fogColor[4] = { 0.035f, 0.045f, 0.085f, 1.0f }; // Matches night sky horizon
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.022f);
    glHint(GL_FOG_HINT, GL_NICEST);
    if (g_fogEnabled) glEnable(GL_FOG); else glDisable(GL_FOG);

    // ------------------------------------------------------------------------
    // SETUP LIGHT 0: POINT LIGHT (Hanging Porch Bulb)
    // ------------------------------------------------------------------------
    float pDiffuse[]   = { 1.0f, 0.82f, 0.35f, 1.0f }; // Warm incandescent amber
    float pSpecular[]  = { 1.0f, 0.85f, 0.40f, 1.0f };
    float pAmbient[]   = { 0.08f, 0.05f, 0.01f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  pDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, pSpecular);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  pAmbient);
    // Attenuation coefficients: 1 / (1.0 + 0.12*d + 0.03*d^2)
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.12f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.03f);

    // ------------------------------------------------------------------------
    // SETUP LIGHT 1: DIRECTIONAL LIGHT (Moonlight)
    // ------------------------------------------------------------------------
    float mDiffuse[]   = { 0.32f, 0.42f, 0.65f, 1.0f }; // Cool silvery blue
    float mSpecular[]  = { 0.45f, 0.55f, 0.80f, 1.0f };
    float mAmbient[]   = { 0.05f, 0.07f, 0.12f, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  mDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, mSpecular);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  mAmbient);

    // ------------------------------------------------------------------------
    // SETUP LIGHT 2: SPOT LIGHT (First-Person Flashlight)
    // ------------------------------------------------------------------------
    float fDiffuse[]   = { 0.95f, 0.98f, 1.00f, 1.0f }; // Bright crisp LED white/pale blue
    float fSpecular[]  = { 1.00f, 1.00f, 1.00f, 1.0f };
    float fAmbient[]   = { 0.00f, 0.00f, 0.00f, 1.0f };
    glLightfv(GL_LIGHT2, GL_DIFFUSE,  fDiffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, fSpecular);
    glLightfv(GL_LIGHT2, GL_AMBIENT,  fAmbient);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF,   18.5f); // Half-angle cone of 18.5°
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 28.0f); // Falloff concentration factor
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION,    0.04f);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.006f);

    // ------------------------------------------------------------------------
    // SETUP LIGHT 3: AREA LIGHT EMULATION (Window Glow)
    // ------------------------------------------------------------------------
    float wDiffuse[]   = { 0.90f, 0.55f, 0.18f, 1.0f }; // Warm radiant orange
    float wSpecular[]  = { 0.60f, 0.35f, 0.10f, 1.0f };
    float wAmbient[]   = { 0.22f, 0.12f, 0.03f, 1.0f }; // Elevated ambient to emulate soft area dispersion
    glLightfv(GL_LIGHT3, GL_DIFFUSE,  wDiffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, wSpecular);
    glLightfv(GL_LIGHT3, GL_AMBIENT,  wAmbient);
    glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION,    0.08f);
    glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.015f);

    glEnable(GL_LIGHTING);
    initStars();
}

// ============================================================================
// GEOMETRIC PRIMITIVE RENDERING HELPERS (WITH STRICT SURFACE NORMALS)
// ============================================================================

// Draw a solid cuboid box centered at origin with exact normals per face
void drawBox(float width, float height, float depth) {
    float x = width * 0.5f;
    float y = height * 0.5f;
    float z = depth * 0.5f;

    glBegin(GL_QUADS);
    // Front Face (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-x, -y,  z);
    glVertex3f( x, -y,  z);
    glVertex3f( x,  y,  z);
    glVertex3f(-x,  y,  z);

    // Back Face (-Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f( x, -y, -z);
    glVertex3f(-x, -y, -z);
    glVertex3f(-x,  y, -z);
    glVertex3f( x,  y, -z);

    // Top Face (+Y)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-x,  y,  z);
    glVertex3f( x,  y,  z);
    glVertex3f( x,  y, -z);
    glVertex3f(-x,  y, -z);

    // Bottom Face (-Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-x, -y, -z);
    glVertex3f( x, -y, -z);
    glVertex3f( x, -y,  z);
    glVertex3f(-x, -y,  z);

    // Right Face (+X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f( x, -y,  z);
    glVertex3f( x, -y, -z);
    glVertex3f( x,  y, -z);
    glVertex3f( x,  y,  z);

    // Left Face (-X)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-x, -y, -z);
    glVertex3f(-x, -y,  z);
    glVertex3f(-x,  y,  z);
    glVertex3f(-x,  y, -z);
    glEnd();
}

// Draw a cylinder along +Y axis from y=0 to y=height
void drawCylinder(float baseRadius, float topRadius, float height, int slices) {
    float angleStep = 2.0f * (float)M_PI / (float)slices;
    float slope = (baseRadius - topRadius) / height;

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
        float a = i * angleStep;
        float cosA = std::cos(a);
        float sinA = std::sin(a);

        // Compute outward normal accounting for taper
        float nx = cosA;
        float ny = slope;
        float nz = sinA;
        float len = std::sqrt(nx*nx + ny*ny + nz*nz);
        glNormal3f(nx/len, ny/len, nz/len);

        glVertex3f(topRadius * cosA, height, topRadius * sinA);
        glVertex3f(baseRadius * cosA, 0.0f, baseRadius * sinA);
    }
    glEnd();

    // Bottom cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = slices; i >= 0; --i) {
        float a = i * angleStep;
        glVertex3f(baseRadius * std::cos(a), 0.0f, baseRadius * std::sin(a));
    }
    glEnd();

    // Top cap
    if (topRadius > 0.001f) {
        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, height, 0.0f);
        for (int i = 0; i <= slices; ++i) {
            float a = i * angleStep;
            glVertex3f(topRadius * std::cos(a), height, topRadius * std::sin(a));
        }
        glEnd();
    }
}

// Draw a UV Sphere with smooth vertex normals
void drawSphere(float radius, int slices, int stacks) {
    for (int i = 0; i < stacks; ++i) {
        float lat0 = (float)M_PI * (-0.5f + (float)i / stacks);
        float z0  = std::sin(lat0);
        float zr0 = std::cos(lat0);

        float lat1 = (float)M_PI * (-0.5f + (float)(i + 1) / stacks);
        float z1  = std::sin(lat1);
        float zr1 = std::cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float lng = 2.0f * (float)M_PI * (float)j / slices;
            float x = std::cos(lng);
            float y = std::sin(lng);

            glNormal3f(x * zr1, z1, y * zr1);
            glVertex3f(radius * x * zr1, radius * z1, radius * y * zr1);

            glNormal3f(x * zr0, z0, y * zr0);
            glVertex3f(radius * x * zr0, radius * z0, radius * y * zr0);
        }
        glEnd();
    }
}

// Draw a 3D pyramid / gable wedge roof section
void drawPrismRoof(float width, float height, float length) {
    float hw = width * 0.5f;
    float hl = length * 0.5f;

    glBegin(GL_TRIANGLES);
    // Front Triangular Gable (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-hw, 0.0f,  hl);
    glVertex3f( hw, 0.0f,  hl);
    glVertex3f(0.0f, height, hl);

    // Back Triangular Gable (-Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f( hw, 0.0f, -hl);
    glVertex3f(-hw, 0.0f, -hl);
    glVertex3f(0.0f, height, -hl);
    glEnd();

    glBegin(GL_QUADS);
    // Right Slope (+X)
    float nx = height;
    float ny = hw;
    float nlen = std::sqrt(nx*nx + ny*ny);
    glNormal3f(nx/nlen, ny/nlen, 0.0f);
    glVertex3f( hw, 0.0f,  hl);
    glVertex3f( hw, 0.0f, -hl);
    glVertex3f(0.0f, height, -hl);
    glVertex3f(0.0f, height,  hl);

    // Left Slope (-X)
    glNormal3f(-nx/nlen, ny/nlen, 0.0f);
    glVertex3f(0.0f, height,  hl);
    glVertex3f(0.0f, height, -hl);
    glVertex3f(-hw, 0.0f, -hl);
    glVertex3f(-hw, 0.0f,  hl);

    // Bottom Face (-Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-hw, 0.0f, -hl);
    glVertex3f( hw, 0.0f, -hl);
    glVertex3f( hw, 0.0f,  hl);
    glVertex3f(-hw, 0.0f,  hl);
    glEnd();
}

// Draw an Octagonal Steeple / Spire for the gothic tower
void drawSteepleSpire(float baseRadius, float height, int facets) {
    float angleStep = 2.0f * (float)M_PI / (float)facets;
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < facets; ++i) {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;
        float x1 = baseRadius * std::cos(a1);
        float z1 = baseRadius * std::sin(a1);
        float x2 = baseRadius * std::cos(a2);
        float z2 = baseRadius * std::sin(a2);

        // Compute face normal for triangular side
        float mx = (x1 + x2) * 0.5f;
        float mz = (z1 + z2) * 0.5f;
        float nx = mx;
        float ny = baseRadius / height;
        float nz = mz;
        float nl = std::sqrt(nx*nx + ny*ny + nz*nz);

        glNormal3f(nx/nl, ny/nl, nz/nl);
        glVertex3f(x1, 0.0f, z1);
        glVertex3f(x2, 0.0f, z2);
        glVertex3f(0.0f, height, 0.0f);
    }
    glEnd();
}

// Draw a Soft Additive Billboard Glow (for moon, bulb, and pumpkin halos)
void drawBillboardHalo(float x, float y, float z, float radius, float r, float g, float b, float maxAlpha) {
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE); // Don't write to depth buffer for soft halo
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for rich radiant glow

    glPushMatrix();
    glTranslatef(x, y, z);

    // Extract camera orientation to face quad directly towards viewer (Spherical Billboard)
    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    // Set rotation part to identity
    modelview[0] = 1.0f; modelview[1] = 0.0f; modelview[2] = 0.0f;
    modelview[4] = 0.0f; modelview[5] = 1.0f; modelview[6] = 0.0f;
    modelview[8] = 0.0f; modelview[9] = 0.0f; modelview[10] = 1.0f;
    glLoadMatrixf(modelview);

    // Multi-ring concentric circle with smooth alpha decay
    int rings = 6;
    int segments = 24;
    for (int ring = 0; ring < rings; ++ring) {
        float r0 = radius * ((float)ring / rings);
        float r1 = radius * ((float)(ring + 1) / rings);
        float a0 = maxAlpha * (1.0f - (float)ring / rings);
        float a1 = maxAlpha * (1.0f - (float)(ring + 1) / rings);
        a0 = a0 * a0; // quadratic smooth falloff
        a1 = a1 * a1;

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segments; ++i) {
            float theta = 2.0f * (float)M_PI * (float)i / segments;
            float ct = std::cos(theta);
            float st = std::sin(theta);

            glColor4f(r, g, b, a1);
            glVertex3f(r1 * ct, r1 * st, 0.0f);

            glColor4f(r, g, b, a0);
            glVertex3f(r0 * ct, r0 * st, 0.0f);
        }
        glEnd();
    }

    glPopMatrix();
    glPopAttrib();
}

// ============================================================================
// DETAILED SCENE GEOMETRY RENDERING
// ============================================================================

// 1. Subdivided Terrain Ground Grid with wet specular gleams and puddles
void drawGround() {
    applyMaterial(MAT_WET_GROUND);

    int gridSize = 50;
    float halfDim = 60.0f;
    float step = (2.0f * halfDim) / gridSize;

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (int i = 0; i < gridSize; ++i) {
        float z0 = -halfDim + i * step;
        float z1 = z0 + step;
        for (int j = 0; j < gridSize; ++j) {
            float x0 = -halfDim + j * step;
            float x1 = x0 + step;

            // Subtle undulating terrain height variations
            float y00 = 0.05f * std::sin(x0 * 0.15f) * std::cos(z0 * 0.15f);
            float y10 = 0.05f * std::sin(x1 * 0.15f) * std::cos(z0 * 0.15f);
            float y11 = 0.05f * std::sin(x1 * 0.15f) * std::cos(z1 * 0.15f);
            float y01 = 0.05f * std::sin(x0 * 0.15f) * std::cos(z1 * 0.15f);

            glVertex3f(x0, y00, z0);
            glVertex3f(x1, y10, z0);
            glVertex3f(x1, y11, z1);
            glVertex3f(x0, y01, z1);
        }
    }
    glEnd();

    // Cobblestone / Flagstone Pathway leading to the porch
    applyMaterial(MAT_STONE);
    int numStones = 28;
    for (int i = 0; i < numStones; ++i) {
        float progress = (float)i / (numStones - 1);
        float pz = 24.0f - progress * 20.0f; // from z=24 down to z=4 (porch)
        float px = -2.8f + 1.2f * std::sin(progress * (float)M_PI * 1.5f); // curved path
        float pWidth  = 1.8f + 0.3f * std::sin(i * 1.7f);
        float pLength = 0.65f;
        float pHeight = 0.06f;

        glPushMatrix();
        glTranslatef(px, 0.03f, pz);
        glRotatef(std::sin(i * 2.3f) * 6.0f, 0.0f, 1.0f, 0.0f);
        drawBox(pWidth, pHeight, pLength);
        glPopMatrix();
    }
}

// 2. Carved Jack-o'-Lantern Pumpkin with glowing triangular eyes & jagged mouth
void drawPumpkin(float x, float y, float z, float scale, float rotY) {
    glPushMatrix();
    glTranslatef(x, y + 0.35f * scale, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glScalef(scale, scale, scale);

    // Pumpkin body: squashed sphere with ribbed segments
    applyMaterial(MAT_PUMPKIN_SKIN);
    glPushMatrix();
    glScalef(0.65f, 0.52f, 0.65f);
    drawSphere(1.0f, 18, 14);
    glPopMatrix();

    // Stem: small brown-green curved cylinder
    applyMaterial(MAT_BARK);
    glPushMatrix();
    glTranslatef(0.0f, 0.48f, 0.0f);
    glRotatef(12.0f, 0.0f, 0.0f, 1.0f);
    drawCylinder(0.06f, 0.035f, 0.22f, 8);
    glPopMatrix();

    // Carved Glowing Face Features (Eyes, Nose, Jagged Smile)
    applyMaterial(MAT_PUMPKIN_GLOW);

    // Left Eye (Triangular cutout)
    glBegin(GL_TRIANGLES);
    glNormal3f(-0.3f, 0.2f, 0.95f);
    glVertex3f(-0.16f, 0.12f, 0.58f);
    glVertex3f(-0.06f, 0.12f, 0.61f);
    glVertex3f(-0.11f, 0.24f, 0.56f);

    // Right Eye
    glNormal3f(0.3f, 0.2f, 0.95f);
    glVertex3f( 0.06f, 0.12f, 0.61f);
    glVertex3f( 0.16f, 0.12f, 0.58f);
    glVertex3f( 0.11f, 0.24f, 0.56f);

    // Center Nose
    glNormal3f(0.0f, 0.1f, 1.0f);
    glVertex3f(-0.04f, 0.02f, 0.64f);
    glVertex3f( 0.04f, 0.02f, 0.64f);
    glVertex3f( 0.00f, 0.09f, 0.63f);
    glEnd();

    // Sinister Jagged Mouth with carved teeth
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -0.2f, 0.98f);
    glVertex3f(0.0f, -0.15f, 0.62f); // mouth center
    glVertex3f(-0.25f, -0.05f, 0.50f);
    glVertex3f(-0.18f, -0.14f, 0.58f);
    glVertex3f(-0.12f, -0.08f, 0.61f);
    glVertex3f(-0.05f, -0.16f, 0.63f);
    glVertex3f( 0.00f, -0.09f, 0.64f);
    glVertex3f( 0.05f, -0.16f, 0.63f);
    glVertex3f( 0.12f, -0.08f, 0.61f);
    glVertex3f( 0.18f, -0.14f, 0.58f);
    glVertex3f( 0.25f, -0.05f, 0.50f);
    glEnd();

    glPopMatrix();

    // Soft pumpkin inner candlelight halo
    drawBillboardHalo(x, y + 0.35f * scale, z + 0.2f * scale, 0.65f * scale, 1.0f, 0.55f, 0.1f, 0.35f);
}

// Draw a cluster of atmospheric Jack-o'-lanterns along the path as seen in reference image
void drawPumpkinArray() {
    drawPumpkin(-2.2f, 0.0f,  4.5f, 1.15f,  25.0f);
    drawPumpkin(-4.2f, 0.0f,  5.8f, 0.95f, -35.0f);
    drawPumpkin(-1.4f, 0.0f,  8.2f, 1.05f,  15.0f);
    drawPumpkin(-3.5f, 0.0f, 10.5f, 1.25f, -10.0f);
    drawPumpkin(-0.8f, 0.0f, 13.0f, 0.85f,  40.0f);
    drawPumpkin(-2.9f, 0.0f, 15.5f, 1.35f,  -5.0f); // Large menacing centerpiece pumpkin
    drawPumpkin(-1.2f, 0.0f, 18.2f, 1.10f,  20.0f);
    drawPumpkin(-4.0f, 0.0f, 20.0f, 0.90f, -45.0f);
    drawPumpkin( 1.5f, 0.0f, 12.0f, 0.80f,  60.0f);
}

// 3. Spooky Bare Creepy Tree with twisting branches
void drawCreepyTree(float x, float z, float trunkRadius, float height, float rotY) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    applyMaterial(MAT_BARK);

    // Flared root base
    glPushMatrix();
    glScalef(1.4f, 0.3f, 1.4f);
    drawCylinder(trunkRadius * 1.5f, trunkRadius, 1.0f, 10);
    glPopMatrix();

    // Main Trunk (twisted cylinder)
    drawCylinder(trunkRadius, trunkRadius * 0.6f, height * 0.65f, 10);

    // Primary Branch 1 (spreading left)
    glPushMatrix();
    glTranslatef(0.0f, height * 0.55f, 0.0f);
    glRotatef(42.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(18.0f, 0.0f, 1.0f, 0.0f);
    drawCylinder(trunkRadius * 0.5f, trunkRadius * 0.25f, height * 0.45f, 8);

    // Twig 1a
    glTranslatef(0.0f, height * 0.38f, 0.0f);
    glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(trunkRadius * 0.22f, 0.03f, height * 0.28f, 6);
    glPopMatrix();

    // Primary Branch 2 (spreading right)
    glPushMatrix();
    glTranslatef(0.0f, height * 0.60f, 0.0f);
    glRotatef(-38.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-25.0f, 0.0f, 1.0f, 0.0f);
    drawCylinder(trunkRadius * 0.45f, trunkRadius * 0.22f, height * 0.42f, 8);

    // Twig 2a
    glTranslatef(0.0f, height * 0.35f, 0.0f);
    glRotatef(35.0f, 0.0f, 0.0f, 1.0f);
    drawCylinder(trunkRadius * 0.2f, 0.02f, height * 0.25f, 6);
    glPopMatrix();

    // Upper Crown Branch
    glPushMatrix();
    glTranslatef(0.0f, height * 0.65f, 0.0f);
    glRotatef(12.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(trunkRadius * 0.4f, 0.05f, height * 0.45f, 8);
    glPopMatrix();

    glPopMatrix();
}

// 4. Gothic Haunted House Model (with Spire Tower, Gabled Roofs, Chimney, Porch & Broken Furniture)
void drawHouse() {
    // ------------------------------------------------------------------------
    // STONE FOUNDATION & BASE
    // ------------------------------------------------------------------------
    applyMaterial(MAT_STONE);
    glPushMatrix();
    glTranslatef(0.0f, 0.4f, 0.0f);
    drawBox(16.5f, 0.8f, 12.5f);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // MAIN HOUSE WOODEN WALLS & BODY
    // ------------------------------------------------------------------------
    applyMaterial(MAT_WEATHERED_WALL);
    glPushMatrix();
    glTranslatef(0.0f, 3.4f, 0.0f);
    drawBox(15.5f, 5.2f, 11.5f); // Main 2-story core
    glPopMatrix();

    // Side Extension Wing (Left side)
    glPushMatrix();
    glTranslatef(-6.5f, 2.8f, 1.5f);
    drawBox(5.0f, 4.0f, 7.5f);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // PORCH STRUCTURE & STAIRS (Front Left where hanging bulb is)
    // ------------------------------------------------------------------------
    applyMaterial(MAT_DARK_WOOD);
    // Porch Deck Floor
    glPushMatrix();
    glTranslatef(-2.8f, 0.75f, 5.2f);
    drawBox(6.5f, 0.3f, 4.0f);
    glPopMatrix();

    // Porch Steps
    glPushMatrix();
    glTranslatef(-2.8f, 0.25f, 7.4f);
    drawBox(3.0f, 0.25f, 0.8f);
    glTranslatef(0.0f, 0.25f, -0.6f);
    drawBox(3.0f, 0.25f, 0.8f);
    glPopMatrix();

    // Porch Support Pillars (4 posts)
    float postPositions[3][2] = {
        { -5.8f, 6.8f },
        { -2.8f, 6.8f },
        {  0.2f, 6.8f }
    };
    for (int i = 0; i < 3; ++i) {
        glPushMatrix();
        glTranslatef(postPositions[i][0], 0.9f, postPositions[i][1]);
        drawCylinder(0.12f, 0.12f, 3.4f, 8);
        glPopMatrix();
    }

    // Porch Header Beam
    glPushMatrix();
    glTranslatef(-2.8f, 4.35f, 6.8f);
    drawBox(6.6f, 0.25f, 0.35f);
    glPopMatrix();

    // Porch Overhang Sloped Roof
    applyMaterial(MAT_ROOF_SHINGLE);
    glPushMatrix();
    glTranslatef(-2.8f, 4.5f, 6.0f);
    glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
    drawBox(6.8f, 0.2f, 3.2f);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // MAIN GABLED ROOF & ATTIC
    // ------------------------------------------------------------------------
    applyMaterial(MAT_ROOF_SHINGLE);
    glPushMatrix();
    glTranslatef(0.0f, 6.0f, 0.0f);
    drawPrismRoof(16.5f, 4.2f, 12.5f);
    glPopMatrix();

    // Left Wing Gabled Cross-Roof
    glPushMatrix();
    glTranslatef(-6.5f, 4.8f, 1.5f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    drawPrismRoof(8.0f, 2.8f, 5.4f);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // GOTHIC SPIRE TOWER (Matches the dramatic silhouette in reference image!)
    // ------------------------------------------------------------------------
    applyMaterial(MAT_WEATHERED_WALL);
    // Tower Shaft
    glPushMatrix();
    glTranslatef(3.5f, 6.0f, 2.0f);
    drawCylinder(2.2f, 1.9f, 6.5f, 8);

    // Tower Roof Balcony Eaves
    applyMaterial(MAT_ROOF_SHINGLE);
    glTranslatef(0.0f, 6.5f, 0.0f);
    drawCylinder(2.4f, 2.0f, 0.4f, 8);

    // Tall Pointed Steeple Spire
    glTranslatef(0.0f, 0.4f, 0.0f);
    drawSteepleSpire(2.0f, 7.5f, 8);

    // Finial Iron Tip
    applyMaterial(MAT_RUSTY_METAL);
    glTranslatef(0.0f, 7.5f, 0.0f);
    drawCylinder(0.05f, 0.01f, 1.2f, 6);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // BRICK CHIMNEY
    // ------------------------------------------------------------------------
    applyMaterial(MAT_STONE);
    glPushMatrix();
    glTranslatef(-4.5f, 8.2f, -1.8f);
    drawBox(1.2f, 4.5f, 1.2f);
    // Chimney Cap
    glTranslatef(0.0f, 2.3f, 0.0f);
    drawBox(1.5f, 0.25f, 1.5f);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // BROKEN DOOR (Hanging ajar on one hinge at an angle)
    // ------------------------------------------------------------------------
    applyMaterial(MAT_DARK_WOOD);
    glPushMatrix();
    glTranslatef(-2.2f, 1.0f, 5.8f);
    glRotatef(-22.0f, 0.0f, 1.0f, 0.0f); // Swung open
    glRotatef(6.0f, 1.0f, 0.0f, 0.0f);  // Sagging crooked off top hinge
    glTranslatef(0.75f, 1.25f, 0.0f);
    drawBox(1.5f, 2.5f, 0.1f);
    // Iron door handle
    applyMaterial(MAT_RUSTY_METAL);
    glTranslatef(0.5f, 0.0f, 0.08f);
    drawSphere(0.06f, 6, 6);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // WINDOWS WITH FRAMES & GLOWING EMISSIVE PANES
    // ------------------------------------------------------------------------
    // 1. Main Parlor Window (Right Front - AREA LIGHT SOURCE GL_LIGHT3)
    applyMaterial(MAT_WINDOW_GLOW);
    glPushMatrix();
    glTranslatef(3.5f, 3.2f, 5.82f);
    drawBox(2.8f, 2.2f, 0.08f); // Glowing window glass

    // Window Wooden Muntins/Frame Cross
    applyMaterial(MAT_DARK_WOOD);
    drawBox(2.9f, 0.12f, 0.14f); // Horizontal bar
    drawBox(0.12f, 2.3f, 0.14f);  // Vertical bar
    drawBox(3.0f, 2.4f, 0.10f);   // Outer frame border
    glPopMatrix();

    // Soft radiant aura outside the window
    drawBillboardHalo(3.5f, 3.2f, 6.0f, 2.8f, 1.0f, 0.65f, 0.15f, 0.45f);

    // 2. Second Floor & Attic Windows
    applyMaterial(MAT_WINDOW_GLOW);
    glPushMatrix();
    glTranslatef(-2.8f, 6.5f, 4.2f);
    drawBox(1.6f, 1.6f, 0.08f);
    applyMaterial(MAT_DARK_WOOD);
    drawBox(1.7f, 0.10f, 0.12f);
    drawBox(0.10f, 1.7f, 0.12f);
    glPopMatrix();

    // 3. Spire Tower Windows (Arched lancet windows)
    glPushMatrix();
    glTranslatef(3.5f, 9.2f, 3.8f);
    applyMaterial(MAT_WINDOW_GLOW);
    drawBox(0.9f, 1.4f, 0.08f);
    applyMaterial(MAT_DARK_WOOD);
    drawBox(0.95f, 0.08f, 0.12f);
    drawBox(0.08f, 1.45f, 0.12f);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // OLD BROKEN FURNITURE (Inside and Porch)
    // ------------------------------------------------------------------------
    // Broken Chair on Porch (toppled / tilted)
    applyMaterial(MAT_DARK_WOOD);
    glPushMatrix();
    glTranslatef(-0.8f, 1.15f, 5.5f);
    glRotatef(75.0f, 0.0f, 0.0f, 1.0f); // Fallen on its side
    glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
    // Seat
    drawBox(0.8f, 0.08f, 0.8f);
    // Legs
    glTranslatef(-0.35f, -0.4f, -0.35f);
    drawCylinder(0.04f, 0.03f, 0.4f, 6);
    glTranslatef(0.7f, 0.0f, 0.0f);
    drawCylinder(0.04f, 0.03f, 0.4f, 6);
    glTranslatef(0.0f, 0.0f, 0.7f);
    drawCylinder(0.04f, 0.03f, 0.4f, 6);
    // Broken missing 4th leg
    glPopMatrix();

    // Old Wooden Table on Porch
    glPushMatrix();
    glTranslatef(-4.8f, 1.0f, 5.2f);
    // Tabletop
    glTranslatef(0.0f, 0.8f, 0.0f);
    drawBox(1.6f, 0.1f, 1.2f);
    // Table legs
    glTranslatef(-0.7f, -0.8f, -0.5f);
    drawCylinder(0.05f, 0.04f, 0.8f, 6);
    glTranslatef(1.4f, 0.0f, 0.0f);
    drawCylinder(0.05f, 0.04f, 0.8f, 6);
    glTranslatef(0.0f, 0.0f, 1.0f);
    drawCylinder(0.05f, 0.04f, 0.8f, 6);
    glTranslatef(-1.4f, 0.0f, 0.0f);
    drawCylinder(0.05f, 0.04f, 0.8f, 6);
    glPopMatrix();
}

// 5. Hanging Porch Bulb with Pendulum Physics & Dynamic Point Light (GL_LIGHT0)
void drawHangingBulb() {
    // Dynamic swing calculation
    float swayAngleX = 0.0f;
    float swayAngleZ = 0.0f;
    if (g_bulbAnimEnabled) {
        swayAngleX = 14.0f * std::sin(g_time * 2.1f);
        swayAngleZ = 6.0f * std::cos(g_time * 1.6f);
    }

    // World position of ceiling attachment
    float beamX = -2.8f;
    float beamY = 4.35f;
    float beamZ = 5.2f;

    // Calculate swinging bulb position in 3D world space
    float radX = swayAngleX * (float)M_PI / 180.0f;
    float radZ = swayAngleZ * (float)M_PI / 180.0f;
    g_bulbCurX = beamX + g_bulbCordLength * std::sin(radZ);
    g_bulbCurY = beamY - g_bulbCordLength * std::cos(radX) * std::cos(radZ);
    g_bulbCurZ = beamZ - g_bulbCordLength * std::sin(radX);

    // Ceiling mount plate
    applyMaterial(MAT_RUSTY_METAL);
    glPushMatrix();
    glTranslatef(beamX, beamY, beamZ);
    drawCylinder(0.08f, 0.08f, 0.04f, 8);
    glPopMatrix();

    // Render cord line
    glDisable(GL_LIGHTING);
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(beamX, beamY, beamZ);
    glVertex3f(g_bulbCurX, g_bulbCurY + 0.15f, g_bulbCurZ);
    glEnd();
    glEnable(GL_LIGHTING);

    // Render bulb socket
    glPushMatrix();
    glTranslatef(g_bulbCurX, g_bulbCurY + 0.1f, g_bulbCurZ);
    applyMaterial(MAT_RUSTY_METAL);
    drawCylinder(0.06f, 0.05f, 0.12f, 8);
    glPopMatrix();

    // Render glowing bulb glass sphere
    glPushMatrix();
    glTranslatef(g_bulbCurX, g_bulbCurY, g_bulbCurZ);
    if (g_light0PointOn) {
        applyMaterial(MAT_BULB_EMISSIVE);
    } else {
        // Unlit dusty glass
        Material unlitBulb = MAT_DARK_WOOD;
        unlitBulb.diffuse[0] = 0.4f; unlitBulb.diffuse[1] = 0.4f; unlitBulb.diffuse[2] = 0.35f;
        applyMaterial(unlitBulb);
    }
    drawSphere(0.14f, 14, 12);
    glPopMatrix();

    // Render warm glowing halo around bulb when powered on
    if (g_light0PointOn) {
        drawBillboardHalo(g_bulbCurX, g_bulbCurY, g_bulbCurZ, 1.2f, 1.0f, 0.85f, 0.35f, 0.55f * g_bulbFlickerFactor);
    }
}

// 6. Abandoned Rusted Car (Chassis, Cabin Wedge, Cylinder Wheels, Headlights)
void drawRustedCar(float x, float z, float rotY) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    applyMaterial(MAT_RUSTY_METAL);

    // Main Chassis Body
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, 0.0f);
    drawBox(2.2f, 0.65f, 4.6f);
    glPopMatrix();

    // Hood Engine Compartment
    glPushMatrix();
    glTranslatef(0.0f, 0.85f, 1.2f);
    drawBox(2.0f, 0.45f, 2.0f);
    glPopMatrix();

    // Cabin / Roof
    glPushMatrix();
    glTranslatef(0.0f, 1.35f, -0.4f);
    drawBox(1.9f, 0.75f, 2.1f);
    glPopMatrix();

    // Windshield & Windows (Dark reflective glass)
    applyMaterial(MAT_CAR_GLASS);
    // Front Windshield (slanted)
    glPushMatrix();
    glTranslatef(0.0f, 1.3f, 0.7f);
    glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
    drawBox(1.75f, 0.6f, 0.05f);
    glPopMatrix();

    // Side Windows
    glPushMatrix();
    glTranslatef(0.96f, 1.35f, -0.4f);
    drawBox(0.05f, 0.55f, 1.8f);
    glTranslatef(-1.92f, 0.0f, 0.0f);
    drawBox(0.05f, 0.55f, 1.8f);
    glPopMatrix();

    // Wheels (4 Rubber Cylinders with Rusted Rims)
    applyMaterial(MAT_BARK); // Dark rubber
    float wheelX = 1.05f;
    float wheelZ[2] = { 1.3f, -1.3f };
    for (int i = 0; i < 2; ++i) {
        // Right Wheel
        glPushMatrix();
        glTranslatef(wheelX, 0.42f, wheelZ[i]);
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
        drawCylinder(0.42f, 0.42f, 0.28f, 12);
        // Hubcap
        applyMaterial(MAT_RUSTY_METAL);
        glTranslatef(0.0f, 0.28f, 0.0f);
        drawCylinder(0.22f, 0.22f, 0.02f, 10);
        glPopMatrix();

        // Left Wheel
        applyMaterial(MAT_BARK);
        glPushMatrix();
        glTranslatef(-wheelX - 0.28f, 0.42f, wheelZ[i]);
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
        drawCylinder(0.42f, 0.42f, 0.28f, 12);
        // Hubcap
        applyMaterial(MAT_RUSTY_METAL);
        glTranslatef(0.0f, 0.0f, 0.0f);
        drawCylinder(0.22f, 0.22f, 0.02f, 10);
        glPopMatrix();
    }

    // Front Grille & Headlights
    applyMaterial(MAT_RUSTY_METAL);
    glPushMatrix();
    glTranslatef(0.0f, 0.75f, 2.32f);
    drawBox(1.6f, 0.35f, 0.05f); // Grille
    // Left & Right Headlights
    applyMaterial(MAT_CAR_GLASS);
    glTranslatef(-0.7f, 0.1f, 0.03f);
    drawSphere(0.14f, 8, 8);
    glTranslatef(1.4f, 0.0f, 0.0f);
    drawSphere(0.14f, 8, 8);
    glPopMatrix();

    glPopMatrix();
}

// 7. Wooden Picket Fence & Cemetery Tombstones
void drawFenceAndYardProps() {
    applyMaterial(MAT_DARK_WOOD);

    // Left Yard Fence
    for (int i = 0; i < 16; ++i) {
        float z = 24.0f - i * 1.8f;
        float x = -15.0f;
        float rot = std::sin(i * 1.4f) * 8.0f; // crooked weathered pickets

        glPushMatrix();
        glTranslatef(x, 0.8f, z);
        glRotatef(rot, 0.0f, 0.0f, 1.0f);
        drawBox(0.14f, 1.6f, 0.12f);
        // Pointed top
        glTranslatef(0.0f, 0.9f, 0.0f);
        drawPrismRoof(0.16f, 0.18f, 0.14f);
        glPopMatrix();
    }

    // Horizontal fence rails
    glPushMatrix();
    glTranslatef(-15.0f, 0.6f, 10.5f);
    drawBox(0.08f, 0.12f, 28.0f);
    glTranslatef(0.0f, 0.7f, 0.0f);
    drawBox(0.08f, 0.12f, 28.0f);
    glPopMatrix();

    // Weathered Cemetery Headstones / Tombstones
    applyMaterial(MAT_STONE);
    float tombstonePos[4][3] = {
        { 12.0f, 0.0f, 12.0f },
        { 14.5f, 0.0f, 14.5f },
        { 10.5f, 0.0f, 16.0f },
        { 13.0f, 0.0f, 18.5f }
    };
    for (int i = 0; i < 4; ++i) {
        glPushMatrix();
        glTranslatef(tombstonePos[i][0], 0.65f, tombstonePos[i][1]);
        glRotatef(std::sin(i * 2.1f) * 12.0f, 0.0f, 1.0f, 0.0f); // tilted headstones
        glRotatef(std::cos(i * 1.7f) * 6.0f, 1.0f, 0.0f, 0.0f);
        drawBox(0.7f, 1.3f, 0.22f);
        // Rounded headstone top arch
        glTranslatef(0.0f, 0.65f, 0.0f);
        glScalef(0.35f, 0.25f, 0.11f);
        drawSphere(1.0f, 10, 8);
        glPopMatrix();
    }

    // Scattered Yard Rocks & Boulders
    applyMaterial(MAT_STONE);
    float rockPos[8][4] = {
        { -8.5f, 0.0f, 14.0f, 0.85f },
        { -6.0f, 0.0f, 19.0f, 1.10f },
        {  4.5f, 0.0f, 16.0f, 0.70f },
        {  7.5f, 0.0f, 10.0f, 1.30f },
        { -12.0f,0.0f,  6.0f, 0.95f },
        {  8.0f, 0.0f, 22.0f, 0.75f },
        { -3.0f, 0.0f, 23.0f, 0.60f },
        { 11.0f, 0.0f,  5.0f, 1.15f }
    };
    for (int i = 0; i < 8; ++i) {
        glPushMatrix();
        glTranslatef(rockPos[i][0], rockPos[i][3] * 0.35f, rockPos[i][2]);
        glScalef(rockPos[i][3], rockPos[i][3] * 0.65f, rockPos[i][3] * 0.85f);
        drawSphere(0.8f, 8, 6); // Faceted rock geometry
        glPopMatrix();
    }
}

// 8. Glowing Giant Moon and Starfield
void drawMoonAndStars() {
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);

    // Draw Twinkling Stars
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < g_stars.size(); ++i) {
        float b = g_stars[i].brightness * (0.8f + 0.2f * std::sin(g_time * 3.0f + i));
        glColor4f(0.85f * b, 0.92f * b, 1.0f * b, 0.95f);
        glVertex3f(g_stars[i].x, g_stars[i].y, g_stars[i].z);
    }
    glEnd();

    // Draw Giant Full Moon (Matches the reference image framing behind the roof & spire!)
    float moonX =  8.0f;
    float moonY = 32.0f;
    float moonZ = -38.0f;
    float moonRadius = 9.5f;

    glEnable(GL_LIGHTING);
    applyMaterial(MAT_MOON);
    glPushMatrix();
    glTranslatef(moonX, moonY, moonZ);
    drawSphere(moonRadius, 24, 20);
    glPopMatrix();

    // Soft celestial atmospheric lunar halo
    drawBillboardHalo(moonX, moonY, moonZ, moonRadius * 2.8f, 0.75f, 0.85f, 1.0f, 0.50f);

    glPopAttrib();
}

// ============================================================================
// PLANAR PROJECTED SHADOW MATRIX & SHADOW PASS
// ============================================================================

// Build a 4x4 matrix that projects geometry onto plane Ax + By + Cz + D = 0
// with respect to light position (lx, ly, lz, lw)
void buildShadowMatrix(float shadowMat[16], const float groundPlane[4], const float lightPos[4]) {
    float dot = groundPlane[0] * lightPos[0] +
                groundPlane[1] * lightPos[1] +
                groundPlane[2] * lightPos[2] +
                groundPlane[3] * lightPos[3];

    shadowMat[0]  = dot - lightPos[0] * groundPlane[0];
    shadowMat[4]  = 0.0f - lightPos[0] * groundPlane[1];
    shadowMat[8]  = 0.0f - lightPos[0] * groundPlane[2];
    shadowMat[12] = 0.0f - lightPos[0] * groundPlane[3];

    shadowMat[1]  = 0.0f - lightPos[1] * groundPlane[0];
    shadowMat[5]  = dot - lightPos[1] * groundPlane[1];
    shadowMat[9]  = 0.0f - lightPos[1] * groundPlane[2];
    shadowMat[13] = 0.0f - lightPos[1] * groundPlane[3];

    shadowMat[2]  = 0.0f - lightPos[2] * groundPlane[0];
    shadowMat[6]  = 0.0f - lightPos[2] * groundPlane[1];
    shadowMat[10] = dot - lightPos[2] * groundPlane[2];
    shadowMat[14] = 0.0f - lightPos[2] * groundPlane[3];

    shadowMat[3]  = 0.0f - lightPos[3] * groundPlane[0];
    shadowMat[7]  = 0.0f - lightPos[3] * groundPlane[1];
    shadowMat[11] = 0.0f - lightPos[3] * groundPlane[2];
    shadowMat[15] = dot - lightPos[3] * groundPlane[3];
}

// Render simplified shadow caster geometry for ground planar projection
void renderShadowCasters() {
    drawHouse();
    drawRustedCar(9.0f, 8.5f, -32.0f);
    drawCreepyTree(-11.0f,  4.0f, 0.45f, 10.5f,  20.0f);
    drawCreepyTree( 12.0f,  2.0f, 0.50f, 12.0f, -40.0f);
    drawCreepyTree(-14.0f, 16.0f, 0.38f,  8.5f,  65.0f);
    drawCreepyTree( 15.0f, 15.0f, 0.42f,  9.5f, -15.0f);
}

void renderPlanarShadows() {
    if (!g_light1DirectionalOn) return; // Shadows cast predominantly by the low moonlight

    float groundPlane[4] = { 0.0f, 1.0f, 0.0f, -0.005f }; // slightly above y=0 to prevent z-fighting
    float shadowMatrix[16];
    buildShadowMatrix(shadowMatrix, groundPlane, g_moonDir);

    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG); // Keep shadows crisp on the ground

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Semi-transparent deep dark shadow color
    glColor4f(0.01f, 0.02f, 0.04f, 0.55f);

    glPushMatrix();
    glMultMatrixf(shadowMatrix);
    renderShadowCasters();
    glPopMatrix();

    glPopAttrib();
}

// ============================================================================
// MAIN SCENE RENDERING PIPELINE
// ============================================================================
void render3DScene() {
    // ------------------------------------------------------------------------
    // 1. UPDATE & POSITION THE FOUR LIGHTS
    // ------------------------------------------------------------------------

    // Light 0: POINT LIGHT (Porch Bulb)
    if (g_light0PointOn) {
        glEnable(GL_LIGHT0);
        float pPos[4] = { g_bulbCurX, g_bulbCurY, g_bulbCurZ, 1.0f }; // w = 1.0 (Positional)
        glLightfv(GL_LIGHT0, GL_POSITION, pPos);

        // Dynamic flickering modulation
        float pDiff[4] = { 
            1.0f * g_bulbFlickerFactor, 
            0.82f * g_bulbFlickerFactor, 
            0.35f * g_bulbFlickerFactor, 
            1.0f 
        };
        glLightfv(GL_LIGHT0, GL_DIFFUSE, pDiff);
    } else {
        glDisable(GL_LIGHT0);
    }

    // Light 1: DIRECTIONAL LIGHT (Moonlight)
    if (g_light1DirectionalOn) {
        glEnable(GL_LIGHT1);
        glLightfv(GL_LIGHT1, GL_POSITION, g_moonDir); // w = 0.0 (Directional)
    } else {
        glDisable(GL_LIGHT1);
    }

    // Light 2: SPOT LIGHT (First-Person Flashlight)
    if (g_light2SpotOn) {
        glEnable(GL_LIGHT2);
        // Positioned at camera eye with slight right-hand offset
        float radYaw   = g_cam.yaw * (float)M_PI / 180.0f;
        float radPitch = g_cam.pitch * (float)M_PI / 180.0f;
        float fx = std::cos(radYaw) * std::cos(radPitch);
        float fy = std::sin(radPitch);
        float fz = std::sin(radYaw) * std::cos(radPitch);

        float rx = -std::sin(radYaw);
        float rz =  std::cos(radYaw);

        float flashPosX = g_cam.x + rx * 0.25f;
        float flashPosY = g_cam.y - 0.15f;
        float flashPosZ = g_cam.z + rz * 0.25f;

        float flashPos[4] = { flashPosX, flashPosY, flashPosZ, 1.0f };
        float flashDir[3] = { fx, fy, fz };

        glLightfv(GL_LIGHT2, GL_POSITION, flashPos);
        glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, flashDir);
    } else {
        glDisable(GL_LIGHT2);
    }

    // Light 3: AREA LIGHT EMULATION (Warm Window Interior Glow)
    if (g_light3AreaOn) {
        glEnable(GL_LIGHT3);
        float winPos[4] = { 3.5f, 3.2f, 5.85f, 1.0f }; // w = 1.0 (Positional point at window center)
        glLightfv(GL_LIGHT3, GL_POSITION, winPos);
    } else {
        glDisable(GL_LIGHT3);
    }

    // Fog toggle
    if (g_fogEnabled) glEnable(GL_FOG); else glDisable(GL_FOG);

    // ------------------------------------------------------------------------
    // 2. RENDER ENVIRONMENT & OBJECTS
    // ------------------------------------------------------------------------
    drawMoonAndStars();
    drawGround();
    drawHouse();
    drawHangingBulb();
    drawPumpkinArray();
    drawRustedCar(9.0f, 8.5f, -32.0f);
    drawFenceAndYardProps();

    // Bare Creepy Spooky Trees around yard
    drawCreepyTree(-11.0f,  4.0f, 0.45f, 10.5f,  20.0f);
    drawCreepyTree( 12.0f,  2.0f, 0.50f, 12.0f, -40.0f);
    drawCreepyTree(-14.0f, 16.0f, 0.38f,  8.5f,  65.0f);
    drawCreepyTree( 15.0f, 15.0f, 0.42f,  9.5f, -15.0f);
    drawCreepyTree( -7.0f, 26.0f, 0.40f,  9.0f,  10.0f);
    drawCreepyTree(  8.0f, 27.0f, 0.44f, 11.0f, -25.0f);

    // ------------------------------------------------------------------------
    // 3. RENDER PLANAR PROJECTED SHADOWS
    // ------------------------------------------------------------------------
    renderPlanarShadows();
}

// ============================================================================
// 2D HUD, CINEMATIC OVERLAY & TITLE SCREEN
// ============================================================================

// Helper to draw clean bitmap text on screen
void drawString2D(float x, float y, void* font, const char* str, float r, float g, float b, float a = 1.0f) {
    glColor4f(r, g, b, a);
    glRasterPos2f(x, y);
    while (*str) {
        glutBitmapCharacter(font, *str);
        str++;
    }
}

// Draw a semi-transparent 2D rounded-look quad panel
void drawUIPanel(float x, float y, float w, float h, float r, float g, float b, float a) {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();

    // Border outline
    glColor4f(r * 1.8f + 0.1f, g * 1.8f + 0.1f, b * 1.8f + 0.15f, a * 1.4f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();
}

// Screen-edge Vignette effect for eerie horror cinematography
void drawScreenVignette() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float w = (float)g_windowWidth;
    float h = (float)g_windowHeight;
    float inset = 90.0f;

    // Top border dark gradient
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(w,    0.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(w,    inset);
    glVertex2f(0.0f, inset);

    // Bottom border gradient
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, h - inset);
    glVertex2f(w,    h - inset);
    glColor4f(0.0f, 0.0f, 0.0f, 0.85f);
    glVertex2f(w,    h);
    glVertex2f(0.0f, h);

    // Left border gradient
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glVertex2f(0.0f,  0.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(inset, 0.0f);
    glVertex2f(inset, h);
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glVertex2f(0.0f,  h);

    // Right border gradient
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(w - inset, 0.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glVertex2f(w,         0.0f);
    glVertex2f(w,         h);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(w - inset, h);
    glEnd();
}

// 2D Intro Title Screen
void renderTitleScreen() {
    float w = (float)g_windowWidth;
    float h = (float)g_windowHeight;

    // Background dark cinematic backdrop
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    glColor4f(0.015f, 0.02f, 0.04f, 1.0f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(w,    0.0f);
    glColor4f(0.035f, 0.015f, 0.02f, 1.0f);
    glVertex2f(w,    h);
    glVertex2f(0.0f, h);
    glEnd();

    drawScreenVignette();

    // Central Title Card Container
    float cardW = 760.0f;
    float cardH = 460.0f;
    float cardX = (w - cardW) * 0.5f;
    float cardY = (h - cardH) * 0.5f;

    drawUIPanel(cardX, cardY, cardW, cardH, 0.04f, 0.05f, 0.08f, 0.88f);

    // Title & Subtitles
    drawString2D(cardX + 180.0f, cardY + 55.0f,  GLUT_BITMAP_TIMES_ROMAN_24, "HORROR HOUSE AT NIGHT", 1.0f, 0.35f, 0.15f);
    drawString2D(cardX + 160.0f, cardY + 85.0f,  GLUT_BITMAP_HELVETICA_18,   "Advanced 4-Light Model Demonstration", 0.85f, 0.85f, 0.95f);

    // Divider Line
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor4f(0.9f, 0.45f, 0.1f, 0.7f);
    glVertex2f(cardX + 40.0f, cardY + 105.0f);
    glVertex2f(cardX + cardW - 40.0f, cardY + 105.0f);
    glEnd();

    // Student & Project Information
    drawString2D(cardX + 50.0f, cardY + 140.0f, GLUT_BITMAP_HELVETICA_12, "PROJECT       : Computer Graphics Sessional (CSE 4-1)", 0.75f, 0.85f, 1.0f);
    drawString2D(cardX + 50.0f, cardY + 165.0f, GLUT_BITMAP_HELVETICA_18, "DEVELOPER     : MD JAHID HASAN JIM", 1.0f, 0.85f, 0.35f);
    drawString2D(cardX + 50.0f, cardY + 190.0f, GLUT_BITMAP_HELVETICA_18, "ROLL NUMBER   : 2107054", 1.0f, 0.85f, 0.35f);

    // Core Lighting Features Summary Box
    drawUIPanel(cardX + 40.0f, cardY + 215.0f, cardW - 80.0f, 155.0f, 0.02f, 0.03f, 0.05f, 0.75f);
    drawString2D(cardX + 55.0f, cardY + 240.0f, GLUT_BITMAP_HELVETICA_12, "[Light 1] POINT LIGHT       : Hanging Porch Bulb (w=1.0, Attenuation, Pendulum & Flicker)", 1.0f, 0.85f, 0.3f);
    drawString2D(cardX + 55.0f, cardY + 265.0f, GLUT_BITMAP_HELVETICA_12, "[Light 2] DIRECTIONAL LIGHT : Moonlight Sky (w=0.0, Low Angle, Casts Planar Shadows)", 0.4f, 0.75f, 1.0f);
    drawString2D(cardX + 55.0f, cardY + 290.0f, GLUT_BITMAP_HELVETICA_12, "[Light 3] SPOT LIGHT        : Flashlight (Positional, 18.5 deg Cone Cutoff, Follows Camera)", 0.9f, 0.95f, 1.0f);
    drawString2D(cardX + 55.0f, cardY + 315.0f, GLUT_BITMAP_HELVETICA_12, "[Light 4] AREA LIGHT EMUL.  : Parlor Window Glow (Elevated Ambient Dispersion)", 1.0f, 0.6f, 0.2f);
    drawString2D(cardX + 55.0f, cardY + 345.0f, GLUT_BITMAP_HELVETICA_12, "+ GL_FOG Atmosphere, Phong Materials, Carved Jack-o'-Lanterns & Cinematic Camera Tour", 0.6f, 0.9f, 0.6f);

    // Flashing "Press ENTER to begin"
    float pulse = 0.6f + 0.4f * std::sin(g_time * 5.0f);
    drawString2D(cardX + 220.0f, cardY + 415.0f, GLUT_BITMAP_HELVETICA_18, ">> PRESS  [ ENTER ]  OR  [ SPACE ]  TO ENTER <<", 1.0f * pulse, 0.8f * pulse, 0.2f * pulse);
}

// In-Scene HUD Overlay
void renderSceneHUD() {
    float w = (float)g_windowWidth;
    float h = (float)g_windowHeight;

    drawScreenVignette();

    // Center Flashlight Aim Reticle / Crosshair (+)
    glDisable(GL_LIGHTING);
    glLineWidth(1.5f);
    glColor4f(1.0f, 1.0f, 1.0f, 0.35f);
    glBegin(GL_LINES);
    glVertex2f(w * 0.5f - 8.0f, h * 0.5f);
    glVertex2f(w * 0.5f + 8.0f, h * 0.5f);
    glVertex2f(w * 0.5f, h * 0.5f - 8.0f);
    glVertex2f(w * 0.5f, h * 0.5f + 8.0f);
    glEnd();

    if (!g_showHUD) return;

    // Top Header Banner
    drawUIPanel(20.0f, 15.0f, 440.0f, 40.0f, 0.04f, 0.05f, 0.08f, 0.80f);
    drawString2D(35.0f, 40.0f, GLUT_BITMAP_HELVETICA_18, "HORROR HOUSE AT NIGHT", 1.0f, 0.4f, 0.15f);
    
    char fpsStr[32];
    snprintf(fpsStr, sizeof(fpsStr), "FPS: %.0f", g_fps);
    drawString2D(370.0f, 40.0f, GLUT_BITMAP_HELVETICA_12, fpsStr, 0.4f, 0.9f, 0.4f);

    // Left Panel: 4-Light Live Status Indicator Card
    drawUIPanel(20.0f, 65.0f, 440.0f, 160.0f, 0.03f, 0.04f, 0.07f, 0.82f);
    drawString2D(35.0f, 90.0f, GLUT_BITMAP_HELVETICA_12, "LIGHTING STATUS [Toggle 1, 2, 3, 4, 0]:", 0.9f, 0.85f, 0.6f);

    // Light 0 Status
    if (g_light0PointOn) {
        drawString2D(35.0f, 115.0f, GLUT_BITMAP_HELVETICA_12, "[1] Point Light (Porch Bulb)  : [ ON ] Warm Amber (Sway & Flicker)", 0.2f, 1.0f, 0.3f);
    } else {
        drawString2D(35.0f, 115.0f, GLUT_BITMAP_HELVETICA_12, "[1] Point Light (Porch Bulb)  : [ OFF ]", 0.7f, 0.2f, 0.2f);
    }

    // Light 1 Status
    if (g_light1DirectionalOn) {
        drawString2D(35.0f, 140.0f, GLUT_BITMAP_HELVETICA_12, "[2] Directional (Moonlight)   : [ ON ] Cool Silvery Blue (Shadows)", 0.4f, 0.8f, 1.0f);
    } else {
        drawString2D(35.0f, 140.0f, GLUT_BITMAP_HELVETICA_12, "[2] Directional (Moonlight)   : [ OFF ]", 0.7f, 0.2f, 0.2f);
    }

    // Light 2 Status
    if (g_light2SpotOn) {
        drawString2D(35.0f, 165.0f, GLUT_BITMAP_HELVETICA_12, "[3] Spot Light (Flashlight)   : [ ON ] Focused 18.5 deg Beam [F]", 1.0f, 1.0f, 0.4f);
    } else {
        drawString2D(35.0f, 165.0f, GLUT_BITMAP_HELVETICA_12, "[3] Spot Light (Flashlight)   : [ OFF ] [F]", 0.7f, 0.2f, 0.2f);
    }

    // Light 3 Status
    if (g_light3AreaOn) {
        drawString2D(35.0f, 190.0f, GLUT_BITMAP_HELVETICA_12, "[4] Area Light Emul (Window)  : [ ON ] Soft Ambient Dispersion", 1.0f, 0.6f, 0.2f);
    } else {
        drawString2D(35.0f, 190.0f, GLUT_BITMAP_HELVETICA_12, "[4] Area Light Emul (Window)  : [ OFF ]", 0.7f, 0.2f, 0.2f);
    }

    // Features Status row
    char featStr[128];
    snprintf(featStr, sizeof(featStr), "Fog: %s [G] | Bulb Sway: %s [B] | Tour: %s [C]",
             g_fogEnabled ? "ON" : "OFF",
             g_bulbAnimEnabled ? "ON" : "OFF",
             g_cinematicMode ? "ACTIVE" : "OFF");
    drawString2D(35.0f, 215.0f, GLUT_BITMAP_HELVETICA_12, featStr, 0.8f, 0.8f, 0.9f);

    // Right / Bottom Controls Cheat-Sheet
    drawUIPanel(w - 380.0f, 15.0f, 360.0f, 145.0f, 0.03f, 0.04f, 0.07f, 0.80f);
    drawString2D(w - 365.0f, 38.0f,  GLUT_BITMAP_HELVETICA_12, "CONTROLS GUIDE:", 0.9f, 0.85f, 0.6f);
    drawString2D(w - 365.0f, 60.0f,  GLUT_BITMAP_HELVETICA_12, "W, A, S, D     : First-Person Walk / Strafe", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 80.0f,  GLUT_BITMAP_HELVETICA_12, "Mouse Move     : Look Around (Yaw / Pitch)", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 100.0f, GLUT_BITMAP_HELVETICA_12, "Space / Ctrl   : Fly Up / Fly Down", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 120.0f, GLUT_BITMAP_HELVETICA_12, "C: Auto-Tour | R: Reset View | H: Toggle HUD", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 140.0f, GLUT_BITMAP_HELVETICA_12, "ESC: Quit Application", 0.8f, 0.85f, 0.9f);

    // Cinematic Mode Banner when active
    if (g_cinematicMode) {
        drawUIPanel(w * 0.5f - 240.0f, h - 65.0f, 480.0f, 45.0f, 0.08f, 0.03f, 0.02f, 0.90f);
        drawString2D(w * 0.5f - 210.0f, h - 38.0f, GLUT_BITMAP_HELVETICA_18, "CINEMATIC AUTO-TOUR ACTIVE", 1.0f, 0.45f, 0.2f);
        drawString2D(w * 0.5f + 90.0f, h - 38.0f, GLUT_BITMAP_HELVETICA_12, "[Press 'C' to Exit]", 0.8f, 0.8f, 0.8f);
    }
}

// ============================================================================
// CINEMATIC AUTO-TOUR CAMERA FLIGHT PATH
// ============================================================================
struct Keyframe {
    float time;
    float x, y, z;
    float yaw, pitch;
};

const Keyframe TOUR_KEYS[] = {
    {  0.0f,   0.0f,  2.2f, 26.0f, -90.0f,  -2.0f }, // 1. Establishing view at gate
    {  6.0f,  -2.5f,  1.6f, 14.0f, -80.0f,   5.0f }, // 2. Low angle along pumpkin path
    { 12.0f,  -2.8f,  2.8f,  7.0f, -75.0f,  18.0f }, // 3. Under porch looking at swinging bulb
    { 18.0f,   3.2f,  3.2f,  9.5f, -95.0f,   2.0f }, // 4. Approaching glowing window
    { 24.0f,   8.8f,  1.8f, 12.0f, -145.0f, -4.0f }, // 5. Orbiting rusted car
    { 30.0f,   9.5f,  7.5f, 22.0f, -120.0f, -18.0f },// 6. High crane view of house & full moon
    { 36.0f,   0.0f,  2.2f, 26.0f, -90.0f,  -2.0f }  // 7. Loop back to entrance
};
const int NUM_TOUR_KEYS = sizeof(TOUR_KEYS) / sizeof(TOUR_KEYS[0]);
const float TOTAL_TOUR_DURATION = 36.0f;

void updateCinematicCamera(float dt) {
    g_cinematicTime += dt;
    if (g_cinematicTime >= TOTAL_TOUR_DURATION) {
        g_cinematicTime = std::fmod(g_cinematicTime, TOTAL_TOUR_DURATION);
    }

    // Find current keyframe segment
    int idx = 0;
    for (int i = 0; i < NUM_TOUR_KEYS - 1; ++i) {
        if (g_cinematicTime >= TOUR_KEYS[i].time && g_cinematicTime <= TOUR_KEYS[i + 1].time) {
            idx = i;
            break;
        }
    }

    float segDur = TOUR_KEYS[idx + 1].time - TOUR_KEYS[idx].time;
    float t = (g_cinematicTime - TOUR_KEYS[idx].time) / segDur;
    // Smooth step curve (Hermite interpolation)
    float s = t * t * (3.0f - 2.0f * t);

    g_cam.x = TOUR_KEYS[idx].x + (TOUR_KEYS[idx + 1].x - TOUR_KEYS[idx].x) * s;
    g_cam.y = TOUR_KEYS[idx].y + (TOUR_KEYS[idx + 1].y - TOUR_KEYS[idx].y) * s;
    g_cam.z = TOUR_KEYS[idx].z + (TOUR_KEYS[idx + 1].z - TOUR_KEYS[idx].z) * s;
    g_cam.yaw = TOUR_KEYS[idx].yaw + (TOUR_KEYS[idx + 1].yaw - TOUR_KEYS[idx].yaw) * s;
    g_cam.pitch = TOUR_KEYS[idx].pitch + (TOUR_KEYS[idx + 1].pitch - TOUR_KEYS[idx].pitch) * s;
}

// ============================================================================
// INPUT PROCESSING & CAMERA MOVEMENT
// ============================================================================
void processKeyboardInput(float dt) {
    if (g_cinematicMode) return; // Camera controlled by auto-tour

    float radYaw = g_cam.yaw * (float)M_PI / 180.0f;
    float fx = std::cos(radYaw);
    float fz = std::sin(radYaw);

    float rx = -std::sin(radYaw);
    float rz =  std::cos(radYaw);

    float moveSpeed = g_cam.speed * dt;

    if (g_keyState['w'] || g_keyState['W']) {
        g_cam.x += fx * moveSpeed;
        g_cam.z += fz * moveSpeed;
    }
    if (g_keyState['s'] || g_keyState['S']) {
        g_cam.x -= fx * moveSpeed;
        g_cam.z -= fz * moveSpeed;
    }
    if (g_keyState['a'] || g_keyState['A']) {
        g_cam.x -= rx * moveSpeed;
        g_cam.z -= rz * moveSpeed;
    }
    if (g_keyState['d'] || g_keyState['D']) {
        g_cam.x += rx * moveSpeed;
        g_cam.z += rz * moveSpeed;
    }
    if (g_keyState[' ']) {
        g_cam.y += moveSpeed;
    }
    if (g_isCtrlPressed || g_keyState['x'] || g_keyState['X']) {
        g_cam.y -= moveSpeed;
        if (g_cam.y < 0.5f) g_cam.y = 0.5f; // prevent going below ground
    }
}

// ============================================================================
// GLUT CALLBACK FUNCTIONS
// ============================================================================
void displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (g_appState == STATE_TITLE) {
        // 2D Orthographic view for Title Screen
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, g_windowWidth, g_windowHeight, 0.0);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        renderTitleScreen();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    } else {
        // 3D Perspective Projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0, (double)g_windowWidth / (double)g_windowHeight, 0.2, 350.0);

        // 3D Modelview Camera Transform
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        float radYaw   = g_cam.yaw * (float)M_PI / 180.0f;
        float radPitch = g_cam.pitch * (float)M_PI / 180.0f;
        float targetX = g_cam.x + std::cos(radYaw) * std::cos(radPitch);
        float targetY = g_cam.y + std::sin(radPitch);
        float targetZ = g_cam.z + std::sin(radYaw) * std::cos(radPitch);

        gluLookAt(g_cam.x, g_cam.y, g_cam.z,
                  targetX, targetY, targetZ,
                  0.0, 1.0, 0.0);

        render3DScene();

        // 2D Orthographic HUD Overlay
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, g_windowWidth, g_windowHeight, 0.0);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        renderSceneHUD();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    glutSwapBuffers();
}

void reshapeCallback(int w, int h) {
    if (h == 0) h = 1;
    g_windowWidth  = w;
    g_windowHeight = h;
    glViewport(0, 0, w, h);
}

void idleCallback() {
    int curTimeMs = glutGet(GLUT_ELAPSED_TIME);
    if (g_prevTimeMs == 0) g_prevTimeMs = curTimeMs;
    g_deltaTime = (curTimeMs - g_prevTimeMs) * 0.001f;
    g_prevTimeMs = curTimeMs;
    if (g_deltaTime > 0.1f) g_deltaTime = 0.1f; // Clamp delta spike

    g_time += g_deltaTime;

    // FPS Counter calculation
    g_frameCount++;
    g_fpsTimer += g_deltaTime;
    if (g_fpsTimer >= 0.5f) {
        g_fps = g_frameCount / g_fpsTimer;
        g_frameCount = 0;
        g_fpsTimer = 0.0f;
    }

    // Porch Bulb random filament flicker physics
    if (g_bulbAnimEnabled) {
        float f1 = std::sin(g_time * 18.0f);
        float f2 = std::cos(g_time * 33.0f);
        float noise = ((float)(rand() % 100) / 100.0f) * 0.12f;
        g_bulbFlickerFactor = 0.88f + 0.08f * f1 * f2 + noise;
        if (rand() % 90 == 0) g_bulbFlickerFactor = 0.25f; // Occasional deep flicker dip
    } else {
        g_bulbFlickerFactor = 1.0f;
    }

    if (g_cinematicMode) {
        updateCinematicCamera(g_deltaTime);
    } else {
        processKeyboardInput(g_deltaTime);
    }

    glutPostRedisplay();
}

// Save current framebuffer render to a 24-bit uncompressed BMP image
void saveScreenshot(const char* filename) {
    int w = g_windowWidth;
    int h = g_windowHeight;
    int rowPadded = (w * 3 + 3) & (~3);
    int dataSize = rowPadded * h;

    std::vector<unsigned char> pixels(dataSize, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadPixels(0, 0, w, h, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels.data());

    unsigned char fileHeader[14] = {
        'B', 'M',
        0, 0, 0, 0, // file size
        0, 0, 0, 0, // reserved
        54, 0, 0, 0 // offset
    };
    int fileSize = 54 + dataSize;
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);

    unsigned char infoHeader[40] = {
        40, 0, 0, 0,
        0, 0, 0, 0, // width
        0, 0, 0, 0, // height
        1, 0,       // planes
        24, 0,      // bits per pixel
        0, 0, 0, 0, // compression
        0, 0, 0, 0, // image size
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    infoHeader[4]  = (unsigned char)(w);
    infoHeader[5]  = (unsigned char)(w >> 8);
    infoHeader[6]  = (unsigned char)(w >> 16);
    infoHeader[7]  = (unsigned char)(w >> 24);
    infoHeader[8]  = (unsigned char)(h);
    infoHeader[9]  = (unsigned char)(h >> 8);
    infoHeader[10] = (unsigned char)(h >> 16);
    infoHeader[11] = (unsigned char)(h >> 24);
    infoHeader[20] = (unsigned char)(dataSize);
    infoHeader[21] = (unsigned char)(dataSize >> 8);
    infoHeader[22] = (unsigned char)(dataSize >> 16);
    infoHeader[23] = (unsigned char)(dataSize >> 24);

    FILE* fp = fopen(filename, "wb");
    if (fp) {
        fwrite(fileHeader, 1, 14, fp);
        fwrite(infoHeader, 1, 40, fp);
        fwrite(pixels.data(), 1, dataSize, fp);
        fclose(fp);
        std::cout << "[SCREENSHOT] Saved capture to " << filename << std::endl;
    } else {
        std::cerr << "[SCREENSHOT ERROR] Could not open file " << filename << " for writing." << std::endl;
    }
}

void keyboardDownCallback(unsigned char key, int x, int y) {
    (void)x; (void)y;
    g_keyState[key] = true;

    if (g_appState == STATE_TITLE) {
        if (key == 13 || key == ' ') { // Enter or Space to begin
            g_appState = STATE_SCENE;
        }
        if (key == 27) exit(0);
        return;
    }

    switch (key) {
        case 27: // ESC
            exit(0);
            break;
        case '1':
            g_light0PointOn = !g_light0PointOn;
            std::cout << "[LIGHT 0] Point Light (Porch Bulb) : " << (g_light0PointOn ? "ON" : "OFF") << std::endl;
            break;
        case '2':
            g_light1DirectionalOn = !g_light1DirectionalOn;
            std::cout << "[LIGHT 1] Directional (Moonlight)   : " << (g_light1DirectionalOn ? "ON" : "OFF") << std::endl;
            break;
        case '3':
        case 'f':
        case 'F':
            g_light2SpotOn = !g_light2SpotOn;
            std::cout << "[LIGHT 2] Spot Light (Flashlight)   : " << (g_light2SpotOn ? "ON" : "OFF") << std::endl;
            break;
        case '4':
            g_light3AreaOn = !g_light3AreaOn;
            std::cout << "[LIGHT 3] Area Light (Window Glow)  : " << (g_light3AreaOn ? "ON" : "OFF") << std::endl;
            break;
        case '0': {
            bool anyOn = g_light0PointOn || g_light1DirectionalOn || g_light2SpotOn || g_light3AreaOn;
            g_light0PointOn = g_light1DirectionalOn = g_light2SpotOn = g_light3AreaOn = !anyOn;
            std::cout << "[LIGHTS] Master Toggle : " << (!anyOn ? "ALL ON" : "ALL OFF") << std::endl;
            break;
        }
        case 'g':
        case 'G':
            g_fogEnabled = !g_fogEnabled;
            std::cout << "[ATMOSPHERE] Fog : " << (g_fogEnabled ? "ON" : "OFF") << std::endl;
            break;
        case 'b':
        case 'B':
            g_bulbAnimEnabled = !g_bulbAnimEnabled;
            std::cout << "[ANIMATION] Bulb Sway & Flicker : " << (g_bulbAnimEnabled ? "ON" : "OFF") << std::endl;
            break;
        case 'c':
        case 'C':
            g_cinematicMode = !g_cinematicMode;
            if (g_cinematicMode) g_cinematicTime = 0.0f;
            std::cout << "[CAMERA] Cinematic Auto-Tour : " << (g_cinematicMode ? "ACTIVE" : "DISABLED") << std::endl;
            break;
        case 'p':
        case 'P': {
            static int ssCount = 1;
            char ssName[64];
            snprintf(ssName, sizeof(ssName), "screenshot_%03d.bmp", ssCount++);
            saveScreenshot(ssName);
            break;
        }
        case 'h':
        case 'H':
            g_showHUD = !g_showHUD;
            break;
        case 'r':
        case 'R':
            // Reset camera to starting position
            g_cam.x = 0.0f; g_cam.y = 2.2f; g_cam.z = 28.0f;
            g_cam.yaw = -90.0f; g_cam.pitch = -2.0f;
            g_cinematicMode = false;
            std::cout << "[CAMERA] Reset to Start Position" << std::endl;
            break;
    }
}

void keyboardUpCallback(unsigned char key, int x, int y) {
    (void)x; (void)y;
    g_keyState[key] = false;
}

void specialKeyDownCallback(int key, int x, int y) {
    (void)x; (void)y;
    int mod = glutGetModifiers();
    if (mod & GLUT_ACTIVE_CTRL) {
        g_isCtrlPressed = true;
    }
    if (key == GLUT_KEY_PAGE_DOWN || key == GLUT_KEY_DOWN) {
        g_isCtrlPressed = true;
    }
}

void specialKeyUpCallback(int key, int x, int y) {
    (void)x; (void)y;
    int mod = glutGetModifiers();
    if (!(mod & GLUT_ACTIVE_CTRL)) {
        g_isCtrlPressed = false;
    }
    if (key == GLUT_KEY_PAGE_DOWN || key == GLUT_KEY_DOWN) {
        g_isCtrlPressed = false;
    }
}

void mouseMotionCallback(int x, int y) {
    if (g_appState != STATE_SCENE || g_cinematicMode) return;

    if (g_lastMouseX == -1 || g_lastMouseY == -1) {
        g_lastMouseX = x;
        g_lastMouseY = y;
        return;
    }

    float dx = (float)(x - g_lastMouseX);
    float dy = (float)(y - g_lastMouseY);

    g_lastMouseX = x;
    g_lastMouseY = y;

    g_cam.yaw   += dx * g_cam.sens;
    g_cam.pitch -= dy * g_cam.sens;

    // Clamp pitch to prevent camera gimbal inversion
    if (g_cam.pitch >  85.0f) g_cam.pitch =  85.0f;
    if (g_cam.pitch < -85.0f) g_cam.pitch = -85.0f;
}

void mousePassiveMotionCallback(int x, int y) {
    mouseMotionCallback(x, y);
}

void mouseButtonCallback(int button, int state, int x, int y) {
    (void)x; (void)y;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (g_appState == STATE_TITLE) {
            g_appState = STATE_SCENE;
        } else {
            // Left click toggles flashlight as a handy shortcut
            g_light2SpotOn = !g_light2SpotOn;
        }
    }
    if (state == GLUT_UP) {
        g_lastMouseX = -1;
        g_lastMouseY = -1;
    }
}

// ============================================================================
// ENTRY POINT
// ============================================================================
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_INIT_WIDTH, WINDOW_INIT_HEIGHT);
    glutInitWindowPosition(80, 50);
    glutCreateWindow("Horror House at Night - 3D 4-Light Scene (Jim 2107054)");

    initOpenGL();

    // Register GLUT Event Callbacks
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutIdleFunc(idleCallback);
    glutKeyboardFunc(keyboardDownCallback);
    glutKeyboardUpFunc(keyboardUpCallback);
    glutSpecialFunc(specialKeyDownCallback);
    glutSpecialUpFunc(specialKeyUpCallback);
    glutMotionFunc(mouseMotionCallback);
    glutPassiveMotionFunc(mousePassiveMotionCallback);
    glutMouseFunc(mouseButtonCallback);

    std::cout << "==========================================================" << std::endl;
    std::cout << "  HORROR HOUSE AT NIGHT - COMPUTER GRAPHICS PROJECT       " << std::endl;
    std::cout << "  Developer: MD Jahid Hasan Jim (Roll: 2107054)           " << std::endl;
    std::cout << "==========================================================" << std::endl;
    std::cout << "  [1] Toggle Point Light (Porch Bulb)                     " << std::endl;
    std::cout << "  [2] Toggle Directional Light (Moonlight)                " << std::endl;
    std::cout << "  [3/F] Toggle Spot Light (Flashlight)                    " << std::endl;
    std::cout << "  [4] Toggle Area Light (Window Interior Glow)            " << std::endl;
    std::cout << "  [0] Master Toggle All Lights                            " << std::endl;
    std::cout << "  [G] Toggle Fog                                          " << std::endl;
    std::cout << "  [B] Toggle Bulb Sway & Flicker                          " << std::endl;
    std::cout << "  [C] Toggle Cinematic Auto-Tour Presentation             " << std::endl;
    std::cout << "  [H] Toggle HUD Overlay                                  " << std::endl;
    std::cout << "  [R] Reset Camera Position                               " << std::endl;
    std::cout << "  [W/A/S/D + Mouse] First-Person Exploration              " << std::endl;
    std::cout << "==========================================================" << std::endl;

    glutMainLoop();
    return 0;
}
