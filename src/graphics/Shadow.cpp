#include "Shadow.h"
#include "Material.h"
#include "TextureManager.h"
#include "Primitives.h"
#include "Lighting.h"
#include "../entities/House.h"
#include "../entities/Vegetation.h"
#include "../entities/Props.h"

void buildShadowMatrix(float shadowMat[16], const float groundPlane[4], const float lightPos[4]) {    float dot = groundPlane[0] * lightPos[0] +
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

void renderShadowCasters() {    drawHouse();
    drawAllTrees();
    drawRustedCar(8.2f, 6.8f, -22.0f);
    drawTelephonePole(10.8f, 5.8f, -12.0f);
}

void renderBulbShadowCasters() {    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glPushMatrix();
    glTranslatef(-4.6f + g_houseShiftX, 2.0f, 5.7f + g_houseShiftZ);
    drawBox(6.4f, 2.8f, 0.20f);
    glPopMatrix();
}

void renderPlanarShadows() {    // 1. Directional Moonlight Shadows
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
