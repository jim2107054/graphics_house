#pragma once
#include "../core/Config.h"

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

extern std::vector<FallingLeafParticle> g_fallingLeaves;

void initFallingLeaves();
void updateFallingLeaves(float dt);
void drawFallingLeaves();
void drawGroundMist();
void drawBatWing(float side, float flapAngle);
void drawBat(float x, float y, float z, float roll, float flapAngle, float scale = 1.0f);
void drawAllBats();
