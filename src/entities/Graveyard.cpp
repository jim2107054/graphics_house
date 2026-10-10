#include "Graveyard.h"
#include "Terrain.h"
#include "../graphics/Material.h"
#include "../graphics/TextureManager.h"
#include "../graphics/Primitives.h"
#include <cmath>

void drawDetailedBrokenFenceSection(float x, float z, float rotY) {    float gy = getTerrainHeight(x, z);
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
void drawBrokenFence() {    // 1. Featured Detailed Broken Fence Section (Foreground / Path flank - Image 14)
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


void drawArchedHeadstone(float x, float z, float scale, float rotY, float lean, float leanDir, int stoneType) {    float groundY = getTerrainHeight(x, z);

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
void drawCelticCrossGrave(float x, float z, float scale, float rotY, float lean, float leanDir) {    float groundY = getTerrainHeight(x, z);

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
void drawStoneCrossGrave(float x, float z, float scale, float rotY, float lean, float leanDir) {    float groundY = getTerrainHeight(x, z);

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
void drawStoneSarcophagusGrave(float x, float z, float scale, float rotY, float tilt, bool lidAjar) {    float groundY = getTerrainHeight(x, z);

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
void drawEarthBurialMound(float x, float z, float scale, float rotY) {    float groundY = getTerrainHeight(x, z);

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
void drawGraveyardCrosses() {    // ------------------------------------------------------------------------
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



void drawFenceAndYardProps() {    applyMaterial(MAT_DARK_WOOD);
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

