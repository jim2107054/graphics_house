#include "Lighting.h"
#include "Material.h"
#include "TextureManager.h"
#include "Primitives.h"
#include "../entities/Terrain.h"
#include <cmath>
#include <iostream>

LightningSystem g_lightning = {
    false,
    0.0f,
    12.0f,
    0.0f,
    0.72f,
    0.0f,
    0.0f,
    false
};

void triggerLightning() {    g_lightning.active = true;
    g_lightning.strikeProgress = 0.0f;
    g_lightning.strikeDuration = 0.70f + ((float)(rand() % 25) * 0.01f);
    g_lightning.timer = 0.0f;
    g_lightning.nextStrikeInterval = 14.0f + ((float)(rand() % 140) * 0.1f); // 14s to 28s
    g_lightning.thunderCountdown = 0.35f + ((float)(rand() % 35) * 0.01f);
    g_lightning.thunderPending = true;
    std::cout << "[ATMOSPHERE] Distant Lightning Flash Triggered!" << std::endl;
}


void drawHangingBulb() {    float swayAngleX = 0.0f;
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



void drawBulbLightPool() {    if (!g_light0PointOn) return;

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
