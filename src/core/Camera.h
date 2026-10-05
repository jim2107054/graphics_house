#pragma once
#include "Config.h"

// Camera State (First Person - Distant 3/4 Corner Perspective View matching Reference)
struct Camera {
    float x, y, z;
    float yaw;
    float pitch;
    float speed;
    float sens;
    float fov;
    float targetFov;
};

extern Camera g_cam;
extern int g_camFloorState;

bool isHouseLocationFree(float worldX, float worldZ);
void processKeyboardInput(float dt);
void updateCinematicCamera(float dt);
