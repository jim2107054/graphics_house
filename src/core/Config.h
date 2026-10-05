#pragma once

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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
// CONSTANTS & CONFIGURATION
// ============================================================================
const int WINDOW_INIT_WIDTH  = 1280;
const int WINDOW_INIT_HEIGHT = 720;

enum AppState {
    STATE_TITLE,
    STATE_SCENE
};

extern AppState g_appState;

extern int g_windowWidth;
extern int g_windowHeight;

extern float g_time;
extern float g_deltaTime;
extern int   g_prevTimeMs;
extern int   g_frameCount;
extern float g_fps;
extern float g_fpsTimer;

extern bool g_keyState[256];
extern bool g_isCtrlPressed;
extern int  g_lastMouseX;
extern int  g_lastMouseY;

// House Global Positioning Offset & Orientation
const float g_houseShiftX = -2.8f;
const float g_houseShiftZ = -11.5f;
const float g_houseRotY   = -18.0f;

// Lighting & Feature Switches
extern bool g_light0PointOn;
extern bool g_light1DirectionalOn;
extern bool g_light2SpotOn;
extern bool g_light3AreaOn;
extern bool g_pumpkinLightsOn;

extern bool g_texturesEnabled;
extern bool g_fogEnabled;
extern bool g_bulbAnimEnabled;
extern bool g_showHUD;
extern bool g_cinematicMode;
extern float g_cinematicTime;

extern std::string g_autoScreenshotFile;
extern int g_autoScreenshotFrames;

// Porch bulb dynamic state
extern float g_bulbBaseX;
extern float g_bulbBaseY;
extern float g_bulbBaseZ;
extern float g_bulbCordLength;
extern float g_bulbCurX;
extern float g_bulbCurY;
extern float g_bulbCurZ;
extern float g_bulbFlickerFactor;
extern float g_pumpkinFlicker;

extern float g_moonDir[4];
extern std::string g_tourStageTitle;
