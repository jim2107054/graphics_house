#include "Shadow.h"
#include "Material.h"
#include "TextureManager.h"
#include "Primitives.h"
#include "Lighting.h"
#include "../entities/House.h"
#include "../entities/Vegetation.h"
#include "../entities/Props.h"
#include "../entities/Terrain.h"
#include "../entities/Graveyard.h"
#include "../entities/Creatures.h"
#include "../entities/Clouds.h"

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

// All major environmental objects that cast realistic moonlight shadows
void renderShadowCasters() {
    drawHouse();
    drawAllTrees();
    drawRustedCar(8.2f, 6.8f, -22.0f);
    drawTelephonePole(10.8f, 5.8f, -12.0f);
    drawEnvironmentalClutter();
    drawGraveyardCrosses();
    drawPumpkinArray();
    drawCloudShadowCasters();
}

void renderBulbShadowCasters() {
    float postX[4] = { -7.4f, -5.8f, -3.4f, -1.8f };
    for (int p = 0; p < 4; ++p) {
        glPushMatrix();
        glTranslatef(postX[p], 1.2f, 5.7f);
        drawBox(0.24f, 1.2f, 0.24f);
        glPopMatrix();
    }
}

void renderPlanarShadows() {
    // ------------------------------------------------------------------------
    // 1. DIRECTIONAL MOONLIGHT SHADOWS (Hardware Stencil Projection)
    // Eliminates:
    // - Inverted house reflection / water appearance (Color writes blocked in step 1)
    // - Z-fighting / trembling / moiré ("glitch kore, kape")
    // - Overlapping polygon dark banding
    // ------------------------------------------------------------------------
    if (g_light1DirectionalOn) {
        float groundPlane[4] = { 0.0f, 1.0f, 0.0f, 0.0f }; // Horizontal ground plane
        float shadowMatrix[16];
        buildShadowMatrix(shadowMatrix, groundPlane, g_moonDir);

        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT |
                     GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT |
                     GL_POLYGON_BIT);

        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_FOG);
        bindTexture(TEX_NONE);

        // --- STEP 1: RENDER SHADOW CASTERS INTO STENCIL BUFFER ONLY ---
        // Completely turn off color writing: NO textures, materials, or child colors leak!
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE); // Do NOT alter depth buffer
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // Polygon offset pushes the shadow geometry slightly towards camera in depth
        // Eliminates 100% of Z-fighting with the ground terrain
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-2.0f, -4.0f);

        // Mark stencil buffer = 1 wherever projected shadow fragments pass depth test
        glEnable(GL_STENCIL_TEST);
        glClear(GL_STENCIL_BUFFER_BIT);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glPushMatrix();
        glMultMatrixf(shadowMatrix);
        renderShadowCasters();
        glPopMatrix();

        glDisable(GL_POLYGON_OFFSET_FILL);

        // --- STEP 2: DRAW PURE, UNIFORM MOONLIGHT SHADOW SILHOUETTE ---
        // Restore color writing
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        // Only shade pixels where stencil == 1
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // Lock stencil

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST); // Screen-space overlay, depth was already verified in Step 1

        // Pure, smooth, atmospheric deep moonlight shadow tone (no textures, no reflections!)
        glColor4f(0.008f, 0.012f, 0.024f, 0.58f);

        // Draw 2D viewport quad covering all stenciled shadow pixels
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(1.0f, 0.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(0.0f, 1.0f);
        glEnd();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glPopAttrib();
    }

    // ------------------------------------------------------------------------
    // 2. DYNAMIC POINT LIGHT MOVING SHADOWS FROM SWAYING BULB ON PORCH DECK
    // ------------------------------------------------------------------------
    if (g_light0PointOn && g_bulbCurY > 1.0f) {
        float bulbPos[4] = { g_bulbCurX, g_bulbCurY, g_bulbCurZ, 1.0f };
        float porchFloorPlane[4] = { 0.0f, 1.0f, 0.0f, -0.805f };
        float porchShadowMat[16];
        buildShadowMatrix(porchShadowMat, porchFloorPlane, bulbPos);

        glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT |
                     GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_FOG);
        bindTexture(TEX_NONE);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE); // Don't write depth, prevents z-fighting with deck boards

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-2.0f, -4.0f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Warm dark moving shadow on porch deck
        glColor4f(0.02f, 0.015f, 0.01f, 0.35f * g_bulbFlickerFactor);

        glPushMatrix();
        glMultMatrixf(porchShadowMat);
        renderBulbShadowCasters();
        glPopMatrix();

        glPopAttrib();
    }
}
