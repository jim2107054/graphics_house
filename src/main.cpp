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
     * Definition   : A positional light source emitting rays spherically in all 
                      directions from a single 3D position (homogenous coordinate w = 1.0).
     * Implementation: Position set via glLightfv(GL_LIGHT0, GL_POSITION, pos) where
                      pos = (x, y, z, 1.0f).
     * Attenuation  : Decays with distance via 1 / (kc + kl*d + kq*d^2) using
                      GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION, GL_QUADRATIC_ATTENUATION.
     * Animation    : Sways dynamically like a physical pendulum using sin(t) / cos(t) 
                      harmonic functions; flickers randomly to simulate an incandescent bulb.

  2. DIRECTIONAL LIGHT (GL_LIGHT1) - Moonlit Sky
     * Definition   : Light rays arrive parallel from an infinite distance (such as the 
                      sun or moon). There is no specific position, only a direction vector 
                      (homogenous coordinate w = 0.0).
     * Implementation: Position set via glLightfv(GL_LIGHT1, GL_POSITION, dir) where
                      dir = (-dx, -dy, -dz, 0.0f).
     * Attenuation  : Distance attenuation does NOT apply to directional lights.
     * Visual Role  : Low-angle cool blue-white illumination casting long dramatic planar 
                      shadows across the wet ground and yard.

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
  TEXTURE MAPPING (GL_MODULATE & UV COORDINATES)
================================================================================
  * Texture Loader  : Uses stb_image to load standard images (PNG, JPG, BMP) from the
                      textures/ directory. If an image file is missing, the system 
                      automatically synthesizes high-detail procedural texture patterns 
                      in memory so it never crashes!
  * Texture Wrap    : GL_TEXTURE_WRAP_S and GL_TEXTURE_WRAP_T set to GL_REPEAT so textures 
                      tile naturally across wide walls, roofs, terrain, and cylindrical trunks.
  * Texture Filter  : Uses mipmapped filtering (GL_LINEAR_MIPMAP_LINEAR) for crisp close-ups 
                      and smooth, anti-aliased distant views without moire artifacts.
  * Environment Mode: GL_MODULATE multiplies surface texture texels with Phong lighting 
                      (ambient + diffuse + specular), preserving all dynamic lights and shadows!
  * Toggle Key      : Press [T] to toggle Texture Mapping ON/OFF for viva comparison.

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

  Q4: Why use GL_MODULATE instead of GL_REPLACE for texture mapping?
  A4: GL_REPLACE overwrites polygon colors completely, erasing all lighting calculations. 
      GL_MODULATE multiplies texture texel color with the computed Phong lighting (C = Ct * Cl), 
      ensuring lights, spotlights, and shadows affect the textured surface realistically.

  Q5: How do Planar Projected Shadows work mathematically?
  A5: We construct a 4x4 projection matrix that flattens 3D object vertices onto the ground 
      plane (y = 0) along the ray originating from the light source. The geometry is rendered 
      in a dark semi-transparent shadow pass with lighting disabled and blending enabled.

  Q6: How are terrain surface normals calculated on undulating heightmaps?
  A6: We evaluate finite central differences along the X and Z axes:
      nx = (h(x - e, z) - h(x + e, z)) / (2*e),  ny = 1.0,  nz = (h(x, z - e) - h(x, z + e)) / (2*e)
      and normalize the resulting vector. Puddle reflections are achieved with high specular
      shininess (ns = 115) and low diffuse albedo under GL_MODULATE.

================================================================================
  CONTROLS & SHORTCUTS
================================================================================
  - [W] / [S]           : Move Camera Forward / Backward
  - [A] / [D]           : Strafe Camera Left / Right
  - [Space] / [Ctrl]    : Fly Up / Fly Down (or PageUp/PageDown)
  - [Mouse Move]        : Look Around (Yaw & Pitch, clamped to avoid gimbal flip)
  - [1]                 : Toggle Point Light (Porch Bulb)
  - [2]                 : Toggle Directional Light (Moonlight)
  - [3] / [F]           : Toggle Spot Light (Flashlight)
  - [4]                 : Toggle Area Light (Window Interior Glow)
  - [0]                 : Toggle ALL Lights (Master Switch)
  - [T]                 : Toggle Texture Mapping ON / OFF
  - [G]                 : Toggle Atmospheric Fog (GL_FOG)
  - [B]                 : Toggle Bulb Pendulum Sway & Random Flicker
  - [C]                 : Toggle Hands-Free Cinematic Auto-Tour Presentation Mode
  - [H]                 : Toggle HUD & Controls Overlay
  - [P]                 : Take Screenshot (Saves as uncompressed 24-bit .bmp)
  - [R]                 : Reset Camera to Default Reference Image View
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
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// CONSTANTS & CONFIGURATION
// ============================================================================
const int WINDOW_INIT_WIDTH  = 1280;
const int WINDOW_INIT_HEIGHT = 720;

enum AppState {
    STATE_TITLE,
    STATE_SCENE
};

AppState g_appState = STATE_TITLE;

int g_windowWidth  = WINDOW_INIT_WIDTH;
int g_windowHeight = WINDOW_INIT_HEIGHT;

float g_time = 0.0f;
float g_deltaTime = 0.016f;
int   g_prevTimeMs = 0;
int   g_frameCount = 0;
float g_fps = 60.0f;
float g_fpsTimer = 0.0f;

// Camera State (First Person - Distant 3/4 Corner Perspective View matching Reference)
struct Camera {
    float x, y, z;
    float yaw;
    float pitch;
    float speed;
    float sens;
} g_cam = {
    1.2f, 1.35f, 22.0f,
    -94.0f, 5.0f,
    12.0f,
    0.15f
};

bool g_keyState[256] = { false };
bool g_isCtrlPressed = false;
int  g_lastMouseX = -1;
int  g_lastMouseY = -1;

// House Global Positioning Offset & Orientation
const float g_houseShiftX = -2.8f;
const float g_houseShiftZ = -11.5f;
const float g_houseRotY   = -18.0f;

// Lighting & Feature Switches
bool g_light0PointOn       = true;  // Porch Bulb
bool g_light1DirectionalOn = true;  // Moonlight
bool g_light2SpotOn        = false; // Flashlight (OFF by default to remove glare on player's side)
bool g_light3AreaOn        = true;  // Window Glow

bool g_texturesEnabled     = true;  // Texture mapping switch
bool g_fogEnabled          = true;
bool g_bulbAnimEnabled     = true;
bool g_showHUD             = true;
bool g_cinematicMode       = false;
float g_cinematicTime      = 0.0f;

std::string g_autoScreenshotFile = "";
int g_autoScreenshotFrames = 0;

// Porch bulb dynamic state
float g_bulbBaseX = -1.8f;
float g_bulbBaseY = 3.6f;
float g_bulbBaseZ = -4.2f;
float g_bulbCordLength = 0.8f;
float g_bulbCurX = -1.8f;
float g_bulbCurY = 2.8f;
float g_bulbCurZ = -4.2f;
float g_bulbFlickerFactor = 1.0f;
float g_pumpkinFlicker = 1.0f;

float g_moonDir[4] = { -0.07f, 0.44f, -0.90f, 0.0f };

// ============================================================================
// DYNAMIC FALLING AUTUMN LEAVES PARTICLE SYSTEM (REQUIREMENT 2 & 7)
// ============================================================================
struct FallingLeafParticle {
    float x, y, z;
    float vy;
    float rotX, rotY, rotZ;
    float rotSpeedX, rotSpeedY, rotSpeedZ;
    float size;
    float swayPhase;
    float swayAmp;
    float swayFreq;
    float r, g, b;
    float originX, originZ;
};

const int MAX_FALLING_LEAVES = 15;
std::vector<FallingLeafParticle> g_fallingLeaves;

void initFallingLeaves() {
    g_fallingLeaves.resize(MAX_FALLING_LEAVES);
    for (int i = 0; i < MAX_FALLING_LEAVES; ++i) {
        // Distribute across tree canopies where leaf clusters exist:
        // Right foreground framing tree (50%), Left foreground framing tree (35%), Path breeze drift (15%)
        float rChoice = (float)(rand() % 100) / 100.0f;
        float ox, oz;
        if (rChoice < 0.50f) {
            ox = 5.0f + ((float)(rand() % 35) / 10.0f);   // Right foreground natural tree canopy
            oz = 13.5f + ((float)(rand() % 30) / 10.0f);  // 13.5 to 16.5
        } else if (rChoice < 0.85f) {
            ox = -7.5f + ((float)(rand() % 30) / 10.0f); // Left foreground natural tree canopy
            oz = 13.5f + ((float)(rand() % 30) / 10.0f);  // 13.5 to 16.5
        } else {
            ox = -2.0f + ((float)(rand() % 40) / 10.0f);   // Path drift
            oz = 10.0f + ((float)(rand() % 60) / 10.0f);
        }

        g_fallingLeaves[i].originX = ox;
        g_fallingLeaves[i].originZ = oz;
        g_fallingLeaves[i].x = ox;
        g_fallingLeaves[i].y = 1.8f + ((float)(rand() % 30) / 10.0f); // 1.8m to 4.8m (natural tree canopy height)
        g_fallingLeaves[i].z = oz;
        g_fallingLeaves[i].vy = 0.40f + ((float)(rand() % 40) / 100.0f); // 0.40 to 0.80 m/s slow flutter drift
        g_fallingLeaves[i].rotX = (float)(rand() % 360);
        g_fallingLeaves[i].rotY = (float)(rand() % 360);
        g_fallingLeaves[i].rotZ = (float)(rand() % 360);
        g_fallingLeaves[i].rotSpeedX = 25.0f + ((float)(rand() % 60));
        g_fallingLeaves[i].rotSpeedY = 35.0f + ((float)(rand() % 75));
        g_fallingLeaves[i].rotSpeedZ = 20.0f + ((float)(rand() % 50));
        g_fallingLeaves[i].size = 0.15f + ((float)(rand() % 10) / 100.0f);
        g_fallingLeaves[i].swayPhase = (float)(rand() % 628) / 100.0f;
        g_fallingLeaves[i].swayAmp = 0.5f + ((float)(rand() % 50) / 100.0f);
        g_fallingLeaves[i].swayFreq = 1.1f + ((float)(rand() % 60) / 100.0f);

        // Dark muted reddish-brown / desaturated orange-brown (RGB 90,55,35 to 130,80,45)
        int tone = rand() % 3;
        if (tone == 0) {
            g_fallingLeaves[i].r = 0.38f; g_fallingLeaves[i].g = 0.22f; g_fallingLeaves[i].b = 0.14f;
        } else if (tone == 1) {
            g_fallingLeaves[i].r = 0.48f; g_fallingLeaves[i].g = 0.30f; g_fallingLeaves[i].b = 0.16f;
        } else {
            g_fallingLeaves[i].r = 0.32f; g_fallingLeaves[i].g = 0.19f; g_fallingLeaves[i].b = 0.12f;
        }
    }
}

void updateFallingLeaves(float dt);
void drawFallingLeaves();


// ============================================================================
// LIGHTNING & ATMOSPHERIC PHENOMENA
// ============================================================================
struct LightningSystem {
    bool  active;
    float timer;
    float nextStrikeInterval;
    float strikeProgress;
    float strikeDuration;
    float flashIntensity;
    float thunderCountdown;
    bool  thunderPending;
} g_lightning = {
    false,
    0.0f,
    12.0f, // Initial strike after 12s
    0.0f,
    0.72f,
    0.0f,
    0.0f,
    false
};

void triggerLightning() {
    g_lightning.active = true;
    g_lightning.strikeProgress = 0.0f;
    g_lightning.strikeDuration = 0.70f + ((float)(rand() % 25) * 0.01f);
    g_lightning.timer = 0.0f;
    g_lightning.nextStrikeInterval = 14.0f + ((float)(rand() % 140) * 0.1f); // 14s to 28s
    g_lightning.thunderCountdown = 0.35f + ((float)(rand() % 35) * 0.01f);
    g_lightning.thunderPending = true;
    std::cout << "[ATMOSPHERE] Distant Lightning Flash Triggered!" << std::endl;
}

// ============================================================================
// PROCEDURAL AUDIO SYNTHESIS & WINMM PLAYBACK (ZERO EXTERNAL ASSETS NEEDED)
// ============================================================================
#ifdef _WIN32
bool g_audioEnabled = true;
std::vector<unsigned char> g_ambientWav;
std::vector<unsigned char> g_thunderWav;
float g_thunderAudioTimer = 0.0f;

void createWavHeader(unsigned char* header, int sampleRate, int numSamples) {
    int dataSize = numSamples * 2;
    int fileSize = 36 + dataSize;
    
    // "RIFF"
    header[0] = 'R'; header[1] = 'I'; header[2] = 'F'; header[3] = 'F';
    header[4] = (unsigned char)(fileSize & 0xFF);
    header[5] = (unsigned char)((fileSize >> 8) & 0xFF);
    header[6] = (unsigned char)((fileSize >> 16) & 0xFF);
    header[7] = (unsigned char)((fileSize >> 24) & 0xFF);
    
    // "WAVE"
    header[8] = 'W'; header[9] = 'A'; header[10] = 'V'; header[11] = 'E';
    
    // "fmt "
    header[12] = 'f'; header[13] = 'm'; header[14] = 't'; header[15] = ' ';
    header[16] = 16; header[17] = 0; header[18] = 0; header[19] = 0; // Subchunk1Size (16 for PCM)
    header[20] = 1;  header[21] = 0; // AudioFormat (1 = PCM)
    header[22] = 1;  header[23] = 0; // NumChannels (1 = Mono)
    
    header[24] = (unsigned char)(sampleRate & 0xFF);
    header[25] = (unsigned char)((sampleRate >> 8) & 0xFF);
    header[26] = (unsigned char)((sampleRate >> 16) & 0xFF);
    header[27] = (unsigned char)((sampleRate >> 24) & 0xFF);
    
    int byteRate = sampleRate * 2;
    header[28] = (unsigned char)(byteRate & 0xFF);
    header[29] = (unsigned char)((byteRate >> 8) & 0xFF);
    header[30] = (unsigned char)((byteRate >> 16) & 0xFF);
    header[31] = (unsigned char)((byteRate >> 24) & 0xFF);
    
    header[32] = 2; header[33] = 0;  // BlockAlign (2 bytes)
    header[34] = 16; header[35] = 0; // BitsPerSample (16 bits)
    
    // "data"
    header[36] = 'd'; header[37] = 'a'; header[38] = 't'; header[39] = 'a';
    header[40] = (unsigned char)(dataSize & 0xFF);
    header[41] = (unsigned char)((dataSize >> 8) & 0xFF);
    header[42] = (unsigned char)((dataSize >> 16) & 0xFF);
    header[43] = (unsigned char)((dataSize >> 24) & 0xFF);
}

void initProceduralAudio() {
    int sampleRate = 22050;
    
    // 1. Ambient Night Loop (6.0 seconds continuous seamless cycle)
    float ambDuration = 6.0f;
    int ambSamples = (int)(sampleRate * ambDuration);
    g_ambientWav.resize(44 + ambSamples * 2);
    createWavHeader(g_ambientWav.data(), sampleRate, ambSamples);
    
    short* ambData = (short*)(g_ambientWav.data() + 44);
    for (int i = 0; i < ambSamples; ++i) {
        float t = (float)i / sampleRate;
        
        // Low howling night wind (sub-harmonics + noise breath)
        float wind1 = std::sin(2.0f * (float)M_PI * 48.0f * t + 0.4f * std::sin(2.0f * (float)M_PI * 0.35f * t));
        float wind2 = std::sin(2.0f * (float)M_PI * 72.0f * t);
        float noise = ((float)(rand() % 2000) / 1000.0f - 1.0f) * 0.28f;
        float windVol = 0.20f + 0.10f * std::sin(2.0f * (float)M_PI * (t / ambDuration));
        float wind = (wind1 * 0.5f + wind2 * 0.3f + noise * 0.2f) * windVol;
        
        // Faint distant night crickets (intermittent chirp bursts around 4200 Hz)
        float chirpCadence = std::fmod(t, 0.48f);
        float cricket = 0.0f;
        if (chirpCadence < 0.055f) {
            cricket = std::sin(2.0f * (float)M_PI * 4200.0f * t) * 0.045f;
        }
        
        float sampleVal = (wind + cricket) * 13000.0f;
        if (sampleVal > 32767.0f) sampleVal = 32767.0f;
        if (sampleVal < -32768.0f) sampleVal = -32768.0f;
        ambData[i] = (short)sampleVal;
    }
    
    // 2. Thunder Sound Effect (3.0 seconds)
    float thDuration = 3.0f;
    int thSamples = (int)(sampleRate * thDuration);
    g_thunderWav.resize(44 + thSamples * 2);
    createWavHeader(g_thunderWav.data(), sampleRate, thSamples);
    
    short* thData = (short*)(g_thunderWav.data() + 44);
    for (int i = 0; i < thSamples; ++i) {
        float t = (float)i / sampleRate;
        float sampleVal = 0.0f;
        
        if (t < 0.03f) {
            sampleVal = 0.0f;
        } else if (t < 0.22f) {
            float tBoom = t - 0.03f;
            float sub = std::sin(2.0f * (float)M_PI * 45.0f * tBoom) * std::exp(-tBoom * 16.0f);
            float crack = ((float)(rand() % 2000) / 1000.0f - 1.0f) * std::exp(-tBoom * 20.0f);
            sampleVal = (sub * 0.70f + crack * 0.55f);
        } else {
            float tRoll = t - 0.22f;
            float rollEnv = std::exp(-tRoll * 1.15f);
            float r1 = std::sin(2.0f * (float)M_PI * 36.0f * tRoll);
            float r2 = std::sin(2.0f * (float)M_PI * 52.0f * tRoll + std::sin(tRoll * 5.0f));
            float rNoise = ((float)(rand() % 2000) / 1000.0f - 1.0f) * 0.22f;
            sampleVal = (r1 * 0.5f + r2 * 0.35f + rNoise * 0.15f) * rollEnv * 0.85f;
        }
        
        float finalSample = sampleVal * 23000.0f;
        if (finalSample > 32767.0f) finalSample = 32767.0f;
        if (finalSample < -32768.0f) finalSample = -32768.0f;
        thData[i] = (short)finalSample;
    }
}

void playAmbientAudio() {
    if (!g_audioEnabled || g_ambientWav.empty()) return;
    PlaySoundA((LPCSTR)g_ambientWav.data(), NULL, SND_MEMORY | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
}

void playThunderAudio() {
    if (!g_audioEnabled || g_thunderWav.empty()) return;
    PlaySoundA((LPCSTR)g_thunderWav.data(), NULL, SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
    g_thunderAudioTimer = 3.1f; // Resume ambient loop when thunder finishes
}

void toggleAudio() {
    g_audioEnabled = !g_audioEnabled;
    if (g_audioEnabled) {
        playAmbientAudio();
        std::cout << "[AUDIO] Ambient Night Audio: ON" << std::endl;
    } else {
        PlaySoundA(NULL, NULL, 0);
        std::cout << "[AUDIO] Ambient Night Audio: MUTED" << std::endl;
    }
}
#endif

struct Star {
    float x, y, z;
    float size;
    float brightness;
};
std::vector<Star> g_stars;

// ============================================================================
// TEXTURE MANAGEMENT & PROCEDURAL FALLBACK SYSTEM
// ============================================================================
enum TextureID {
    TEX_NONE = 0,
    TEX_WALL,
    TEX_ROOF,
    TEX_GROUND,
    TEX_STONE,
    TEX_BARK,
    TEX_RUST,
    TEX_MOON,
    TEX_COUNT
};

GLuint g_textureHandles[TEX_COUNT] = { 0 };

// Bind active texture or disable if TEX_NONE / textures disabled
void bindTexture(TextureID id) {
    if (!g_texturesEnabled || id == TEX_NONE || g_textureHandles[id] == 0) {
        glDisable(GL_TEXTURE_2D);
    } else {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_textureHandles[id]);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
}

// Generate high-resolution procedural textures in memory as robust fallbacks
void generateProceduralTexture(TextureID id, int width, int height, std::vector<unsigned char>& data) {
    data.resize(width * height * 4);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 4;
            unsigned char r = 200, g = 200, b = 200, a = 255;

            switch (id) {
                case TEX_WALL: {
                    // Weathered dark charcoal-brown horizontal wood planks with grain & seams (Reference Image)
                    int plankHeight = height / 8;
                    int plankIdx = y / plankHeight;
                    int lineInPlank = y % plankHeight;
                    float grain = 0.88f + 0.12f * std::sin(x * 0.25f + std::sin(y * 0.05f) * 4.0f);
                    float woodR = 46 + (plankIdx * 3) % 8;
                    float woodG = 42 + (plankIdx * 2) % 6;
                    float woodB = 38 + (plankIdx * 2) % 6;

                    if (lineInPlank < 2 || lineInPlank > plankHeight - 3) {
                        // Dark plank seam/crack
                        woodR *= 0.40f; woodG *= 0.40f; woodB *= 0.40f;
                    }
                    r = (unsigned char)(woodR * grain);
                    g = (unsigned char)(woodG * grain);
                    b = (unsigned char)(woodB * grain);
                    break;
                }
                case TEX_ROOF: {
                    // Deep slate near-black shingles pattern (Reference Image)
                    int shingleH = height / 12;
                    int shingleW = width / 8;
                    int row = y / shingleH;
                    int col = (x + (row % 2) * (shingleW / 2)) / shingleW;
                    int inX = (x + (row % 2) * (shingleW / 2)) % shingleW;
                    int inY = y % shingleH;

                    float tileShade = 0.85f + 0.15f * std::sin((float)(row * 17 + col * 23));
                    float base = 30.0f * tileShade;
                    if (inX < 2 || inY < 2) base *= 0.45f; // Shingle borders

                    r = (unsigned char)(base * 0.90f);
                    g = (unsigned char)(base * 0.95f);
                    b = (unsigned char)(base * 1.05f);
                    break;
                }
                case TEX_GROUND: {
                    // Dark damp midnight soil / earth with subtle dark noise (zero bright green)
                    float n1 = std::sin(x * 0.15f) * std::cos(y * 0.15f);
                    float n2 = std::sin(x * 0.4f + y * 0.3f);
                    float noise = 0.8f + 0.2f * (n1 + n2 * 0.5f);
                    r = (unsigned char)(22.0f * noise);
                    g = (unsigned char)(26.0f * noise);
                    b = (unsigned char)(22.0f * noise);
                    break;
                }
                case TEX_STONE: {
                    // Weathered cobblestone / rock flagstones
                    int cellW = width / 6;
                    int cellH = height / 6;
                    int cx = x % cellW;
                    int cy = y % cellH;
                    float distCenter = std::sqrt((float)((cx - cellW/2)*(cx - cellW/2) + (cy - cellH/2)*(cy - cellH/2)));
                    float stoneBase = 120.0f + 25.0f * std::sin(x * 0.1f + y * 0.1f);
                    if (cx < 3 || cy < 3 || distCenter > cellW * 0.48f) {
                        stoneBase *= 0.45f; // Mortar groove
                    }
                    r = (unsigned char)(stoneBase * 0.92f);
                    g = (unsigned char)(stoneBase * 0.95f);
                    b = (unsigned char)(stoneBase * 1.05f);
                    break;
                }
                case TEX_BARK: {
                    // Deep vertical tree bark ridges and grooves
                    float ridge = std::sin(x * 0.35f + std::sin(y * 0.08f) * 6.0f);
                    float base = 75.0f + 35.0f * ridge;
                    r = (unsigned char)(base * 0.95f);
                    g = (unsigned char)(base * 0.80f);
                    b = (unsigned char)(base * 0.65f);
                    break;
                }
                case TEX_RUST: {
                    // Peeling vintage paint, deep iron oxidation rust, and lower mud splatter
                    float p1 = std::sin(x * 0.12f + y * 0.08f);
                    float p2 = std::cos(x * 0.25f - y * 0.20f);
                    float p3 = std::sin(x * 0.45f + std::sin(y * 0.35f) * 2.0f);
                    float rustPatch = p1 * p2 + p3 * 0.22f;

                    // Vertical mud / dirt gradient near lower vehicle base (y < height * 0.38)
                    float dirtFactor = 0.0f;
                    if (y < height * 0.38f) {
                        float v = 1.0f - (float)y / (height * 0.38f);
                        dirtFactor = v * (0.75f + 0.25f * std::sin(x * 0.32f));
                    }

                    if (dirtFactor > 0.38f) {
                        // Dark damp earth grime & mud splatters near wheels and rocker panels
                        r = (unsigned char)(42.0f + 14.0f * p1);
                        g = (unsigned char)(32.0f + 10.0f * p2);
                        b = (unsigned char)(22.0f +  8.0f * p1);
                    } else if (rustPatch > 0.08f) {
                        // Oxidized orange-brown iron rust pitting and blistered metal
                        float t = (rustPatch - 0.08f) * 2.2f;
                        if (t > 1.0f) t = 1.0f;
                        r = (unsigned char)(145.0f + 35.0f * t);
                        g = (unsigned char)( 62.0f + 20.0f * t);
                        b = (unsigned char)( 28.0f + 12.0f * t);
                    } else {
                        // Weathered peeling paint (classic 1960s faded slate/teal with chipped borders)
                        float chip = (rustPatch > 0.02f) ? 0.70f : 1.0f;
                        r = (unsigned char)((72.0f + 20.0f * p1) * chip);
                        g = (unsigned char)((86.0f + 22.0f * p2) * chip);
                        b = (unsigned char)((96.0f + 18.0f * p1) * chip);
                    }
                    break;
                }
                case TEX_MOON: {
                    // Luminous cratered lunar surface
                    float crater1 = std::sin(x * 0.08f) * std::cos(y * 0.08f);
                    float crater2 = std::sin(x * 0.22f + y * 0.18f);
                    float shade = 0.82f + 0.18f * (crater1 * 0.6f + crater2 * 0.4f);
                    r = (unsigned char)(235.0f * shade);
                    g = (unsigned char)(242.0f * shade);
                    b = (unsigned char)(255.0f * shade);
                    break;
                }
                default:
                    break;
            }

            data[idx + 0] = r;
            data[idx + 1] = g;
            data[idx + 2] = b;
            data[idx + 3] = a;
        }
    }
}

// Load texture from disk or generate fallback
void loadSceneTexture(TextureID id, const std::vector<std::string>& fileCandidates) {
    int width = 0, height = 0, channels = 0;
    unsigned char* imgData = nullptr;
    std::string loadedPath = "";

    stbi_set_flip_vertically_on_load(true);

    for (const auto& path : fileCandidates) {
        imgData = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (imgData) {
            loadedPath = path;
            break;
        }
    }

    glGenTextures(1, &g_textureHandles[id]);
    glBindTexture(GL_TEXTURE_2D, g_textureHandles[id]);

    // Set GL_REPEAT wrapping so textures tile seamlessly across geometry
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (imgData) {
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
        stbi_image_free(imgData);
        std::cout << "[TEXTURE LOADED] " << loadedPath << " -> ID " << id << " (" << width << "x" << height << ")" << std::endl;
    } else {
        // Synthesize fallback procedural texture
        width = 256; height = 256;
        std::vector<unsigned char> procData;
        generateProceduralTexture(id, width, height, procData);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, procData.data());
        std::cout << "[TEXTURE SYNTHESIZED] Procedural pattern generated for ID " << id << " (256x256)" << std::endl;
    }
}

void initAllTextures() {
    loadSceneTexture(TEX_WALL,   { "textures/wall.bmp",   "textures/wall.png",   "textures/wall.jpg",   "textures/wood.bmp",   "textures/wood.png",   "textures/wood.jpg" });
    loadSceneTexture(TEX_ROOF,   { "textures/roof.bmp",   "textures/roof.png",   "textures/roof.jpg",   "textures/shingle.bmp","textures/shingle.png","textures/shingle.jpg" });
    loadSceneTexture(TEX_GROUND, { "textures/ground.bmp", "textures/ground.png", "textures/ground.jpg", "textures/mud.bmp",   "textures/mud.png",   "textures/mud.jpg" });
    loadSceneTexture(TEX_STONE,  { "textures/stone.bmp",  "textures/stone.png",  "textures/stone.jpg",  "textures/cobble.bmp", "textures/cobble.png", "textures/cobble.jpg" });
    loadSceneTexture(TEX_BARK,   { "textures/bark.bmp",   "textures/bark.jpg",   "textures/tree.bmp",   "textures/tree.png",   "textures/tree.jpg" });
    loadSceneTexture(TEX_RUST,   { "textures/rust.bmp",   "textures/rust.jpg",   "textures/metal.bmp",  "textures/metal.png",  "textures/metal.jpg" });
    loadSceneTexture(TEX_MOON,   { "textures/moon.bmp",   "textures/moon.png",   "textures/moon.jpg" });
}

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

const Material MAT_DARK_WOOD = {
    { 0.020f, 0.018f, 0.016f, 1.0f },
    { 0.050f, 0.045f, 0.040f, 1.0f },
    { 0.015f, 0.015f, 0.012f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    6.0f
};

const Material MAT_WEATHERED_WALL = {
    { 0.06f, 0.06f, 0.07f, 1.0f },
    { 0.28f, 0.26f, 0.24f, 1.0f },
    { 0.08f, 0.08f, 0.09f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    10.0f
};

const Material MAT_ROOF_SHINGLE = {
    { 0.06f, 0.07f, 0.09f, 1.0f },
    { 0.26f, 0.28f, 0.35f, 1.0f },
    { 0.16f, 0.18f, 0.22f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    16.0f
};

const Material MAT_STONE = {
    { 0.040f, 0.045f, 0.055f, 1.0f },
    { 0.110f, 0.120f, 0.135f, 1.0f },
    { 0.060f, 0.070f, 0.080f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    20.0f
};

const Material MAT_WET_GROUND = {
    { 0.025f, 0.028f, 0.032f, 1.0f },
    { 0.065f, 0.072f, 0.080f, 1.0f },
    { 0.050f, 0.055f, 0.065f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    25.0f
};

const Material MAT_PUDDLE_WATER = {
    { 0.03f, 0.04f, 0.07f, 0.92f },
    { 0.08f, 0.10f, 0.15f, 0.92f },
    { 0.95f, 0.98f, 1.00f, 0.92f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    115.0f
};

const Material MAT_DEAD_GRASS = {
    { 0.030f, 0.035f, 0.028f, 1.0f },
    { 0.075f, 0.085f, 0.065f, 1.0f },
    { 0.020f, 0.025f, 0.018f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    8.0f
};

const Material MAT_FALLEN_LEAF = {
    { 0.06f, 0.04f, 0.02f, 1.0f },
    { 0.22f, 0.13f, 0.07f, 1.0f },
    { 0.02f, 0.02f, 0.01f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    6.0f
};

const Material MAT_CLAY_BRICK = {
    { 0.26f, 0.14f, 0.10f, 1.0f },
    { 0.64f, 0.34f, 0.22f, 1.0f },
    { 0.10f, 0.08f, 0.05f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    12.0f
};

const Material MAT_MOSS_STONE = {
    { 0.16f, 0.20f, 0.14f, 1.0f },
    { 0.40f, 0.50f, 0.34f, 1.0f },
    { 0.12f, 0.14f, 0.10f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    14.0f
};

const Material MAT_RUSTY_METAL = {
    { 0.24f, 0.16f, 0.12f, 1.0f },
    { 0.58f, 0.40f, 0.28f, 1.0f },
    { 0.88f, 0.82f, 0.72f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    78.0f
};

const Material MAT_BLACK_IRON = {
    { 0.08f, 0.08f, 0.08f, 1.0f },
    { 0.18f, 0.18f, 0.18f, 1.0f },
    { 0.45f, 0.45f, 0.45f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    45.0f
};

const Material MAT_CAR_GLASS = {
    { 0.06f, 0.08f, 0.12f, 0.88f },
    { 0.14f, 0.18f, 0.26f, 0.88f },
    { 0.98f, 0.98f, 1.00f, 0.88f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    128.0f
};

const Material MAT_RUBBER_TYRE = {
    { 0.10f, 0.10f, 0.10f, 1.0f },
    { 0.22f, 0.22f, 0.22f, 1.0f },
    { 0.08f, 0.08f, 0.08f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    10.0f
};

const Material MAT_CHROME_TRIM = {
    { 0.34f, 0.36f, 0.40f, 1.0f },
    { 0.82f, 0.85f, 0.90f, 1.0f },
    { 1.00f, 1.00f, 1.00f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    128.0f
};

const Material MAT_CAR_INTERIOR = {
    { 0.14f, 0.12f, 0.10f, 1.0f },
    { 0.36f, 0.30f, 0.25f, 1.0f },
    { 0.16f, 0.14f, 0.12f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    16.0f
};

const Material MAT_SEAT_FOAM = {
    { 0.28f, 0.24f, 0.12f, 1.0f },
    { 0.70f, 0.62f, 0.32f, 1.0f },
    { 0.06f, 0.06f, 0.03f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    8.0f
};

const Material MAT_PUMPKIN_SKIN = {
    { 0.50f, 0.22f, 0.06f, 1.0f },
    { 0.98f, 0.46f, 0.12f, 1.0f },
    { 0.40f, 0.22f, 0.08f, 1.0f },
    { 0.18f, 0.08f, 0.02f, 1.0f },
    28.0f
};

const Material MAT_PUMPKIN_GLOW = {
    { 1.00f, 0.72f, 0.22f, 1.0f },
    { 1.00f, 0.80f, 0.28f, 1.0f },
    { 1.00f, 0.95f, 0.60f, 1.0f },
    { 1.00f, 0.75f, 0.22f, 1.0f },
    60.0f
};

const Material MAT_WINDOW_GLOW = {
    { 0.45f, 0.28f, 0.10f, 1.0f },
    { 0.85f, 0.55f, 0.20f, 1.0f },
    { 0.25f, 0.15f, 0.05f, 1.0f },
    { 1.00f, 0.72f, 0.26f, 1.0f },
    40.0f
};

const Material MAT_BULB_EMISSIVE = {
    { 1.00f, 0.88f, 0.35f, 1.0f },
    { 1.00f, 0.95f, 0.55f, 1.0f },
    { 1.00f, 1.00f, 0.90f, 1.0f },
    { 1.00f, 0.90f, 0.40f, 1.0f },
    75.0f
};

const Material MAT_MOON = {
    { 0.85f, 0.88f, 0.94f, 1.0f },
    { 0.95f, 0.97f, 1.00f, 1.0f },
    { 0.60f, 0.65f, 0.75f, 1.0f },
    { 0.88f, 0.90f, 0.95f, 1.0f },
    35.0f
};

const Material MAT_GHOST_SPECTRAL = {
    { 0.45f, 0.75f, 0.95f, 0.60f },
    { 0.75f, 0.90f, 1.00f, 0.60f },
    { 0.95f, 1.00f, 1.00f, 0.60f },
    { 0.55f, 0.88f, 1.00f, 0.60f },
    50.0f
};

const Material MAT_BARK = {
    { 0.08f, 0.08f, 0.10f, 1.0f },
    { 0.18f, 0.18f, 0.22f, 1.0f },
    { 0.04f, 0.04f, 0.06f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    4.0f
};

const Material MAT_FOLIAGE_DARK = {
    { 0.10f, 0.16f, 0.10f, 1.0f },
    { 0.18f, 0.32f, 0.18f, 1.0f },
    { 0.03f, 0.05f, 0.03f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    12.0f
};

const Material MAT_FOLIAGE_LUSH = {
    { 0.12f, 0.20f, 0.12f, 1.0f },
    { 0.22f, 0.44f, 0.24f, 1.0f },
    { 0.04f, 0.06f, 0.04f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    14.0f
};

const Material MAT_FOLIAGE_AUTUMN = {
    { 0.14f, 0.10f, 0.05f, 1.0f },
    { 0.38f, 0.26f, 0.10f, 1.0f },
    { 0.04f, 0.04f, 0.03f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    10.0f
};

const Material MAT_PINE_NEEDLES = {
    { 0.09f, 0.16f, 0.10f, 1.0f },
    { 0.16f, 0.34f, 0.18f, 1.0f },
    { 0.02f, 0.04f, 0.02f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    8.0f
};

// ============================================================================
// INITIALIZATION FUNCTIONS
// ============================================================================
void initStars() {
    g_stars.clear();
    srand(1337);
    for (int i = 0; i < 350; ++i) {
        float theta = ((float)rand() / RAND_MAX) * 2.0f * (float)M_PI;
        float phi   = ((float)rand() / RAND_MAX) * 0.45f * (float)M_PI;
        float r     = 180.0f;
        Star s;
        s.x = r * std::cos(phi) * std::cos(theta);
        s.y = r * std::sin(phi) + 12.0f;
        s.z = r * std::cos(phi) * std::sin(theta);
        s.size = 1.0f + ((float)rand() / RAND_MAX) * 2.5f;
        s.brightness = 0.4f + ((float)rand() / RAND_MAX) * 0.6f;
        g_stars.push_back(s);
    }
}

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.035f, 0.045f, 0.075f, 1.0f); // Deep midnight blue-black night sky

    // Deep, dark horror midnight global ambient (scene shrouded in darkness)
    float globalAmbient[] = { 0.05f, 0.06f, 0.08f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // Atmospheric Fog (Dark midnight mist)
    float fogColor[4] = { 0.04f, 0.05f, 0.08f, 1.0f };
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.007f);
    glHint(GL_FOG_HINT, GL_NICEST);
    if (g_fogEnabled) glEnable(GL_FOG); else glDisable(GL_FOG);

    // Light 0: Point Light (Warm Golden Porch Bulb with distance attenuation)
    float pDiffuse[]   = { 1.0f, 0.85f, 0.42f, 1.0f };
    float pSpecular[]  = { 1.0f, 0.90f, 0.50f, 1.0f };
    float pAmbient[]   = { 0.03f, 0.02f, 0.01f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  pDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, pSpecular);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  pAmbient);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.10f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.02f);

    // Light 1: Directional Moonlight (Cool silvery-blue base illumination)
    float mDiffuse[]   = { 0.35f, 0.42f, 0.58f, 1.0f };
    float mSpecular[]  = { 0.45f, 0.55f, 0.75f, 1.0f };
    float mAmbient[]   = { 0.04f, 0.05f, 0.07f, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  mDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, mSpecular);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  mAmbient);

    // Light 2: Spot Light (First-Person Flashlight with soft organic falloff)
    float fDiffuse[]   = { 0.95f, 0.96f, 0.92f, 1.0f };
    float fSpecular[]  = { 1.00f, 1.00f, 1.00f, 1.0f };
    float fAmbient[]   = { 0.00f, 0.00f, 0.00f, 1.0f };
    glLightfv(GL_LIGHT2, GL_DIFFUSE,  fDiffuse);
    glLightfv(GL_LIGHT2, GL_SPECULAR, fSpecular);
    glLightfv(GL_LIGHT2, GL_AMBIENT,  fAmbient);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF,   22.0f); // Wide natural cone
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 16.0f); // Soft Gaussian-like radial edge falloff
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION,    0.035f);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.005f);

    // Light 3: Area Light Emulation (Warm Amber Window Interior Glow)
    float wDiffuse[]   = { 0.85f, 0.55f, 0.15f, 1.0f };
    float wSpecular[]  = { 0.55f, 0.35f, 0.10f, 1.0f };
    float wAmbient[]   = { 0.08f, 0.05f, 0.01f, 1.0f };
    glLightfv(GL_LIGHT3, GL_DIFFUSE,  wDiffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, wSpecular);
    glLightfv(GL_LIGHT3, GL_AMBIENT,  wAmbient);
    glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION,    0.08f);
    glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.015f);

    glEnable(GL_LIGHTING);
    initAllTextures();
    initStars();
    initFallingLeaves();
#ifdef _WIN32
    initProceduralAudio();
#endif
}

// ============================================================================
// GEOMETRIC PRIMITIVE RENDERING WITH UV TEXTURE COORDINATES
// ============================================================================

void drawBox(float width, float height, float depth, float tileU = 1.0f, float tileV = 1.0f) {
    float x = width * 0.5f;
    float y = height * 0.5f;
    float z = depth * 0.5f;

    glBegin(GL_QUADS);
    // Front (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-x, -y,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( x, -y,  z);
    glTexCoord2f(tileU, tileV); glVertex3f( x,  y,  z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-x,  y,  z);

    // Back (-Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( x, -y, -z);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-x, -y, -z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-x,  y, -z);
    glTexCoord2f(tileU, tileV); glVertex3f( x,  y, -z);

    // Top (+Y)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-x,  y,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( x,  y,  z);
    glTexCoord2f(tileU, tileV); glVertex3f( x,  y, -z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-x,  y, -z);

    // Bottom (-Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-x, -y, -z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( x, -y, -z);
    glTexCoord2f(tileU, tileV); glVertex3f( x, -y,  z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-x, -y,  z);

    // Right (+X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( x, -y,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( x, -y, -z);
    glTexCoord2f(tileU, tileV); glVertex3f( x,  y, -z);
    glTexCoord2f(0.0f, tileV);  glVertex3f( x,  y,  z);

    // Left (-X)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-x, -y, -z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-x, -y,  z);
    glTexCoord2f(tileU, tileV); glVertex3f(-x,  y,  z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-x,  y, -z);
    glEnd();
}

void drawBeveledBox(float width, float height, float depth, float bevel = 0.04f, float tileU = 1.0f, float tileV = 1.0f) {
    float x = width * 0.5f;
    float y = height * 0.5f;
    float z = depth * 0.5f;
    float b = std::min(bevel, std::min(x * 0.45f, std::min(y * 0.45f, z * 0.45f)));
    if (b <= 0.001f) {
        drawBox(width, height, depth, tileU, tileV);
        return;
    }

    float bx = x - b;
    float by = y - b;
    float bz = z - b;
    float invSqrt2 = 0.70710678f;
    float invSqrt3 = 0.57735027f;

    glBegin(GL_QUADS);
    // 1. Primary 6 Main Faces
    // Front (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx, -by,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx, -by,  z);
    glTexCoord2f(tileU, tileV); glVertex3f( bx,  by,  z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-bx,  by,  z);

    // Back (-Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx, -by, -z);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx, -by, -z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-bx,  by, -z);
    glTexCoord2f(tileU, tileV); glVertex3f( bx,  by, -z);

    // Top (+Y)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx,  y,  bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx,  y,  bz);
    glTexCoord2f(tileU, tileV); glVertex3f( bx,  y, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-bx,  y, -bz);

    // Bottom (-Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx, -y, -bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx, -y, -bz);
    glTexCoord2f(tileU, tileV); glVertex3f( bx, -y,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-bx, -y,  bz);

    // Right (+X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( x, -by,  bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( x, -by, -bz);
    glTexCoord2f(tileU, tileV); glVertex3f( x,  by, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( x,  by,  bz);

    // Left (-X)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-x, -by, -bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-x, -by,  bz);
    glTexCoord2f(tileU, tileV); glVertex3f(-x,  by,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-x,  by, -bz);

    // 2. 12 Beveled Edge Quads
    // Top-Front (+Y, +Z)
    glNormal3f(0.0f, invSqrt2, invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx,  by,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx,  by,  z);
    glTexCoord2f(tileU, tileV); glVertex3f( bx,   y,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-bx,   y,  bz);

    // Top-Back (+Y, -Z)
    glNormal3f(0.0f, invSqrt2, -invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( bx,  by, -z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-bx,  by, -z);
    glTexCoord2f(tileU, tileV); glVertex3f(-bx,   y, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( bx,   y, -bz);

    // Top-Right (+Y, +X)
    glNormal3f(invSqrt2, invSqrt2, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( bx,   y,  bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx,   y, -bz);
    glTexCoord2f(tileU, tileV); glVertex3f(  x,  by, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(  x,  by,  bz);

    // Top-Left (+Y, -X)
    glNormal3f(-invSqrt2, invSqrt2, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx,   y, -bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-bx,   y,  bz);
    glTexCoord2f(tileU, tileV); glVertex3f( -x,  by,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( -x,  by, -bz);

    // Bottom-Front (-Y, +Z)
    glNormal3f(0.0f, -invSqrt2, invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( bx, -by,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-bx, -by,  z);
    glTexCoord2f(tileU, tileV); glVertex3f(-bx,  -y,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( bx,  -y,  bz);

    // Bottom-Back (-Y, -Z)
    glNormal3f(0.0f, -invSqrt2, -invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx, -by, -z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx, -by, -z);
    glTexCoord2f(tileU, tileV); glVertex3f( bx,  -y, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-bx,  -y, -bz);

    // Bottom-Right (-Y, +X)
    glNormal3f(invSqrt2, -invSqrt2, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( bx,  -y, -bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx,  -y,  bz);
    glTexCoord2f(tileU, tileV); glVertex3f(  x, -by,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(  x, -by, -bz);

    // Bottom-Left (-Y, -X)
    glNormal3f(-invSqrt2, -invSqrt2, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx,  -y,  bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-bx,  -y, -bz);
    glTexCoord2f(tileU, tileV); glVertex3f( -x, -by, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( -x, -by,  bz);

    // Front-Right (+Z, +X)
    glNormal3f(invSqrt2, 0.0f, invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( bx, -by,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx,  by,  z);
    glTexCoord2f(tileU, tileV); glVertex3f(  x,  by,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(  x, -by,  bz);

    // Front-Left (+Z, -X)
    glNormal3f(-invSqrt2, 0.0f, invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx,  by,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-bx, -by,  z);
    glTexCoord2f(tileU, tileV); glVertex3f( -x, -by,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( -x,  by,  bz);

    // Back-Right (-Z, +X)
    glNormal3f(invSqrt2, 0.0f, -invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( bx,  by, -z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx, -by, -z);
    glTexCoord2f(tileU, tileV); glVertex3f(  x, -by, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(  x,  by, -bz);

    // Back-Left (-Z, -X)
    glNormal3f(-invSqrt2, 0.0f, -invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx, -by, -z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-bx,  by, -z);
    glTexCoord2f(tileU, tileV); glVertex3f( -x,  by, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( -x, -by, -bz);
    glEnd();

    // 3. 8 Corner Triangles
    glBegin(GL_TRIANGLES);
    // Top-Front-Right (+X, +Y, +Z)
    glNormal3f(invSqrt3, invSqrt3, invSqrt3);
    glVertex3f( bx,  by,   z); glVertex3f(  x,  by,  bz); glVertex3f( bx,   y,  bz);

    // Top-Front-Left (-X, +Y, +Z)
    glNormal3f(-invSqrt3, invSqrt3, invSqrt3);
    glVertex3f(-bx,  by,   z); glVertex3f(-bx,   y,  bz); glVertex3f( -x,  by,  bz);

    // Top-Back-Right (+X, +Y, -Z)
    glNormal3f(invSqrt3, invSqrt3, -invSqrt3);
    glVertex3f( bx,  by,  -z); glVertex3f( bx,   y, -bz); glVertex3f(  x,  by, -bz);

    // Top-Back-Left (-X, +Y, -Z)
    glNormal3f(-invSqrt3, invSqrt3, -invSqrt3);
    glVertex3f(-bx,  by,  -z); glVertex3f( -x,  by, -bz); glVertex3f(-bx,   y, -bz);

    // Bottom-Front-Right (+X, -Y, +Z)
    glNormal3f(invSqrt3, -invSqrt3, invSqrt3);
    glVertex3f( bx, -by,   z); glVertex3f( bx,  -y,  bz); glVertex3f(  x, -by,  bz);

    // Bottom-Front-Left (-X, -Y, +Z)
    glNormal3f(-invSqrt3, -invSqrt3, invSqrt3);
    glVertex3f(-bx, -by,   z); glVertex3f( -x, -by,  bz); glVertex3f(-bx,  -y,  bz);

    // Bottom-Back-Right (+X, -Y, -Z)
    glNormal3f(invSqrt3, -invSqrt3, -invSqrt3);
    glVertex3f( bx, -by,  -z); glVertex3f(  x, -by, -bz); glVertex3f( bx,  -y, -bz);

    // Bottom-Back-Left (-X, -Y, -Z)
    glNormal3f(-invSqrt3, -invSqrt3, -invSqrt3);
    glVertex3f(-bx, -by,  -z); glVertex3f(-bx,  -y, -bz); glVertex3f( -x, -by, -bz);
    glEnd();
}

void drawCylinder(float baseRadius, float topRadius, float height, int slices, float tileU = 1.0f, float tileV = 1.0f) {
    float angleStep = 2.0f * (float)M_PI / (float)slices;
    float slope = (baseRadius - topRadius) / height;

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
        float a = i * angleStep;
        float u = (float)i / slices * tileU;
        float cosA = std::cos(a);
        float sinA = std::sin(a);

        float nx = cosA;
        float ny = slope;
        float nz = sinA;
        float len = std::sqrt(nx*nx + ny*ny + nz*nz);
        glNormal3f(nx/len, ny/len, nz/len);

        glTexCoord2f(u, 0.0f);
        glVertex3f(baseRadius * cosA, 0.0f, baseRadius * sinA);

        glTexCoord2f(u, tileV);
        glVertex3f(topRadius * cosA, height, topRadius * sinA);
    }
    glEnd();

    // Bottom Cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = slices; i >= 0; --i) {
        float a = i * angleStep;
        glTexCoord2f(0.5f + 0.5f * std::cos(a), 0.5f + 0.5f * std::sin(a));
        glVertex3f(baseRadius * std::cos(a), 0.0f, baseRadius * std::sin(a));
    }
    glEnd();

    // Top Cap
    if (topRadius > 0.001f) {
        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.5f, 0.5f);
        glVertex3f(0.0f, height, 0.0f);
        for (int i = 0; i <= slices; ++i) {
            float a = i * angleStep;
            glTexCoord2f(0.5f + 0.5f * std::cos(a), 0.5f + 0.5f * std::sin(a));
            glVertex3f(topRadius * std::cos(a), height, topRadius * std::sin(a));
        }
        glEnd();
    }
}

void drawSphere(float radius, int slices, int stacks, float tileU = 1.0f, float tileV = 1.0f) {
    for (int i = 0; i < stacks; ++i) {
        float phi0 = (float)M_PI * (float)i / (float)stacks;
        float phi1 = (float)M_PI * (float)(i + 1) / (float)stacks;

        float y0 = radius * std::cos(phi0);
        float r0 = radius * std::sin(phi0);
        float v0 = (float)i / (float)stacks * tileV;

        float y1 = radius * std::cos(phi1);
        float r1 = radius * std::sin(phi1);
        float v1 = (float)(i + 1) / (float)stacks * tileV;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * (float)M_PI * (float)j / (float)slices;
            float u = (float)j / (float)slices * tileU;

            float cosT = std::cos(theta);
            float sinT = std::sin(theta);

            float x0 = r0 * cosT;
            float z0 = r0 * sinT;
            float x1 = r1 * cosT;
            float z1 = r1 * sinT;

            // Correct CCW quad-strip winding (bottom vertex first, then top vertex)
            glNormal3f(x1 / radius, y1 / radius, z1 / radius);
            glTexCoord2f(u, v1);
            glVertex3f(x1, y1, z1);

            glNormal3f(x0 / radius, y0 / radius, z0 / radius);
            glTexCoord2f(u, v0);
            glVertex3f(x0, y0, z0);
        }
        glEnd();
    }
}

void drawPrismRoof(float width, float height, float length, float tileU = 2.0f, float tileV = 2.0f) {
    float hw = width * 0.5f;
    float hl = length * 0.5f;

    glBegin(GL_TRIANGLES);
    // Front (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);        glVertex3f(-hw, 0.0f,  hl);
    glTexCoord2f(tileU, 0.0f);       glVertex3f( hw, 0.0f,  hl);
    glTexCoord2f(tileU * 0.5f, tileV); glVertex3f(0.0f, height, hl);

    // Back (-Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(tileU, 0.0f);       glVertex3f( hw, 0.0f, -hl);
    glTexCoord2f(0.0f, 0.0f);        glVertex3f(-hw, 0.0f, -hl);
    glTexCoord2f(tileU * 0.5f, tileV); glVertex3f(0.0f, height, -hl);
    glEnd();

    glBegin(GL_QUADS);
    // Right (+X)
    float nx = height; float ny = hw; float nlen = std::sqrt(nx*nx + ny*ny);
    glNormal3f(nx/nlen, ny/nlen, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( hw, 0.0f,  hl);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( hw, 0.0f, -hl);
    glTexCoord2f(tileU, tileV); glVertex3f(0.0f, height, -hl);
    glTexCoord2f(0.0f, tileV);  glVertex3f(0.0f, height,  hl);

    // Left (-X)
    glNormal3f(-nx/nlen, ny/nlen, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(0.0f, height,  hl);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(0.0f, height, -hl);
    glTexCoord2f(tileU, tileV); glVertex3f(-hw, 0.0f, -hl);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-hw, 0.0f,  hl);

    // Bottom (-Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-hw, 0.0f, -hl);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( hw, 0.0f, -hl);
    glTexCoord2f(tileU, tileV); glVertex3f( hw, 0.0f,  hl);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-hw, 0.0f,  hl);
    glEnd();
}

void drawSteepleSpire(float baseRadius, float height, int facets, float tileU = 2.0f, float tileV = 3.0f) {
    float angleStep = 2.0f * (float)M_PI / (float)facets;
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < facets; ++i) {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;
        float x1 = baseRadius * std::cos(a1);
        float z1 = baseRadius * std::sin(a1);
        float x2 = baseRadius * std::cos(a2);
        float z2 = baseRadius * std::sin(a2);

        float mx = (x1 + x2) * 0.5f;
        float mz = (z1 + z2) * 0.5f;
        float nx = mx;
        float ny = baseRadius / height;
        float nz = mz;
        float nl = std::sqrt(nx*nx + ny*ny + nz*nz);

        glNormal3f(nx/nl, ny/nl, nz/nl);
        glTexCoord2f(0.0f, 0.0f);        glVertex3f(x1, 0.0f, z1);
        glTexCoord2f(tileU, 0.0f);       glVertex3f(x2, 0.0f, z2);
        glTexCoord2f(tileU * 0.5f, tileV); glVertex3f(0.0f, height, 0.0f);
    }
    glEnd();
}

void drawBillboardHalo(float x, float y, float z, float radius, float r, float g, float b, float maxAlpha) {
    bindTexture(TEX_NONE);
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive luminous bloom

    glPushMatrix();
    glTranslatef(x, y, z);

    // Billboarding: Extract camera orientation to keep halo billboarded towards screen
    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    modelview[0] = 1.0f; modelview[1] = 0.0f; modelview[2] = 0.0f;
    modelview[4] = 0.0f; modelview[5] = 1.0f; modelview[6] = 0.0f;
    modelview[8] = 0.0f; modelview[9] = 0.0f; modelview[10] = 1.0f;
    glLoadMatrixf(modelview);

    int segments = 28;
    float coreRadius = radius * 0.18f;

    // 1. Intense Overbright Core Glare Disc (Hotspot)
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 1.0f, 1.0f, maxAlpha * 0.92f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * (float)M_PI * (float)i / segments;
        glColor4f(r * 1.05f, g * 1.05f, b * 1.05f, maxAlpha * 0.45f);
        glVertex3f(coreRadius * std::cos(theta), coreRadius * std::sin(theta), 0.0f);
    }
    glEnd();

    // 2. Multi-tier Soft Gaussian Bloom Rings (Smooth cubic falloff)
    int rings = 9;
    for (int ring = 0; ring < rings; ++ring) {
        float t0 = (float)ring / rings;
        float t1 = (float)(ring + 1) / rings;
        float r0 = coreRadius + (radius - coreRadius) * t0;
        float r1 = coreRadius + (radius - coreRadius) * t1;

        // Smooth cubic Gaussian decay curve
        float a0 = maxAlpha * (1.0f - t0) * (1.0f - t0) * (1.0f - t0);
        float a1 = maxAlpha * (1.0f - t1) * (1.0f - t1) * (1.0f - t1);

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

// Volumetric Flashlight Beam Cone & Drifting Dust Motes in the Fog
void drawVolumetricFlashlightBeam(float posX, float posY, float posZ, float dirX, float dirY, float dirZ) {
    if (!g_light2SpotOn) return;

    bindTexture(TEX_NONE);
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_POINT_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive soft volumetric glow

    // Forward direction vector
    float fLen = std::sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
    if (fLen < 0.0001f) { glPopAttrib(); return; }
    float fx = dirX / fLen;
    float fy = dirY / fLen;
    float fz = dirZ / fLen;

    // Right and Up orthogonal coordinate vectors
    float ux = 0.0f, uy = 1.0f, uz = 0.0f;
    if (std::abs(fy) > 0.95f) { ux = 1.0f; uy = 0.0f; uz = 0.0f; }
    float rx = fy * uz - fz * uy;
    float ry = fz * ux - fx * uz;
    float rz = fx * uy - fy * ux;
    float rLen = std::sqrt(rx * rx + ry * ry + rz * rz);
    rx /= rLen; ry /= rLen; rz /= rLen;

    ux = ry * fz - rz * fy;
    uy = rz * fx - rx * fz;
    uz = rx * fy - ry * fx;

    // Multi-segment soft translucent beam cone
    int slices = 18;
    int rings = 8;
    float maxDist = 16.0f;
    float spreadAngle = 21.5f * (float)M_PI / 180.0f; // matches spot cutoff
    float tanSpread = std::tan(spreadAngle);

    for (int ring = 0; ring < rings; ++ring) {
        float d0 = 0.35f + maxDist * ((float)ring / rings);
        float d1 = 0.35f + maxDist * ((float)(ring + 1) / rings);
        float coneR0 = d0 * tanSpread;
        float coneR1 = d1 * tanSpread;

        float t0 = (float)ring / rings;
        float t1 = (float)(ring + 1) / rings;
        float a0 = 0.032f * (1.0f - t0 * t0);
        float a1 = 0.032f * (1.0f - t1 * t1);

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= slices; ++i) {
            float theta = 2.0f * (float)M_PI * (float)i / slices;
            float ct = std::cos(theta);
            float st = std::sin(theta);

            float cx1 = posX + fx * d1 + (rx * ct + ux * st) * coneR1;
            float cy1 = posY + fy * d1 + (ry * ct + uy * st) * coneR1;
            float cz1 = posZ + fz * d1 + (rz * ct + uz * st) * coneR1;

            float cx0 = posX + fx * d0 + (rx * ct + ux * st) * coneR0;
            float cy0 = posY + fy * d0 + (ry * ct + uy * st) * coneR0;
            float cz0 = posZ + fz * d0 + (rz * ct + uz * st) * coneR0;

            glColor4f(0.85f, 0.90f, 0.98f, a1);
            glVertex3f(cx1, cy1, cz1);

            glColor4f(0.85f, 0.90f, 0.98f, a0);
            glVertex3f(cx0, cy0, cz0);
        }
        glEnd();
    }

    // Floating dust motes illuminated in the beam
    glPointSize(2.4f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 28; ++i) {
        float seed = (float)i * 137.5f;
        float speed = 0.12f + 0.08f * std::sin(seed * 0.3f);
        float drift = std::fmod(g_time * speed + seed, 12.0f) + 0.6f;

        float spread = drift * tanSpread * 0.65f;
        float angle = seed + g_time * 0.25f;
        float dr = std::sin(seed * 1.7f) * spread;

        float mx = posX + fx * drift + (rx * std::cos(angle) + ux * std::sin(angle)) * dr;
        float my = posY + fy * drift + (ry * std::cos(angle) + uy * std::sin(angle)) * dr + 0.05f * std::sin(g_time + seed);
        float mz = posZ + fz * drift + (rz * std::cos(angle) + uz * std::sin(angle)) * dr;

        float moteAlpha = (0.28f + 0.22f * std::sin(g_time * 3.5f + seed)) * (1.0f - drift / 13.0f);
        if (moteAlpha > 0.0f) {
            glColor4f(0.95f, 0.96f, 0.90f, moteAlpha);
            glVertex3f(mx, my, mz);
        }
    }
    glEnd();

    glPopAttrib();
}

// ============================================================================
// DETAILED SCENE RENDERING WITH TEXTURES
// ============================================================================

// Deterministic LCG RNG for stable, repeatable organic rocks, vegetation & trees
struct TreeRNG {
    unsigned int state;
    TreeRNG(unsigned int seed) : state(seed) {}
    float nextFloat(float minVal, float maxVal) {
        state = (state * 1664525u + 1013904223u);
        float norm = (float)(state & 0x00FFFFFF) / (float)0x00FFFFFF;
        return minVal + norm * (maxVal - minVal);
    }
};

// ============================================================================
// REALISTIC TERRAIN, REFLECTIVE PUDDLES & SCATTERED GROUND PROPS
// ============================================================================

// Terrain elevation function with smooth blending around house and path
float getTerrainHeight(float x, float z) {
    // Distance to house foundation [-8.5, 6.5] x [-7.0, 5.0] and porch
    float dx = std::max(0.0f, std::abs(x + 1.0f) - 7.5f);
    float dz = std::max(0.0f, std::abs(z - 0.5f) - 6.5f);
    float dHouse = std::sqrt(dx * dx + dz * dz);
    float houseBlend = 1.0f - std::exp(-dHouse * 0.45f);

    // Distance to cobblestone pathway
    float pathX = -0.5f + 1.2f * std::sin((24.0f - z) / 20.0f * (float)M_PI * 1.4f);
    float dPath = (z >= 3.0f && z <= 24.5f) ? std::abs(x - pathX) : 10.0f;
    float pathBlend = (dPath < 2.0f) ? (dPath / 2.0f) : 1.0f;

    float blend = houseBlend * pathBlend;

    // Gentle multi-frequency undulating terrain
    float h1 = 0.30f * std::sin(x * 0.075f + 1.2f) * std::cos(z * 0.065f + 0.5f);
    float h2 = 0.12f * std::sin(x * 0.18f - z * 0.15f) * std::cos(x * 0.12f + z * 0.20f);
    float h3 = 0.05f * std::sin(x * 0.38f + z * 0.32f);

    // Puddle depression 1: (x = -4.5, z = 13.5)
    float dp1 = std::sqrt((x + 4.5f)*(x + 4.5f)*1.0f + (z - 13.5f)*(z - 13.5f)*1.4f);
    float dip1 = (dp1 < 2.5f) ? (-0.08f * (1.0f - dp1 / 2.5f)) : 0.0f;

    // Puddle depression 2: (x = 5.0, z = 10.5)
    float dp2 = std::sqrt((x - 5.0f)*(x - 5.0f)*1.3f + (z - 10.5f)*(z - 10.5f)*1.0f);
    float dip2 = (dp2 < 2.2f) ? (-0.07f * (1.0f - dp2 / 2.2f)) : 0.0f;

    // Puddle depression 3: (x = -9.5, z = 8.2)
    float dp3 = std::sqrt((x + 9.5f)*(x + 9.5f)*1.0f + (z - 8.2f)*(z - 8.2f)*1.2f);
    float dip3 = (dp3 < 2.0f) ? (-0.06f * (1.0f - dp3 / 2.0f)) : 0.0f;

    return (h1 + h2 + h3 + dip1 + dip2 + dip3) * blend;
}

// Compute accurate surface normals using finite differences
void getTerrainNormal(float x, float z, float& nx, float& ny, float& nz) {
    const float eps = 0.15f;
    float hL = getTerrainHeight(x - eps, z);
    float hR = getTerrainHeight(x + eps, z);
    float hD = getTerrainHeight(x, z - eps);
    float hU = getTerrainHeight(x, z + eps);

    nx = (hL - hR) / (2.0f * eps);
    ny = 1.0f;
    nz = (hD - hU) / (2.0f * eps);

    float len = std::sqrt(nx * nx + ny * ny + nz * nz);
    if (len > 0.0001f) {
        nx /= len; ny /= len; nz /= len;
    }
}

// Draw a single reflective water puddle with dark wet mud rim
void drawPuddle(float cx, float cz, float radiusX, float radiusZ, float rotAngle) {
    float baseY = getTerrainHeight(cx, cz) + 0.02f;
    int segments = 24;

    glPushMatrix();
    glTranslatef(cx, baseY, cz);
    glRotatef(rotAngle, 0.0f, 1.0f, 0.0f);

    // 1. Dark damp saturated mud border fringe around puddle
    applyMaterial(MAT_WET_GROUND);
    bindTexture(TEX_GROUND);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float angle = (float)i * 2.0f * (float)M_PI / segments;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);

        float inX  = radiusX * cosA;
        float inZ  = radiusZ * sinA;
        float outX = (radiusX + 0.55f) * cosA;
        float outZ = (radiusZ + 0.55f) * sinA;

        float nx, ny, nz;
        getTerrainNormal(cx + inX, cz + inZ, nx, ny, nz);
        glNormal3f(nx, ny, nz);

        // Dark soaked mud
        glColor4f(0.22f, 0.22f, 0.25f, 1.0f);
        glTexCoord2f(inX * 0.2f, inZ * 0.2f);
        glVertex3f(inX, 0.002f, inZ);

        // Fading outward to normal terrain
        glColor4f(0.70f, 0.70f, 0.72f, 1.0f);
        glTexCoord2f(outX * 0.2f, outZ * 0.2f);
        glVertex3f(outX, -0.015f, outZ);
    }
    glEnd();

    // 2. Reflective Water Surface Disk (High specular mirror highlight)
    applyMaterial(MAT_PUDDLE_WATER);
    bindTexture(TEX_NONE);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glColor4f(0.12f, 0.16f, 0.22f, 0.92f);
    glVertex3f(0.0f, 0.008f, 0.0f);

    for (int i = 0; i <= segments; ++i) {
        float angle = (float)i * 2.0f * (float)M_PI / segments;
        float px = radiusX * std::cos(angle);
        float pz = radiusZ * std::sin(angle);
        glVertex3f(px, 0.008f, pz);
    }
    glEnd();

    // 3. Dynamic Water Ripple Drip Rings (expanding concentric rings with fading alpha)
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive luminous water glint
    glLineWidth(1.6f);

    for (int d = 0; d < 2; ++d) {
        float dripTime = g_time * 1.2f + (float)d * 1.4f + std::abs(cx) * 0.4f;
        float cycle = std::fmod(dripTime, 2.4f);
        float rProgress = cycle / 2.4f;
        float ripRadius = rProgress * (radiusX * 0.72f);
        float ripAlpha = (1.0f - rProgress) * 0.35f;

        float dripOffsetX = (d == 0) ? -0.25f : 0.35f;
        float dripOffsetZ = (d == 0) ? 0.15f : -0.20f;

        glColor4f(0.55f, 0.72f, 0.95f, ripAlpha);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 20; ++i) {
            float theta = 2.0f * (float)M_PI * (float)i / 20.0f;
            float px = dripOffsetX + ripRadius * std::cos(theta);
            float pz = dripOffsetZ + (ripRadius * (radiusZ / radiusX)) * std::sin(theta);
            glVertex3f(px, 0.012f, pz);
        }
        glEnd();
    }
    glPopAttrib();

    glPopMatrix();
}

// Draw all 3 reflective puddles
void drawPuddles() {
    // Puddle 1: Front yard pathside puddle (catches moon + porch light)
    drawPuddle(-4.5f, 13.5f, 2.3f, 1.6f, -18.0f);

    // Puddle 2: Near monster tree & rusted car (catches moonlight)
    drawPuddle( 5.0f, 10.5f, 2.0f, 1.4f,  24.0f);

    // Puddle 3: Left side near cemetery / porch corner
    drawPuddle(-9.5f,  8.2f, 1.8f, 1.3f, -10.0f);
}

// Gossamer Cobweb Geometry (Translucent additive spiderweb in corners)
void drawCobweb(float x, float y, float z, float size, float rotX, float rotY, float rotZ) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotZ, 0.0f, 0.0f, 1.0f);

    bindTexture(TEX_NONE);
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive luminous thread glint

    // 1. Semi-translucent web veil fan
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.85f, 0.90f, 0.98f, 0.16f);
    glVertex3f(0.0f, 0.0f, 0.0f); // Corner origin

    int segments = 8;
    for (int i = 0; i <= segments; ++i) {
        float theta = (float)M_PI * 0.5f * ((float)i / segments);
        float r = size * (0.85f + 0.15f * std::sin((float)i * 1.8f));
        glColor4f(0.70f, 0.78f, 0.92f, 0.02f);
        glVertex3f(r * std::cos(theta), r * std::sin(theta), 0.01f * std::sin(theta * 3.0f));
    }
    glEnd();

    // 2. Radial Spoke Strands
    glLineWidth(1.4f);
    glColor4f(0.92f, 0.95f, 1.0f, 0.32f);
    glBegin(GL_LINES);
    for (int i = 0; i <= segments; ++i) {
        float theta = (float)M_PI * 0.5f * ((float)i / segments);
        float r = size * (0.85f + 0.15f * std::sin((float)i * 1.8f));
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(r * std::cos(theta), r * std::sin(theta), 0.0f);
    }
    glEnd();

    // 3. Concentric Spiral Threads
    int rings = 4;
    for (int r = 1; r <= rings; ++r) {
        float ringFrac = (float)r / rings;
        float rRad = size * ringFrac;
        glColor4f(0.85f, 0.92f, 1.0f, 0.25f * (1.0f - ringFrac * 0.5f));
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= segments; ++i) {
            float theta = (float)M_PI * 0.5f * ((float)i / segments);
            glVertex3f(rRad * std::cos(theta), rRad * std::sin(theta), 0.0f);
        }
        glEnd();
    }

    glPopAttrib();
    glPopMatrix();
}

// Exact Cobblestone Road Centerline Formula (Stretches from foreground to house porch)
inline float getRoadCenterX(float pz) {
    if (pz > 24.0f) return 1.2f;
    if (pz < -3.5f) return -2.8f;
    float t = (22.0f - pz) / 25.5f;
    t = std::max(0.0f, std::min(1.0f, t));
    float ease = t * t * (3.0f - 2.0f * t);
    return (1.0f - ease) * 1.2f + ease * (-2.8f) - 1.1f * std::sin(t * (float)M_PI);
}

// Slender, dense dark wild grass tufts (Dark silhouette tones - NO bright green)
void drawGrassTuft(float x, float z, float width, float height, float rotY) {
    float y = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // Subtle gentle night wind sway
    float windAngle = std::sin(g_time * 1.45f + x * 0.35f + z * 0.25f) * 2.8f;
    glRotatef(windAngle, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_DEAD_GRASS);
    bindTexture(TEX_NONE);

    float hw = width * 0.5f;

    glBegin(GL_TRIANGLES);

    // 1. Low Ground-Hugging Undergrowth Base Leaves (Dark decayed charcoal tones)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glColor4f(0.040f, 0.045f, 0.038f, 1.0f);
    glVertex3f(0.0f, 0.01f, 0.0f);
    glVertex3f(-hw * 0.65f, 0.02f, -hw * 0.45f);
    glVertex3f(-hw * 0.20f, 0.03f, -hw * 0.85f);

    glVertex3f(0.0f, 0.01f, 0.0f);
    glVertex3f( hw * 0.20f, 0.03f, -hw * 0.85f);
    glVertex3f( hw * 0.65f, 0.02f, -hw * 0.45f);

    glVertex3f(0.0f, 0.01f, 0.0f);
    glVertex3f( hw * 0.75f, 0.02f,  hw * 0.35f);
    glVertex3f( hw * 0.35f, 0.03f,  hw * 0.75f);

    glVertex3f(0.0f, 0.01f, 0.0f);
    glVertex3f(-hw * 0.35f, 0.03f,  hw * 0.75f);
    glVertex3f(-hw * 0.75f, 0.02f,  hw * 0.35f);

    // 2. Eight Tall Fanning Jungle-Grass Blades (Dark moody night shading)
    // Blade 1 (0 deg)
    glNormal3f(0.0f, 0.3f, 0.95f);
    glColor4f(0.050f, 0.058f, 0.045f, 1.0f);
    glVertex3f(-hw * 0.35f, 0.0f, 0.0f);
    glVertex3f( hw * 0.35f, 0.0f, 0.0f);
    glColor4f(0.105f, 0.120f, 0.090f, 1.0f);
    glVertex3f(0.02f, height, 0.05f);

    // Blade 2 (25 deg)
    glNormal3f(0.4f, 0.3f, 0.85f);
    glColor4f(0.048f, 0.055f, 0.044f, 1.0f);
    glVertex3f(-hw * 0.30f, 0.0f, -hw * 0.15f);
    glVertex3f( hw * 0.30f, 0.0f,  hw * 0.15f);
    glColor4f(0.110f, 0.125f, 0.095f, 1.0f);
    glVertex3f(0.06f, height * 0.96f, -0.02f);

    // Blade 3 (50 deg)
    glNormal3f(0.75f, 0.3f, 0.6f);
    glColor4f(0.046f, 0.052f, 0.042f, 1.0f);
    glVertex3f(-hw * 0.22f, 0.0f, -hw * 0.26f);
    glVertex3f( hw * 0.22f, 0.0f,  hw * 0.26f);
    glColor4f(0.100f, 0.115f, 0.088f, 1.0f);
    glVertex3f(0.05f, height * 0.90f, 0.04f);

    // Blade 4 (75 deg)
    glNormal3f(0.95f, 0.3f, 0.25f);
    glColor4f(0.048f, 0.055f, 0.044f, 1.0f);
    glVertex3f(-hw * 0.10f, 0.0f, -hw * 0.32f);
    glVertex3f( hw * 0.10f, 0.0f,  hw * 0.32f);
    glColor4f(0.112f, 0.128f, 0.096f, 1.0f);
    glVertex3f(-0.02f, height * 0.94f, 0.02f);

    // Blade 5 (100 deg)
    glNormal3f(0.95f, 0.3f, -0.25f);
    glColor4f(0.046f, 0.052f, 0.042f, 1.0f);
    glVertex3f( hw * 0.10f, 0.0f, -hw * 0.32f);
    glVertex3f(-hw * 0.10f, 0.0f,  hw * 0.32f);
    glColor4f(0.098f, 0.112f, 0.086f, 1.0f);
    glVertex3f(-0.04f, height * 0.88f, -0.03f);

    // Blade 6 (125 deg)
    glNormal3f(0.75f, 0.3f, -0.6f);
    glColor4f(0.045f, 0.050f, 0.040f, 1.0f);
    glVertex3f( hw * 0.22f, 0.0f, -hw * 0.26f);
    glVertex3f(-hw * 0.22f, 0.0f,  hw * 0.26f);
    glColor4f(0.102f, 0.118f, 0.090f, 1.0f);
    glVertex3f(-0.06f, height * 0.92f, -0.02f);

    // Blade 7 (150 deg)
    glNormal3f(0.4f, 0.3f, -0.85f);
    glColor4f(0.048f, 0.054f, 0.044f, 1.0f);
    glVertex3f( hw * 0.30f, 0.0f, -hw * 0.15f);
    glVertex3f(-hw * 0.30f, 0.0f,  hw * 0.15f);
    glColor4f(0.108f, 0.124f, 0.094f, 1.0f);
    glVertex3f(-0.04f, height * 0.95f, 0.03f);

    // Blade 8 (175 deg)
    glNormal3f(-0.1f, 0.3f, -0.95f);
    glColor4f(0.045f, 0.050f, 0.040f, 1.0f);
    glVertex3f( hw * 0.35f, 0.0f, 0.0f);
    glVertex3f(-hw * 0.35f, 0.0f, 0.0f);
    glColor4f(0.095f, 0.110f, 0.084f, 1.0f);
    glVertex3f(0.01f, height * 0.87f, -0.04f);

    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// Fallen decayed autumnal leaves on the ground
void drawFallenLeaf(float x, float z, float size, float rotY, float pitch, float r, float g, float b) {
    float y = getTerrainHeight(x, z) + 0.012f;
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(pitch, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_FALLEN_LEAF);
    bindTexture(TEX_NONE);
    glColor4f(r, g, b, 1.0f);

    float hs = size * 0.5f;
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.15f);
    glVertex3f(0.0f, 0.008f, 0.0f);
    glVertex3f(-hs, 0.0f, -hs * 0.6f);
    glVertex3f(0.0f, 0.004f, -hs * 1.2f);
    glVertex3f(hs, 0.0f, -hs * 0.6f);
    glVertex3f(hs * 0.5f, 0.004f, hs * 0.8f);
    glVertex3f(-hs * 0.5f, 0.004f, hs * 0.8f);
    glVertex3f(-hs, 0.0f, -hs * 0.6f);
    glEnd();

    glPopMatrix();
}

// Dynamic falling & fluttering leaves system update
void updateFallingLeaves(float dt) {
    // Wind direction: Right → Left (negative X) with slight forward drift
    const float windSpeedX = -1.0f;  // m/s lateral wind drift
    const float windSpeedZ = -0.15f; // Slight forward drift

    for (size_t i = 0; i < g_fallingLeaves.size(); ++i) {
        FallingLeafParticle& l = g_fallingLeaves[i];
        l.y -= l.vy * dt;

        // Apply wind drift to origin tracking
        l.originX += windSpeedX * dt * (0.8f + 0.4f * std::sin(g_time * 0.5f + l.swayPhase));
        l.originZ += windSpeedZ * dt;

        // Fluttering sway oscillation (relative to drifting origin)
        float swayTime = g_time * l.swayFreq + l.swayPhase;
        l.x = l.originX + std::sin(swayTime) * l.swayAmp;
        l.z = l.originZ + std::cos(swayTime * 0.8f) * (l.swayAmp * 0.65f);

        // 3D tumbling rotation
        l.rotX += l.rotSpeedX * dt;
        l.rotY += l.rotSpeedY * dt;
        l.rotZ += l.rotSpeedZ * dt;

        // Check ground landing OR drifted too far left
        float ground = getTerrainHeight(l.x, l.z) + 0.04f;
        if (l.y <= ground || l.originX < -20.0f) {
            // Respawn in the upper canopy of natural trees on the RIGHT side (wind source)
            l.y = 3.6f + ((float)(rand() % 15) / 10.0f); // 3.6m to 5.1m (matching natural small tree height)
            float rChoice = (float)(rand() % 100) / 100.0f;
            if (rChoice < 0.60f) {
                l.originX = 5.0f + ((float)(rand() % 40) / 10.0f);
                l.originZ = 13.0f + ((float)(rand() % 40) / 10.0f);
            } else if (rChoice < 0.90f) {
                l.originX = -7.0f + ((float)(rand() % 40) / 10.0f);
                l.originZ = 13.0f + ((float)(rand() % 40) / 10.0f);
            } else {
                l.originX = -2.0f + ((float)(rand() % 40) / 10.0f);
                l.originZ = 10.0f + ((float)(rand() % 60) / 10.0f);
            }
            l.x = l.originX;
            l.z = l.originZ;
        }
    }
}

// Render dynamic airborne falling leaves
void drawFallingLeaves() {
    applyMaterial(MAT_FALLEN_LEAF);
    bindTexture(TEX_NONE);

    for (size_t i = 0; i < g_fallingLeaves.size(); ++i) {
        const FallingLeafParticle& l = g_fallingLeaves[i];
        glPushMatrix();
        glTranslatef(l.x, l.y, l.z);
        glRotatef(l.rotX, 1.0f, 0.0f, 0.0f);
        glRotatef(l.rotY, 0.0f, 1.0f, 0.0f);
        glRotatef(l.rotZ, 0.0f, 0.0f, 1.0f);

        glColor4f(l.r, l.g, l.b, 0.95f);
        float hs = l.size * 0.5f;
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-hs, 0.0f, -hs * 0.7f);
        glVertex3f( hs, 0.0f, -hs * 0.7f);
        glVertex3f( hs * 0.8f, 0.0f,  hs * 0.7f);
        glVertex3f(-hs * 0.8f, 0.0f,  hs * 0.7f);
        glEnd();

        glPopMatrix();
    }
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

// Small pebbles and rocks resting on the ground
void drawPebble(float x, float z, float scaleX, float scaleY, float scaleZ, float rotY) {
    float y = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, y + scaleY * 0.35f, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glScalef(scaleX, scaleY, scaleZ);

    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    drawSphere(1.0f, 6, 5, 0.5f, 0.5f);
    glPopMatrix();
}

// ----------------------------------------------------------------------------
// IRREGULAR MOSSY ROCKS & ENVIRONMENTAL ABANDONED CLUTTER
// ----------------------------------------------------------------------------

// Irregular Low-Poly Boulder with Deterministic Vertex Perturbation & Moss-Green Top Tint
void drawIrregularRock(float x, float z, float rx, float ry, float rz, float rotY, float rotX, unsigned int seed, float mossFactor = 0.65f) {
    float groundY = getTerrainHeight(x, z);
    // Partially sunk into terrain
    float y = groundY - ry * 0.30f;

    TreeRNG rng(seed);

    int stacks = 8;
    int slices = 12;

    struct RockVert {
        float x, y, z;
        float nx, ny, nz;
        float moss;
    };
    std::vector<RockVert> verts((stacks + 1) * (slices + 1));

    for (int i = 0; i <= stacks; ++i) {
        float phi = (float)M_PI * (-0.5f + (float)i / stacks);
        float cosPhi = std::cos(phi);
        float sinPhi = std::sin(phi);

        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * (float)M_PI * (float)j / slices;
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            // Natural organic perturbation per vertex
            float perturb = 1.0f + rng.nextFloat(-0.24f, 0.24f);

            float vx = rx * cosPhi * cosTheta * perturb;
            float vy = ry * sinPhi * perturb;
            float vz = rz * cosPhi * sinTheta * perturb;

            int idx = i * (slices + 1) + j;
            verts[idx].x = vx;
            verts[idx].y = vy;
            verts[idx].z = vz;

            // Approximate vertex normal
            float nx = vx / (rx * rx);
            float ny = vy / (ry * ry);
            float nz = vz / (rz * rz);
            float nlen = std::sqrt(nx*nx + ny*ny + nz*nz);
            if (nlen > 0.001f) { nx /= nlen; ny /= nlen; nz /= nlen; }
            verts[idx].nx = nx;
            verts[idx].ny = ny;
            verts[idx].nz = nz;

            // Moss tint calculation on upward-facing surfaces (ny > 0.15)
            float upMoss = (ny > 0.15f) ? ((ny - 0.15f) / 0.85f * mossFactor) : 0.0f;
            verts[idx].moss = upMoss;
        }
    }

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);

    glBegin(GL_QUADS);
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int idx00 = i * (slices + 1) + j;
            int idx10 = (i + 1) * (slices + 1) + j;
            int idx11 = (i + 1) * (slices + 1) + (j + 1);
            int idx01 = i * (slices + 1) + (j + 1);

            // Facet normal for crisp, rugged, craggy stone edges
            float edge1X = verts[idx10].x - verts[idx00].x;
            float edge1Y = verts[idx10].y - verts[idx00].y;
            float edge1Z = verts[idx10].z - verts[idx00].z;

            float edge2X = verts[idx01].x - verts[idx00].x;
            float edge2Y = verts[idx01].y - verts[idx00].y;
            float edge2Z = verts[idx01].z - verts[idx00].z;

            float fnx = edge1Y * edge2Z - edge1Z * edge2Y;
            float fny = edge1Z * edge2X - edge1X * edge2Z;
            float fnz = edge1X * edge2Y - edge1Y * edge2X;
            float fnlen = std::sqrt(fnx*fnx + fny*fny + fnz*fnz);
            if (fnlen > 0.0001f) { fnx /= fnlen; fny /= fnlen; fnz /= fnlen; }

            glNormal3f(fnx, fny, fnz);

            auto emitVert = [&](int idx, float u, float v) {
                float m = verts[idx].moss;
                // Moss-green tint multiplier on upper faces
                float r = 1.0f - 0.35f * m;
                float g = 1.0f + 0.18f * m;
                float b = 1.0f - 0.45f * m;
                glColor4f(r, g, b, 1.0f);
                glTexCoord2f(u, v);
                glVertex3f(verts[idx].x, verts[idx].y, verts[idx].z);
            };

            emitVert(idx00, (float)j / slices * 2.0f, (float)i / stacks * 2.0f);
            emitVert(idx10, (float)j / slices * 2.0f, (float)(i + 1) / stacks * 2.0f);
            emitVert(idx11, (float)(j + 1) / slices * 2.0f, (float)(i + 1) / stacks * 2.0f);
            emitVert(idx01, (float)(j + 1) / slices * 2.0f, (float)i / stacks * 2.0f);
        }
    }
    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// Broken Wooden Crate with Missing / Cracked Planks & Spilling Boards
void drawBrokenCrate(float x, float z, float rotY, float tilt = 6.0f) {
    float groundY = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, groundY + 0.38f, z); // Embedded slightly into dirt
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(tilt, 0.0f, 0.0f, 1.0f);

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);

    float cw = 1.05f, ch = 0.85f, cd = 0.85f;
    float hw = cw * 0.5f, hh = ch * 0.5f, hd = cd * 0.5f;

    // 4 Vertical corner posts
    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            glPushMatrix();
            glTranslatef(i * (hw - 0.04f), 0.0f, j * (hd - 0.04f));
            drawBox(0.08f, ch, 0.08f, 0.3f, 1.0f);
            glPopMatrix();
        }
    }

    // Bottom floor slats
    for (int i = -2; i <= 2; ++i) {
        glPushMatrix();
        glTranslatef(i * 0.20f, -hh + 0.02f, 0.0f);
        drawBox(0.16f, 0.04f, cd - 0.08f, 0.5f, 1.0f);
        glPopMatrix();
    }

    // Back side planks (+Z)
    for (int i = -1; i <= 1; ++i) {
        glPushMatrix();
        glTranslatef(0.0f, i * 0.26f, hd - 0.02f);
        drawBox(cw, 0.20f, 0.04f, 1.0f, 0.4f);
        glPopMatrix();
    }

    // Left side planks (-X)
    for (int i = -1; i <= 1; ++i) {
        glPushMatrix();
        glTranslatef(-hw + 0.02f, i * 0.26f, 0.0f);
        drawBox(0.04f, 0.20f, cd, 0.4f, 1.0f);
        glPopMatrix();
    }

    // Right side planks (+X)
    for (int i = -1; i <= 1; ++i) {
        glPushMatrix();
        glTranslatef(hw - 0.02f, i * 0.26f, 0.0f);
        drawBox(0.04f, 0.20f, cd, 0.4f, 1.0f);
        glPopMatrix();
    }

    // Broken Front side (-Z): bottom plank intact, middle missing, top tilted cracked!
    glPushMatrix();
    glTranslatef(0.0f, -0.26f, -hd + 0.02f);
    drawBox(cw, 0.20f, 0.04f, 1.0f, 0.4f); // Bottom plank
    glPopMatrix();

    // Smashed loose plank dangling out of the front
    glPushMatrix();
    glTranslatef(0.15f, 0.08f, -hd - 0.15f);
    glRotatef(28.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-18.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.75f, 0.18f, 0.04f, 1.0f, 0.3f);
    glPopMatrix();

    // Loose broken board sticking out of the top
    glPushMatrix();
    glTranslatef(-0.10f, hh + 0.12f, 0.05f);
    glRotatef(-35.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(22.0f, 1.0f, 0.0f, 0.0f);
    drawBox(0.85f, 0.04f, 0.18f, 1.0f, 0.3f);
    glPopMatrix();

    glPopMatrix();
}

// Old Wooden Barrel with Rusty Iron Hoops
void drawOldBarrel(float x, float z, float rotY, float tilt = 0.0f) {
    float groundY = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, groundY + 0.48f, z); // Embedded base into soil
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(tilt, 1.0f, 0.0f, 0.0f);

    float rEnd = 0.44f;
    float rMid = 0.54f;
    float h = 1.05f;

    // Wooden Staved Body (two tapered halves meeting at bulging belly)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);

    glPushMatrix();
    glTranslatef(0.0f, -h * 0.5f, 0.0f);
    drawCylinder(rEnd, rMid, h * 0.5f, 16, 2.0f, 1.0f); // Lower half
    glTranslatef(0.0f, h * 0.5f, 0.0f);
    drawCylinder(rMid, rEnd, h * 0.5f, 16, 2.0f, 1.0f); // Upper half
    glPopMatrix();

    // Top & Bottom Recessed Lid Caps
    glPushMatrix();
    glTranslatef(0.0f, h * 0.5f - 0.02f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(rEnd * 0.95f, 0.01f, 0.02f, 14, 1.0f, 1.0f);
    glPopMatrix();

    // 3 Rusty Metal Bands / Hoops around the barrel
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);

    float hoopPositions[3] = { -h * 0.35f, 0.0f, h * 0.35f };
    float hoopRadii[3]     = { rEnd * 1.08f, rMid * 1.03f, rEnd * 1.08f };

    for (int i = 0; i < 3; ++i) {
        glPushMatrix();
        glTranslatef(0.0f, hoopPositions[i] - 0.025f, 0.0f);
        drawCylinder(hoopRadii[i], hoopRadii[i], 0.05f, 16, 1.0f, 0.2f);
        glPopMatrix();
    }

    glPopMatrix();
}

// Fallen Fence Planks / Pointed Pickets Lying in the Mud
void drawFallenPlank(float x, float z, float rotY, float pitch) {
    float y = getTerrainHeight(x, z) + 0.015f;
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(pitch, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);

    drawBox(0.14f, 0.04f, 1.55f, 0.3f, 2.0f);
    // Pointed picket top
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.85f);
    drawPrismRoof(0.14f, 0.16f, 0.04f, 0.3f, 0.3f);
    glPopMatrix();

    glPopMatrix();
}

// Rusty Metal Bucket with Arched Wire Handle
void drawRustyBucket(float x, float z, float rotY, float tilt = 22.0f) {
    float y = getTerrainHeight(x, z) + 0.05f;
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(tilt, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);

    float rBase = 0.15f;
    float rTop  = 0.22f;
    float h     = 0.36f;

    // Tapered bucket body
    drawCylinder(rBase, rTop, h, 14, 1.0f, 1.0f);

    // Bottom rim & top rolled rim
    glPushMatrix();
    glTranslatef(0.0f, h - 0.01f, 0.0f);
    drawCylinder(rTop * 1.04f, rTop * 1.04f, 0.025f, 14, 1.0f, 0.2f);
    glPopMatrix();

    // Arched Wire Handle (Bail) draped over the bucket
    bindTexture(TEX_NONE);
    glLineWidth(2.5f);
    glColor3f(0.35f, 0.25f, 0.20f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 12; ++i) {
        float a = (float)i * (float)M_PI / 12.0f;
        float hx = (rTop + 0.02f) * std::cos(a);
        float hy = h + 0.22f * std::sin(a);
        glVertex3f(hx, hy, 0.0f);
    }
    glEnd();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glPopMatrix();
}

// Scattered Clay Bricks around the house foundation & corners
void drawSingleBrick(float x, float z, float rotY, float pitch = 0.0f) {
    float y = getTerrainHeight(x, z) + 0.035f;
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(pitch, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_CLAY_BRICK);
    bindTexture(TEX_STONE); // subtle rough stone/brick texture

    drawBox(0.25f, 0.07f, 0.12f, 0.5f, 0.3f);
    glPopMatrix();
}

void drawScatteredBricks() {
    // Cluster 1: Near cracked foundation wall corner
    drawSingleBrick( 7.5f, 3.8f,  18.0f);
    drawSingleBrick( 7.9f, 3.2f, -42.0f,  6.0f);
    drawSingleBrick( 7.2f, 4.3f,  65.0f);
    drawSingleBrick( 8.3f, 4.0f, -10.0f, 12.0f);
    drawSingleBrick( 7.8f, 4.8f,  30.0f);

    // Cluster 2: Near porch steps / foundation
    drawSingleBrick(-5.2f, 7.8f,  33.0f);
    drawSingleBrick(-5.6f, 7.2f, -25.0f,  8.0f);
    drawSingleBrick(-4.9f, 8.4f,  70.0f);

    // Cluster 3: Near broken crate
    drawSingleBrick(-6.8f, 10.8f, -15.0f);
    drawSingleBrick(-5.8f, 12.2f,  40.0f, 10.0f);
}

// Tangled Bare Dead Bush / Bramble Shrub (with wind sway)
void drawDeadBush(float x, float z, float scale, float rotY, unsigned int seed) {
    float groundY = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, groundY, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // Subtle gentle night wind sway
    float bushWind = std::sin(g_time * 1.25f + x * 0.3f + z * 0.2f) * 2.5f;
    glRotatef(bushWind, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_BARK);
    bindTexture(TEX_BARK);

    TreeRNG rng(seed);
    int numStems = 7;
    for (int i = 0; i < numStems; ++i) {
        float azimuth = (float)i * (360.0f / numStems) + rng.nextFloat(-20.0f, 20.0f);
        float outAngle = rng.nextFloat(35.0f, 65.0f);
        float stemLen = scale * rng.nextFloat(0.70f, 1.15f);

        glPushMatrix();
        glRotatef(azimuth, 0.0f, 1.0f, 0.0f);
        glRotatef(outAngle, 1.0f, 0.0f, 0.0f);

        // Lower stem
        drawCylinder(0.045f * scale, 0.025f * scale, stemLen * 0.5f, 5, 1.0f, 1.0f);
        glTranslatef(0.0f, stemLen * 0.5f, 0.0f);

        // Crooked bend
        glRotatef(rng.nextFloat(-25.0f, 25.0f), 1.0f, 0.0f, 0.0f);
        glRotatef(rng.nextFloat(-25.0f, 25.0f), 0.0f, 0.0f, 1.0f);
        drawCylinder(0.025f * scale, 0.010f * scale, stemLen * 0.5f, 4, 1.0f, 1.0f);
        glTranslatef(0.0f, stemLen * 0.5f, 0.0f);

        // Sub-twigs
        for (int t = 0; t < 2; ++t) {
            glPushMatrix();
            glRotatef(rng.nextFloat(30.0f, 60.0f), 1.0f, 0.0f, 0.0f);
            glRotatef((float)t * 180.0f + rng.nextFloat(-20.0f, 20.0f), 0.0f, 1.0f, 0.0f);
            drawCylinder(0.012f * scale, 0.004f * scale, stemLen * 0.40f, 4);
            glPopMatrix();
        }

        glPopMatrix();
    }

    glPopMatrix();
}

// Leaning Old Wrought-Iron Lamppost with Faint Glowing Lantern
void drawLeaningLamppost(float x, float z, float rotY, float leanAngle = 11.5f) {
    float groundY = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, groundY, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(leanAngle, 0.0f, 0.0f, 1.0f); // Haunting historic soil lean

    // Stepped Pedestal Base Plinth (deepened to stay grounded under soil lean)
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);

    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.0f);
    drawBox(0.52f, 0.38f, 0.52f); // Deepened bottom step (penetrates terrain)
    glTranslatef(0.0f, 0.25f, 0.0f);
    drawBox(0.40f, 0.16f, 0.40f); // Middle tier
    glTranslatef(0.0f, 0.14f, 0.0f);
    drawCylinder(0.18f, 0.12f, 0.16f, 10, 1.0f, 0.5f); // Base collar
    glPopMatrix();

    // Fluted Tapered Iron Shaft (3.2m tall)
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.0f);
    drawCylinder(0.10f, 0.065f, 3.1f, 10, 1.0f, 3.0f);

    // Mid-shaft decorative ring collar
    glTranslatef(0.0f, 1.8f, 0.0f);
    drawCylinder(0.09f, 0.09f, 0.06f, 10, 1.0f, 0.2f);

    // Top capital header
    glTranslatef(0.0f, 1.3f, 0.0f);
    drawBox(0.18f, 0.08f, 0.18f);
    glPopMatrix();

    // Decorative Scrollwork Curved Bracket Arm holding the lantern
    glPushMatrix();
    glTranslatef(0.0f, 3.85f, 0.0f);

    // Horizontal bracket arm
    glPushMatrix();
    glTranslatef(0.35f, 0.0f, 0.0f);
    drawBox(0.70f, 0.05f, 0.05f);
    glPopMatrix();

    // Curved lower support strut
    glPushMatrix();
    glTranslatef(0.20f, -0.22f, 0.0f);
    glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.45f, 0.04f, 0.04f);
    glPopMatrix();

    // Lantern Drop Mount
    glTranslatef(0.65f, -0.15f, 0.0f);

    // Antique 4-Sided Carriage Lantern Housing
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);

    // Top roof cap
    glPushMatrix();
    glTranslatef(0.0f, 0.28f, 0.0f);
    drawPrismRoof(0.38f, 0.18f, 0.38f, 0.5f, 0.5f);
    glTranslatef(0.0f, 0.20f, 0.0f);
    drawSphere(0.04f, 8, 6); // Top finial
    glPopMatrix();

    // Bottom finial drop
    glPushMatrix();
    glTranslatef(0.0f, -0.32f, 0.0f);
    drawCylinder(0.06f, 0.01f, 0.12f, 8);
    glPopMatrix();

    // Glass Lantern Panes
    applyMaterial(MAT_CAR_GLASS);
    bindTexture(TEX_NONE);
    glPushMatrix();
    drawBox(0.30f, 0.46f, 0.30f);
    glPopMatrix();

    // Decayed dark filament inside lantern (no bright glare on player's side)
    applyMaterial(MAT_DARK_WOOD);
    glPushMatrix();
    drawSphere(0.05f, 8, 6);
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();
}

// ----------------------------------------------------------------------------
// OLD FURNITURE (Broken Chair & Table - Reference Panel 2)
// ----------------------------------------------------------------------------
void drawBrokenChair(float x, float z, float rotY, float tilt = 0.0f) {
    float gy = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, gy, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    if (tilt != 0.0f) {
        glRotatef(tilt, 0.0f, 0.0f, 1.0f);
    }

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.32f, 0.28f, 0.24f, 1.0f);

    // Seat plank
    glPushMatrix();
    glTranslatef(0.0f, 0.45f, 0.0f);
    drawBox(0.48f, 0.04f, 0.48f);
    glPopMatrix();

    // 4 Legs (Front-left leg is broken shorter)
    glPushMatrix();
    glTranslatef(-0.19f, 0.225f, -0.19f); drawBox(0.045f, 0.45f, 0.045f); // Back-left
    glTranslatef( 0.38f, 0.0f,   0.0f);   drawBox(0.045f, 0.45f, 0.045f); // Back-right
    glTranslatef( 0.0f,  0.0f,   0.38f);  drawBox(0.045f, 0.45f, 0.045f); // Front-right
    glTranslatef(-0.38f, 0.10f,  0.0f);   drawBox(0.045f, 0.25f, 0.045f); // Front-left (broken short)
    glPopMatrix();

    // Backrest vertical posts
    glPushMatrix();
    glTranslatef(-0.19f, 0.72f, -0.19f); drawBox(0.045f, 0.52f, 0.045f);
    glTranslatef( 0.38f, 0.0f,   0.0f);   drawBox(0.045f, 0.52f, 0.045f);
    // Backrest top rail
    glTranslatef(-0.19f, 0.24f,  0.0f);   drawBox(0.46f, 0.06f, 0.04f);
    // Backrest middle horizontal slat
    glTranslatef( 0.0f, -0.16f,  0.0f);   drawBox(0.42f, 0.05f, 0.035f);
    // Vertical center spindle (crooked/broken)
    glTranslatef(-0.06f, -0.14f, 0.0f);   glRotatef(12.0f, 0.0f, 0.0f, 1.0f); drawBox(0.03f, 0.24f, 0.03f);
    glPopMatrix();

    glPopMatrix();
}

void drawWoodenTable(float x, float z, float rotY, float tilt = 0.0f) {
    float gy = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, gy, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    if (tilt != 0.0f) {
        glRotatef(tilt, 1.0f, 0.0f, 0.0f);
    }

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.30f, 0.26f, 0.22f, 1.0f);

    // Tabletop slab
    glPushMatrix();
    glTranslatef(0.0f, 0.72f, 0.0f);
    drawBox(1.10f, 0.05f, 0.65f);
    // Apron frame under top
    glTranslatef(0.0f, -0.04f, 0.0f);
    drawBox(1.02f, 0.07f, 0.58f);
    glPopMatrix();

    // 4 Sturdy Legs
    glPushMatrix();
    glTranslatef(-0.48f, 0.35f, -0.26f); drawBox(0.06f, 0.70f, 0.06f);
    glTranslatef( 0.96f, 0.0f,   0.0f);   drawBox(0.06f, 0.70f, 0.06f);
    glTranslatef( 0.0f,  0.0f,   0.52f);  drawBox(0.06f, 0.70f, 0.06f);
    glTranslatef(-0.96f, 0.0f,   0.0f);   drawBox(0.06f, 0.70f, 0.06f);
    glPopMatrix();

    glPopMatrix();
}

void drawOldFurniture() {
    // 1. On the Porch (Under the roof near the warm glowing bulb)
    drawWoodenTable(-2.2f + g_houseShiftX, 4.2f + g_houseShiftZ, -12.0f);
    drawBrokenChair(-3.3f + g_houseShiftX, 4.0f + g_houseShiftZ,  22.0f, 4.0f);
    drawBrokenChair(-1.3f + g_houseShiftX, 4.2f + g_houseShiftZ, -60.0f, 0.0f);

    // 2. Inside the House (Visible through open doorway and windows)
    drawWoodenTable(-5.5f + g_houseShiftX, 1.5f + g_houseShiftZ,  15.0f);
    drawBrokenChair(-4.5f + g_houseShiftX, 1.8f + g_houseShiftZ,  45.0f);
    drawBrokenChair(-6.8f + g_houseShiftX, 1.2f + g_houseShiftZ, -30.0f);
}

// ----------------------------------------------------------------------------
// 3D CARVED LETTERING HELPER (For Rustic Signboards & Inscriptions)
// ----------------------------------------------------------------------------
static void drawLetterStroke2D(float x1, float y1, float x2, float y2, float thick, float depth) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.0001f) return;
    float nx = -dy / len * (thick * 0.5f);
    float ny =  dx / len * (thick * 0.5f);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(x1 - nx, y1 - ny, depth);
    glVertex3f(x2 - nx, y2 - ny, depth);
    glVertex3f(x2 + nx, y2 + ny, depth);
    glVertex3f(x1 + nx, y1 + ny, depth);
    glEnd();
}

static void drawSignChar(char c, float cx, float cy, float w, float h, float thick, float depth) {
    float hw = w * 0.5f;
    float hh = h * 0.5f;
    switch (c) {
        case 'A':
            drawLetterStroke2D(cx - hw, cy - hh, cx, cy + hh, thick, depth);
            drawLetterStroke2D(cx, cy + hh, cx + hw, cy - hh, thick, depth);
            drawLetterStroke2D(cx - hw * 0.55f, cy - hh * 0.15f, cx + hw * 0.55f, cy - hh * 0.15f, thick, depth);
            break;
        case 'B':
            drawLetterStroke2D(cx - hw, cy - hh, cx - hw, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh, cx + hw * 0.5f, cy + hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.5f, cy + hh, cx + hw, cy + hh * 0.5f, thick, depth);
            drawLetterStroke2D(cx + hw, cy + hh * 0.5f, cx + hw * 0.5f, cy, thick, depth);
            drawLetterStroke2D(cx - hw, cy, cx + hw * 0.5f, cy, thick, depth);
            drawLetterStroke2D(cx + hw * 0.5f, cy, cx + hw, cy - hh * 0.5f, thick, depth);
            drawLetterStroke2D(cx + hw, cy - hh * 0.5f, cx + hw * 0.5f, cy - hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.5f, cy - hh, cx - hw, cy - hh, thick, depth);
            break;
        case 'N':
            drawLetterStroke2D(cx - hw, cy - hh, cx - hw, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh, cx + hw, cy - hh, thick, depth);
            drawLetterStroke2D(cx + hw, cy - hh, cx + hw, cy + hh, thick, depth);
            break;
        case 'D':
            drawLetterStroke2D(cx - hw, cy - hh, cx - hw, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh, cx + hw * 0.35f, cy + hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.35f, cy + hh, cx + hw, cy, thick, depth);
            drawLetterStroke2D(cx + hw, cy, cx + hw * 0.35f, cy - hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.35f, cy - hh, cx - hw, cy - hh, thick, depth);
            break;
        case 'O':
            drawLetterStroke2D(cx - hw * 0.4f, cy + hh, cx + hw * 0.4f, cy + hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.4f, cy + hh, cx + hw, cy + hh * 0.4f, thick, depth);
            drawLetterStroke2D(cx + hw, cy + hh * 0.4f, cx + hw, cy - hh * 0.4f, thick, depth);
            drawLetterStroke2D(cx + hw, cy - hh * 0.4f, cx + hw * 0.4f, cy - hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.4f, cy - hh, cx - hw * 0.4f, cy - hh, thick, depth);
            drawLetterStroke2D(cx - hw * 0.4f, cy - hh, cx - hw, cy - hh * 0.4f, thick, depth);
            drawLetterStroke2D(cx - hw, cy - hh * 0.4f, cx - hw, cy + hh * 0.4f, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh * 0.4f, cx - hw * 0.4f, cy + hh, thick, depth);
            break;
        case 'E':
            drawLetterStroke2D(cx - hw, cy - hh, cx - hw, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh, cx + hw * 0.85f, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh * 0.05f, cx + hw * 0.6f, cy + hh * 0.05f, thick, depth);
            drawLetterStroke2D(cx - hw, cy - hh, cx + hw * 0.85f, cy - hh, thick, depth);
            break;
        case 'H':
            drawLetterStroke2D(cx - hw, cy - hh, cx - hw, cy + hh, thick, depth);
            drawLetterStroke2D(cx + hw, cy - hh, cx + hw, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw, cy, cx + hw, cy, thick, depth);
            break;
        case 'U':
            drawLetterStroke2D(cx - hw, cy + hh, cx - hw, cy - hh * 0.4f, thick, depth);
            drawLetterStroke2D(cx - hw, cy - hh * 0.4f, cx - hw * 0.4f, cy - hh, thick, depth);
            drawLetterStroke2D(cx - hw * 0.4f, cy - hh, cx + hw * 0.4f, cy - hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.4f, cy - hh, cx + hw, cy - hh * 0.4f, thick, depth);
            drawLetterStroke2D(cx + hw, cy - hh * 0.4f, cx + hw, cy + hh, thick, depth);
            break;
        case 'S':
            drawLetterStroke2D(cx + hw * 0.8f, cy + hh, cx - hw * 0.4f, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw * 0.4f, cy + hh, cx - hw, cy + hh * 0.6f, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh * 0.6f, cx - hw * 0.4f, cy + hh * 0.1f, thick, depth);
            drawLetterStroke2D(cx - hw * 0.4f, cy + hh * 0.1f, cx + hw * 0.4f, cy - hh * 0.1f, thick, depth);
            drawLetterStroke2D(cx + hw * 0.4f, cy - hh * 0.1f, cx + hw, cy - hh * 0.6f, thick, depth);
            drawLetterStroke2D(cx + hw, cy - hh * 0.6f, cx + hw * 0.4f, cy - hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.4f, cy - hh, cx - hw * 0.8f, cy - hh, thick, depth);
            break;
    }
}

// ----------------------------------------------------------------------------
// SIGN BOARD ("ABANDONED HOUSE" - Image 17 & 16)
// ----------------------------------------------------------------------------
void drawSignBoard(float x, float z, float rotY) {
    float gy = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, gy, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(-2.8f, 0.0f, 0.0f, 1.0f); // Slight weathered crooked tilt

    // 1. Vertical main wooden post with beveled top
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.19f, 0.14f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 1.15f, 0.0f);
    drawBox(0.18f, 2.30f, 0.18f, 0.5f, 2.0f);
    // Pyramid weather cap on post top
    glTranslatef(0.0f, 1.15f, 0.0f);
    drawPrismRoof(0.20f, 0.12f, 0.20f, 0.5f, 0.5f);
    glPopMatrix();

    // 2. Diagonal wooden support brace struts underneath the signboard
    glPushMatrix();
    glTranslatef(-0.42f, 1.22f, 0.0f);
    glRotatef(-42.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.09f, 0.75f, 0.09f, 0.5f, 1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.42f, 1.22f, 0.0f);
    glRotatef(42.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.09f, 0.75f, 0.09f, 0.5f, 1.0f);
    glPopMatrix();

    // 3. Wooden Sign Board Assembly (3 weathered horizontal planks with chipped irregular ends)
    glPushMatrix();
    glTranslatef(0.0f, 1.82f, 0.12f);

    // Board Plank 1 (Top Plank)
    glPushMatrix();
    glTranslatef(-0.02f, 0.22f, 0.0f);
    drawBox(1.92f, 0.22f, 0.065f, 2.0f, 0.5f);
    glPopMatrix();

    // Board Plank 2 (Middle Plank - slightly offset and chipped)
    glPushMatrix();
    glTranslatef(0.03f, 0.0f, 0.0f);
    drawBox(1.96f, 0.21f, 0.065f, 2.0f, 0.5f);
    glPopMatrix();

    // Board Plank 3 (Bottom Plank)
    glPushMatrix();
    glTranslatef(-0.01f, -0.22f, 0.0f);
    drawBox(1.88f, 0.22f, 0.065f, 2.0f, 0.5f);
    glPopMatrix();

    // Weathered edge chips & notches on the board silhouette
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glPushMatrix();
    glTranslatef(-0.96f, 0.12f, 0.0f);
    glRotatef(25.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.08f, 0.14f, 0.07f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.97f, -0.15f, 0.0f);
    glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.08f, 0.12f, 0.07f);
    glPopMatrix();

    // 4. Heavy Rustic Iron Corner Brackets & Bolt Caps (Image 17)
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    float bracketPositions[4][2] = {
        { -0.85f,  0.26f },
        {  0.85f,  0.26f },
        { -0.85f, -0.26f },
        {  0.85f, -0.26f }
    };
    for (int b = 0; b < 4; ++b) {
        glPushMatrix();
        glTranslatef(bracketPositions[b][0], bracketPositions[b][1], 0.038f);
        drawBox(0.08f, 0.08f, 0.015f); // Square washer bracket
        glTranslatef(0.0f, 0.0f, 0.010f);
        drawCylinder(0.022f, 0.022f, 0.025f, 6); // Iron bolt head
        glPopMatrix();
    }

    // 5. Inscribed Dark Weathered Typography: "ABANDONED" (Top Line) & "HOUSE" (Bottom Line)
    applyMaterial(MAT_BLACK_IRON);
    bindTexture(TEX_NONE);
    glColor4f(0.08f, 0.06f, 0.05f, 1.0f); // Dark scorched / carved pigment

    // Line 1: "ABANDONED"
    const char* line1 = "ABANDONED";
    float l1StartX = -0.74f;
    float l1Spacing = 0.185f;
    float l1CharW   = 0.125f;
    float l1CharH   = 0.155f;
    float l1StrokeT = 0.022f;
    float l1Y       = 0.11f;
    for (int c = 0; c < 9; ++c) {
        drawSignChar(line1[c], l1StartX + c * l1Spacing, l1Y, l1CharW, l1CharH, l1StrokeT, 0.038f);
    }

    // Line 2: "HOUSE" (Centered, slightly larger)
    const char* line2 = "HOUSE";
    float l2StartX = -0.46f;
    float l2Spacing = 0.23f;
    float l2CharW   = 0.155f;
    float l2CharH   = 0.18f;
    float l2StrokeT = 0.026f;
    float l2Y       = -0.12f;
    for (int c = 0; c < 5; ++c) {
        drawSignChar(line2[c], l2StartX + c * l2Spacing, l2Y, l2CharW, l2CharH, l2StrokeT, 0.038f);
    }

    glPopMatrix(); // End board assembly

    // 6. Scattered base rocks & splinters at post foot
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glPushMatrix();
    glTranslatef(0.18f, 0.08f, 0.14f);
    drawBox(0.24f, 0.14f, 0.20f);
    glTranslatef(-0.35f, 0.0f, -0.10f);
    drawBox(0.18f, 0.10f, 0.16f);
    glPopMatrix();

    glPopMatrix();
}

// ----------------------------------------------------------------------------
// UTILITY / ELECTRIC TELEPHONE POLE WITH SAGGING CABLES (Image 16)
// ----------------------------------------------------------------------------
void drawTelephonePole(float x, float z, float rotY) {
    float gy = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, gy, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(2.4f, 0.0f, 0.0f, 1.0f); // Weathered slight lean

    // Main tall timber pole
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.26f, 0.22f, 0.18f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 4.2f, 0.0f);
    drawBox(0.26f, 8.4f, 0.26f, 1.0f, 4.0f);
    glPopMatrix();

    // Horizontal crossarm near top (6.8m height)
    glPushMatrix();
    glTranslatef(0.0f, 7.2f, 0.0f);
    drawBox(2.6f, 0.18f, 0.16f, 2.0f, 0.5f);

    // Diagonal metal support struts
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(-0.75f, -0.42f, 0.0f);
    glRotatef(-38.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.05f, 0.95f, 0.04f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.75f, -0.42f, 0.0f);
    glRotatef(38.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.05f, 0.95f, 0.04f);
    glPopMatrix();

    // 4 Ceramic / glass electrical insulators with metal mounting pins
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    glColor4f(0.70f, 0.74f, 0.78f, 1.0f);
    float insX[4] = { -1.10f, -0.45f, 0.45f, 1.10f };
    for (int p = 0; p < 4; ++p) {
        glPushMatrix();
        glTranslatef(insX[p], 0.10f, 0.0f);
        drawCylinder(0.045f, 0.03f, 0.22f, 8);
        glTranslatef(0.0f, 0.16f, 0.0f);
        drawCylinder(0.055f, 0.04f, 0.08f, 8); // Flanged top cap
        glPopMatrix();
    }
    glPopMatrix();

    // Sagging electrical cables & loose dangling broken wire (Image 16)
    bindTexture(TEX_NONE);
    applyMaterial(MAT_BLACK_IRON);
    glColor4f(0.12f, 0.12f, 0.12f, 1.0f);
    glLineWidth(2.2f);

    // Loose wire dangling from right insulator all the way down towards the ground
    glBegin(GL_LINE_STRIP);
    glVertex3f(1.10f, 7.35f, 0.0f);
    glVertex3f(1.25f, 6.20f, 0.15f);
    glVertex3f(1.40f, 4.50f, 0.35f);
    glVertex3f(1.30f, 2.80f, 0.20f);
    glVertex3f(1.45f, 1.20f, 0.40f);
    glVertex3f(1.35f, 0.15f, 0.30f);
    glEnd();

    // Cross-catenary hanging loop under crossarm
    glBegin(GL_LINE_STRIP);
    glVertex3f(-1.10f, 7.35f, 0.0f);
    glVertex3f(-0.60f, 6.60f, 0.10f);
    glVertex3f(-0.10f, 6.45f, 0.15f);
    glVertex3f( 0.45f, 7.35f, 0.0f);
    glEnd();

    glPopMatrix();
}

// ----------------------------------------------------------------------------
// HIGH-FIDELITY BROKEN FENCE (Matching Reference Image 14)
// Features: Staggered posts, split-rails, snapped/hanging pickets & ground debris
// ----------------------------------------------------------------------------
void drawDetailedBrokenFenceSection(float x, float z, float rotY) {
    float gy = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, gy, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.26f, 0.22f, 0.18f, 1.0f);

    // --- 1. VERTICAL POSTS ---
    // Post A: Left main post (tall, slight forward lean)
    glPushMatrix();
    glTranslatef(-1.80f, 0.85f, 0.0f);
    glRotatef(-3.5f, 1.0f, 0.0f, 0.0f);
    glRotatef(4.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.18f, 1.70f, 0.18f, 0.5f, 1.5f);
    // Notched split top
    glTranslatef(0.0f, 0.85f, 0.0f);
    drawPrismRoof(0.19f, 0.10f, 0.19f, 0.5f, 0.5f);
    glPopMatrix();

    // Post B: Middle post (medium height, tilted sideways)
    glPushMatrix();
    glTranslatef(0.10f, 0.78f, 0.02f);
    glRotatef(6.5f, 0.0f, 0.0f, 1.0f);
    glRotatef(-2.0f, 1.0f, 0.0f, 0.0f);
    drawBox(0.17f, 1.55f, 0.17f, 0.5f, 1.5f);
    glPopMatrix();

    // Post C: Right post (tall, cracked top)
    glPushMatrix();
    glTranslatef(1.85f, 0.92f, -0.04f);
    glRotatef(-4.5f, 0.0f, 0.0f, 1.0f);
    drawBox(0.19f, 1.85f, 0.19f, 0.5f, 1.5f);
    glPopMatrix();

    // Post D & E: Corner perpendicular fence posts (Image 14 Right Angle)
    glPushMatrix();
    glTranslatef(2.65f, 0.98f, 0.85f);
    glRotatef(3.0f, 1.0f, 0.0f, 0.0f);
    drawBox(0.18f, 1.95f, 0.18f, 0.5f, 1.5f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.70f, 0.72f, 2.10f);
    glRotatef(-5.0f, 1.0f, 0.0f, 0.0f);
    drawBox(0.16f, 1.45f, 0.16f, 0.5f, 1.5f);
    glPopMatrix();

    // --- 2. HORIZONTAL SPLIT-RAIL CROSSBEAMS ---
    // Upper Rail Left Segment (Intact span from Post A to Post B)
    glPushMatrix();
    glTranslatef(-0.85f, 1.18f, 0.09f);
    glRotatef(-1.5f, 0.0f, 0.0f, 1.0f);
    drawBox(1.95f, 0.13f, 0.07f, 2.0f, 0.5f);
    glPopMatrix();

    // Lower Rail Left Segment (Intact span)
    glPushMatrix();
    glTranslatef(-0.85f, 0.55f, 0.09f);
    glRotatef(-1.0f, 0.0f, 0.0f, 1.0f);
    drawBox(1.95f, 0.13f, 0.07f, 2.0f, 0.5f);
    glPopMatrix();

    // Upper Rail Right Segment (Snapped in middle, drooping downwards - Image 14)
    glPushMatrix();
    glTranslatef(0.95f, 1.05f, 0.09f);
    glRotatef(-14.0f, 0.0f, 0.0f, 1.0f); // Snapped and sagging downward
    drawBox(1.85f, 0.12f, 0.07f, 2.0f, 0.5f);
    // Jagged fracture splinter end
    glTranslatef(0.85f, 0.0f, 0.0f);
    glRotatef(28.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.22f, 0.08f, 0.06f);
    glPopMatrix();

    // Lower Rail Right Segment (Broken splintered rail)
    glPushMatrix();
    glTranslatef(0.92f, 0.50f, 0.09f);
    glRotatef(5.0f, 0.0f, 0.0f, 1.0f);
    drawBox(1.40f, 0.11f, 0.07f, 1.5f, 0.5f);
    glPopMatrix();

    // Corner Perpendicular Connecting Rails
    glPushMatrix();
    glTranslatef(2.68f, 1.10f, 1.48f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(6.0f, 0.0f, 0.0f, 1.0f);
    drawBox(1.35f, 0.12f, 0.07f, 1.5f, 0.5f);
    glTranslatef(0.0f, -0.55f, 0.0f);
    drawBox(1.35f, 0.12f, 0.07f, 1.5f, 0.5f);
    glPopMatrix();

    // --- 3. VERTICAL PICKETS / SLATS (Image 14 Broken, Snapped & Hanging States) ---
    // Picket 1: Far left intact picket
    glPushMatrix();
    glTranslatef(-1.40f, 0.85f, 0.15f);
    drawBox(0.12f, 1.15f, 0.035f, 0.5f, 1.0f);
    glTranslatef(0.0f, 0.58f, 0.0f);
    drawPrismRoof(0.13f, 0.10f, 0.04f); // Pointed picket top
    glPopMatrix();

    // Picket 2: Broken top picket (snapped jagged top)
    glPushMatrix();
    glTranslatef(-0.95f, 0.72f, 0.15f);
    glRotatef(3.5f, 0.0f, 0.0f, 1.0f);
    drawBox(0.11f, 0.82f, 0.035f, 0.5f, 1.0f);
    glPopMatrix();

    // Picket 3: Short snapped stump
    glPushMatrix();
    glTranslatef(-0.48f, 0.42f, 0.15f);
    drawBox(0.11f, 0.38f, 0.035f);
    glPopMatrix();

    // Picket 4: Hanging skewed picket attached only by upper nail (Image 14)
    glPushMatrix();
    glTranslatef(0.55f, 0.82f, 0.15f);
    glRotatef(-28.0f, 0.0f, 0.0f, 1.0f); // Swinging sideways at sharp angle
    drawBox(0.12f, 0.78f, 0.035f, 0.5f, 1.0f);
    glPopMatrix();

    // Picket 5: Broken bottom picket segment
    glPushMatrix();
    glTranslatef(1.35f, 0.75f, 0.15f);
    glRotatef(12.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.11f, 0.65f, 0.035f);
    glPopMatrix();

    // Pickets on the corner perpendicular fence segment
    glPushMatrix();
    glTranslatef(2.72f, 0.82f, 1.45f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(-15.0f, 0.0f, 0.0f, 1.0f); // Hanging slanted picket
    drawBox(0.11f, 0.85f, 0.035f);
    glPopMatrix();

    // --- 4. GROUND DEBRIS (Scattered broken plank chunks, rocks & splinters - Image 14) ---
    // Fallen broken plank 1
    glPushMatrix();
    glTranslatef(-0.25f, 0.04f, 0.45f);
    glRotatef(35.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(4.0f, 1.0f, 0.0f, 0.0f);
    drawBox(0.65f, 0.04f, 0.12f);
    glPopMatrix();

    // Fallen broken plank 2
    glPushMatrix();
    glTranslatef(0.85f, 0.04f, 0.38f);
    glRotatef(-48.0f, 0.0f, 1.0f, 0.0f);
    drawBox(0.55f, 0.035f, 0.11f);
    glPopMatrix();

    // Fallen wood splinter chunk
    glPushMatrix();
    glTranslatef(1.70f, 0.03f, 0.60f);
    glRotatef(75.0f, 0.0f, 1.0f, 0.0f);
    drawBox(0.38f, 0.03f, 0.08f);
    glPopMatrix();

    // Base stones & pebbles around fence posts
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glPushMatrix();
    glTranslatef(-1.75f, 0.06f, 0.28f);
    drawBox(0.28f, 0.12f, 0.22f);
    glTranslatef(1.10f, -0.02f, 0.20f);
    drawBox(0.20f, 0.09f, 0.18f);
    glTranslatef(1.45f, 0.03f, 0.15f);
    drawBox(0.32f, 0.14f, 0.24f);
    glPopMatrix();

    glPopMatrix();
}

// ----------------------------------------------------------------------------
// BROKEN WOODEN FENCE MASTER FUNCTION
// ----------------------------------------------------------------------------
void drawBrokenFence() {
    // 1. Featured Detailed Broken Fence Section (Foreground / Path flank - Image 14)
    drawDetailedBrokenFenceSection(-6.2f, 15.2f, -16.0f);

    // 2. Secondary Broken Fence Section along Entrance Approach
    drawDetailedBrokenFenceSection(7.8f, 17.2f, 24.0f);

    // 3. Left perimeter fence line (Along property boundary x ≈ -13.5)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.26f, 0.22f, 0.18f, 1.0f);

    for (int i = 0; i < 9; ++i) {
        float z = 19.5f - i * 2.3f;
        float x = -13.5f + (float)(i % 2) * 0.25f;
        float y = getTerrainHeight(x, z);
        float rot = std::sin((float)i * 1.6f) * 11.0f;

        glPushMatrix();
        glTranslatef(x, y + 0.70f, z);
        glRotatef(rot, 0.0f, 0.0f, 1.0f);
        // Post
        drawBox(0.14f, 1.50f, 0.14f, 0.5f, 1.0f);

        // Horizontal crossboards connecting posts (some broken / missing / sagging)
        if (i < 8 && i != 3 && i != 6) {
            glPushMatrix();
            glTranslatef(0.0f, 0.28f, -1.15f);
            if (i == 4) glRotatef(19.0f, 1.0f, 0.0f, 0.0f); // Sagging broken board
            drawBox(0.07f, 0.13f, 2.3f, 0.5f, 2.0f);
            glTranslatef(0.0f, -0.55f, 0.0f);
            if (i == 1) glRotatef(-14.0f, 1.0f, 0.0f, 0.0f);
            drawBox(0.07f, 0.13f, 2.3f, 0.5f, 2.0f);
            glPopMatrix();
        }
        glPopMatrix();
    }

    // 4. Right perimeter fence line (Near cemetery & car, Along x ≈ 12.5)
    for (int i = 0; i < 7; ++i) {
        float z = 18.0f - i * 2.3f;
        float x = 12.5f + (float)(i % 2) * 0.20f;
        float y = getTerrainHeight(x, z);
        float rot = std::cos((float)i * 1.8f) * 9.0f;

        glPushMatrix();
        glTranslatef(x, y + 0.65f, z);
        glRotatef(rot, 0.0f, 0.0f, 1.0f);
        drawBox(0.14f, 1.40f, 0.14f, 0.5f, 1.0f);

        if (i < 6 && i != 2 && i != 5) {
            glPushMatrix();
            glTranslatef(0.0f, 0.25f, -1.15f);
            if (i == 1) glRotatef(-16.0f, 1.0f, 0.0f, 0.0f);
            drawBox(0.07f, 0.13f, 2.3f, 0.5f, 2.0f);
            glTranslatef(0.0f, -0.50f, 0.0f);
            drawBox(0.07f, 0.13f, 2.3f, 0.5f, 2.0f);
            glPopMatrix();
        }
        glPopMatrix();
    }
}

// ----------------------------------------------------------------------------
// SCATTERED LOW-POLY ROCKS (Reference Panel 4)
// ----------------------------------------------------------------------------
void drawScatteredRocks() {
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);

    struct RockData {
        float x, z;
        float rx, ry, rz;
        float rotY, rotX;
        unsigned int seed;
    };
    static const RockData ROCKS[10] = {
        { -2.8f, 17.5f, 0.38f, 0.28f, 0.35f,  25.0f, 12.0f, 501 }, // Left path entrance
        {  3.2f, 17.0f, 0.42f, 0.32f, 0.38f, -40.0f,  8.0f, 502 }, // Right path entrance
        { -5.5f, 13.5f, 0.65f, 0.48f, 0.58f,  15.0f, 18.0f, 503 }, // Left knoll boulder
        {  6.2f, 13.8f, 0.55f, 0.40f, 0.50f,  65.0f, -10.0f, 504 }, // Right midground
        { -1.8f,  9.5f, 0.30f, 0.22f, 0.28f, -20.0f,  5.0f, 505 }, // Along path
        {  2.5f,  8.5f, 0.35f, 0.26f, 0.32f,  30.0f, 14.0f, 506 }, // Along path
        {  6.5f,  6.8f, 0.52f, 0.38f, 0.48f, -50.0f,  9.0f, 507 }, // Beside car
        {  9.8f,  5.2f, 0.44f, 0.30f, 0.40f,  10.0f, 12.0f, 508 }, // Behind car
        { -7.8f,  6.0f, 0.48f, 0.35f, 0.42f,  45.0f, -8.0f, 509 }, // Near porch
        { -12.0f, 9.0f, 0.60f, 0.45f, 0.55f, -30.0f, 15.0f, 510 }  // Left fence
    };

    for (int i = 0; i < 10; ++i) {
        const auto& r = ROCKS[i];
        drawIrregularRock(r.x, r.z, r.rx, r.ry, r.rz, r.rotY, r.rotX, r.seed, 0.55f);
    }
}

// Complete Environmental Clutter & Props Master Function
void drawEnvironmentalClutter() {
    // 1. Scattered Low-Poly Rocks
    drawScatteredRocks();

    // 2. Broken Wooden Crates
    drawBrokenCrate(-6.2f, 11.5f, 22.0f, 6.0f);
    drawBrokenCrate(-8.2f + g_houseShiftX, 4.2f + g_houseShiftZ, -35.0f, 4.0f);
    drawBrokenCrate( 9.5f, 6.5f, 45.0f, 5.0f);

    // 3. Old Wooden Barrels
    drawOldBarrel(-4.8f, 5.2f, -15.0f, 0.0f);  // Upright near porch
    drawOldBarrel( 8.8f, 9.2f,  48.0f, 72.0f); // Tilted on side in mud near car
    drawOldBarrel(-7.5f + g_houseShiftX, 4.0f + g_houseShiftZ, 20.0f, 0.0f);

    // 4. Fallen Fence Planks
    drawFallenPlank(-13.5f, 18.0f,  35.0f,  4.0f);
    drawFallenPlank(-13.8f, 10.5f, -50.0f, -3.0f);
    drawFallenPlank(-14.2f,  2.0f,  20.0f,  5.0f);

    // 5. Rusty Metal Bucket
    drawRustyBucket(-3.5f, 8.8f, 30.0f, 24.0f);

    // 6. Scattered Clay Bricks
    drawScatteredBricks();

    // 7. Tangled Bare Dead Bushes
    drawDeadBush(-12.5f, 12.0f, 1.10f,  15.0f, 901); // Near fence
    drawDeadBush(  8.2f, 16.5f, 1.20f, -35.0f, 902); // Near cemetery
    drawDeadBush( -7.5f, 19.5f, 0.90f,  45.0f, 903); // Near road entrance
    drawDeadBush( 12.0f, 11.0f, 1.00f, -60.0f, 904); // Behind car

    // 8. Leaning Old Wrought Iron Lamppost
    drawLeaningLamppost(-3.8f, 19.2f, 25.0f, 11.5f);

    // 9. Sign Board & Telephone Pole & Broken Fence & Furniture
    drawSignBoard(-4.6f, 16.5f, 18.0f);
    drawTelephonePole(10.8f, 5.8f, -12.0f);
    drawBrokenFence();
    drawOldFurniture();
}

// Tasteful 3D Wild Grass Clumps along distant yard boundaries (zero clutter on pathway)
void drawDenseGrassField() {
    applyMaterial(MAT_DEAD_GRASS);
    bindTexture(TEX_NONE);

    TreeRNG rng(7721);
    for (int i = 0; i < 320; ++i) {
        float gx = rng.nextFloat(-25.0f, 25.0f);
        float gz = rng.nextFloat(-22.0f, 24.0f);

        // Keep cobblestone walkway clear while flanking path edges with overgrown tufts
        if (gz >= -6.0f && gz <= 25.0f) {
            float roadX = getRoadCenterX(gz);
            float pathHalfW = (3.4f * (1.0f - 0.45f * ((23.5f - gz) / 27.0f))) * 0.5f;
            if (std::abs(gx - roadX) < (pathHalfW + 0.15f)) continue;
        }

        // Avoid house footprint
        if (gx >= -12.0f && gx <= 5.0f && gz >= -15.0f && gz <= -3.0f) continue;

        float gw = rng.nextFloat(0.35f, 0.65f);
        float gh = rng.nextFloat(0.30f, 0.60f);
        float rot = rng.nextFloat(0.0f, 180.0f);

        drawGrassTuft(gx, gz, gw, gh, rot);
    }
}

// Scatter dead grass tufts
void drawGroundProps() {
    drawDenseGrassField();
}

// 1. Terrain & Wet Cobblestone Pathway leading to house porch
void drawGround() {
    applyMaterial(MAT_WET_GROUND);
    bindTexture(TEX_GROUND);

    int gridSize = 64;
    float halfDim = 65.0f;
    float step = (2.0f * halfDim) / gridSize;
    float tileScale = 0.22f;

    glBegin(GL_QUADS);
    for (int i = 0; i < gridSize; ++i) {
        float z0 = -halfDim + i * step;
        float z1 = z0 + step;
        for (int j = 0; j < gridSize; ++j) {
            float x0 = -halfDim + j * step;
            float x1 = x0 + step;

            // Height and analytical surface normals
            float y00 = getTerrainHeight(x0, z0);
            float y10 = getTerrainHeight(x1, z0);
            float y11 = getTerrainHeight(x1, z1);
            float y01 = getTerrainHeight(x0, z1);

            float nx00, ny00, nz00; getTerrainNormal(x0, z0, nx00, ny00, nz00);
            float nx10, ny10, nz10; getTerrainNormal(x1, z0, nx10, ny10, nz10);
            float nx11, ny11, nz11; getTerrainNormal(x1, z1, nx11, ny11, nz11);
            float nx01, ny01, nz01; getTerrainNormal(x0, z1, nx01, ny01, nz01);

            // Ground Contact Darkening / Analytical Ambient Occlusion & Mud Wetness
            auto calcGroundOcclusion = [](float px, float pz) {
                float ao = 1.0f;

                // House Main Foundation Perimeter AO
                float dxMain = std::max(0.0f, std::max((-9.75f + g_houseShiftX) - px, px - (7.75f + g_houseShiftX)));
                float dzMain = std::max(0.0f, std::max((-7.75f + g_houseShiftZ) - pz, pz - (5.75f + g_houseShiftZ)));
                float dHouse = std::sqrt(dxMain * dxMain + dzMain * dzMain);
                if (dHouse < 2.2f) {
                    float houseAO = 0.48f + 0.52f * std::pow(dHouse / 2.2f, 0.70f);
                    ao = std::min(ao, houseAO);
                }

                // Porch Perimeter AO
                float dxPorch = std::max(0.0f, std::max((-6.2f + g_houseShiftX) - px, px - (0.6f + g_houseShiftX)));
                float dzPorch = std::max(0.0f, std::max((3.2f + g_houseShiftZ) - pz, pz - (7.4f + g_houseShiftZ)));
                float dPorch = std::sqrt(dxPorch * dxPorch + dzPorch * dzPorch);
                if (dPorch < 1.4f) {
                    float porchAO = 0.52f + 0.48f * (dPorch / 1.4f);
                    ao = std::min(ao, porchAO);
                }

                return ao;
            };

            float w00 = calcGroundOcclusion(x0, z0);
            float w10 = calcGroundOcclusion(x1, z0);
            float w11 = calcGroundOcclusion(x1, z1);
            float w01 = calcGroundOcclusion(x0, z1);

            // V00
            glNormal3f(nx00, ny00, nz00);
            glColor4f(w00, w00, w00 * 1.04f, 1.0f);
            glTexCoord2f(x0 * tileScale, z0 * tileScale);
            glVertex3f(x0, y00, z0);

            // V10
            glNormal3f(nx10, ny10, nz10);
            glColor4f(w10, w10, w10 * 1.04f, 1.0f);
            glTexCoord2f(x1 * tileScale, z0 * tileScale);
            glVertex3f(x1, y10, z0);

            // V11
            glNormal3f(nx11, ny11, nz11);
            glColor4f(w11, w11, w11 * 1.04f, 1.0f);
            glTexCoord2f(x1 * tileScale, z1 * tileScale);
            glVertex3f(x1, y11, z1);

            // V01
            glNormal3f(nx01, ny01, nz01);
            glColor4f(w01, w01, w01 * 1.04f, 1.0f);
            glTexCoord2f(x0 * tileScale, z1 * tileScale);
            glVertex3f(x0, y01, z1);
        }
    }
    glEnd();

    // Reset base vertex color
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Old Irregular Wet Cobblestone / Flagstone Pathway (Curving from foreground to mansion porch)
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);

    int numRows = 44;
    TreeRNG pathRng(4411);
    for (int i = 0; i < numRows; ++i) {
        float t = (float)i / (float)(numRows - 1);
        float pz = 23.5f - t * 27.0f; // from 23.5 down to -3.5 (porch steps)
        float roadX = getRoadCenterX(pz);
        float pathW = 3.4f * (1.0f - 0.45f * t); // starts ~3.4m wide, converges to ~1.85m
        int stonesInRow = (i % 2 == 0) ? 3 : 4;
        float stoneW = pathW / (float)stonesInRow;

        for (int s = 0; s < stonesInRow; ++s) {
            float offsetU = -pathW * 0.5f + (s + 0.5f) * stoneW + pathRng.nextFloat(-0.06f, 0.06f);
            float sx = roadX + offsetU;
            float sz = pz + pathRng.nextFloat(-0.07f, 0.07f);
            float sy = getTerrainHeight(sx, sz) + 0.035f + pathRng.nextFloat(0.0f, 0.025f);
            float slen = 0.56f + pathRng.nextFloat(-0.05f, 0.06f);
            float swid = stoneW * 0.90f + pathRng.nextFloat(-0.04f, 0.04f);
            float srot = pathRng.nextFloat(-8.0f, 8.0f) - 14.0f * t;

            glPushMatrix();
            glTranslatef(sx, sy, sz);
            glRotatef(srot, 0.0f, 1.0f, 0.0f);
            drawBeveledBox(swid, 0.06f, slen, 0.025f, 1.0f, 1.0f);
            glPopMatrix();
        }
    }

    // Ground details
    drawGroundProps();
}

// 2. Carved Jack-o'-Lantern Pumpkins (Exact Reference Styling & Full Visibility)
void drawPumpkin(float x, float y, float z, float scale, float rotY, int faceStyle) {
    float groundY = y + getTerrainHeight(x, z);

    glPushMatrix();
    // Position body firmly on the ground surface
    glTranslatef(x, groundY + 0.40f * scale, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glScalef(scale, scale * 0.90f, scale);

    // 1. Ribbed Pumpkin Body (Segmented vertical lobes for authentic organic shape)
    glDisable(GL_CULL_FACE);
    applyMaterial(MAT_PUMPKIN_SKIN);
    bindTexture(TEX_NONE);
    glColor4f(0.95f, 0.46f, 0.12f, 1.0f);

    int numLobes = 10;
    for (int l = 0; l < numLobes; ++l) {
        float lobeAngle = (float)l * (360.0f / numLobes);
        glPushMatrix();
        glRotatef(lobeAngle, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.18f, 0.0f, 0.0f);
        glScalef(0.48f, 0.52f, 0.45f);
        drawSphere(1.0f, 12, 10);
        glPopMatrix();
    }

    // Central pumpkin core to seal body
    glPushMatrix();
    glScalef(0.60f, 0.50f, 0.60f);
    drawSphere(1.0f, 16, 12);
    glPopMatrix();

    // 2. Curled Pumpkin Stem / Stalk on Top
    applyMaterial(MAT_BARK);
    bindTexture(TEX_BARK);
    glPushMatrix();
    glTranslatef(0.0f, 0.48f, 0.0f);
    glRotatef(22.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.065f, 0.035f, 0.28f, 8, 1.0f, 1.0f);
    glPopMatrix();

    // 3. Glowing Carved Face (Eyes, Nose, Sinister Jagged Mouth - Strongly illuminated)
    glDisable(GL_LIGHTING);
    bindTexture(TEX_NONE);
    glColor4f(1.0f, 0.75f, 0.20f, 1.0f);

    if (faceStyle == 0) {
        // Classic jagged wicked grin
        glBegin(GL_TRIANGLES);
        // Left Eye (triangular slanted)
        glNormal3f(-0.25f, 0.2f, 0.95f);
        glVertex3f(-0.24f, 0.12f, 0.58f);
        glVertex3f(-0.06f, 0.16f, 0.63f);
        glVertex3f(-0.16f, 0.30f, 0.56f);

        // Right Eye
        glNormal3f(0.25f, 0.2f, 0.95f);
        glVertex3f( 0.06f, 0.16f, 0.63f);
        glVertex3f( 0.24f, 0.12f, 0.58f);
        glVertex3f( 0.16f, 0.30f, 0.56f);

        // Nose
        glNormal3f(0.0f, 0.1f, 1.0f);
        glVertex3f(-0.06f, 0.02f, 0.66f);
        glVertex3f( 0.06f, 0.02f, 0.66f);
        glVertex3f( 0.00f, 0.12f, 0.65f);
        glEnd();

        // Sinister toothy grin
        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0.0f, -0.2f, 0.98f);
        glVertex3f( 0.00f, -0.10f, 0.65f);
        glVertex3f(-0.38f, -0.01f, 0.46f);
        glVertex3f(-0.28f, -0.16f, 0.58f);
        glVertex3f(-0.18f, -0.05f, 0.63f);
        glVertex3f(-0.09f, -0.18f, 0.65f);
        glVertex3f( 0.00f, -0.06f, 0.66f);
        glVertex3f( 0.09f, -0.18f, 0.65f);
        glVertex3f( 0.18f, -0.05f, 0.63f);
        glVertex3f( 0.28f, -0.16f, 0.58f);
        glVertex3f( 0.38f, -0.01f, 0.46f);
        glEnd();
    } else {
        // Angled menacing grin
        glBegin(GL_TRIANGLES);
        // Slanted eyes
        glNormal3f(-0.35f, 0.2f, 0.93f);
        glVertex3f(-0.22f, 0.14f, 0.59f);
        glVertex3f(-0.08f, 0.20f, 0.63f);
        glVertex3f(-0.20f, 0.28f, 0.57f);

        glNormal3f(0.35f, 0.2f, 0.93f);
        glVertex3f( 0.08f, 0.20f, 0.63f);
        glVertex3f( 0.22f, 0.14f, 0.59f);
        glVertex3f( 0.20f, 0.28f, 0.57f);

        // Nose
        glNormal3f(0.0f, 0.1f, 1.0f);
        glVertex3f(-0.05f, 0.04f, 0.66f);
        glVertex3f( 0.05f, 0.04f, 0.66f);
        glVertex3f( 0.00f, 0.13f, 0.65f);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0.0f, -0.2f, 0.98f);
        glVertex3f( 0.00f, -0.12f, 0.65f);
        glVertex3f(-0.32f, -0.02f, 0.54f);
        glVertex3f(-0.22f, -0.15f, 0.60f);
        glVertex3f(-0.11f, -0.07f, 0.64f);
        glVertex3f( 0.00f, -0.16f, 0.65f);
        glVertex3f( 0.11f, -0.07f, 0.64f);
        glVertex3f( 0.22f, -0.15f, 0.60f);
        glVertex3f( 0.32f, -0.02f, 0.54f);
        glEnd();
    }
    glEnable(GL_LIGHTING);

    // 4. Localized Warm Ground Light Halo under Pumpkin
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 0.60f, 0.12f, 0.28f * g_pumpkinFlicker);
    glVertex3f(0.0f, -0.38f, 0.2f);
    int haloSegs = 14;
    float haloR = 0.95f;
    for (int h = 0; h <= haloSegs; ++h) {
        float ang = (float)h * (2.0f * (float)M_PI / (float)haloSegs);
        glColor4f(1.0f, 0.45f, 0.05f, 0.0f);
        glVertex3f(haloR * std::cos(ang), -0.38f, 0.2f + haloR * std::sin(ang) * 0.8f);
    }
    glEnd();
    glPopAttrib();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// Master Array of Halloween Jack-o'-Lanterns (Tastefully placed at porch corners & fence)
void drawPumpkinArray() {
    // Subtle, atmospheric pumpkins on porch deck corners & near sign
    drawPumpkin(-6.8f, 0.40f, 5.6f, 0.42f,  35.0f, 0); // Left porch corner
    drawPumpkin(-2.4f, 0.40f, 5.6f, 0.38f, -25.0f, 1); // Right porch corner
    drawPumpkin(-5.5f, 0.00f, 16.2f, 0.45f, 18.0f, 0); // Beside Abandoned House sign
}

// ----------------------------------------------------------------------------
// 3. NATURAL ORGANIC TREES (Small, Compact, Lush Deciduous & Conifer Evergreens)
// ----------------------------------------------------------------------------

// Volumetric organic multi-lobed foliage clump with soft leaf accents
void drawNaturalFoliageCluster(float radius, TreeRNG& rng, int foliageType = 0) {
    if (foliageType == 1) {
        applyMaterial(MAT_FOLIAGE_DARK);
    } else if (foliageType == 2) {
        applyMaterial(MAT_FOLIAGE_AUTUMN);
    } else {
        applyMaterial(MAT_FOLIAGE_LUSH);
    }
    bindTexture(TEX_NONE);

    // Central primary foliage mass (smooth shaded)
    drawSphere(radius * 0.85f, 14, 10);

    // 5 interlocking satellite lobes around the perimeter for rich organic silhouettes
    for (int i = 0; i < 5; ++i) {
        float ang = (float)i * (2.0f * (float)M_PI / 5.0f) + rng.nextFloat(-0.25f, 0.25f);
        float elev = rng.nextFloat(-0.18f, 0.25f);
        float dist = radius * rng.nextFloat(0.32f, 0.48f);
        float rSub = radius * rng.nextFloat(0.55f, 0.70f);

        glPushMatrix();
        glTranslatef(dist * std::cos(ang), dist * elev, dist * std::sin(ang));
        drawSphere(rSub, 12, 8);
        glPopMatrix();
    }

    // Outer subtle leaf accents to soften silhouette edges
    int leafCards = 6;
    for (int l = 0; l < leafCards; ++l) {
        float la = (float)l * (2.0f * (float)M_PI / (float)leafCards) + rng.nextFloat(-0.3f, 0.3f);
        float ld = radius * rng.nextFloat(0.70f, 0.92f);
        float ly = radius * rng.nextFloat(-0.22f, 0.40f);
        float ls = radius * rng.nextFloat(0.18f, 0.28f);

        glPushMatrix();
        glTranslatef(ld * std::cos(la), ly, ld * std::sin(la));
        glRotatef(rng.nextFloat(0.0f, 360.0f), 0.0f, 1.0f, 0.0f);
        glRotatef(rng.nextFloat(-25.0f, 25.0f), 1.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLES);
        glNormal3f(0.0f, 0.8f, 0.6f);
        glVertex3f(0.0f, 0.0f, -ls * 0.5f);
        glVertex3f(-ls * 0.4f, 0.0f, ls * 0.2f);
        glVertex3f(ls * 0.4f, ls * 0.2f, ls * 0.4f);
        glEnd();
        glPopMatrix();
    }

    // Restore bark material & texture
    applyMaterial(MAT_BARK);
    bindTexture(TEX_BARK);
}

// Flaring root base anchoring trunk into terrain
void drawRootFlare(float trunkR, TreeRNG& rng) {
    int numRoots = 5;
    for (int i = 0; i < numRoots; ++i) {
        float angle = (float)i * (360.0f / (float)numRoots) + rng.nextFloat(-15.0f, 15.0f);
        glPushMatrix();
        glRotatef(angle, 0.0f, 1.0f, 0.0f);
        glTranslatef(trunkR * 0.30f, 0.03f, 0.0f);

        glRotatef(rng.nextFloat(55.0f, 68.0f), 0.0f, 0.0f, 1.0f);
        float r1Len = trunkR * rng.nextFloat(1.1f, 1.4f);
        drawCylinder(trunkR * 0.45f, trunkR * 0.22f, r1Len, 6, 1.0f, 1.0f);

        glTranslatef(0.0f, r1Len, 0.0f);
        glRotatef(rng.nextFloat(22.0f, 35.0f), 0.0f, 0.0f, 1.0f);
        float r2Len = trunkR * rng.nextFloat(1.1f, 1.5f);
        drawCylinder(trunkR * 0.22f, trunkR * 0.06f, r2Len, 5, 1.0f, 1.0f);

        glPopMatrix();
    }
}

// 1. Natural Organic Deciduous Tree (Small, compact, beautiful leafy canopy, 100% fully visible)
void drawNaturalDeciduousTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed, int foliageType = 0, float colorTint = 1.0f) {
    TreeRNG rng(seed);

    float groundY = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, groundY, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // Natural gentle wind sway in the night breeze
    float treeWindPhase = g_time * 1.15f + (float)(seed % 100) * 0.08f;
    float swayAmp = 0.55f;
    float swayX = std::sin(treeWindPhase) * swayAmp;
    float swayZ = std::cos(treeWindPhase * 0.9f) * (swayAmp * 0.6f);
    glRotatef(swayX * 0.35f, 1.0f, 0.0f, 0.0f);
    glRotatef(swayZ * 0.35f, 0.0f, 0.0f, 1.0f);

    // Bark material with tone variations
    Material treeMat = MAT_BARK;
    treeMat.diffuse[0] *= colorTint;
    treeMat.diffuse[1] *= colorTint * 0.96f;
    treeMat.diffuse[2] *= colorTint * 0.92f;
    applyMaterial(treeMat);
    bindTexture(TEX_BARK);

    // Flared roots into terrain
    drawRootFlare(trunkRadius, rng);

    // Trunk segments with natural curvature
    float seg1H = height * 0.16f;
    float seg2H = height * 0.16f;
    float seg3H = height * 0.14f;

    float r0 = trunkRadius;
    float r1 = trunkRadius * 0.85f;
    float r2 = trunkRadius * 0.70f;
    float r3 = trunkRadius * 0.55f;

    // Segment 1 (Base)
    drawCylinder(r0, r1, seg1H, 8, 1.0f, 1.0f);
    glTranslatef(0.0f, seg1H, 0.0f);

    // Segment 2 (Mid trunk with slight bend)
    float bend1X = rng.nextFloat(-3.5f, 3.5f);
    float bend1Z = rng.nextFloat(-3.5f, 3.5f);
    glRotatef(bend1X, 1.0f, 0.0f, 0.0f);
    glRotatef(bend1Z, 0.0f, 0.0f, 1.0f);
    drawCylinder(r1, r2, seg2H, 8, 1.0f, 1.0f);
    glTranslatef(0.0f, seg2H, 0.0f);

    // Segment 3 (Upper trunk heading into main fork)
    float bend2X = rng.nextFloat(-4.0f, 4.0f);
    float bend2Z = rng.nextFloat(-4.0f, 4.0f);
    glRotatef(bend2X, 1.0f, 0.0f, 0.0f);
    glRotatef(bend2Z, 0.0f, 0.0f, 1.0f);
    drawCylinder(r2, r3, seg3H, 7, 1.0f, 1.0f);
    glTranslatef(0.0f, seg3H, 0.0f);

    // Primary branch boughs (3 main natural limbs spreading outward and upward)
    int numBoughs = 3 + (int)(rng.nextFloat(0.0f, 1.9f));
    for (int b = 0; b < numBoughs; ++b) {
        float azimuth = (float)b * (360.0f / (float)numBoughs) + rng.nextFloat(-18.0f, 18.0f);
        float pitch = rng.nextFloat(30.0f, 45.0f);
        float boughLen = height * rng.nextFloat(0.26f, 0.35f);
        float boughBaseR = r3 * rng.nextFloat(0.65f, 0.80f);
        float boughTopR = boughBaseR * 0.40f;

        glPushMatrix();
        glRotatef(azimuth, 0.0f, 1.0f, 0.0f);
        glRotatef(pitch, 1.0f, 0.0f, 0.0f);

        // Branch wind motion
        glRotatef(swayX * (0.8f + (float)b * 0.2f), 1.0f, 0.0f, 0.0f);

        drawCylinder(boughBaseR, boughTopR, boughLen, 6, 1.0f, 1.0f);
        glTranslatef(0.0f, boughLen, 0.0f);

        // Sub-branches
        int numTwigs = 2;
        for (int t = 0; t < numTwigs; ++t) {
            float twigAz = (float)t * 180.0f + rng.nextFloat(-25.0f, 25.0f);
            float twigPitch = rng.nextFloat(22.0f, 38.0f);
            float twigLen = boughLen * rng.nextFloat(0.40f, 0.60f);

            glPushMatrix();
            glRotatef(twigAz, 0.0f, 1.0f, 0.0f);
            glRotatef(twigPitch, 1.0f, 0.0f, 0.0f);
            drawCylinder(boughTopR * 0.70f, 0.012f, twigLen, 4, 1.0f, 1.0f);
            glTranslatef(0.0f, twigLen, 0.0f);

            // Foliage cluster at tip of sub-branch
            drawNaturalFoliageCluster(height * rng.nextFloat(0.16f, 0.22f), rng, foliageType);
            glPopMatrix();
        }

        // Main bough foliage cluster
        drawNaturalFoliageCluster(height * rng.nextFloat(0.20f, 0.26f), rng, foliageType);
        glPopMatrix();
    }

    // Central crown top foliage dome
    glPushMatrix();
    glTranslatef(0.0f, height * 0.06f, 0.0f);
    drawNaturalFoliageCluster(height * 0.28f, rng, foliageType);
    glPopMatrix();

    glPopMatrix();
}

// 2. Natural Evergreen Conifer / Pine Tree (Small, elegant, tiered needle foliage, 100% fully visible)
void drawNaturalPineTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed) {
    TreeRNG rng(seed);

    float groundY = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, groundY, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // Subtle pine tree wind sway
    float treeWindPhase = g_time * 1.10f + (float)(seed % 100) * 0.08f;
    float swayAmp = 0.45f;
    float swayX = std::sin(treeWindPhase) * swayAmp;
    float swayZ = std::cos(treeWindPhase * 0.85f) * (swayAmp * 0.5f);
    glRotatef(swayX * 0.25f, 1.0f, 0.0f, 0.0f);
    glRotatef(swayZ * 0.25f, 0.0f, 0.0f, 1.0f);

    // Bark material & trunk
    applyMaterial(MAT_BARK);
    bindTexture(TEX_BARK);
    drawRootFlare(trunkRadius, rng);

    // Tapered trunk to tree top
    drawCylinder(trunkRadius, trunkRadius * 0.15f, height * 0.92f, 8, 1.0f, 2.0f);

    // Tiered conical evergreen needle foliage layers
    applyMaterial(MAT_PINE_NEEDLES);
    bindTexture(TEX_NONE);

    struct PineTier {
        float yFrac;
        float baseRadiusFrac;
        float coneHeightFrac;
    };
    static const PineTier TIERS[5] = {
        { 0.26f, 0.34f, 0.24f },
        { 0.42f, 0.28f, 0.22f },
        { 0.56f, 0.22f, 0.20f },
        { 0.70f, 0.16f, 0.18f },
        { 0.82f, 0.11f, 0.16f }
    };

    for (int t = 0; t < 5; ++t) {
        float tierY = height * TIERS[t].yFrac;
        float tierBaseR = height * TIERS[t].baseRadiusFrac;
        float tierConeH = height * TIERS[t].coneHeightFrac;

        glPushMatrix();
        glTranslatef(0.0f, tierY, 0.0f);

        // Wind flex on higher tiers
        float tierFlex = (float)(t + 1) * 0.18f;
        glRotatef(swayX * tierFlex, 1.0f, 0.0f, 0.0f);
        glRotatef(swayZ * tierFlex, 0.0f, 0.0f, 1.0f);

        // Conical foliage tier
        drawCylinder(tierBaseR, 0.005f, tierConeH, 10, 1.0f, 1.0f);

        // Scalloped undergrowth needles
        int subNeedles = 6;
        for (int n = 0; n < subNeedles; ++n) {
            float ang = (float)n * (360.0f / (float)subNeedles) + rng.nextFloat(-10.0f, 10.0f);
            glPushMatrix();
            glRotatef(ang, 0.0f, 1.0f, 0.0f);
            glTranslatef(tierBaseR * 0.60f, 0.02f, 0.0f);
            glRotatef(rng.nextFloat(40.0f, 55.0f), 0.0f, 0.0f, 1.0f);
            drawSphere(tierBaseR * 0.20f, 8, 5);
            glPopMatrix();
        }

        glPopMatrix();
    }

    // Reset material
    applyMaterial(MAT_BARK);
    bindTexture(TEX_BARK);

    glPopMatrix();
}

// Compatibility wrapper for any legacy organic tree calls
void drawOrganicCreepyTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed, float colorTint = 1.0f) {
    (void)trunkRadius;
    drawNaturalDeciduousTree(x, z, 0.16f, std::min(height, 4.2f), rotY, seed, (seed % 2), colorTint);
}

// Compatibility wrapper for gothic foreground tree
void drawGothicForegroundTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed) {
    (void)trunkRadius; (void)height;
    drawNaturalDeciduousTree(x, z, 0.16f, 3.8f, rotY, seed, 0, 1.0f);
}

// Scatter Natural Organic Trees across the entire landscape (Small, natural, 100% fully visible)
void drawAllTrees() {
    // 1. FRONT YARD & ENTRANCE TREES (Framing the entrance knolls, small and completely visible in full)
    drawNaturalDeciduousTree(-4.8f, 15.5f, 0.15f, 3.4f,  25.0f, 1001, 0, 0.95f); // Front Left Lawn (In full view)
    drawNaturalPineTree(     13.5f, 14.5f, 0.16f, 3.8f,  40.0f, 1002);         // Far Right Entrance Flank
    drawNaturalDeciduousTree(15.5f, 16.5f, 0.14f, 3.4f, -30.0f, 1003, 1, 0.90f); // Cemetery Far Border

    // 2. MIDGROUND YARD & GRAVEYARD TREES
    drawNaturalDeciduousTree( -7.8f, 12.5f, 0.15f, 3.4f, -15.0f, 1004, 1, 0.92f); // Graveyard Knoll
    drawNaturalPineTree(     -11.5f, 10.0f, 0.16f, 3.7f,  30.0f, 1005);         // Left Fence Line
    drawNaturalDeciduousTree(-13.5f,  5.5f, 0.17f, 3.9f,  48.0f, 1006, 0, 0.94f); // Left Manor Approach
    drawNaturalDeciduousTree( 13.8f,  7.0f, 0.16f, 3.9f, -42.0f, 1007, 0, 0.95f); // Right Lawn behind car
    drawNaturalPineTree(      16.0f,  9.5f, 0.16f, 3.8f,  55.0f, 1008);         // Far Right Manor Approach

    // 3. MANOR FLANKING TREES (Flanking the house on left and right)
    drawNaturalDeciduousTree(-15.0f, -3.5f, 0.17f, 4.1f,  12.0f, 2001, 0, 0.90f); // Left House Flank
    drawNaturalPineTree(      13.5f, -3.0f, 0.17f, 4.1f, -20.0f, 2002);         // Right House Flank

    // 4. BACKGROUND FOREST HORIZON (Small natural trees on the ridge behind the mansion, creating a dense natural backdrop)
    drawNaturalPineTree(     -12.5f, -14.0f, 0.16f, 4.0f,  15.0f, 3001);
    drawNaturalDeciduousTree( -8.0f, -16.0f, 0.16f, 3.8f, -22.0f, 3002, 1, 0.88f);
    drawNaturalPineTree(      -4.0f, -17.5f, 0.17f, 4.0f,  45.0f, 3003);
    drawNaturalDeciduousTree(  0.0f, -18.5f, 0.17f, 3.9f, -28.0f, 3004, 0, 0.88f);
    drawNaturalPineTree(       4.0f, -17.5f, 0.17f, 4.0f,  35.0f, 3005);
    drawNaturalDeciduousTree(  8.0f, -16.0f, 0.16f, 3.8f, -15.0f, 3006, 1, 0.88f);
    drawNaturalPineTree(      12.5f, -14.0f, 0.16f, 4.0f,  60.0f, 3007);
}


// ============================================================================
// GOTHIC HAUNTED HOUSE ARCHITECTURE (MULTI-SECTION SILHOUETTE & LOCALIZED WINDOW GLOW)
// ============================================================================

// Helper to draw realistic Gothic windows with dark sills, casings, cross muntins, and warm amber glass
void drawHouseWindow(float x, float y, float z, float width, float height, bool hasArch = true, bool hasCrossMuntin = true) {
    (void)hasArch;
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    // 1. Dark outer wooden sill / casing
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.28f, 0.25f, 0.22f, 1.0f);
    glPushMatrix();
    glTranslatef(x, y - height * 0.5f, z + 0.04f);
    drawBox(width + 0.25f, 0.10f, 0.18f); // Sill ledge
    glTranslatef(0.0f, height + 0.05f, 0.0f);
    drawBox(width + 0.20f, 0.08f, 0.14f); // Top drip cap
    glPopMatrix();

    // 2. Glowing Amber Glass Pane with warm radiant center hotspot
    glDisable(GL_LIGHTING);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(x, y, z + 0.02f);

    // Outer warm amber / orange edge
    glColor4f(0.95f, 0.58f, 0.18f, 1.0f);
    drawBox(width, height, 0.04f);

    // Inner bright warm cream / yellow luminous core
    glColor4f(1.0f, 0.88f, 0.48f, 1.0f);
    drawBox(width * 0.72f, height * 0.72f, 0.045f);

    glPopMatrix();
    glEnable(GL_LIGHTING);

    // 3. Dark Wooden Cross Muntins / Glazing Bars
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.25f, 0.22f, 0.20f, 1.0f);
    if (hasCrossMuntin) {
        glPushMatrix();
        glTranslatef(x, y, z + 0.025f);
        drawBox(width + 0.02f, 0.06f, 0.06f); // Horizontal bar
        drawBox(0.06f, height + 0.02f, 0.06f); // Vertical bar
        glPopMatrix();
    }

    glPopAttrib();
}

// ----------------------------------------------------------------------------
// HAUNTED INTERIOR COBWEBS (Delicate Radial Web Lines)
// ----------------------------------------------------------------------------
void drawCobweb(float x, float y, float z, float size, float rotY = 0.0f) {
    bindTexture(TEX_NONE);
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    glColor4f(0.85f, 0.88f, 0.95f, 0.42f);
    glLineWidth(1.0f);

    int numRadials = 6;
    int numRings = 5;

    // Radial spokes
    glBegin(GL_LINES);
    for (int r = 0; r < numRadials; ++r) {
        float angle = (float)r * ((float)M_PI * 0.5f / (float)(numRadials - 1));
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(size * std::cos(angle), -size * std::sin(angle), 0.0f);
    }
    glEnd();

    // Concentric web swags
    glBegin(GL_LINE_STRIP);
    for (int ring = 1; ring <= numRings; ++ring) {
        float rDist = size * ((float)ring / (float)numRings);
        for (int r = 0; r < numRadials; ++r) {
            float angle = (float)r * ((float)M_PI * 0.5f / (float)(numRadials - 1));
            float sag = (r > 0 && r < numRadials - 1) ? 0.92f : 1.0f;
            glVertex3f(rDist * std::cos(angle) * sag, -rDist * std::sin(angle) * sag, 0.0f);
        }
    }
    glEnd();

    glPopMatrix();
    glPopAttrib();
}

// ----------------------------------------------------------------------------
// ETHEREAL HAUNTED GHOST APPARITION (Panel 2 - Floating Spectral Phantom)
// ----------------------------------------------------------------------------
void drawHauntedGhost(float x, float y, float z, float rotY = 0.0f) {
    float floatBob = 0.08f * std::sin(g_time * 2.2f);
    float swayX    = 0.05f * std::sin(g_time * 1.4f);
    float waveFolds = std::sin(g_time * 3.5f);

    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING); // Unlit emissive spectral glow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive luminous glow
    bindTexture(TEX_NONE);

    glPushMatrix();
    glTranslatef(x + swayX, y + floatBob, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // 1. Ethereal Spectral Aura (Outer soft glowing halo)
    float auraPulse = 0.30f + 0.12f * std::sin(g_time * 2.8f);
    glColor4f(0.25f, 0.70f, 1.0f, auraPulse);
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.0f);
    glScalef(0.70f, 1.15f, 0.70f);
    drawSphere(1.0f, 12, 8);
    glPopMatrix();

    // 2. Ghostly Hood / Head
    glColor4f(0.82f, 0.94f, 1.0f, 0.80f);
    glPushMatrix();
    glTranslatef(0.0f, 0.92f, 0.0f);
    drawSphere(0.25f, 14, 10);
    // Flowing cowl / cowl rim
    glTranslatef(0.0f, -0.06f, 0.04f);
    drawCylinder(0.25f, 0.30f, 0.18f, 10);
    glPopMatrix();

    // 3. Eerie Glowing Spectral Eyes (Intense cyan-white piercing gaze)
    glColor4f(0.35f, 1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.07f, 0.95f, 0.22f);
    drawSphere(0.040f, 8, 6);
    glTranslatef(0.14f, 0.0f, 0.0f);
    drawSphere(0.040f, 8, 6);
    glPopMatrix();

    // 4. Ghost Body / Tapered Flowing Shroud
    glColor4f(0.72f, 0.88f, 1.0f, 0.70f);
    glPushMatrix();
    glTranslatef(0.0f, 0.25f, 0.0f);
    // Upper torso
    drawCylinder(0.26f, 0.36f, 0.65f, 12);
    // Lower flowing drapery / wispy trailing vapor
    glTranslatef(0.0f, -0.38f, 0.0f);
    glRotatef(waveFolds * 6.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.36f, 0.12f, 0.42f, 10);
    glPopMatrix();

    // 5. Floating Spectral Arms (Reaching forward)
    glColor4f(0.68f, 0.85f, 0.98f, 0.60f);
    // Left arm
    glPushMatrix();
    glTranslatef(-0.28f, 0.68f, 0.10f);
    glRotatef(-30.0f + 6.0f * waveFolds, 0.0f, 0.0f, 1.0f);
    glRotatef( 35.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.065f, 0.030f, 0.45f, 8);
    glPopMatrix();
    // Right arm
    glPushMatrix();
    glTranslatef(0.28f, 0.68f, 0.10f);
    glRotatef(30.0f - 6.0f * waveFolds, 0.0f, 0.0f, 1.0f);
    glRotatef(35.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.065f, 0.030f, 0.45f, 8);
    glPopMatrix();

    glPopMatrix();
    glPopAttrib();
}

// ----------------------------------------------------------------------------
// HAUNTED HOUSE INTERIOR (Panel 2: Dilapidated Room, Ghost, Furniture, Candle, Stairs)
// ----------------------------------------------------------------------------
void drawHouseInterior() {
    // 1. Weathered, Peeling Floorboards
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.26f, 0.22f, 0.18f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.8f, 0.74f, 0.5f);
    drawBox(8.2f, 0.08f, 8.4f, 4.0f, 3.0f);
    glPopMatrix();

    // 2. Interior Walls (Weathered, water-stained rotting wallpaper)
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_WALL);
    glColor4f(0.32f, 0.30f, 0.28f, 1.0f);
    // Left wall inner face
    glPushMatrix();
    glTranslatef(-8.85f, 2.45f, 0.5f);
    drawBox(0.06f, 3.4f, 8.4f, 2.0f, 1.5f);
    glPopMatrix();
    // Right partition inner face
    glPushMatrix();
    glTranslatef(-0.75f, 2.45f, 0.5f);
    drawBox(0.06f, 3.4f, 8.4f, 2.0f, 1.5f);
    glPopMatrix();
    // Back wall inner face
    glPushMatrix();
    glTranslatef(-4.8f, 2.45f, -3.70f);
    drawBox(8.2f, 3.4f, 0.06f, 2.5f, 1.5f);
    glPopMatrix();

    // 3. Hallway Partition Wall & Gothic Doorway Opening (Back of room)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.20f, 0.17f, 1.0f);
    // Left hallway partition
    glPushMatrix();
    glTranslatef(-6.85f, 2.45f, -1.20f);
    drawBox(4.0f, 3.4f, 0.08f, 1.5f, 1.5f);
    glPopMatrix();
    // Right hallway partition
    glPushMatrix();
    glTranslatef(-2.45f, 2.45f, -1.20f);
    drawBox(3.3f, 3.4f, 0.08f, 1.5f, 1.5f);
    glPopMatrix();
    // Lintel above hallway door (high archway)
    glPushMatrix();
    glTranslatef(-4.6f, 3.85f, -1.20f);
    drawBox(1.8f, 0.65f, 0.08f);
    glPopMatrix();
    // Doorway trim casing
    glPushMatrix();
    glTranslatef(-4.6f, 2.2f, -1.16f);
    glPushMatrix(); glTranslatef(-0.85f, 0.0f, 0.0f); drawBox(0.10f, 3.1f, 0.10f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.85f, 0.0f, 0.0f); drawBox(0.10f, 3.1f, 0.10f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.0f, 1.55f, 0.0f); drawBox(1.8f, 0.10f, 0.10f); glPopMatrix();
    glPopMatrix();

    // 4. FLOATING SPECTRAL GHOST (In back hallway doorway)
    drawHauntedGhost(-4.6f, 1.35f, -2.5f, 0.0f);

    // 5. Heavy Exposed Dark Ceiling Timber Beams
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.20f, 0.17f, 0.14f, 1.0f);
    float beamZs[4] = { 3.5f, 1.5f, -0.5f, -2.5f };
    for (int b = 0; b < 4; ++b) {
        glPushMatrix();
        glTranslatef(-4.8f, 4.08f, beamZs[b]);
        drawBox(8.2f, 0.22f, 0.20f, 2.5f, 0.2f);
        glPopMatrix();
    }

    // 6. Interior Hanging Flickering Incandescent Bulb
    float intBulbFlicker = 0.85f + 0.15f * std::sin(g_time * 7.5f) * std::cos(g_time * 13.0f);
    glPushMatrix();
    glTranslatef(-4.6f, 4.16f, 1.7f);
    // Wire
    bindTexture(TEX_NONE);
    glDisable(GL_LIGHTING);
    glColor3f(0.12f, 0.12f, 0.12f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, -0.95f, 0.0f);
    glEnd();
    glEnable(GL_LIGHTING);
    // Socket
    glTranslatef(0.0f, -0.95f, 0.0f);
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    drawCylinder(0.045f, 0.04f, 0.08f, 8);
    // Bulb glass & glowing filament
    glTranslatef(0.0f, -0.06f, 0.0f);
    applyMaterial(MAT_BULB_EMISSIVE);
    bindTexture(TEX_NONE);
    glColor4f(1.0f, 0.82f, 0.35f, 1.0f);
    drawSphere(0.085f, 10, 8);
    glPopMatrix();

    // 7. Dilapidated Wooden Dining Table (Center Room)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.28f, 0.24f, 0.20f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.8f, 0.74f, 1.7f);
    // Tabletop
    glTranslatef(0.0f, 0.72f, 0.0f);
    drawBox(1.50f, 0.06f, 0.85f);
    // Apron
    glTranslatef(0.0f, -0.04f, 0.0f);
    drawBox(1.40f, 0.08f, 0.75f);
    // 4 Sturdy Legs
    glTranslatef(-0.65f, -0.34f, -0.32f); drawBox(0.07f, 0.68f, 0.07f);
    glTranslatef( 1.30f,  0.0f,   0.0f);  drawBox(0.07f, 0.68f, 0.07f);
    glTranslatef( 0.0f,   0.0f,   0.64f); drawBox(0.07f, 0.68f, 0.07f);
    glTranslatef(-1.30f,  0.0f,   0.0f);  drawBox(0.07f, 0.68f, 0.07f);
    glPopMatrix();

    // 8. Melting Wax Candle & Dusty Wine Bottle on Table
    glPushMatrix();
    glTranslatef(-4.55f, 1.50f, 1.55f);
    // Brass candle holder base
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_NONE);
    glColor4f(0.55f, 0.45f, 0.25f, 1.0f);
    drawCylinder(0.065f, 0.045f, 0.03f, 8);
    // White wax candle column with melted wax drips
    glColor4f(0.92f, 0.90f, 0.82f, 1.0f);
    glTranslatef(0.0f, 0.03f, 0.0f);
    drawCylinder(0.022f, 0.020f, 0.12f, 6);
    // Candle flame (flickering orange-yellow teardrop)
    glDisable(GL_LIGHTING);
    glTranslatef(0.0f, 0.12f, 0.0f);
    glColor4f(1.0f, 0.65f, 0.15f, 0.95f * intBulbFlicker);
    drawSphere(0.025f, 6, 6);
    glColor4f(1.0f, 0.92f, 0.45f, 1.0f);
    drawSphere(0.012f, 6, 6);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Tabletop warm glowing light pool
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glPushMatrix();
    glTranslatef(-4.8f, 1.505f, 1.7f);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 0.72f, 0.25f, 0.42f * intBulbFlicker);
    glVertex3f(0.0f, 0.005f, 0.0f);
    for (int i = 0; i <= 16; ++i) {
        float th = 2.0f * (float)M_PI * (float)i / 16.0f;
        glColor4f(1.0f, 0.55f, 0.10f, 0.0f);
        glVertex3f(0.85f * std::cos(th), 0.005f, 0.55f * std::sin(th));
    }
    glEnd();
    glPopMatrix();
    glPopAttrib();

    // Dusty Wine Bottle on table
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_NONE);
    glColor4f(0.18f, 0.25f, 0.16f, 1.0f);
    glPushMatrix();
    glTranslatef(-5.15f, 1.50f, 1.85f);
    drawCylinder(0.042f, 0.042f, 0.20f, 8);
    glTranslatef(0.0f, 0.20f, 0.0f);
    drawCylinder(0.042f, 0.016f, 0.05f, 8);
    glTranslatef(0.0f, 0.05f, 0.0f);
    drawCylinder(0.016f, 0.016f, 0.08f, 8);
    glPopMatrix();

    // 9. Broken Chairs around Dining Table
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.30f, 0.26f, 0.22f, 1.0f);
    // Chair 1 (Tilted backward / broken leg)
    glPushMatrix();
    glTranslatef(-3.75f, 0.74f, 1.65f);
    glRotatef(-75.0f, 0.0f, 1.0f, 0.0f);
    glRotatef( 16.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, 0.45f, 0.0f); drawBox(0.48f, 0.04f, 0.48f);
    glPushMatrix();
    glTranslatef(-0.19f, -0.225f, -0.19f); drawBox(0.045f, 0.45f, 0.045f);
    glTranslatef( 0.38f,  0.0f,    0.0f);  drawBox(0.045f, 0.45f, 0.045f);
    glTranslatef( 0.0f,   0.0f,    0.38f); drawBox(0.045f, 0.45f, 0.045f);
    glTranslatef(-0.38f,  0.10f,   0.0f);  drawBox(0.045f, 0.25f, 0.045f);
    glPopMatrix();
    glTranslatef(-0.19f, 0.26f, -0.19f); drawBox(0.045f, 0.52f, 0.045f);
    glTranslatef( 0.38f, 0.0f,   0.0f);   drawBox(0.045f, 0.52f, 0.045f);
    glTranslatef(-0.19f, 0.24f,  0.0f);   drawBox(0.46f, 0.06f, 0.04f);
    glPopMatrix();

    // Chair 2 (Upright on opposite side)
    glPushMatrix();
    glTranslatef(-5.85f, 0.74f, 1.85f);
    glRotatef(85.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.45f, 0.0f); drawBox(0.48f, 0.04f, 0.48f);
    glPushMatrix();
    glTranslatef(-0.19f, -0.225f, -0.19f); drawBox(0.045f, 0.45f, 0.045f);
    glTranslatef( 0.38f,  0.0f,    0.0f);  drawBox(0.045f, 0.45f, 0.045f);
    glTranslatef( 0.0f,   0.0f,    0.38f); drawBox(0.045f, 0.45f, 0.045f);
    glTranslatef(-0.38f,  0.0f,    0.0f);  drawBox(0.045f, 0.45f, 0.045f);
    glPopMatrix();
    glTranslatef(-0.19f, 0.26f, -0.19f); drawBox(0.045f, 0.52f, 0.045f);
    glTranslatef( 0.38f, 0.0f,   0.0f);   drawBox(0.045f, 0.52f, 0.045f);
    glTranslatef(-0.19f, 0.24f,  0.0f);   drawBox(0.46f, 0.06f, 0.04f);
    glPopMatrix();

    // 10. Dilapidated Wooden Staircase (Ascending into upper darkness along right wall)
    int numSteps = 8;
    float stepWidth = 1.10f;
    float stepDepth = 0.42f;
    float stepHeight = 0.28f;
    for (int s = 0; s < numSteps; ++s) {
        float sy = 0.74f + (float)s * stepHeight;
        float sz = 3.2f - (float)s * stepDepth;
        float sx = -1.45f;

        glPushMatrix();
        glTranslatef(sx, sy + stepHeight * 0.5f, sz);
        drawBox(stepWidth, stepHeight, stepDepth, 0.8f, 0.5f);
        glTranslatef(-stepWidth * 0.45f, stepHeight * 0.5f + 0.40f, 0.0f);
        drawBox(0.04f, 0.80f, 0.04f);
        glPopMatrix();
    }
    // Handrail
    glPushMatrix();
    glTranslatef(-1.45f - stepWidth * 0.45f, 0.74f + (float)numSteps * 0.5f * stepHeight + 0.80f, 3.2f - (float)numSteps * 0.5f * stepDepth);
    glRotatef(-34.0f, 1.0f, 0.0f, 0.0f);
    drawBox(0.06f, 0.08f, 3.8f);
    glPopMatrix();

    // 11. Creepy Vintage Portrait Painting on Left Interior Wall
    glPushMatrix();
    glTranslatef(-8.78f, 2.40f, 1.60f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glRotatef( 3.5f, 0.0f, 0.0f, 1.0f);
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_NONE);
    glColor4f(0.22f, 0.18f, 0.12f, 1.0f);
    drawBox(0.85f, 1.15f, 0.04f);
    glTranslatef(0.0f, 0.0f, 0.022f);
    glColor4f(0.10f, 0.09f, 0.08f, 1.0f);
    drawBox(0.70f, 1.00f, 0.01f);
    glColor4f(0.85f, 0.35f, 0.15f, 0.65f * intBulbFlicker);
    glPushMatrix(); glTranslatef(-0.06f, 0.15f, 0.01f); drawSphere(0.015f, 6, 4); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.06f, 0.15f, 0.01f); drawSphere(0.015f, 6, 4); glPopMatrix();
    glPopMatrix();

    // 12. Corner Cobwebs in Ceiling Corners
    drawCobweb(-8.75f, 4.10f,  4.65f, 0.95f,   0.0f);
    drawCobweb(-0.85f, 4.10f,  4.65f, 0.85f,  90.0f);
    drawCobweb(-8.75f, 4.10f, -3.65f, 1.10f, -90.0f);
    drawCobweb(-4.60f, 3.50f, -1.15f, 0.65f,   0.0f);
}

// 4. Multi-Section Victorian Gothic Haunted House (Reference B Architectural Rebuild)
void drawHouse() {
    glPushMatrix();
    glTranslatef(g_houseShiftX, 0.0f, g_houseShiftZ);
    glRotatef(g_houseRotY, 0.0f, 1.0f, 0.0f);

    // ========================================================================
    // 1. HEAVY STONE FOUNDATION & BASE PLATFORM
    // ========================================================================
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glColor4f(0.42f, 0.44f, 0.48f, 1.0f);
    glPushMatrix();
    glTranslatef(-2.0f, 0.4f, 0.5f);
    drawBox(17.5f, 1.2f, 10.5f, 5.0f, 1.0f);
    glPopMatrix();
    // Stepped stone plinth / water-table band
    glColor4f(0.38f, 0.40f, 0.44f, 1.0f);
    glPushMatrix();
    glTranslatef(-2.0f, 0.92f, 0.5f);
    drawBox(17.8f, 0.18f, 10.8f, 5.0f, 0.5f);
    glPopMatrix();

    // Dark wood trim band above stone
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.30f, 0.27f, 0.24f, 1.0f);
    glPushMatrix();
    glTranslatef(-2.0f, 1.04f, 0.5f);
    drawBox(17.9f, 0.10f, 10.9f, 5.0f, 0.3f);
    glPopMatrix();

    // Render Full Walk-in Haunted Interior (Dilapidated room, Ghost, Furniture, Candle, Stairs)
    drawHouseInterior();

    // ========================================================================
    // 2. SECTION A: MAIN LEFT WING (Hollow Room with Doorway Opening)
    // ========================================================================
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_WALL);
    glColor4f(0.38f, 0.36f, 0.34f, 1.0f);

    // Left outer side wall
    glPushMatrix();
    glTranslatef(-8.95f, 3.8f, 0.5f);
    drawBox(0.24f, 5.6f, 8.8f, 1.0f, 2.0f);
    glPopMatrix();

    // Right interior partition wall (separating wing from central tower)
    glPushMatrix();
    glTranslatef(-0.65f, 3.8f, 0.5f);
    drawBox(0.24f, 5.6f, 8.8f, 1.0f, 2.0f);
    glPopMatrix();

    // Back wall
    glPushMatrix();
    glTranslatef(-4.8f, 3.8f, -3.78f);
    drawBox(8.5f, 5.6f, 0.24f, 3.0f, 2.0f);
    glPopMatrix();

    // Front wall: Left panel (from left edge to door frame)
    glPushMatrix();
    glTranslatef(-7.15f, 3.8f, 4.78f);
    drawBox(3.6f, 5.6f, 0.24f, 1.5f, 2.0f);
    glPopMatrix();

    // Front wall: Right panel (from door frame to right edge)
    glPushMatrix();
    glTranslatef(-2.25f, 3.8f, 4.78f);
    drawBox(3.2f, 5.6f, 0.24f, 1.5f, 2.0f);
    glPopMatrix();

    // Front wall: Top lintel wall above doorway
    glPushMatrix();
    glTranslatef(-4.6f, 5.55f, 4.78f);
    drawBox(1.8f, 2.1f, 0.24f, 0.8f, 0.8f);
    glPopMatrix();

    // Second-floor ceiling slab dividing ground haunted room from upper level
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glPushMatrix();
    glTranslatef(-4.8f, 4.20f, 0.5f);
    drawBox(8.5f, 0.18f, 8.8f, 2.0f, 2.0f);
    glPopMatrix();

    // Half-timber decorative framing on left wing front facade
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.22f, 0.18f, 0.15f, 1.0f);
    // Vertical timbers
    glPushMatrix(); glTranslatef(-8.6f, 3.8f, 4.95f); drawBox(0.12f, 5.6f, 0.08f); glPopMatrix();
    glPushMatrix(); glTranslatef(-6.4f, 3.8f, 4.95f); drawBox(0.12f, 5.6f, 0.08f); glPopMatrix();
    glPushMatrix(); glTranslatef(-3.0f, 3.8f, 4.95f); drawBox(0.12f, 5.6f, 0.08f); glPopMatrix();
    glPushMatrix(); glTranslatef(-1.0f, 3.8f, 4.95f); drawBox(0.12f, 5.6f, 0.08f); glPopMatrix();
    // Horizontal beam mid-story
    glPushMatrix(); glTranslatef(-4.8f, 4.6f, 4.95f); drawBox(8.5f, 0.12f, 0.08f); glPopMatrix();
    // Diagonal braces (decorative X pattern)
    glPushMatrix(); glTranslatef(-7.5f, 5.5f, 4.96f); glRotatef(35.0f, 0.0f, 0.0f, 1.0f); drawBox(0.08f, 2.0f, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(-7.5f, 5.5f, 4.96f); glRotatef(-35.0f, 0.0f, 0.0f, 1.0f); drawBox(0.08f, 2.0f, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(-2.0f, 5.5f, 4.96f); glRotatef(35.0f, 0.0f, 0.0f, 1.0f); drawBox(0.08f, 2.0f, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(-2.0f, 5.5f, 4.96f); glRotatef(-35.0f, 0.0f, 0.0f, 1.0f); drawBox(0.08f, 2.0f, 0.06f); glPopMatrix();

    // Steep High Gabled Roof on Main Left Section
    applyMaterial(MAT_ROOF_SHINGLE);
    bindTexture(TEX_ROOF);
    glColor4f(0.38f, 0.42f, 0.50f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.8f, 6.6f, 0.5f);
    drawPrismRoof(9.2f, 4.8f, 9.4f, 3.5f, 3.0f);
    glPopMatrix();

    // Roof ridge cap trim
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.28f, 0.25f, 0.22f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.8f, 11.38f, 0.5f);
    drawBox(0.18f, 0.14f, 9.5f, 1.0f, 4.0f);
    glPopMatrix();

    // Left Chimney Block (Rising on Left Roof Slope)
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glColor4f(0.45f, 0.47f, 0.52f, 1.0f);
    glPushMatrix();
    glTranslatef(-8.2f, 8.2f, -1.0f);
    drawBox(1.3f, 4.8f, 1.3f, 1.0f, 3.0f);
    // Chimney crown stepped corbels
    glTranslatef(0.0f, 2.45f, 0.0f);
    drawBox(1.6f, 0.22f, 1.6f, 1.0f, 0.5f);
    glTranslatef(0.0f, 0.22f, 0.0f);
    drawBox(1.45f, 0.12f, 1.45f, 1.0f, 0.3f);
    // Twin Clay Chimney Pots
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glColor4f(0.62f, 0.52f, 0.42f, 1.0f);
    glTranslatef(-0.35f, 0.15f, 0.0f);
    drawCylinder(0.18f, 0.15f, 0.65f, 8);
    glTranslatef(0.70f, 0.0f, 0.0f);
    drawCylinder(0.18f, 0.15f, 0.65f, 8);
    glPopMatrix();

    // Second chimney (right side of left wing)
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glColor4f(0.45f, 0.47f, 0.52f, 1.0f);
    glPushMatrix();
    glTranslatef(-2.0f, 8.8f, 1.5f);
    drawBox(1.0f, 3.8f, 1.0f, 1.0f, 2.5f);
    glTranslatef(0.0f, 1.95f, 0.0f);
    drawBox(1.25f, 0.18f, 1.25f);
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glColor4f(0.62f, 0.52f, 0.42f, 1.0f);
    glTranslatef(0.0f, 0.12f, 0.0f);
    drawCylinder(0.15f, 0.12f, 0.55f, 8);
    glPopMatrix();

    // Left Front Dormer with Peaked Roof
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_WALL);
    glColor4f(0.38f, 0.36f, 0.34f, 1.0f);
    glPushMatrix();
    glTranslatef(-5.2f, 6.8f, 4.2f);
    drawBox(2.2f, 2.0f, 2.0f, 1.0f, 1.0f);
    applyMaterial(MAT_ROOF_SHINGLE);
    bindTexture(TEX_ROOF);
    glColor4f(0.38f, 0.42f, 0.50f, 1.0f);
    glTranslatef(0.0f, 1.0f, 0.0f);
    drawPrismRoof(2.5f, 1.6f, 2.2f, 1.0f, 1.0f);
    glPopMatrix();
    drawHouseWindow(-5.2f, 7.0f, 5.25f, 1.2f, 1.4f, true, true);

    // Second Dormer (right side of left wing)
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_WALL);
    glColor4f(0.38f, 0.36f, 0.34f, 1.0f);
    glPushMatrix();
    glTranslatef(-3.2f, 6.8f, 4.2f);
    drawBox(1.8f, 1.8f, 1.8f, 1.0f, 1.0f);
    applyMaterial(MAT_ROOF_SHINGLE);
    bindTexture(TEX_ROOF);
    glColor4f(0.38f, 0.42f, 0.50f, 1.0f);
    glTranslatef(0.0f, 0.9f, 0.0f);
    drawPrismRoof(2.1f, 1.3f, 2.0f, 1.0f, 1.0f);
    glPopMatrix();
    drawHouseWindow(-3.2f, 6.9f, 5.15f, 1.0f, 1.2f, true, true);

    // ========================================================================
    // 3. SECTION B: TALL CENTRAL GOTHIC TOWER (Silhouetted against Moon)
    // ========================================================================
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_WALL);
    glColor4f(0.36f, 0.34f, 0.32f, 1.0f);
    glPushMatrix();
    glTranslatef(1.2f, 0.9f, 2.2f);
    drawCylinder(2.2f, 1.9f, 9.2f, 8, 3.0f, 3.0f);

    // Corbel ledge ring (decorative balcony)
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glColor4f(0.44f, 0.46f, 0.52f, 1.0f);
    glTranslatef(0.0f, 9.2f, 0.0f);
    drawCylinder(2.50f, 2.10f, 0.50f, 8, 2.0f, 0.5f);
    glTranslatef(0.0f, 0.50f, 0.0f);
    drawCylinder(2.15f, 2.05f, 0.15f, 8, 1.5f, 0.3f);

    // Tall steep pointed turret spire
    applyMaterial(MAT_ROOF_SHINGLE);
    bindTexture(TEX_ROOF);
    glColor4f(0.35f, 0.38f, 0.46f, 1.0f);
    glTranslatef(0.0f, 0.15f, 0.0f);
    drawSteepleSpire(2.05f, 9.5f, 8, 2.0f, 4.0f);

    // Thin iron spike / finial needle
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glColor4f(0.50f, 0.50f, 0.55f, 1.0f);
    glTranslatef(0.0f, 9.5f, 0.0f);
    drawCylinder(0.05f, 0.008f, 2.6f, 6);
    // Cross finial
    glTranslatef(0.0f, 1.3f, 0.0f);
    drawBox(0.42f, 0.04f, 0.04f);
    drawBox(0.04f, 0.04f, 0.42f);
    // Decorative weather vane
    glTranslatef(0.0f, 0.3f, 0.0f);
    drawBox(0.55f, 0.03f, 0.03f);
    glPopMatrix();

    // Tower narrow slit windows at multiple heights
    drawHouseWindow(1.2f, 8.6f, 4.35f, 1.1f, 1.6f, true, true);
    drawHouseWindow(1.2f, 4.8f, 4.35f, 1.1f, 1.6f, true, true);
    drawHouseWindow(1.2f, 6.6f, 4.35f, 0.8f, 1.2f, true, false);

    // Buttresses on tower sides
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glColor4f(0.40f, 0.42f, 0.46f, 1.0f);
    for (int bt = 0; bt < 4; ++bt) {
        float angle = bt * 90.0f + 45.0f;
        float rad = angle * (float)M_PI / 180.0f;
        float bx = 1.2f + std::cos(rad) * 2.3f;
        float bz = 2.2f + std::sin(rad) * 2.3f;
        glPushMatrix();
        glTranslatef(bx, 3.0f, bz);
        glRotatef(-angle, 0.0f, 1.0f, 0.0f);
        drawBox(0.35f, 5.0f, 0.65f, 0.5f, 2.5f);
        glTranslatef(0.0f, 2.5f, -0.10f);
        glRotatef(15.0f, 1.0f, 0.0f, 0.0f);
        drawBox(0.38f, 0.15f, 0.75f, 0.5f, 0.5f);
        glPopMatrix();
    }

    // ========================================================================
    // 4. SECTION C: RIGHT WING WITH LOWER PEAKED GABLE ROOF
    // ========================================================================
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_WALL);
    glColor4f(0.38f, 0.36f, 0.34f, 1.0f);
    glPushMatrix();
    glTranslatef(3.8f, 3.3f, 0.5f);
    drawBox(4.4f, 4.6f, 7.6f, 2.0f, 2.0f);
    glPopMatrix();

    // Half-timber on right wing front
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.22f, 0.18f, 0.15f, 1.0f);
    glPushMatrix(); glTranslatef(2.0f, 3.3f, 4.35f); drawBox(0.10f, 4.6f, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(5.6f, 3.3f, 4.35f); drawBox(0.10f, 4.6f, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(3.8f, 4.2f, 4.35f); drawBox(4.4f, 0.10f, 0.06f); glPopMatrix();

    // Right wing peaked gable roof
    applyMaterial(MAT_ROOF_SHINGLE);
    bindTexture(TEX_ROOF);
    glColor4f(0.38f, 0.42f, 0.50f, 1.0f);
    glPushMatrix();
    glTranslatef(3.8f, 5.6f, 0.5f);
    drawPrismRoof(4.8f, 3.2f, 8.0f, 2.0f, 2.0f);
    glPopMatrix();

    // Gable decorative barge boards
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.25f, 0.22f, 0.20f, 1.0f);
    glPushMatrix();
    glTranslatef(3.8f, 7.2f, 4.55f);
    glRotatef(53.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.10f, 2.2f, 0.08f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(3.8f, 7.2f, 4.55f);
    glRotatef(-53.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.10f, 2.2f, 0.08f);
    glPopMatrix();

    // Upper gable window
    drawHouseWindow(3.8f, 6.2f, 4.55f, 1.5f, 1.5f, false, true);

    // Ground-floor right wing windows
    drawHouseWindow(2.6f, 2.6f, 4.35f, 1.2f, 1.6f, true, true);
    drawHouseWindow(4.8f, 2.6f, 4.35f, 1.2f, 1.6f, true, true);

    // Side facade window on right wing
    drawHouseWindow(6.05f, 3.2f, 2.5f, 1.0f, 1.4f, true, true);
    drawHouseWindow(6.05f, 5.5f, 2.5f, 0.9f, 1.2f, true, false);

    // ========================================================================
    // 5. SECTION D: GROUND-FLOOR PORCH & ENTRANCE WITH GOTHIC ARCHED DOORWAY
    // ========================================================================
    // Porch foundation & deck floor
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.30f, 0.27f, 0.25f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.6f, 0.72f, 4.2f);
    drawBox(6.2f, 0.16f, 3.4f, 2.5f, 0.5f);
    glPopMatrix();

    // Porch steps (three steps down to ground)
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glColor4f(0.44f, 0.46f, 0.50f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.6f, 0.12f, 6.5f);
    drawBox(3.4f, 0.24f, 0.65f);
    glTranslatef(0.0f, 0.12f, -0.34f);
    drawBox(3.2f, 0.12f, 0.65f);
    glTranslatef(0.0f, 0.12f, -0.34f);
    drawBox(3.0f, 0.18f, 0.65f);
    glPopMatrix();

    // Porch columns (4 Gothic-style square posts with chamfered edges)
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glColor4f(0.44f, 0.46f, 0.50f, 1.0f);
    float postX[4] = { -7.4f, -5.8f, -3.4f, -1.8f };
    for (int p = 0; p < 4; ++p) {
        glPushMatrix();
        glTranslatef(postX[p], 0.80f, 5.7f);
        drawBox(0.30f, 0.30f, 0.30f);
        glTranslatef(0.0f, 0.15f, 0.0f);
        drawCylinder(0.11f, 0.09f, 3.0f, 8);
        glTranslatef(0.0f, 3.0f, 0.0f);
        drawBox(0.30f, 0.08f, 0.30f);
        glTranslatef(0.0f, 0.08f, 0.0f);
        drawBox(0.34f, 0.06f, 0.34f);
        glPopMatrix();
    }

    // Porch beam header
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.28f, 0.25f, 0.22f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.6f, 3.95f, 5.7f);
    drawBox(6.4f, 0.24f, 0.35f);
    for (int sc = 0; sc < 7; ++sc) {
        glPushMatrix();
        glTranslatef(-3.0f + sc * 1.0f, -0.18f, 0.0f);
        drawBox(0.08f, 0.14f, 0.08f);
        glPopMatrix();
    }
    glPopMatrix();

    // Porch sloped overhang roof
    applyMaterial(MAT_ROOF_SHINGLE);
    bindTexture(TEX_ROOF);
    glColor4f(0.38f, 0.42f, 0.50f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.6f, 4.15f, 4.6f);
    glRotatef(18.0f, 1.0f, 0.0f, 0.0f);
    drawBox(6.6f, 0.18f, 2.8f, 2.5f, 1.5f);
    glPopMatrix();

    // Gothic arched doorway entrance (Open door showing haunted walk-in interior)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.28f, 0.24f, 0.20f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.6f, 2.2f, 4.85f);
    // Heavy door casing with Gothic pointed arch
    glPushMatrix(); glTranslatef(-0.82f, 0.0f, 0.0f); drawBox(0.16f, 2.8f, 0.14f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.82f, 0.0f, 0.0f); drawBox(0.16f, 2.8f, 0.14f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.0f, 1.35f, 0.0f); drawBox(1.8f, 0.16f, 0.14f); glPopMatrix();
    // Pointed arch peak above door
    glPushMatrix();
    glTranslatef(0.0f, 1.45f, 0.0f);
    glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.12f, 0.65f, 0.10f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 1.45f, 0.0f);
    glRotatef(-45.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.12f, 0.65f, 0.10f);
    glPopMatrix();

    // Broken ajar wooden panel door leaf (swung inward/ajar with crooked tilt)
    glPushMatrix();
    glTranslatef(-0.68f, -0.05f, 0.0f);
    glRotatef(42.0f, 0.0f, 1.0f, 0.0f);
    glRotatef( 3.5f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.60f, 0.0f, 0.0f);
    drawBox(1.20f, 2.45f, 0.06f, 1.0f, 2.0f);
    // Door panels
    glTranslatef(0.0f, 0.0f, 0.035f);
    drawBox(1.02f, 1.02f, 0.02f);
    glTranslatef(0.0f, -1.15f, 0.0f);
    drawBox(1.02f, 0.90f, 0.02f);
    // Rusted iron door handle
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_NONE);
    glTranslatef(0.48f, 0.62f, 0.04f);
    drawCylinder(0.02f, 0.02f, 0.08f, 6);
    glPopMatrix();

    glPopMatrix();

    // Main-floor windows under porch / left wing
    drawHouseWindow(-7.4f, 2.6f, 4.95f, 1.2f, 1.6f, true, true);
    drawHouseWindow(-1.8f, 2.6f, 4.95f, 1.2f, 1.6f, true, true);

    // ========================================================================
    // 6. ADDITIONAL ARCHITECTURAL DETAILS (Reference B Fidelity)
    // ========================================================================
    // Side facade windows on left wing
    drawHouseWindow(-9.05f, 3.2f, 1.5f, 1.0f, 1.4f, true, true);
    drawHouseWindow(-9.05f, 5.5f, 1.5f, 0.9f, 1.2f, true, false);
    drawHouseWindow(-9.05f, 3.2f, -1.5f, 1.0f, 1.4f, true, true);

    // Overhanging eaves / fascia boards on main sections
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.25f, 0.22f, 0.20f, 1.0f);
    // Left wing eaves
    glPushMatrix(); glTranslatef(-4.8f, 6.58f, 5.1f); drawBox(8.8f, 0.14f, 0.28f); glPopMatrix();
    // Right wing eaves
    glPushMatrix(); glTranslatef(3.8f, 5.58f, 4.5f); drawBox(4.6f, 0.12f, 0.24f); glPopMatrix();

    // Corner quoins (decorative stone blocks at building corners)
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glColor4f(0.48f, 0.50f, 0.54f, 1.0f);
    float quoinY[4] = { 1.4f, 2.6f, 3.8f, 5.0f };
    for (int q = 0; q < 4; ++q) {
        glPushMatrix(); glTranslatef(-9.0f, quoinY[q], 4.95f); drawBox(0.25f, 0.35f, 0.18f); glPopMatrix();
    }
    for (int q = 0; q < 4; ++q) {
        glPushMatrix(); glTranslatef(6.0f, quoinY[q], 4.35f); drawBox(0.25f, 0.35f, 0.18f); glPopMatrix();
    }

    // Decorative lintel stones above main-floor windows
    applyMaterial(MAT_STONE);
    glColor4f(0.46f, 0.48f, 0.52f, 1.0f);
    glPushMatrix(); glTranslatef(-7.4f, 3.55f, 4.98f); drawBox(1.5f, 0.14f, 0.10f); glPopMatrix();
    glPushMatrix(); glTranslatef(-1.8f, 3.55f, 4.98f); drawBox(1.5f, 0.14f, 0.10f); glPopMatrix();
    glPushMatrix(); glTranslatef(2.6f, 3.55f, 4.38f); drawBox(1.5f, 0.14f, 0.10f); glPopMatrix();
    glPushMatrix(); glTranslatef(4.8f, 3.55f, 4.38f); drawBox(1.5f, 0.14f, 0.10f); glPopMatrix();

    // Small rear extension / lean-to
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_WALL);
    glColor4f(0.35f, 0.33f, 0.30f, 1.0f);
    glPushMatrix();
    glTranslatef(-6.5f, 2.5f, -3.0f);
    drawBox(3.2f, 3.0f, 3.5f, 1.5f, 1.0f);
    applyMaterial(MAT_ROOF_SHINGLE);
    bindTexture(TEX_ROOF);
    glColor4f(0.36f, 0.40f, 0.48f, 1.0f);
    glTranslatef(0.0f, 1.5f, 0.0f);
    drawPrismRoof(3.4f, 1.8f, 3.7f, 1.5f, 1.5f);
    glPopMatrix();

    glPopMatrix(); // End House
}

// ----------------------------------------------------------------------------
// INDIVIDUAL GRAVE / TOMBSTONE STYLES (Reference Panel 12)
// ----------------------------------------------------------------------------

// 1. Gothic Arched Headstone with Carved Relief Cross & Beveled Edge
void drawArchedHeadstone(float x, float z, float scale, float rotY, float lean, float leanDir, int stoneType = 0) {
    float groundY = getTerrainHeight(x, z);

    glPushMatrix();
    glTranslatef(x, groundY, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(lean, std::sin(leanDir * (float)M_PI / 180.0f), 0.0f,
                   std::cos(leanDir * (float)M_PI / 180.0f));
    glScalef(scale, scale, scale);

    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);

    float tint = (stoneType == 1) ? 0.88f : (stoneType == 2) ? 1.05f : 0.96f;
    glColor4f(0.42f * tint, 0.44f * tint, 0.48f * tint, 1.0f);

    // Stone Base Plinth
    glPushMatrix();
    glTranslatef(0.0f, 0.10f, 0.0f);
    drawBox(0.95f, 0.20f, 0.45f, 0.8f, 0.5f);
    glPopMatrix();

    // Main Vertical Headstone Slab
    glPushMatrix();
    glTranslatef(0.0f, 0.85f, 0.0f);
    drawBox(0.78f, 1.30f, 0.18f, 0.8f, 1.0f);

    // Rounded / Arched Top Cap
    glTranslatef(0.0f, 0.65f, 0.0f);
    glPushMatrix();
    glScalef(0.39f, 0.28f, 0.09f);
    drawSphere(1.0f, 12, 8);
    glPopMatrix();

    // Relief Cross Carving on Headstone Front
    glColor4f(0.28f * tint, 0.29f * tint, 0.32f * tint, 1.0f);
    glTranslatef(0.0f, -0.22f, 0.095f);
    drawBox(0.12f, 0.55f, 0.02f);
    glPushMatrix();
    glTranslatef(0.0f, 0.08f, 0.0f);
    drawBox(0.38f, 0.10f, 0.02f);
    glPopMatrix();
    glPopMatrix();

    // Overgrown Dirt / Moss Mound at Base
    applyMaterial(MAT_MOSS_STONE);
    bindTexture(TEX_NONE);
    glColor4f(0.22f, 0.25f, 0.18f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.55f);
    glScalef(0.55f, 0.18f, 0.95f);
    drawSphere(1.0f, 8, 6);
    glPopMatrix();

    glPopMatrix();
}

// 2. Celtic Weathered Stone Cross with Ring Halo
void drawCelticCrossGrave(float x, float z, float scale, float rotY, float lean, float leanDir) {
    float groundY = getTerrainHeight(x, z);

    glPushMatrix();
    glTranslatef(x, groundY, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(lean, std::sin(leanDir * (float)M_PI / 180.0f), 0.0f,
                   std::cos(leanDir * (float)M_PI / 180.0f));
    glScalef(scale, scale, scale);

    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glColor4f(0.40f, 0.42f, 0.46f, 1.0f);

    // Stepped Pedestal Base
    glPushMatrix();
    glTranslatef(0.0f, 0.12f, 0.0f);
    drawBox(0.85f, 0.24f, 0.65f);
    glTranslatef(0.0f, 0.20f, 0.0f);
    drawBox(0.65f, 0.16f, 0.50f);
    glPopMatrix();

    // Vertical Cross Shaft
    glPushMatrix();
    glTranslatef(0.0f, 1.25f, 0.0f);
    drawBox(0.22f, 1.90f, 0.16f, 0.5f, 1.5f);

    // Horizontal Crossbar
    glPushMatrix();
    glTranslatef(0.0f, 0.42f, 0.0f);
    drawBox(1.10f, 0.20f, 0.16f, 1.0f, 0.5f);
    glPopMatrix();

    // Circular Halo Ring behind cross intersection
    glPushMatrix();
    glTranslatef(0.0f, 0.42f, 0.0f);
    int ringSegs = 16;
    float rIn = 0.32f;
    float rOut = 0.44f;
    float depth = 0.08f;
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= ringSegs; ++i) {
        float th = 2.0f * (float)M_PI * (float)i / ringSegs;
        glVertex3f(rIn * std::cos(th), rIn * std::sin(th), depth * 0.5f);
        glVertex3f(rOut * std::cos(th), rOut * std::sin(th), depth * 0.5f);
    }
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= ringSegs; ++i) {
        float th = 2.0f * (float)M_PI * (float)i / ringSegs;
        glVertex3f(rIn * std::cos(th), rIn * std::sin(th), -depth * 0.5f);
        glVertex3f(rOut * std::cos(th), rOut * std::sin(th), -depth * 0.5f);
    }
    glEnd();
    glPopMatrix();

    // Central ornamental boss
    glTranslatef(0.0f, 0.42f, 0.09f);
    drawSphere(0.07f, 8, 6);
    glPopMatrix();

    // Earth mound
    applyMaterial(MAT_WET_GROUND);
    bindTexture(TEX_GROUND);
    glPushMatrix();
    glTranslatef(0.0f, 0.06f, 0.65f);
    glScalef(0.60f, 0.20f, 1.05f);
    drawSphere(1.0f, 8, 6);
    glPopMatrix();

    glPopMatrix();
}

// 3. Rough-Hewn Weathered Stone Cross
void drawStoneCrossGrave(float x, float z, float scale, float rotY, float lean, float leanDir) {
    float groundY = getTerrainHeight(x, z);

    glPushMatrix();
    glTranslatef(x, groundY, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(lean, std::sin(leanDir * (float)M_PI / 180.0f), 0.0f,
                   std::cos(leanDir * (float)M_PI / 180.0f));
    glScalef(scale, scale, scale);

    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glColor4f(0.36f, 0.38f, 0.42f, 1.0f);

    // Vertical post
    glPushMatrix();
    glTranslatef(0.0f, 0.80f, 0.0f);
    drawBox(0.16f, 1.60f, 0.12f, 0.5f, 1.5f);

    // Horizontal crossbar
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 0.0f);
    drawBox(0.85f, 0.14f, 0.12f, 1.0f, 0.5f);
    glPopMatrix();

    // Rough cap
    glTranslatef(0.0f, 0.82f, 0.0f);
    drawBox(0.20f, 0.08f, 0.15f);
    glPopMatrix();

    // Mound of dirt at base
    applyMaterial(MAT_WET_GROUND);
    bindTexture(TEX_NONE);
    glColor4f(0.24f, 0.20f, 0.16f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.0f);
    glScalef(0.65f, 0.20f, 0.50f);
    drawSphere(1.0f, 8, 6);
    glPopMatrix();

    glPopMatrix();
}

// 4. Heavy Raised Stone Sarcophagus / Tomb Crypt with Cracked Ajar Lid
void drawStoneSarcophagusGrave(float x, float z, float scale, float rotY, float tilt, bool lidAjar = true) {
    float groundY = getTerrainHeight(x, z);

    glPushMatrix();
    glTranslatef(x, groundY, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    if (tilt != 0.0f) glRotatef(tilt, 1.0f, 0.0f, 0.0f);
    glScalef(scale, scale, scale);

    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glColor4f(0.40f, 0.42f, 0.46f, 1.0f);

    // Stepped plinth foundation
    glPushMatrix();
    glTranslatef(0.0f, 0.10f, 0.0f);
    drawBox(1.15f, 0.20f, 2.25f, 1.0f, 2.0f);
    glPopMatrix();

    // Main sarcophagus body box
    glPushMatrix();
    glTranslatef(0.0f, 0.45f, 0.0f);
    drawBox(0.95f, 0.50f, 2.05f, 1.0f, 2.0f);
    glPopMatrix();

    // Heavy Stone Lid Slab (Shifted/Cracked Ajar revealing dark hollow)
    glPushMatrix();
    glTranslatef(0.0f, 0.74f, 0.0f);
    if (lidAjar) {
        glTranslatef(0.10f, 0.02f, 0.06f);
        glRotatef(7.5f, 0.0f, 1.0f, 0.0f);
        glRotatef(3.0f, 0.0f, 0.0f, 1.0f);
    }
    drawBox(1.05f, 0.14f, 2.15f, 1.0f, 2.0f);

    // Carved Cross on top of lid
    glColor4f(0.28f, 0.30f, 0.34f, 1.0f);
    glTranslatef(0.0f, 0.075f, 0.0f);
    drawBox(0.14f, 0.02f, 1.40f);
    glTranslatef(0.0f, 0.0f, -0.20f);
    drawBox(0.65f, 0.02f, 0.14f);
    glPopMatrix();

    // Moss / Mud patches around sarcophagus
    applyMaterial(MAT_MOSS_STONE);
    bindTexture(TEX_NONE);
    glColor4f(0.22f, 0.26f, 0.18f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.50f, 0.06f, 0.40f);
    glScalef(0.40f, 0.14f, 0.60f);
    drawSphere(1.0f, 6, 4);
    glPopMatrix();

    glPopMatrix();
}

// 5. Elongated Earth Burial Mound with Head & Foot Markers
void drawEarthBurialMound(float x, float z, float scale, float rotY) {
    float groundY = getTerrainHeight(x, z);

    glPushMatrix();
    glTranslatef(x, groundY, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glScalef(scale, scale, scale);

    // Earthen mound
    applyMaterial(MAT_WET_GROUND);
    bindTexture(TEX_GROUND);
    glColor4f(0.28f, 0.24f, 0.20f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.14f, 0.0f);
    glScalef(0.50f, 0.28f, 1.10f);
    drawSphere(1.0f, 10, 8);
    glPopMatrix();

    // Headstone (Small weathered arch)
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glColor4f(0.38f, 0.40f, 0.44f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.42f, -1.05f);
    glRotatef(-10.0f, 1.0f, 0.0f, 0.0f);
    drawBox(0.48f, 0.70f, 0.12f);
    glPopMatrix();

    // Foot marker stone
    glPushMatrix();
    glTranslatef(0.0f, 0.20f, 1.05f);
    glRotatef(12.0f, 1.0f, 0.0f, 0.0f);
    drawBox(0.32f, 0.35f, 0.10f);
    glPopMatrix();

    glPopMatrix();
}

// ============================================================================
// COMPLETE EXPANDED GRAVEYARD & CEMETERY (Surrounding the Haunted House)
// ============================================================================
void drawGraveyardCrosses() {
    // ------------------------------------------------------------------------
    // 1. LEFT CEMETERY KNOLL (Primary Dense Graveyard)
    // ------------------------------------------------------------------------
    drawCelticCrossGrave(     -14.5f, -2.0f, 0.95f,  15.0f,  8.0f,  30.0f);
    drawArchedHeadstone(      -13.0f, -0.5f, 1.05f,  -8.0f,  5.0f, -20.0f, 0);
    drawStoneCrossGrave(      -15.8f,  0.8f, 0.85f,  28.0f, 14.0f,  60.0f);
    drawArchedHeadstone(      -12.5f,  1.8f, 0.95f, -20.0f,  7.0f, -45.0f, 1);
    drawStoneSarcophagusGrave(-16.2f, -3.5f, 0.90f,  12.0f,  3.0f, true);
    drawCelticCrossGrave(     -13.8f, -4.2f, 0.80f, -32.0f, 11.0f, -80.0f);
    drawArchedHeadstone(      -15.0f,  3.0f, 0.75f,  42.0f, 18.0f,  45.0f, 2);
    drawEarthBurialMound(     -17.2f, -1.2f, 0.85f,  18.0f);
    drawStoneSarcophagusGrave(-11.5f, -2.8f, 0.85f, -15.0f, -2.0f, false);
    drawStoneCrossGrave(      -16.5f,  4.5f, 0.80f, -22.0f, 12.0f,  90.0f);

    // ------------------------------------------------------------------------
    // 2. LEFT FOREGROUND & FENCE LINE GRAVES
    // ------------------------------------------------------------------------
    drawArchedHeadstone(      -10.5f,  7.5f, 0.85f,  35.0f,  9.0f,  25.0f, 1);
    drawEarthBurialMound(     -12.8f,  9.5f, 0.80f, -40.0f);
    drawStoneCrossGrave(      -14.2f, 13.0f, 0.75f,  18.0f, 15.0f, -60.0f);
    drawArchedHeadstone(       -8.8f, 12.2f, 0.70f, -50.0f, 12.0f,  40.0f, 0);

    // ------------------------------------------------------------------------
    // 3. RIGHT YARD GRAVEYARD (Beside Car & Surrounding Right Grounds)
    // ------------------------------------------------------------------------
    drawArchedHeadstone(       12.0f,  3.2f, 0.90f, -25.0f,  6.0f, -35.0f, 2);
    drawCelticCrossGrave(      14.5f,  1.5f, 0.95f,  30.0f, 10.0f,  50.0f);
    drawStoneSarcophagusGrave( 13.2f, -2.0f, 0.90f, -10.0f,  4.0f, true);
    drawArchedHeadstone(       15.8f, -4.0f, 0.80f,  45.0f, 12.0f, -40.0f, 0);
    drawStoneCrossGrave(       10.5f,  8.5f, 0.85f, -35.0f,  8.0f,  80.0f);
    drawArchedHeadstone(       14.0f, 11.2f, 0.80f,  20.0f, 14.0f, -30.0f, 1);
    drawEarthBurialMound(      16.2f,  7.8f, 0.85f, -15.0f);
    drawStoneCrossGrave(       11.8f, 14.5f, 0.75f,  55.0f, 16.0f,  45.0f);

    // ------------------------------------------------------------------------
    // 4. DISTANT MIST GRAVEYARD (Scattered in background mist around trees)
    // ------------------------------------------------------------------------
    drawCelticCrossGrave(     -16.0f,  -9.5f, 0.85f,  10.0f, 12.0f,  30.0f);
    drawArchedHeadstone(      -13.5f, -12.0f, 0.80f, -40.0f,  8.0f, -60.0f, 0);
    drawStoneCrossGrave(       -8.5f, -11.5f, 0.75f,  25.0f, 14.0f,  90.0f);
    drawArchedHeadstone(        7.5f, -10.5f, 0.80f, -18.0f,  9.0f, -45.0f, 2);
    drawStoneSarcophagusGrave( 11.5f, -11.0f, 0.85f,  35.0f,  3.0f, false);
    drawStoneCrossGrave(       15.0f, -12.5f, 0.80f, -30.0f, 15.0f,  70.0f);
}

// 5. Hanging Porch Bulb with Dynamic Point Light
void drawHangingBulb() {
    float swayAngleX = 0.0f;
    float swayAngleZ = 0.0f;
    if (g_bulbAnimEnabled) {
        swayAngleX = 14.0f * std::sin(g_time * 2.1f);
        swayAngleZ = 6.0f * std::cos(g_time * 1.6f);
    }

    // World position corresponding to porch beam:
    float radHouseRot = g_houseRotY * (float)M_PI / 180.0f;
    float localX = -4.6f;
    float localZ = 5.7f;
    float beamX = g_houseShiftX + localX * std::cos(radHouseRot) + localZ * std::sin(radHouseRot);
    float beamY = 3.95f;
    float beamZ = g_houseShiftZ - localX * std::sin(radHouseRot) + localZ * std::cos(radHouseRot);

    float radX = swayAngleX * (float)M_PI / 180.0f;
    float radZ = swayAngleZ * (float)M_PI / 180.0f;
    g_bulbCurX = beamX + g_bulbCordLength * std::sin(radZ);
    g_bulbCurY = beamY - g_bulbCordLength * std::cos(radX) * std::cos(radZ);
    g_bulbCurZ = beamZ - g_bulbCordLength * std::sin(radX);

    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(beamX, beamY, beamZ);
    drawCylinder(0.08f, 0.08f, 0.04f, 8, 1.0f, 1.0f);
    glPopMatrix();

    bindTexture(TEX_NONE);
    glDisable(GL_LIGHTING);
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(beamX, beamY, beamZ);
    glVertex3f(g_bulbCurX, g_bulbCurY + 0.15f, g_bulbCurZ);
    glEnd();
    glEnable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(g_bulbCurX, g_bulbCurY + 0.1f, g_bulbCurZ);
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    drawCylinder(0.06f, 0.05f, 0.12f, 8, 1.0f, 1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(g_bulbCurX, g_bulbCurY, g_bulbCurZ);
    if (g_light0PointOn) {
        applyMaterial(MAT_BULB_EMISSIVE);
    } else {
        Material unlitBulb = MAT_DARK_WOOD;
        unlitBulb.diffuse[0] = 0.4f; unlitBulb.diffuse[1] = 0.4f; unlitBulb.diffuse[2] = 0.35f;
        applyMaterial(unlitBulb);
    }
    bindTexture(TEX_NONE);
    drawSphere(0.12f, 12, 10);
    glPopMatrix();
}

// 6. Abandoned Rusted Car with Beveled Contours, Open Door, Deflated Tyre & Details
void drawRustedCar(float x, float z, float rotY) {
    float groundY = getTerrainHeight(x, z);

    glPushMatrix();
    // Partially sunk into mud, with authentic deflated tyre listing & forward pitch
    glTranslatef(x, groundY - 0.10f, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(-5.0f, 0.0f, 0.0f, 1.0f); // Roll listing to the right (flat tyre side)
    glRotatef( 3.4f, 1.0f, 0.0f, 0.0f); // Pitch dipped down at front-right

    // ------------------------------------------------------------------------
    // A. WET MUD RUT & SUNKEN GROUND DEPRESSION UNDER FLAT TYRE
    // ------------------------------------------------------------------------
    applyMaterial(MAT_WET_GROUND);
    bindTexture(TEX_GROUND);
    glPushMatrix();
    glTranslatef(1.02f, 0.04f, 1.35f); // Directly under front-right deflated tyre
    drawBox(0.95f, 0.05f, 1.10f, 1.0f, 1.0f);
    // Surrounding splashed mud ridge
    glTranslatef(0.0f, 0.03f, 0.0f);
    drawBox(1.15f, 0.03f, 1.30f, 1.0f, 1.0f);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // B. LOWER CHASSIS, UNDERCARRIAGE & RUSTY EXHAUST SYSTEM
    // ------------------------------------------------------------------------
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);

    // Lower Chassis Frame / Rocker panels (Beveled to catch light)
    glPushMatrix();
    glTranslatef(0.0f, 0.40f, 0.0f);
    drawBeveledBox(1.95f, 0.26f, 4.40f, 0.04f, 2.0f, 1.5f);
    glPopMatrix();

    // Undercarriage Transmission Tunnel
    glPushMatrix();
    glTranslatef(0.0f, 0.50f, 0.0f);
    drawBox(0.45f, 0.16f, 3.40f);
    glPopMatrix();

    // Rusted Exhaust Pipe & Muffler trailing underneath to the rear
    glPushMatrix();
    glTranslatef(-0.48f, 0.28f, 0.60f);
    // Exhaust pipe from engine bay
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.032f, 0.032f, 1.60f, 8);
    // Rusted oval muffler box
    glTranslatef(0.0f, 1.60f, 0.0f);
    drawBeveledBox(0.32f, 0.16f, 0.65f, 0.03f);
    // Tailpipe leading past rear bumper
    glTranslatef(0.0f, 0.65f, 0.0f);
    drawCylinder(0.030f, 0.030f, 0.65f, 8);
    // Slanted down-turned tailpipe tip
    glTranslatef(0.0f, 0.65f, 0.0f);
    glRotatef(-25.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.030f, 0.028f, 0.18f, 8);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // C. MAIN BODY PANELS, SCULPTED FENDERS, HOOD & TRUNK
    // ------------------------------------------------------------------------
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);

    // Main Lower Body Tub (Wheel wells, lower doors, quarter panels)
    glPushMatrix();
    glTranslatef(0.0f, 0.68f, 0.0f);
    drawBeveledBox(2.18f, 0.38f, 4.62f, 0.06f, 2.0f, 1.8f);
    glPopMatrix();

    // 4 Curved Flared Wheel Well Arches (Fender Flares with mud splatter)
    float archX[2] = { -1.10f, 1.10f };
    float archZ[2] = { -1.35f, 1.35f };
    for (int ix = 0; ix < 2; ++ix) {
        for (int iz = 0; iz < 2; ++iz) {
            glPushMatrix();
            glTranslatef(archX[ix], 0.72f, archZ[iz]);
            drawBeveledBox(0.12f, 0.28f, 1.12f, 0.03f, 0.5f, 0.5f);
            glPopMatrix();
        }
    }

    // Upper Body Waistline / Shoulder Crease (Beveled transition)
    glPushMatrix();
    glTranslatef(0.0f, 0.94f, 0.0f);
    drawBeveledBox(2.08f, 0.22f, 4.42f, 0.04f, 2.0f, 1.2f);
    glPopMatrix();

    // Sloped Front Engine Hood with Raised Central Power Crease
    glPushMatrix();
    glTranslatef(0.0f, 0.98f, 1.35f);
    glRotatef(-4.8f, 1.0f, 0.0f, 0.0f);
    drawBeveledBox(1.98f, 0.14f, 1.72f, 0.04f, 1.5f, 1.0f); // Hood main plate
    // Central raised power bulge / crease line
    glTranslatef(0.0f, 0.05f, 0.0f);
    drawBeveledBox(0.65f, 0.04f, 1.62f, 0.02f);
    // Chrome Hood Center Ornament / Emblem base
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glTranslatef(0.0f, 0.03f, 0.76f);
    drawBox(0.06f, 0.05f, 0.14f);
    glPopMatrix();

    // Sloped Rear Trunk Deck Lid
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(0.0f, 0.98f, -1.55f);
    glRotatef(3.2f, 1.0f, 0.0f, 0.0f);
    drawBeveledBox(1.92f, 0.14f, 1.32f, 0.04f, 1.5f, 1.0f);
    // Chrome Trunk Keyhole Cylinder
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glTranslatef(0.0f, -0.04f, -0.66f);
    drawCylinder(0.025f, 0.025f, 0.03f, 8);
    glPopMatrix();

    // Rear Quarter Panel Fuel Filler Door Flap
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(1.05f, 0.92f, -1.45f);
    drawBox(0.02f, 0.14f, 0.14f);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // D. FRONT GRILLE, HEADLIGHTS, BUMPERS & CRUMPLED ACCENTS
    // ------------------------------------------------------------------------
    // Front Radiator Grille Shell Housing
    glPushMatrix();
    glTranslatef(0.0f, 0.72f, 2.34f);
    drawBeveledBox(1.88f, 0.44f, 0.10f, 0.03f, 1.0f, 0.5f);

    // Deep Dark Radiator Core Mesh behind grille
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.02f);
    drawBox(1.70f, 0.36f, 0.02f);
    glPopMatrix();

    // Chrome Grille Matrix: Vertical Slats & Horizontal Crossbars
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    for (int i = -4; i <= 4; ++i) {
        glPushMatrix();
        glTranslatef(i * 0.18f, 0.0f, 0.055f);
        drawBox(0.028f, 0.34f, 0.035f);
        glPopMatrix();
    }
    for (int j = -1; j <= 1; ++j) {
        glPushMatrix();
        glTranslatef(0.0f, j * 0.11f, 0.055f);
        drawBox(1.68f, 0.025f, 0.035f);
        glPopMatrix();
    }
    // Center Vintage Insignia Emblem Badge
    glPushMatrix();
    glTranslatef(0.0f, 0.06f, 0.075f);
    drawSphere(0.055f, 10, 8);
    glPopMatrix();

    glPopMatrix();

    // Dual Round Headlights (Left Intact with Glass, Right Broken with Exposed Bulb!)
    float headLightX[2] = { -0.74f, 0.74f };
    // 1. Left Headlight (Intact, chrome bezel with fluted reflective glass lens)
    glPushMatrix();
    glTranslatef(headLightX[0], 0.78f, 2.34f);
    // Chrome Bezel Housing
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawCylinder(0.165f, 0.165f, 0.06f, 14, 1.0f, 0.2f);
    // Chrome Reflector Bowl inside
    glTranslatef(0.0f, 0.0f, 0.02f);
    drawSphere(0.13f, 10, 8);
    // Glass Convex Lens (High specular glint)
    glTranslatef(0.0f, 0.0f, 0.04f);
    applyMaterial(MAT_CAR_GLASS);
    drawSphere(0.145f, 12, 10);
    // Soft specular lens flare halo
    drawBillboardHalo(0.0f, 0.0f, 0.08f, 0.45f, 0.85f, 0.92f, 1.0f, 0.35f);
    glPopMatrix();

    // 2. Right Headlight (Broken/Abandoned: dented chrome rim, shattered shards, exposed bulb!)
    glPushMatrix();
    glTranslatef(headLightX[1], 0.78f, 2.34f);
    glRotatef(6.0f, 0.0f, 1.0f, 0.2f); // Askew/dented
    // Dented Chrome Bezel
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawCylinder(0.165f, 0.150f, 0.05f, 12, 1.0f, 0.2f);
    // Dark Empty Lamp Bucket
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glTranslatef(0.0f, 0.0f, 0.015f);
    drawSphere(0.12f, 8, 6);
    // Tiny Exposed Tungsten Filament Bulb on Wire
    applyMaterial(MAT_BULB_EMISSIVE);
    bindTexture(TEX_NONE);
    glTranslatef(0.0f, 0.0f, 0.025f);
    drawSphere(0.035f, 8, 6);
    drawBillboardHalo(0.0f, 0.0f, 0.02f, 0.22f, 1.0f, 0.70f, 0.25f, 0.30f);
    // Broken Glass Shards on rim edge
    applyMaterial(MAT_CAR_GLASS);
    glTranslatef(0.08f, -0.06f, 0.01f);
    drawBox(0.04f, 0.06f, 0.015f);
    glPopMatrix();

    // Heavy Front Bumper Bar with Overriders (Bumperettes) & Frame Brackets
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(0.0f, 0.44f, 2.42f);
    drawBeveledBox(2.28f, 0.14f, 0.12f, 0.03f, 2.0f, 0.3f);
    // Frame Mounting Brackets
    glTranslatef(-0.55f, 0.0f, -0.10f);
    drawBox(0.08f, 0.10f, 0.12f);
    glTranslatef(1.10f, 0.0f, 0.0f);
    drawBox(0.08f, 0.10f, 0.12f);
    // Chrome / Rusted Overrider Guards with Rubber Buffer Pads
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glTranslatef(0.0f, 0.06f, 0.14f);
    drawBeveledBox(0.08f, 0.28f, 0.08f, 0.02f);
    glTranslatef(-1.10f, 0.0f, 0.0f);
    drawBeveledBox(0.08f, 0.28f, 0.08f, 0.02f);
    glPopMatrix();

    // Heavy Rear Bumper Bar & Bent Rusted License Plate
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(0.0f, 0.44f, -2.36f);
    drawBeveledBox(2.22f, 0.14f, 0.12f, 0.03f, 2.0f, 0.3f);
    // Rear Overriders
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glTranslatef(-0.55f, 0.05f, -0.04f);
    drawBeveledBox(0.08f, 0.26f, 0.08f, 0.02f);
    glTranslatef(1.10f, 0.0f, 0.0f);
    drawBeveledBox(0.08f, 0.26f, 0.08f, 0.02f);

    // Vintage License Plate hanging askew by one loose bolt ("H0RR0R-70")
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    glTranslatef(-0.55f, 0.02f, -0.05f);
    glRotatef(14.0f, 0.0f, 0.0f, 1.0f); // Tilted askew
    drawBox(0.42f, 0.20f, 0.015f);
    // Dark stamp border on plate
    applyMaterial(MAT_DARK_WOOD);
    drawBox(0.38f, 0.16f, 0.018f);
    glPopMatrix();

    // Red Glass Tail Light Lenses with Chrome Bezels
    for (int i = 0; i < 2; ++i) {
        // Chrome Bezel
        applyMaterial(MAT_CHROME_TRIM);
        bindTexture(TEX_NONE);
        glPushMatrix();
        glTranslatef(headLightX[i], 0.82f, -2.34f);
        drawBox(0.20f, 0.14f, 0.04f);
        // Red Glass Lens
        applyMaterial(MAT_PUMPKIN_SKIN);
        glTranslatef(0.0f, 0.0f, -0.02f);
        drawBox(0.16f, 0.10f, 0.03f);
        glPopMatrix();
    }

    // ------------------------------------------------------------------------
    // E. CABIN, BEVELED ROOF, PILLARS & DARK REFLECTIVE WINDOWS
    // ------------------------------------------------------------------------
    // Tapered Cabin Roof with Projecting Rain Gutters / Drip Rails
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(0.0f, 1.68f, -0.32f);
    drawBeveledBox(1.74f, 0.06f, 2.10f, 0.025f, 1.5f, 1.5f);
    // Left & Right Rain Gutters
    glTranslatef(-0.88f, -0.02f, 0.0f);
    drawBox(0.04f, 0.04f, 2.12f);
    glTranslatef(1.76f, 0.0f, 0.0f);
    drawBox(0.04f, 0.04f, 2.12f);
    glPopMatrix();

    // A-Pillars (Front windshield frame struts, sloped at 34 deg)
    glPushMatrix();
    glTranslatef(-0.84f, 1.34f, 0.44f);
    glRotatef(34.0f, 1.0f, 0.0f, 0.0f);
    drawBeveledBox(0.06f, 0.74f, 0.06f, 0.015f);
    glTranslatef(1.68f, 0.0f, 0.0f);
    drawBeveledBox(0.06f, 0.74f, 0.06f, 0.015f);
    glPopMatrix();

    // B-Pillars (Middle vertical side frame)
    glPushMatrix();
    glTranslatef(-0.84f, 1.34f, -0.32f);
    drawBeveledBox(0.06f, 0.66f, 0.06f, 0.015f);
    glTranslatef(1.68f, 0.0f, 0.0f);
    drawBeveledBox(0.06f, 0.66f, 0.06f, 0.015f);
    glPopMatrix();

    // C-Pillars (Rear window frame struts / sail panels, sloped at -28 deg)
    glPushMatrix();
    glTranslatef(-0.84f, 1.34f, -1.06f);
    glRotatef(-28.0f, 1.0f, 0.0f, 0.0f);
    drawBeveledBox(0.08f, 0.72f, 0.08f, 0.02f);
    glTranslatef(1.68f, 0.0f, 0.0f);
    drawBeveledBox(0.08f, 0.72f, 0.08f, 0.02f);
    glPopMatrix();

    // Front Windshield (Dark reflective tinted glass)
    applyMaterial(MAT_CAR_GLASS);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 1.34f, 0.46f);
    glRotatef(34.0f, 1.0f, 0.0f, 0.0f);
    drawBox(1.60f, 0.65f, 0.035f);

    // Cracked Windshield Spiderweb Fractures (Intricate etched impact lines)
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glColor4f(0.88f, 0.92f, 1.00f, 0.82f);
    glBegin(GL_LINES);
    // Impact epicenter at driver's eye level (-0.36, 0.12)
    float cx = -0.36f, cy = 0.12f, cz = 0.024f;
    // 8 Long Radial Shatter Cracks
    glVertex3f(cx, cy, cz); glVertex3f(cx - 0.42f, cy + 0.24f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx + 0.48f, cy + 0.20f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx - 0.32f, cy - 0.28f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx + 0.38f, cy - 0.26f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx - 0.52f, cy - 0.06f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx + 0.62f, cy - 0.10f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx + 0.18f, cy + 0.30f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx - 0.18f, cy - 0.32f, cz);

    // Inner Concentric Shatter Rings (Shockwave ripples)
    glVertex3f(cx - 0.08f, cy + 0.04f, cz); glVertex3f(cx + 0.06f, cy + 0.08f, cz);
    glVertex3f(cx + 0.06f, cy + 0.08f, cz); glVertex3f(cx + 0.09f, cy - 0.05f, cz);
    glVertex3f(cx + 0.09f, cy - 0.05f, cz); glVertex3f(cx - 0.06f, cy - 0.08f, cz);
    glVertex3f(cx - 0.06f, cy - 0.08f, cz); glVertex3f(cx - 0.08f, cy + 0.04f, cz);

    // Outer Concentric Shatter Ring
    glVertex3f(cx - 0.18f, cy + 0.08f, cz); glVertex3f(cx + 0.14f, cy + 0.16f, cz);
    glVertex3f(cx + 0.14f, cy + 0.16f, cz); glVertex3f(cx + 0.20f, cy - 0.12f, cz);
    glVertex3f(cx + 0.20f, cy - 0.12f, cz); glVertex3f(cx - 0.14f, cy - 0.16f, cz);
    glVertex3f(cx - 0.14f, cy - 0.16f, cz); glVertex3f(cx - 0.18f, cy + 0.08f, cz);
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Windshield Wipers: Driver's wiper frozen mid-sweep, passenger wiper at cowl
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_NONE);
    // Driver Wiper Arm (Frozen halfway up windshield at 48 deg)
    glPushMatrix();
    glTranslatef(-0.38f, 1.14f, 0.72f);
    glRotatef(34.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-48.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.022f, 0.42f, 0.022f); // Arm
    glTranslatef(0.015f, 0.18f, 0.015f);
    drawBox(0.012f, 0.36f, 0.018f); // Blade
    glPopMatrix();

    // Passenger Wiper Arm (Bent/resting on lower cowl)
    glPushMatrix();
    glTranslatef(0.38f, 1.10f, 0.72f);
    glRotatef(34.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-15.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.022f, 0.38f, 0.022f);
    glPopMatrix();

    // Rear Window Glass (Dark reflective sloped glass)
    applyMaterial(MAT_CAR_GLASS);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 1.34f, -1.05f);
    glRotatef(-28.0f, 1.0f, 0.0f, 0.0f);
    drawBox(1.58f, 0.64f, 0.035f);
    glPopMatrix();

    // Right Side Windows (Front passenger & rear quarter glass)
    glPushMatrix();
    glTranslatef(0.85f, 1.34f, 0.02f);
    drawBox(0.035f, 0.58f, 0.62f); // Front right window
    glTranslatef(0.0f, 0.0f, -0.68f);
    drawBox(0.035f, 0.58f, 0.62f); // Rear right window
    glPopMatrix();

    // Left Rear Quarter Window
    glPushMatrix();
    glTranslatef(-0.85f, 1.34f, -0.66f);
    drawBox(0.035f, 0.58f, 0.62f);
    glPopMatrix();

    // Interior Rear-View Mirror hanging from center roof header
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 1.62f, 0.32f);
    drawCylinder(0.015f, 0.015f, 0.08f, 6); // Stem
    glTranslatef(0.0f, -0.06f, 0.0f);
    glRotatef(12.0f, 1.0f, 0.0f, 0.0f);
    drawBeveledBox(0.18f, 0.06f, 0.03f, 0.01f); // Housing
    applyMaterial(MAT_CAR_GLASS);
    glTranslatef(0.0f, 0.0f, 0.016f);
    drawBox(0.16f, 0.045f, 0.01f); // Mirror face
    glPopMatrix();

    // ------------------------------------------------------------------------
    // F. OPEN DRIVER'S DOOR (Ajar at 38 deg) & SNAPPED DANGLING SIDE MIRROR
    // ------------------------------------------------------------------------
    // Open Driver's Door (Swung open at 38 degrees on rusty hinges)
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(-0.95f, 0.68f, 0.44f); // Door hinge pivot on A-pillar
    glRotatef(38.0f, 0.0f, 1.0f, 0.0f);  // Swung outward into the yard
    glTranslatef(0.0f, 0.0f, -0.44f);

    // Upper and Lower Heavy Door Hinges
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.42f);
    drawCylinder(0.03f, 0.03f, 0.06f, 8);
    glTranslatef(0.0f, -0.44f, 0.0f);
    drawCylinder(0.03f, 0.03f, 0.06f, 8);
    glPopMatrix();

    // Outer Lower Door Panel (Beveled sheet metal)
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    drawBeveledBox(0.08f, 0.58f, 0.88f, 0.02f, 0.5f, 1.0f);

    // Chrome Outer Push-Button Door Handle
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.055f, 0.18f, -0.32f);
    drawBox(0.035f, 0.04f, 0.14f);
    drawSphere(0.018f, 8, 6); // Push button
    glPopMatrix();

    // Door Window Frame Border
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glTranslatef(0.0f, 0.52f, 0.0f);
    drawBox(0.06f, 0.48f, 0.06f); // Front vertical post
    glTranslatef(0.0f, 0.0f, -0.82f);
    drawBox(0.06f, 0.48f, 0.06f); // Rear vertical post
    glTranslatef(0.0f, 0.22f, 0.41f);
    drawBox(0.06f, 0.06f, 0.88f); // Top header sash

    // Partially Rolled-Down / Broken Driver's Glass
    applyMaterial(MAT_CAR_GLASS);
    bindTexture(TEX_NONE);
    glTranslatef(0.0f, -0.20f, 0.0f);
    drawBox(0.025f, 0.24f, 0.74f);

    // Inner Door Trim Card (Armrest & interior chrome handle)
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.048f, -0.32f, 0.0f);
    drawBox(0.025f, 0.44f, 0.80f); // Trim panel
    // Molded Armrest
    glTranslatef(0.02f, -0.05f, 0.0f);
    drawBeveledBox(0.05f, 0.08f, 0.35f, 0.015f);
    // Inner Chrome Door Latch Handle & Window Crank
    applyMaterial(MAT_CHROME_TRIM);
    glTranslatef(0.02f, 0.12f, 0.15f);
    drawBox(0.03f, 0.035f, 0.08f);
    glPopMatrix();

    // Snapped Dangling Side Mirror (Torn from bracket, hanging by twisted wire)
    glPushMatrix();
    glTranslatef(-0.06f, -0.24f, 0.40f);
    // Broken mounting bracket stub
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_NONE);
    drawBox(0.03f, 0.04f, 0.04f);

    // Dangling wire lines
    glDisable(GL_LIGHTING);
    glColor3f(0.85f, 0.45f, 0.20f); // Copper wire
    glLineWidth(1.8f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.05f, -0.14f, 0.06f);
    glEnd();
    glEnable(GL_LIGHTING);

    // Dangling Mirror Housing (Hanging askew at 52 deg)
    glTranslatef(-0.05f, -0.14f, 0.06f);
    glRotatef(52.0f, 1.0f, 0.2f, 0.8f);
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawBeveledBox(0.025f, 0.13f, 0.19f, 0.01f); // Chrome housing
    // Mirror Glass Face (High specular reflection)
    applyMaterial(MAT_CAR_GLASS);
    glTranslatef(-0.015f, 0.0f, 0.0f);
    drawBox(0.01f, 0.11f, 0.17f);
    glPopMatrix();

    glPopMatrix(); // End Open Driver's Door

    // ------------------------------------------------------------------------
    // G. DETAILED VINTAGE INTERIOR (Viewable through open door & windows)
    // ------------------------------------------------------------------------
    // Dusty Dashboard with Gauge Cluster & Glovebox
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 1.08f, 0.38f);
    drawBeveledBox(1.68f, 0.22f, 0.42f, 0.02f);
    // Instrument Gauge Binnacle (Speedometer dial & fuel/temp gauge)
    applyMaterial(MAT_DARK_WOOD);
    glPushMatrix();
    glTranslatef(-0.42f, 0.04f, -0.18f);
    drawCylinder(0.07f, 0.07f, 0.04f, 12); // Speedometer housing
    glTranslatef(0.20f, 0.0f, 0.0f);
    drawCylinder(0.05f, 0.05f, 0.04f, 10); // Aux gauge
    // Speedometer needle
    applyMaterial(MAT_PUMPKIN_SKIN);
    glTranslatef(-0.20f, 0.0f, 0.042f);
    drawBox(0.008f, 0.05f, 0.008f);
    glPopMatrix();
    // Glovebox Door with Chrome Button
    applyMaterial(MAT_CAR_INTERIOR);
    glPushMatrix();
    glTranslatef(0.45f, -0.04f, -0.18f);
    drawBox(0.42f, 0.14f, 0.02f);
    applyMaterial(MAT_CHROME_TRIM);
    glTranslatef(0.14f, 0.0f, -0.015f);
    drawSphere(0.015f, 6, 6);
    glPopMatrix();
    glPopMatrix();

    // 3-Spoke Classic Dished Steering Wheel on Tilted Column
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.42f, 1.14f, 0.16f);
    glRotatef(-35.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.028f, 0.028f, 0.28f, 8); // Column
    glTranslatef(0.0f, 0.28f, 0.0f);
    // Wheel Rim
    applyMaterial(MAT_RUSTY_METAL);
    drawSphere(0.19f, 14, 10);
    // 3 Chrome Spokes
    applyMaterial(MAT_CHROME_TRIM);
    for (int s = 0; s < 3; ++s) {
        glPushMatrix();
        glRotatef(s * 120.0f, 0.0f, 1.0f, 0.0f);
        drawBox(0.018f, 0.012f, 0.16f);
        glPopMatrix();
    }
    // Center Horn Button
    drawSphere(0.04f, 8, 8);
    glPopMatrix();

    // Floor Shifter Lever & Foot Pedals
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.08f, 0.58f, 0.05f);
    // Wrinkled Rubber Shift Boot
    applyMaterial(MAT_RUBBER_TYRE);
    drawCylinder(0.08f, 0.03f, 0.08f, 8);
    // Chrome Shifter Lever
    applyMaterial(MAT_CHROME_TRIM);
    glTranslatef(0.0f, 0.08f, 0.0f);
    glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.015f, 0.015f, 0.26f, 6);
    // Spherical Shift Knob
    glTranslatef(0.0f, 0.26f, 0.0f);
    applyMaterial(MAT_DARK_WOOD);
    drawSphere(0.035f, 8, 8);
    glPopMatrix();

    // Suspended Foot Pedals (Clutch, Brake, Accelerator)
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_NONE);
    for (int p = -1; p <= 1; ++p) {
        glPushMatrix();
        glTranslatef(-0.42f + p * 0.10f, 0.62f, 0.35f);
        drawBox(0.045f, 0.065f, 0.02f);
        glPopMatrix();
    }

    // Torn Split-Bench Front Seat (Worn upholstery, exposed yellow foam & rusted springs!)
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_BARK);
    glPushMatrix();
    glTranslatef(0.0f, 0.78f, -0.15f);
    drawBeveledBox(1.64f, 0.24f, 0.56f, 0.03f); // Seat bottom cushion
    glTranslatef(0.0f, 0.28f, -0.24f);
    drawBeveledBox(1.64f, 0.46f, 0.16f, 0.03f); // Backrest

    // Severe Tear on Driver's Seat Cushion (Exposed foam & rusted spring wire)
    applyMaterial(MAT_SEAT_FOAM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.42f, -0.20f, 0.18f);
    drawBox(0.38f, 0.12f, 0.26f); // Exposed foam core
    // Rusted coiled seat springs poking out
    applyMaterial(MAT_RUSTY_METAL);
    for (int sp = 0; sp < 3; ++sp) {
        glPushMatrix();
        glTranslatef((sp - 1) * 0.09f, 0.08f, 0.0f);
        drawCylinder(0.025f, 0.025f, 0.06f, 6);
        glPopMatrix();
    }
    glPopMatrix();
    glPopMatrix();

    // Rear Passenger Bench Seat
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_BARK);
    glPushMatrix();
    glTranslatef(0.0f, 0.78f, -0.92f);
    drawBeveledBox(1.64f, 0.24f, 0.52f, 0.03f); // Rear cushion
    glTranslatef(0.0f, 0.28f, -0.22f);
    drawBeveledBox(1.64f, 0.44f, 0.14f, 0.03f); // Rear backrest
    glPopMatrix();

    // ------------------------------------------------------------------------
    // H. WHEELS & DEFLATED SQUASHED FRONT-RIGHT TYRE
    // ------------------------------------------------------------------------
    float wheelX = 1.02f;
    float wheelZ_front = 1.35f;
    float wheelZ_rear  = -1.35f;

    // 1. Rear-Left Wheel (Inflated)
    glPushMatrix();
    glTranslatef(-wheelX - 0.12f, 0.42f, wheelZ_rear);
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_BARK);
    drawCylinder(0.42f, 0.42f, 0.24f, 16, 1.0f, 1.0f);
    // Rusted steel deep-dish rim & chrome hubcap
    glTranslatef(0.0f, -0.01f, 0.0f);
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    drawCylinder(0.24f, 0.24f, 0.04f, 12, 0.5f, 0.5f);
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawSphere(0.12f, 10, 8);
    glPopMatrix();

    // 2. Rear-Right Wheel (Inflated)
    glPushMatrix();
    glTranslatef(wheelX - 0.12f, 0.42f, wheelZ_rear);
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_BARK);
    drawCylinder(0.42f, 0.42f, 0.24f, 16, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.21f, 0.0f);
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    drawCylinder(0.24f, 0.24f, 0.04f, 12, 0.5f, 0.5f);
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawSphere(0.12f, 10, 8);
    glPopMatrix();

    // 3. Front-Left Wheel (Inflated)
    glPushMatrix();
    glTranslatef(-wheelX - 0.12f, 0.42f, wheelZ_front);
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_BARK);
    drawCylinder(0.42f, 0.42f, 0.24f, 16, 1.0f, 1.0f);
    glTranslatef(0.0f, -0.01f, 0.0f);
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    drawCylinder(0.24f, 0.24f, 0.04f, 12, 0.5f, 0.5f);
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawSphere(0.12f, 10, 8);
    glPopMatrix();

    // 4. Front-Right Wheel (Deflated, Severely Flat & Squashed into Mud Rut)
    glPushMatrix();
    glTranslatef(wheelX - 0.14f, 0.22f, wheelZ_front);
    glScalef(1.36f, 0.44f, 1.28f); // Severely flattened oval pancake squashed tyre
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_BARK);
    drawCylinder(0.42f, 0.42f, 0.28f, 16, 1.0f, 1.0f);
    // Sunken rusted rim resting on flattened rubber
    glTranslatef(0.0f, 0.24f, 0.0f);
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    drawCylinder(0.24f, 0.24f, 0.04f, 12, 0.5f, 0.5f);
    // Dented tarnished hubcap
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawSphere(0.10f, 8, 6);
    glPopMatrix();

    glPopMatrix(); // End Car
}

// 7. Wooden Picket Fence, Cemetery Tombstones & Weathered Crosses (Image 4)
void drawFenceAndYardProps() {
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    for (int i = 0; i < 16; ++i) {
        float z = 24.0f - i * 1.8f;
        float x = -15.0f;
        float y = getTerrainHeight(x, z);
        float rot = std::sin(i * 1.4f) * 8.0f;

        glPushMatrix();
        glTranslatef(x, y + 0.75f, z);
        glRotatef(rot, 0.0f, 0.0f, 1.0f);
        drawBox(0.14f, 1.9f, 0.12f, 0.5f, 1.2f); // Deepened post penetrating ground
        glTranslatef(0.0f, 0.95f, 0.0f);
        drawPrismRoof(0.16f, 0.18f, 0.14f, 0.5f, 0.5f);
        glPopMatrix();
    }

    // Weathered Cemetery Headstones
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    float tombstonePos[4][3] = {
        { 12.0f, 0.0f, 12.0f },
        { 14.5f, 0.0f, 14.5f },
        { 10.5f, 0.0f, 16.0f },
        { 13.0f, 0.0f, 18.5f }
    };
    for (int i = 0; i < 4; ++i) {
        float y = getTerrainHeight(tombstonePos[i][0], tombstonePos[i][2]);
        glPushMatrix();
        glTranslatef(tombstonePos[i][0], y + 0.55f, tombstonePos[i][2]);
        glRotatef(std::sin(i * 2.1f) * 12.0f, 0.0f, 1.0f, 0.0f);
        drawBox(0.7f, 1.4f, 0.22f, 1.0f, 1.0f); // Embedded into soil
        glPopMatrix();
    }

    // Small Weathered Cemetery Crosses on Left Grassy Knoll (Image 4)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    float crossPos[3][3] = {
        { -8.5f, 0.0f, 11.5f },
        { -9.8f, 0.0f, 13.2f },
        { -11.2f, 0.0f, 10.5f }
    };
    for (int c = 0; c < 3; ++c) {
        float cy = getTerrainHeight(crossPos[c][0], crossPos[c][2]);
        glPushMatrix();
        glTranslatef(crossPos[c][0], cy + 0.65f, crossPos[c][2]);
        glRotatef(std::sin(c * 2.8f) * 10.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(std::cos(c * 1.5f) * 6.0f, 0.0f, 0.0f, 1.0f); // slight crooked tilt
        // Vertical post
        drawBox(0.10f, 1.30f, 0.08f);
        // Horizontal crossbeam
        glTranslatef(0.0f, 0.28f, 0.0f);
        drawBox(0.75f, 0.10f, 0.08f);
        glPopMatrix();
    }
}

// 8. Giant Moon with Texture (Elevated higher in the sky directly behind the house tower)
void drawMoonAndStars() {
    bindTexture(TEX_NONE);
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);

    // 1. Cinematic Atmospheric Sky Gradient Backdrop (Navy Zenith -> Mid Teal -> Horizon Desaturated Blue)
    glDepthMask(GL_FALSE);
    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);

    glPushMatrix();
    float skyZ = -70.0f;
    float skyW = 180.0f;
    float skyBottom = -15.0f;
    float skyMid    =  16.0f;
    float skyTop    =  60.0f;

    float flash = g_lightning.flashIntensity;

    // Horizon: Slightly lighter desaturated blue-gray (blends seamlessly into fog)
    float horR = 0.07f + 0.22f * flash;
    float horG = 0.13f + 0.25f * flash;
    float horB = 0.19f + 0.35f * flash;

    // Mid-sky: Dark teal-blue
    float midR = 0.04f + 0.20f * flash;
    float midG = 0.08f + 0.22f * flash;
    float midB = 0.14f + 0.32f * flash;

    // Upper Zenith: Deep navy blue
    float topR = 0.02f + 0.18f * flash;
    float topG = 0.035f + 0.20f * flash;
    float topB = 0.075f + 0.28f * flash;

    glBegin(GL_QUAD_STRIP);
    glColor4f(horR, horG, horB, 1.0f);
    glVertex3f(-skyW, skyBottom, skyZ);
    glVertex3f( skyW, skyBottom, skyZ);

    glColor4f(midR, midG, midB, 1.0f);
    glVertex3f(-skyW, skyMid,    skyZ);
    glVertex3f( skyW, skyMid,    skyZ);

    glColor4f(topR, topG, topB, 1.0f);
    glVertex3f(-skyW, skyTop,    skyZ);
    glVertex3f( skyW, skyTop,    skyZ);
    glEnd();
    glPopMatrix();

    // 2. Subtle Faint Distant Stars in Upper Dark Sky (Non-distracting, high above horizon)
    glPointSize(1.2f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < g_stars.size(); ++i) {
        if (g_stars[i].y < 22.0f) continue; // Keep lower sky completely clear for moon & silhouettes
        float b = g_stars[i].brightness * 0.35f * (0.85f + 0.15f * std::sin(g_time * 2.5f + (float)i));
        glColor4f(0.70f * b, 0.82f * b, 0.95f * b, 0.40f * b);
        glVertex3f(g_stars[i].x, g_stars[i].y, g_stars[i].z);
    }
    glEnd();

    // 3. Giant Luminous Moon (Elevated higher in the night sky behind the house)
    float moonX =   2.5f;
    float moonY =  29.5f; // Elevated higher in the sky behind the house
    float moonZ = -52.0f; // Screen X ≈ 0.54
    float moonRadius = 10.8f;

    // 2.1 Soft Luminous Atmospheric Halo Disc behind the Moon
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive halo glow
    glPushMatrix();
    glTranslatef(moonX, moonY, moonZ);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.70f, 0.82f, 0.96f, 0.42f);
    glVertex3f(0.0f, 0.0f, -0.2f);
    int haloSegments = 36;
    float haloR = moonRadius * 1.50f;
    for (int i = 0; i <= haloSegments; ++i) {
        float th = (float)i * (2.0f * (float)M_PI / (float)haloSegments);
        glColor4f(0.35f, 0.50f, 0.70f, 0.0f);
        glVertex3f(haloR * std::cos(th), haloR * std::sin(th), -0.2f);
    }
    glEnd();
    glPopMatrix();

    // 2.2 Crisp Luminous Full Moon with Lunar Maria & Craters
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE); // Strict depth write so front gothic house tower cleanly occludes it
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    bindTexture(TEX_MOON);

    glPushMatrix();
    glTranslatef(moonX, moonY, moonZ);
    // Draw planar camera-facing circular disc with orthogonal UV mapping for zero distortion
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    int discSegments = 48;
    for (int i = 0; i <= discSegments; ++i) {
        float th = (float)i * (2.0f * (float)M_PI / (float)discSegments);
        float u = 0.5f + 0.5f * std::cos(th);
        float v = 0.5f + 0.5f * std::sin(th);
        glTexCoord2f(u, v);
        glVertex3f(moonRadius * std::cos(th), moonRadius * std::sin(th), 0.0f);
    }
    glEnd();
    glPopMatrix();

    glPopAttrib();
}

// ============================================================================
// DYNAMIC SWAYING BULB LIGHT POOL & SHADOW CASTING
// ============================================================================

// Warm swinging radiant pool on porch floorboards & ground from swaying bulb
void drawBulbLightPool() {
    if (!g_light0PointOn) return;

    bindTexture(TEX_NONE);
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive luminous warm light pool

    // Projected ground and porch coordinates based on swaying bulb
    float swingOffsetX = (g_bulbCurX - g_bulbBaseX) * 0.75f;
    float swingOffsetZ = (g_bulbCurZ - g_bulbBaseZ) * 0.75f;
    float poolX = g_bulbBaseX + swingOffsetX;
    float poolZ = g_bulbBaseZ + swingOffsetZ;
    float poolY = 0.73f; // Just resting on porch floorboards

    // 1. Porch Floor Light Pool Disc
    glPushMatrix();
    glTranslatef(poolX, poolY, poolZ);
    int segments = 24;
    float rCore = 0.95f;
    float rOuter = 2.40f;

    // Core warm hotspot
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 0.82f, 0.35f, 0.42f * g_bulbFlickerFactor);
    glVertex3f(0.0f, 0.005f, 0.0f);
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * (float)M_PI * (float)i / segments;
        glColor4f(1.0f, 0.70f, 0.20f, 0.18f * g_bulbFlickerFactor);
        glVertex3f(rCore * std::cos(theta), 0.005f, rCore * std::sin(theta));
    }
    glEnd();

    // Outer soft falloff ring
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * (float)M_PI * (float)i / segments;
        float ct = std::cos(theta);
        float st = std::sin(theta);
        glColor4f(1.0f, 0.65f, 0.15f, 0.18f * g_bulbFlickerFactor);
        glVertex3f(rCore * ct, 0.005f, rCore * st);
        glColor4f(1.0f, 0.50f, 0.10f, 0.0f);
        glVertex3f(rOuter * ct, 0.005f, rOuter * st);
    }
    glEnd();
    glPopMatrix();

    // 2. Terrain Ground Light Pool beneath Porch Steps
    float groundPoolY = getTerrainHeight(poolX, poolZ + 1.8f) + 0.03f;
    glPushMatrix();
    glTranslatef(poolX, groundPoolY, poolZ + 1.8f);
    float rGround = 3.2f;
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 0.75f, 0.25f, 0.22f * g_bulbFlickerFactor);
    glVertex3f(0.0f, 0.01f, 0.0f);
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * (float)M_PI * (float)i / segments;
        glColor4f(1.0f, 0.60f, 0.15f, 0.0f);
        glVertex3f(rGround * std::cos(theta), 0.01f, rGround * std::sin(theta));
    }
    glEnd();
    glPopMatrix();

    glPopAttrib();
}

// ----------------------------------------------------------------------------
// ANIMATED NOCTURNAL BATS (Solid Black Silhouettes Crossing the Moon)
// ----------------------------------------------------------------------------
void drawBatWing(float side, float flapAngle) {
    glPushMatrix();
    glTranslatef(side * 0.12f, 0.0f, 0.0f);
    glRotatef(side * flapAngle, 0.0f, 1.0f, 0.0f);

    glBegin(GL_TRIANGLES);
    // Upper wing bone / elbow
    glVertex3f(0.0f, 0.15f, 0.0f);
    glVertex3f(side * 0.70f, 0.55f, 0.0f);
    glVertex3f(0.0f, -0.20f, 0.0f);

    // Inner wing membrane under elbow
    glVertex3f(0.0f, -0.20f, 0.0f);
    glVertex3f(side * 0.70f, 0.55f, 0.0f);
    glVertex3f(side * 0.45f, -0.35f, 0.0f);

    // Outer wing bone to high arched wing tip
    glVertex3f(side * 0.70f, 0.55f, 0.0f);
    glVertex3f(side * 1.55f, 0.85f, 0.0f);
    glVertex3f(side * 0.45f, -0.35f, 0.0f);

    // Outer scallop 1
    glVertex3f(side * 0.45f, -0.35f, 0.0f);
    glVertex3f(side * 1.55f, 0.85f, 0.0f);
    glVertex3f(side * 1.10f, -0.45f, 0.0f);

    // Outer wingtip point & scallop 2
    glVertex3f(side * 1.10f, -0.45f, 0.0f);
    glVertex3f(side * 1.55f, 0.85f, 0.0f);
    glVertex3f(side * 1.70f, 0.35f, 0.0f);
    glEnd();

    glPopMatrix();
}

void drawBat(float x, float y, float z, float roll, float flapAngle, float scale = 1.0f) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(roll, 0.0f, 0.0f, 1.0f);
    glScalef(scale, scale, scale);

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    bindTexture(TEX_NONE);
    glColor4f(0.005f, 0.005f, 0.01f, 1.0f); // Solid pitch-black silhouette

    // Torso Body in XY plane
    glPushMatrix();
    glScalef(0.18f, 0.38f, 0.18f);
    drawSphere(1.0f, 8, 6);
    glPopMatrix();

    // Head & Pointed Ears in XY plane
    glPushMatrix();
    glTranslatef(0.0f, 0.32f, 0.0f);
    drawSphere(0.14f, 8, 6);
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.10f, 0.05f, 0.0f);
    glVertex3f(-0.02f, 0.05f, 0.0f);
    glVertex3f(-0.08f, 0.25f, 0.0f);

    glVertex3f(0.02f, 0.05f, 0.0f);
    glVertex3f(0.10f, 0.05f, 0.0f);
    glVertex3f(0.08f, 0.25f, 0.0f);
    glEnd();
    glPopMatrix();

    // Articulated Wings in XY plane
    drawBatWing(-1.0f, flapAngle);
    drawBatWing( 1.0f, flapAngle);

    glPopAttrib();
    glPopMatrix();
}

void drawAllBats() {
    // ------------------------------------------------------------------------
    // FLOCK OF NOCTURNAL BATS FLYING IN FRONT OF THE HOUSE: RIGHT → LEFT
    // Bats sweep across the scene in front of the house (z ≈ 11.5 - 14.5m),
    // fly completely out of the scene on the left, and then reappear after a pause
    // from the right side.
    // ------------------------------------------------------------------------

    struct BatFlockMember {
        float xOffset;     // Horizontal spread in flock
        float baseY;       // Base flight height
        float baseZ;       // Depth in front of house (in front of z=7.0 house)
        float scale;       // World-space scale
        float flapPhase;   // Wing flap phase offset
    };
    static const BatFlockMember BATS[5] = {
        {  0.0f, 5.8f, 12.5f, 0.52f, 0.0f }, // Alpha leader bat
        {  2.2f, 6.4f, 13.5f, 0.46f, 1.8f }, // Upper follower
        {  1.6f, 5.0f, 11.8f, 0.44f, 3.2f }, // Lower follower
        {  3.8f, 6.7f, 14.2f, 0.42f, 4.5f }, // High trailer
        {  4.5f, 4.7f, 12.0f, 0.40f, 2.1f }  // Low trailer
    };

    float startX =  26.0f; // Well off-screen right
    float endX   = -26.0f; // Well off-screen left
    float totalDist = startX - endX;

    float activeDuration = 10.5f; // Seconds to fly across the entire scene
    float pauseDuration  =  5.5f; // Seconds of pause off-screen before returning
    float cyclePeriod    = activeDuration + pauseDuration; // 16.0s total cycle

    float cycleTime = std::fmod(g_time, cyclePeriod);

    // Only render while active in flight
    if (cycleTime < activeDuration) {
        float t = cycleTime / activeDuration;
        float baseX = startX - t * totalDist;

        for (int i = 0; i < 5; ++i) {
            const BatFlockMember& b = BATS[i];
            float bx = baseX + b.xOffset;
            float by = b.baseY + 0.35f * std::sin(g_time * 2.5f + b.flapPhase);
            float bz = b.baseZ;

            // Banking roll in flight direction
            float roll = -16.0f + 6.0f * std::sin(g_time * 1.8f + b.flapPhase);
            // Dynamic wing flap
            float flap = std::sin(g_time * 11.0f + b.flapPhase) * 34.0f;

            drawBat(bx, by, bz, roll, flap, b.scale);
        }
    }
}

// ----------------------------------------------------------------------------
// LAYERED GROUND MIST & ROLLING HORIZON FOG WISPS
// ----------------------------------------------------------------------------
void drawGroundMist() {
    if (!g_fogEnabled) return;

    bindTexture(TEX_NONE);
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    struct MistWispDef {
        float cx, cz;
        float rx, rz;
        float heightOffset;
        float speedX, speedZ;
        float phase;
        float baseAlpha;
    };

    static const MistWispDef WISPS[] = {
        {  -4.5f,  13.5f, 4.5f, 3.2f, 0.18f,  0.18f,  0.08f, 0.0f, 0.14f }, // Front puddle wisp
        {   5.0f,  10.5f, 4.2f, 3.0f, 0.22f,  0.14f, -0.06f, 1.2f, 0.13f }, // Car puddle wisp
        {  -9.5f,   8.2f, 3.8f, 2.8f, 0.16f, -0.12f,  0.10f, 2.4f, 0.12f }, // Left porch puddle wisp
        {  12.5f,  15.0f, 5.2f, 3.8f, 0.25f,  0.16f,  0.05f, 3.1f, 0.15f }, // Graveyard wisp
        {  14.0f,   8.0f, 4.8f, 3.5f, 0.28f,  0.10f, -0.08f, 4.5f, 0.12f }, // Car right flank wisp
        {  -0.5f,  18.0f, 5.5f, 3.4f, 0.20f,  0.15f,  0.07f, 5.2f, 0.14f }, // Entry pathway wisp
        {  -8.0f,  18.5f, 4.5f, 3.2f, 0.24f, -0.10f,  0.12f, 1.8f, 0.11f }, // Left fence wisp
        {   7.5f,  20.0f, 5.0f, 3.6f, 0.22f,  0.12f, -0.09f, 2.9f, 0.13f }, // Right fence wisp
        {  -3.0f,   6.5f, 4.0f, 2.8f, 0.15f,  0.08f,  0.06f, 3.7f, 0.12f }, // Porch front step wisp
        {   8.5f,   3.5f, 5.0f, 3.5f, 0.30f,  0.14f, -0.05f, 4.8f, 0.13f }, // Right manor wisp
        { -12.5f,  12.0f, 4.6f, 3.2f, 0.22f, -0.15f,  0.08f, 0.6f, 0.11f }, // Distant left cemetery wisp
        {   2.5f,  13.0f, 4.4f, 3.0f, 0.18f,  0.16f,  0.06f, 1.5f, 0.12f }  // Mid-yard wisp
    };
    static const int NUM_WISPS = sizeof(WISPS) / sizeof(WISPS[0]);

    int segments = 16;
    for (int w = 0; w < NUM_WISPS; ++w) {
        const MistWispDef& wd = WISPS[w];
        float mx = std::fmod(wd.cx + g_time * wd.speedX + 45.0f, 90.0f) - 45.0f;
        float mz = std::fmod(wd.cz + g_time * wd.speedZ + 45.0f, 90.0f) - 45.0f;
        float my = getTerrainHeight(mx, mz) + wd.heightOffset + 0.06f * std::sin(g_time * 0.5f + wd.phase);

        float breath = 0.80f + 0.20f * std::sin(g_time * 0.45f + wd.phase);
        float alpha = wd.baseAlpha * breath;
        if (g_lightning.flashIntensity > 0.05f) {
            alpha *= (1.0f + g_lightning.flashIntensity * 1.5f); // Mist brightly catches lightning!
        }

        float rx = wd.rx * breath;
        float rz = wd.rz * breath;

        glPushMatrix();
        glTranslatef(mx, my, mz);

        // Render soft multi-segment mist disk with Gaussian edge fade
        glBegin(GL_TRIANGLE_FAN);
        float mistR = 0.08f + 0.25f * g_lightning.flashIntensity;
        float mistG = 0.12f + 0.30f * g_lightning.flashIntensity;
        float mistB = 0.20f + 0.40f * g_lightning.flashIntensity;
        glColor4f(mistR, mistG, mistB, alpha);
        glVertex3f(0.0f, 0.04f, 0.0f);

        for (int i = 0; i <= segments; ++i) {
            float theta = 2.0f * (float)M_PI * (float)i / segments;
            float px = rx * std::cos(theta);
            float pz = rz * std::sin(theta);
            float py = getTerrainHeight(mx + px, mz + pz) - getTerrainHeight(mx, mz);
            glColor4f(mistR * 0.5f, mistG * 0.5f, mistB * 0.5f, 0.0f);
            glVertex3f(px, py + 0.02f, pz);
        }
        glEnd();

        glPopMatrix();
    }

    // 2. Distant Horizon Mist Wisps (Drifting through background tree line)
    for (int d = 0; d < 4; ++d) {
        float dx = -25.0f + d * 16.0f + std::sin(g_time * 0.15f + d) * 4.0f;
        float dz = -18.0f - d * 6.0f;
        float dy = 2.5f + std::sin(g_time * 0.2f + d * 1.5f) * 0.8f;
        float dAlpha = 0.09f * (0.8f + 0.2f * std::sin(g_time * 0.3f + d));

        glPushMatrix();
        glTranslatef(dx, dy, dz);
        glBegin(GL_QUADS);
        glColor4f(0.06f, 0.10f, 0.18f, 0.0f);
        glVertex3f(-9.0f, -1.2f, 0.0f);
        glColor4f(0.06f, 0.10f, 0.18f, dAlpha);
        glVertex3f( 0.0f, -0.4f, 0.0f);
        glColor4f(0.06f, 0.10f, 0.18f, dAlpha * 0.8f);
        glVertex3f( 0.0f,  1.4f, 0.0f);
        glColor4f(0.06f, 0.10f, 0.18f, 0.0f);
        glVertex3f(-9.0f,  1.2f, 0.0f);

        glColor4f(0.06f, 0.10f, 0.18f, dAlpha);
        glVertex3f( 0.0f, -0.4f, 0.0f);
        glColor4f(0.06f, 0.10f, 0.18f, 0.0f);
        glVertex3f( 9.0f, -1.2f, 0.0f);
        glColor4f(0.06f, 0.10f, 0.18f, 0.0f);
        glVertex3f( 9.0f,  1.2f, 0.0f);
        glColor4f(0.06f, 0.10f, 0.18f, dAlpha * 0.8f);
        glVertex3f( 0.0f,  1.4f, 0.0f);
        glEnd();
        glPopMatrix();
    }

    glPopAttrib();
}

// ============================================================================
// PLANAR PROJECTED SHADOW PASS (Moonlight + Moving Porch Bulb Shadows)
// ============================================================================

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

void renderShadowCasters() {
    drawHouse();
    drawAllTrees();
    drawRustedCar(8.2f, 6.8f, -22.0f);
    drawTelephonePole(10.8f, 5.8f, -12.0f);
}

void renderBulbShadowCasters() {
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glPushMatrix();
    glTranslatef(-4.6f + g_houseShiftX, 2.0f, 5.7f + g_houseShiftZ);
    drawBox(6.4f, 2.8f, 0.20f);
    glPopMatrix();
}

void renderPlanarShadows() {
    // 1. Directional Moonlight Shadows
    if (g_light1DirectionalOn) {
        float groundPlane[4] = { 0.0f, 1.0f, 0.0f, -0.005f };
        float shadowMatrix[16];
        buildShadowMatrix(shadowMatrix, groundPlane, g_moonDir);

        bindTexture(TEX_NONE);
        glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(0.01f, 0.02f, 0.04f, 0.55f);

        glPushMatrix();
        glMultMatrixf(shadowMatrix);
        renderShadowCasters();
        glPopMatrix();

        glPopAttrib();
    }

    // 2. Dynamic Point Light Moving Shadows from Swaying Bulb
    if (g_light0PointOn && g_bulbCurY > 1.0f) {
        float bulbPos[4] = { g_bulbCurX, g_bulbCurY, g_bulbCurZ, 1.0f };
        float porchFloorPlane[4] = { 0.0f, 1.0f, 0.0f, -0.725f };
        float porchShadowMat[16];
        buildShadowMatrix(porchShadowMat, porchFloorPlane, bulbPos);

        bindTexture(TEX_NONE);
        glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Warm dark moving shadow on porch deck
        glColor4f(0.02f, 0.015f, 0.01f, 0.48f * g_bulbFlickerFactor);

        glPushMatrix();
        glMultMatrixf(porchShadowMat);
        renderBulbShadowCasters();
        glPopMatrix();

        glPopAttrib();
    }
}

// ============================================================================
// MAIN SCENE RENDERING PIPELINE
// ============================================================================
void render3DScene() {
    // 1. UPDATE LIGHTS & ATMOSPHERE
    float flash = g_lightning.flashIntensity;

    // Atmospheric Fog (Brightens and turns electric lavender during lightning)
    if (g_fogEnabled) {
        glEnable(GL_FOG);
        float fogR = 0.06f + 0.22f * flash;
        float fogG = 0.09f + 0.25f * flash;
        float fogB = 0.14f + 0.35f * flash;
        float curFogColor[4] = { fogR, fogG, fogB, 1.0f };
        glFogfv(GL_FOG_COLOR, curFogColor);
    } else {
        glDisable(GL_FOG);
    }

    // Global Ambient (Spikes during lightning strike)
    float ambR = 0.05f + 0.25f * flash;
    float ambG = 0.06f + 0.28f * flash;
    float ambB = 0.09f + 0.38f * flash;
    float curGlobalAmbient[4] = { ambR, ambG, ambB, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, curGlobalAmbient);

    // Clear Color (Sky backdrop flash)
    glClearColor(0.04f + 0.20f * flash, 0.06f + 0.22f * flash, 0.10f + 0.32f * flash, 1.0f);

    // Light 0: Point Light (Porch Bulb)
    if (g_light0PointOn) {
        glEnable(GL_LIGHT0);
        float pPos[4] = { g_bulbCurX, g_bulbCurY, g_bulbCurZ, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, pPos);

        // Warm golden pool of light from porch bulb
        float pDiff[4] = { 
            0.70f * g_bulbFlickerFactor, 
            0.55f * g_bulbFlickerFactor, 
            0.25f * g_bulbFlickerFactor, 
            1.0f 
        };
        glLightfv(GL_LIGHT0, GL_POSITION, pPos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, pDiff);
    } else {
        glDisable(GL_LIGHT0);
    }

    // Light 1: Directional Moonlight / Overhead Lightning Bolt
    if (g_light1DirectionalOn || flash > 0.05f) {
        glEnable(GL_LIGHT1);
        if (flash > 0.05f) {
            // Intense overhead lightning illumination
            float lDir[4] = { 0.25f, 0.92f, 0.30f, 0.0f };
            float lDiff[4] = { 0.45f + 1.25f * flash, 0.55f + 1.30f * flash, 0.75f + 1.55f * flash, 1.0f };
            glLightfv(GL_LIGHT1, GL_POSITION, lDir);
            glLightfv(GL_LIGHT1, GL_DIFFUSE, lDiff);
        } else {
            float mDiff[4] = { 0.28f, 0.35f, 0.48f, 1.0f };
            glLightfv(GL_LIGHT1, GL_POSITION, g_moonDir);
            glLightfv(GL_LIGHT1, GL_DIFFUSE, mDiff);
        }
    } else {
        glDisable(GL_LIGHT1);
    }

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

    if (g_light2SpotOn) {
        glEnable(GL_LIGHT2);
        float flashPos[4] = { flashPosX, flashPosY, flashPosZ, 1.0f };
        float flashDir[3] = { fx, fy, fz };

        glLightfv(GL_LIGHT2, GL_POSITION, flashPos);
        glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, flashDir);
    } else {
        glDisable(GL_LIGHT2);
    }

    if (g_light3AreaOn) {
        glEnable(GL_LIGHT3);
        float winPos[4] = { -4.6f + g_houseShiftX, 4.6f, 4.85f + g_houseShiftZ, 1.0f };
        glLightfv(GL_LIGHT3, GL_POSITION, winPos);
    } else {
        glDisable(GL_LIGHT3);
    }

    // 2. RENDER 3D SCENE OBJECTS
    drawMoonAndStars();
    drawGround();
    drawHouse();
    drawRustedCar(8.2f, 6.8f, -22.0f);
    drawEnvironmentalClutter();
    drawGraveyardCrosses();
    drawHangingBulb();
    drawPumpkinArray();
    drawAllTrees();
    drawFallingLeaves();
    drawGroundMist();
    drawAllBats();

    // 3. RENDER SHADOWS
    renderPlanarShadows();

    // 4. VOLUMETRIC FLASHLIGHT BEAM & DUST MOTES
    if (g_light2SpotOn) {
        drawVolumetricFlashlightBeam(flashPosX, flashPosY, flashPosZ, fx, fy, fz);
    }
}

// ============================================================================
// 2D HUD, CINEMATIC OVERLAY & TITLE SCREEN
// ============================================================================

void drawString2D(float x, float y, void* font, const char* str, float r, float g, float b, float a = 1.0f) {
    glColor4f(r, g, b, a);
    glRasterPos2f(x, y);
    while (*str) {
        glutBitmapCharacter(font, *str);
        str++;
    }
}

void drawUIPanel(float x, float y, float w, float h, float r, float g, float b, float a) {
    bindTexture(TEX_NONE);
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

    glColor4f(r * 2.2f + 0.15f, g * 2.2f + 0.15f, b * 2.2f + 0.25f, a * 1.5f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();
}

void drawCinematicColorGrade() {
    bindTexture(TEX_NONE);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float w = (float)g_windowWidth;
    float h = (float)g_windowHeight;

    // Subtle cool blue-grey color cast for a cohesive cinematic night grade
    glBegin(GL_QUADS);
    glColor4f(0.035f, 0.065f, 0.135f, 0.13f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(w,    0.0f);
    glVertex2f(w,    h);
    glVertex2f(0.0f, h);
    glEnd();

    // Electric blue-white screen flash during lightning strike
    if (g_lightning.flashIntensity > 0.01f) {
        glBegin(GL_QUADS);
        glColor4f(0.60f, 0.72f, 0.98f, 0.28f * g_lightning.flashIntensity);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(w,    0.0f);
        glVertex2f(w,    h);
        glVertex2f(0.0f, h);
        glEnd();
    }
}

void drawScreenVignette() {
    bindTexture(TEX_NONE);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float w = (float)g_windowWidth;
    float h = (float)g_windowHeight;
    float inset = 95.0f;

    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glVertex2f(0.0f, 0.0f); glVertex2f(w, 0.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(w, inset); glVertex2f(0.0f, inset);

    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, h - inset); glVertex2f(w, h - inset);
    glColor4f(0.0f, 0.0f, 0.0f, 0.85f);
    glVertex2f(w, h); glVertex2f(0.0f, h);

    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glVertex2f(0.0f, 0.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(inset, 0.0f); glVertex2f(inset, h);
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glVertex2f(0.0f, h);

    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(w - inset, 0.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glVertex2f(w, 0.0f); glVertex2f(w, h);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(w - inset, h);
    glEnd();
}

void renderTitleScreen() {
    bindTexture(TEX_NONE);
    float w = (float)g_windowWidth;
    float h = (float)g_windowHeight;

    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    glColor4f(0.015f, 0.02f, 0.04f, 1.0f);
    glVertex2f(0.0f, 0.0f); glVertex2f(w, 0.0f);
    glColor4f(0.035f, 0.015f, 0.02f, 1.0f);
    glVertex2f(w, h); glVertex2f(0.0f, h);
    glEnd();

    drawScreenVignette();

    float cardW = 780.0f;
    float cardH = 475.0f;
    float cardX = (w - cardW) * 0.5f;
    float cardY = (h - cardH) * 0.5f;

    drawUIPanel(cardX, cardY, cardW, cardH, 0.04f, 0.05f, 0.08f, 0.88f);

    drawString2D(cardX + 190.0f, cardY + 55.0f,  GLUT_BITMAP_TIMES_ROMAN_24, "HORROR HOUSE AT NIGHT", 1.0f, 0.35f, 0.15f);
    drawString2D(cardX + 170.0f, cardY + 85.0f,  GLUT_BITMAP_HELVETICA_18,   "Advanced 4-Light & Texture Mapping Demo", 0.85f, 0.85f, 0.95f);

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor4f(0.9f, 0.45f, 0.1f, 0.7f);
    glVertex2f(cardX + 40.0f, cardY + 105.0f);
    glVertex2f(cardX + cardW - 40.0f, cardY + 105.0f);
    glEnd();

    drawString2D(cardX + 50.0f, cardY + 140.0f, GLUT_BITMAP_HELVETICA_12, "PROJECT       : Computer Graphics Sessional (CSE 4-1)", 0.75f, 0.85f, 1.0f);
    drawString2D(cardX + 50.0f, cardY + 165.0f, GLUT_BITMAP_HELVETICA_18, "DEVELOPER     : MD JAHID HASAN JIM", 1.0f, 0.85f, 0.35f);
    drawString2D(cardX + 50.0f, cardY + 190.0f, GLUT_BITMAP_HELVETICA_18, "ROLL NUMBER   : 2107054", 1.0f, 0.85f, 0.35f);

    drawUIPanel(cardX + 40.0f, cardY + 215.0f, cardW - 80.0f, 165.0f, 0.02f, 0.03f, 0.05f, 0.75f);
    drawString2D(cardX + 55.0f, cardY + 238.0f, GLUT_BITMAP_HELVETICA_12, "[Light 1] POINT LIGHT       : Hanging Porch Bulb (w=1.0, Attenuation, Moving Shadow & Pool)", 1.0f, 0.85f, 0.3f);
    drawString2D(cardX + 55.0f, cardY + 260.0f, GLUT_BITMAP_HELVETICA_12, "[Light 2] DIRECTIONAL LIGHT : Moonlight Sky (w=0.0, Low Angle, Casts Planar Shadows)", 0.4f, 0.75f, 1.0f);
    drawString2D(cardX + 55.0f, cardY + 282.0f, GLUT_BITMAP_HELVETICA_12, "[Light 3] SPOT LIGHT        : Flashlight (Positional, 22 deg Soft Cone, Follows Camera)", 0.9f, 0.95f, 1.0f);
    drawString2D(cardX + 55.0f, cardY + 304.0f, GLUT_BITMAP_HELVETICA_12, "[Light 4] AREA LIGHT EMUL.  : Parlor Window Glow (Elevated Ambient Dispersion)", 1.0f, 0.6f, 0.2f);
    drawString2D(cardX + 55.0f, cardY + 326.0f, GLUT_BITMAP_HELVETICA_12, "[Atmosphere] LIVING SCENE   : Drifting Ground Mist, Bats, Wind Sway, Lightning [L] & Audio [M]", 0.3f, 0.9f, 0.9f);
    drawString2D(cardX + 55.0f, cardY + 348.0f, GLUT_BITMAP_HELVETICA_12, "+ GL_FOG Atmosphere, Phong Materials, Carved Jack-o'-Lanterns & Cinematic Camera Tour", 0.6f, 0.9f, 0.6f);

    float pulse = 0.6f + 0.4f * std::sin(g_time * 5.0f);
    drawString2D(cardX + 230.0f, cardY + 425.0f, GLUT_BITMAP_HELVETICA_18, ">> PRESS  [ ENTER ]  OR  [ SPACE ]  TO ENTER <<", 1.0f * pulse, 0.8f * pulse, 0.2f * pulse);
}

void renderSceneHUD() {
    bindTexture(TEX_NONE);
    float w = (float)g_windowWidth;
    float h = (float)g_windowHeight;

    drawCinematicColorGrade();
    drawScreenVignette();

    if (!g_showHUD) return;

    // Crosshair (+)
    glDisable(GL_LIGHTING);
    glLineWidth(1.5f);
    glColor4f(1.0f, 1.0f, 1.0f, 0.35f);
    glBegin(GL_LINES);
    glVertex2f(w * 0.5f - 8.0f, h * 0.5f);
    glVertex2f(w * 0.5f + 8.0f, h * 0.5f);
    glVertex2f(w * 0.5f, h * 0.5f - 8.0f);
    glVertex2f(w * 0.5f, h * 0.5f + 8.0f);
    glEnd();

    // Top Banner
    drawUIPanel(20.0f, 15.0f, 450.0f, 40.0f, 0.04f, 0.05f, 0.08f, 0.80f);
    drawString2D(35.0f, 40.0f, GLUT_BITMAP_HELVETICA_18, "HORROR HOUSE AT NIGHT", 1.0f, 0.4f, 0.15f);
    
    char fpsStr[32];
    snprintf(fpsStr, sizeof(fpsStr), "FPS: %.0f", g_fps);
    drawString2D(380.0f, 40.0f, GLUT_BITMAP_HELVETICA_12, fpsStr, 0.4f, 0.9f, 0.4f);

    // Left Panel: 4-Light Live Status Indicator Card
    drawUIPanel(20.0f, 65.0f, 450.0f, 195.0f, 0.03f, 0.04f, 0.07f, 0.82f);
    drawString2D(35.0f, 88.0f, GLUT_BITMAP_HELVETICA_12, "LIGHTING & ATMOSPHERE STATUS [1, 2, 3, 4, 0, T]:", 0.9f, 0.85f, 0.6f);

    if (g_light0PointOn) {
        drawString2D(35.0f, 110.0f, GLUT_BITMAP_HELVETICA_12, "[1] Point Light (Porch Bulb)  : [ ON ] Warm Amber (Moving Shadow & Pool)", 0.2f, 1.0f, 0.3f);
    } else {
        drawString2D(35.0f, 110.0f, GLUT_BITMAP_HELVETICA_12, "[1] Point Light (Porch Bulb)  : [ OFF ]", 0.7f, 0.2f, 0.2f);
    }

    if (g_light1DirectionalOn) {
        drawString2D(35.0f, 132.0f, GLUT_BITMAP_HELVETICA_12, "[2] Directional (Moonlight)   : [ ON ] Cool Silvery Blue (Shadows)", 0.4f, 0.8f, 1.0f);
    } else {
        drawString2D(35.0f, 132.0f, GLUT_BITMAP_HELVETICA_12, "[2] Directional (Moonlight)   : [ OFF ]", 0.7f, 0.2f, 0.2f);
    }

    if (g_light2SpotOn) {
        drawString2D(35.0f, 154.0f, GLUT_BITMAP_HELVETICA_12, "[3] Spot Light (Flashlight)   : [ ON ] Focused 22 deg Soft Cone [F]", 1.0f, 1.0f, 0.4f);
    } else {
        drawString2D(35.0f, 154.0f, GLUT_BITMAP_HELVETICA_12, "[3] Spot Light (Flashlight)   : [ OFF ] [F]", 0.7f, 0.2f, 0.2f);
    }

    if (g_light3AreaOn) {
        drawString2D(35.0f, 176.0f, GLUT_BITMAP_HELVETICA_12, "[4] Area Light Emul (Window)  : [ ON ] Soft Ambient Dispersion", 1.0f, 0.6f, 0.2f);
    } else {
        drawString2D(35.0f, 176.0f, GLUT_BITMAP_HELVETICA_12, "[4] Area Light Emul (Window)  : [ OFF ]", 0.7f, 0.2f, 0.2f);
    }

    if (g_texturesEnabled) {
        drawString2D(35.0f, 198.0f, GLUT_BITMAP_HELVETICA_12, "[T] Texture Mapping (GL_MOD)  : [ ON ] Wood/Roof/Ground/Stone/Bark/Rust", 0.3f, 0.95f, 0.95f);
    } else {
        drawString2D(35.0f, 198.0f, GLUT_BITMAP_HELVETICA_12, "[T] Texture Mapping           : [ OFF ] Solid Phong Materials", 0.8f, 0.6f, 0.3f);
    }

#ifdef _WIN32
    char featStr[160];
    snprintf(featStr, sizeof(featStr), "Fog: %s [G] | Tour: %s [C] | Audio: %s [M] | Lightning: [L]",
             g_fogEnabled ? "ON" : "OFF",
             g_cinematicMode ? "ACTIVE" : "OFF",
             g_audioEnabled ? "ON" : "MUTED");
    drawString2D(35.0f, 222.0f, GLUT_BITMAP_HELVETICA_12, featStr, 0.8f, 0.8f, 0.9f);
#else
    char featStr[160];
    snprintf(featStr, sizeof(featStr), "Fog: %s [G] | Tour: %s [C] | Lightning: [L]",
             g_fogEnabled ? "ON" : "OFF",
             g_cinematicMode ? "ACTIVE" : "OFF");
    drawString2D(35.0f, 222.0f, GLUT_BITMAP_HELVETICA_12, featStr, 0.8f, 0.8f, 0.9f);
#endif

    if (g_lightning.active) {
        drawString2D(35.0f, 244.0f, GLUT_BITMAP_HELVETICA_12, ">> DISTANT LIGHTNING STRIKE ILLUMINATING SCENE <<", 0.9f, 0.95f, 1.0f);
    }

    // Right Controls Cheat-Sheet
    drawUIPanel(w - 380.0f, 15.0f, 360.0f, 185.0f, 0.03f, 0.04f, 0.07f, 0.80f);
    drawString2D(w - 365.0f, 36.0f,  GLUT_BITMAP_HELVETICA_12, "CONTROLS GUIDE:", 0.9f, 0.85f, 0.6f);
    drawString2D(w - 365.0f, 56.0f,  GLUT_BITMAP_HELVETICA_12, "W, A, S, D     : First-Person Walk / Strafe", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 76.0f,  GLUT_BITMAP_HELVETICA_12, "Mouse Move     : Look Around (Yaw / Pitch)", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 96.0f,  GLUT_BITMAP_HELVETICA_12, "Space / Ctrl   : Fly Up / Fly Down", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 116.0f, GLUT_BITMAP_HELVETICA_12, "1, 2, 3, 4, 0  : Toggle Individual/All Lights", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 134.0f, GLUT_BITMAP_HELVETICA_12, "L: Lightning Strike | M: Audio Mute", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 152.0f, GLUT_BITMAP_HELVETICA_12, "T: Textures | C: Tour | R: Reset View", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 170.0f, GLUT_BITMAP_HELVETICA_12, "P: Screenshot | H: HUD | ESC: Quit", 0.8f, 0.85f, 0.9f);

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
    {  0.0f,   1.2f,  1.35f, 22.0f, -94.0f,   5.0f },
    {  6.0f,  -1.2f,  1.20f, 12.0f, -85.0f,   8.0f },
    { 12.0f,  -2.8f,  2.60f,  6.5f, -75.0f,  18.0f },
    { 18.0f,   3.2f,  3.20f,  8.5f, -95.0f,   2.0f },
    { 24.0f,  10.0f,  1.80f, 10.0f, -145.0f, -4.0f },
    { 30.0f,   8.5f,  7.50f, 20.0f, -120.0f, -16.0f },
    { 36.0f,   1.2f,  1.35f, 22.0f, -94.0f,   5.0f }
};
const int NUM_TOUR_KEYS = sizeof(TOUR_KEYS) / sizeof(TOUR_KEYS[0]);
const float TOTAL_TOUR_DURATION = 36.0f;

void updateCinematicCamera(float dt) {
    g_cinematicTime += dt;
    if (g_cinematicTime >= TOTAL_TOUR_DURATION) {
        g_cinematicTime = std::fmod(g_cinematicTime, TOTAL_TOUR_DURATION);
    }

    int idx = 0;
    for (int i = 0; i < NUM_TOUR_KEYS - 1; ++i) {
        if (g_cinematicTime >= TOUR_KEYS[i].time && g_cinematicTime <= TOUR_KEYS[i + 1].time) {
            idx = i;
            break;
        }
    }

    float segDur = TOUR_KEYS[idx + 1].time - TOUR_KEYS[idx].time;
    float t = (g_cinematicTime - TOUR_KEYS[idx].time) / segDur;
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
    if (g_cinematicMode) return;

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
    if (g_keyState[' '] || g_keyState['e'] || g_keyState['E']) {
        g_cam.y += moveSpeed;
    }
    if (g_isCtrlPressed || g_keyState['x'] || g_keyState['X'] || g_keyState['q'] || g_keyState['Q']) {
        g_cam.y -= moveSpeed;
        if (g_cam.y < 0.4f) g_cam.y = 0.4f;
    }

    // Auto-step height tracking for seamless walking on paths, up porch steps, and into the house
    if (!g_keyState[' '] && !g_keyState['e'] && !g_keyState['E'] && !g_isCtrlPressed && !g_keyState['q']) {
        float surfaceY = getTerrainHeight(g_cam.x, g_cam.z);

        // Porch steps (Z: 7.0 to 8.2, X: -4.5 + g_houseShiftX to -1.0 + g_houseShiftX)
        if (g_cam.z >= 7.0f && g_cam.z <= 8.2f && g_cam.x >= (-4.5f + g_houseShiftX) && g_cam.x <= (-1.0f + g_houseShiftX)) {
            float stepFrac = (8.2f - g_cam.z) / 1.2f;
            surfaceY = std::max(surfaceY, stepFrac * 0.72f);
        }
        // Porch deck (Z: 3.2f to 7.0f, X: -6.4 + g_houseShiftX to 0.8 + g_houseShiftX)
        if (g_cam.z >= 3.2f && g_cam.z < 7.0f && g_cam.x >= (-6.4f + g_houseShiftX) && g_cam.x <= (0.8f + g_houseShiftX)) {
            surfaceY = std::max(surfaceY, 0.74f);
        }
        // House interior floor (Z: -7.0f to 5.2f, X: -9.0 + g_houseShiftX to 7.0 + g_houseShiftX)
        if (g_cam.z >= -7.0f && g_cam.z < 5.2f && g_cam.x >= (-9.0f + g_houseShiftX) && g_cam.x <= (7.0f + g_houseShiftX)) {
            surfaceY = std::max(surfaceY, 0.78f);
        }

        float targetEyeY = surfaceY + 1.65f;
        // Smoothly interpolate eye height
        g_cam.y += (targetEyeY - g_cam.y) * std::min(1.0f, dt * 8.0f);
    }
}

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
        0, 0, 0, 0,
        0, 0, 0, 0,
        54, 0, 0, 0
    };
    int fileSize = 54 + dataSize;
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);

    unsigned char infoHeader[40] = {
        40, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        1, 0,
        24, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
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

// ============================================================================
// GLUT CALLBACK FUNCTIONS
// ============================================================================
void displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (g_appState == STATE_TITLE) {
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
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(50.0, (double)g_windowWidth / (double)g_windowHeight, 0.2, 350.0);

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

        static int debugFrame = 0;
        if (++debugFrame % 60 == 0) {
            std::cout << "[DEBUG] Camera: (" << g_cam.x << ", " << g_cam.y << ", " << g_cam.z 
                      << ") | Yaw: " << g_cam.yaw << " deg, Pitch: " << g_cam.pitch 
                      << " deg | House BBox Center: (-1.0, 3.8, -1.0), Extents: (17.6 wide, 10.0 tall, 13.6 deep)" << std::endl;
        }

        render3DScene();

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

    if (!g_autoScreenshotFile.empty()) {
        g_autoScreenshotFrames++;
        if (g_autoScreenshotFrames >= 3) {
            saveScreenshot(g_autoScreenshotFile.c_str());
            std::cout << "[AUTO SCREENSHOT] Saved to " << g_autoScreenshotFile << std::endl;
            exit(0);
        }
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
    if (g_deltaTime > 0.1f) g_deltaTime = 0.1f;

    g_time += g_deltaTime;

    g_frameCount++;
    g_fpsTimer += g_deltaTime;
    if (g_fpsTimer >= 0.5f) {
        g_fps = g_frameCount / g_fpsTimer;
        g_frameCount = 0;
        g_fpsTimer = 0.0f;
    }

    // 1. Porch Bulb Sway & Flicker
    if (g_bulbAnimEnabled) {
        float f1 = std::sin(g_time * 18.0f);
        float f2 = std::cos(g_time * 33.0f);
        float noise = ((float)(rand() % 100) / 100.0f) * 0.12f;
        g_bulbFlickerFactor = 0.88f + 0.08f * f1 * f2 + noise;
        if (rand() % 95 == 0) g_bulbFlickerFactor = 0.25f;
    } else {
        g_bulbFlickerFactor = 1.0f;
    }

    // 2. Pumpkin Warm Candle Flicker
    g_pumpkinFlicker = 0.85f + 0.15f * std::sin(g_time * 4.5f) * std::cos(g_time * 2.8f);

    // 2.1 Dynamic Falling Leaves Animation (drifting and tumbling down from trees)
    updateFallingLeaves(g_deltaTime);

    // 3. Lightning State Machine & Dynamic Flashes
    g_lightning.timer += g_deltaTime;
    if (!g_lightning.active && g_lightning.timer >= g_lightning.nextStrikeInterval) {
        triggerLightning();
    }

    if (g_lightning.active) {
        g_lightning.strikeProgress += g_deltaTime;
        float prog = g_lightning.strikeProgress;
        float dur = g_lightning.strikeDuration;

        if (prog < 0.08f) {
            // Pulse 1: Pre-flash spike
            g_lightning.flashIntensity = (prog / 0.08f) * 0.70f;
        } else if (prog < 0.13f) {
            // Dark dip
            g_lightning.flashIntensity = 0.15f;
        } else if (prog < 0.28f) {
            // Main blinding lightning strike
            float tMain = (prog - 0.13f) / 0.15f;
            g_lightning.flashIntensity = 1.0f - tMain * 0.35f;
        } else if (prog < 0.36f) {
            // Minor dip
            g_lightning.flashIntensity = 0.35f;
        } else if (prog < 0.48f) {
            // Secondary return stroke
            float tSec = (prog - 0.36f) / 0.12f;
            g_lightning.flashIntensity = 0.75f - tSec * 0.40f;
        } else if (prog < dur) {
            // Smooth trailing dissipation
            float tTail = (prog - 0.48f) / (dur - 0.48f);
            g_lightning.flashIntensity = 0.35f * (1.0f - tTail) * (1.0f - tTail);
        } else {
            g_lightning.active = false;
            g_lightning.flashIntensity = 0.0f;
            g_lightning.timer = 0.0f;
        }

#ifdef _WIN32
        // Delayed Thunder Audio Trigger
        if (g_lightning.thunderPending) {
            g_lightning.thunderCountdown -= g_deltaTime;
            if (g_lightning.thunderCountdown <= 0.0f) {
                g_lightning.thunderPending = false;
                playThunderAudio();
            }
        }
#endif
    } else {
        g_lightning.flashIntensity = 0.0f;
    }

#ifdef _WIN32
    // Audio Loop Recovery after Thunder
    if (g_thunderAudioTimer > 0.0f) {
        g_thunderAudioTimer -= g_deltaTime;
        if (g_thunderAudioTimer <= 0.0f) {
            playAmbientAudio();
        }
    }
#endif

    if (g_cinematicMode) {
        updateCinematicCamera(g_deltaTime);
    } else {
        processKeyboardInput(g_deltaTime);
    }

    glutPostRedisplay();
}

void keyboardDownCallback(unsigned char key, int x, int y) {
    (void)x; (void)y;
    g_keyState[key] = true;

    if (g_appState == STATE_TITLE) {
        if (key == 13 || key == ' ') {
            g_appState = STATE_SCENE;
#ifdef _WIN32
            playAmbientAudio();
#endif
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
        case 't':
        case 'T':
            g_texturesEnabled = !g_texturesEnabled;
            std::cout << "[TEXTURES] Texture Mapping : " << (g_texturesEnabled ? "ON (GL_MODULATE)" : "OFF (Materials Only)") << std::endl;
            break;
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
        case 'l':
        case 'L':
            triggerLightning();
            break;
#ifdef _WIN32
        case 'm':
        case 'M':
            toggleAudio();
            break;
#endif
        case 'p':
        case 'P': {
            static int ssCount = 1;
            char ssName[64];
            snprintf(ssName, sizeof(ssName), "horror_house_%03d.bmp", ssCount++);
            saveScreenshot(ssName);
            break;
        }
        case 'h':
        case 'H':
            g_showHUD = !g_showHUD;
            break;
        case 'r':
        case 'R':
            g_cam.x = 1.2f; g_cam.y = 1.35f; g_cam.z = 22.0f;
            g_cam.yaw = -94.0f; g_cam.pitch = 5.0f;
            g_cinematicMode = false;
            std::cout << "[CAMERA] Reset to Exterior Yard Vantage Point" << std::endl;
            break;
        case 'v':
        case 'V':
        case 'i':
        case 'I': {
            static bool inInterior = false;
            inInterior = !inInterior;
            if (inInterior) {
                // View inside Haunted Interior Room (Matching Reference Panel 2)
                g_cam.x = -7.50f; g_cam.y = 1.60f; g_cam.z = -9.20f;
                g_cam.yaw = -100.0f; g_cam.pitch = -4.0f;
                g_cinematicMode = false;
                std::cout << "[CAMERA] Switched to Haunted Room Interior View" << std::endl;
            } else {
                g_cam.x = 1.2f; g_cam.y = 1.35f; g_cam.z = 22.0f;
                g_cam.yaw = -94.0f; g_cam.pitch = 5.0f;
                g_cinematicMode = false;
                std::cout << "[CAMERA] Reset to Exterior Yard Vantage Point" << std::endl;
            }
            break;
        }
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
#ifdef _WIN32
            playAmbientAudio();
#endif
        } else {
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
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--screenshot" && i + 1 < argc) {
            g_autoScreenshotFile = argv[i + 1];
            g_appState = STATE_SCENE;
            g_showHUD = false;
            i++;
        } else if (std::string(argv[i]) == "--interior") {
            g_cam.x = -7.50f; g_cam.y = 1.60f; g_cam.z = -9.20f;
            g_cam.yaw = -100.0f; g_cam.pitch = -4.0f;
        }
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_INIT_WIDTH, WINDOW_INIT_HEIGHT);
    glutInitWindowPosition(80, 50);
    glutCreateWindow("Horror House at Night - 3D 4-Light Scene (Jim 2107054)");

    initOpenGL();

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
    std::cout << "  [T] Toggle Texture Mapping ON / OFF                     " << std::endl;
    std::cout << "  [G] Toggle Fog                                          " << std::endl;
    std::cout << "  [B] Toggle Bulb Sway & Flicker                          " << std::endl;
    std::cout << "  [C] Toggle Cinematic Auto-Tour Presentation             " << std::endl;
    std::cout << "  [L] Trigger Lightning Strike (Random + Manual)          " << std::endl;
    std::cout << "  [M] Toggle Procedural Ambient Audio                     " << std::endl;
    std::cout << "  [H] Toggle HUD Overlay                                  " << std::endl;
    std::cout << "  [P] Take Screenshot (.bmp)                              " << std::endl;
    std::cout << "  [R] Reset Camera to Reference Image Vantage Point       " << std::endl;
    std::cout << "  [W/A/S/D + Mouse] First-Person Exploration              " << std::endl;
    std::cout << "==========================================================" << std::endl;

    glutMainLoop();
    return 0;
}
