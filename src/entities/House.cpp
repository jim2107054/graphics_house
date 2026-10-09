#include "House.h"
#include "Terrain.h"
#include "../graphics/Material.h"
#include "../graphics/TextureManager.h"
#include "../graphics/Primitives.h"
#include <cmath>
#include <algorithm>

void drawOutdoorCobweb(float x, float y, float z, float size, float rotX, float rotY, float rotZ) {    glPushMatrix();
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


void drawHouseWindow(float x, float y, float z, float width, float height, float rotY, bool hasArch, bool hasCrossMuntin) {    (void)hasArch;
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // 1. Dark outer wooden sill / casing
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.28f, 0.25f, 0.22f, 1.0f);

    // Sill ledge at bottom (protrudes forward)
    glPushMatrix();
    glTranslatef(0.0f, -height * 0.5f - 0.04f, 0.06f);
    drawBox(width + 0.24f, 0.09f, 0.20f);
    glPopMatrix();

    // Top Drip Cap Header
    glPushMatrix();
    glTranslatef(0.0f, height * 0.5f + 0.04f, 0.045f);
    drawBox(width + 0.20f, 0.08f, 0.16f);
    glPopMatrix();

    // Left & Right Side Casings
    glPushMatrix();
    glTranslatef(-width * 0.5f - 0.04f, 0.0f, 0.035f);
    drawBox(0.08f, height + 0.06f, 0.16f);
    glTranslatef(width + 0.08f, 0.0f, 0.0f);
    drawBox(0.08f, height + 0.06f, 0.16f);
    glPopMatrix();

    // 2. Translucent Glass Pane (100% visible outside world from indoors!)
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    bindTexture(TEX_NONE);

    // Subtle moonlit reflection tint with high transparency
    glColor4f(0.72f, 0.85f, 0.98f, 0.18f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.020f);
    drawBox(width, height, 0.008f);
    glPopMatrix();

    glEnable(GL_LIGHTING);

    // 3. Dark Wooden Cross Muntins / Glazing Bars (Cleanly seated in front of glass)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.20f, 0.18f, 1.0f);
    if (hasCrossMuntin) {
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.025f);
        drawBox(width + 0.01f, 0.045f, 0.03f); // Horizontal bar
        drawBox(0.045f, height + 0.01f, 0.03f); // Vertical bar
        glPopMatrix();
    }

    glPopMatrix();
    glPopAttrib();
}

// ----------------------------------------------------------------------------
// HAUNTED INTERIOR COBWEBS (Delicate Radial Web Lines)
// ----------------------------------------------------------------------------
void drawCobweb(float x, float y, float z, float size, float rotY) {    bindTexture(TEX_NONE);
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

// ============================================================================
// REALISTIC VICTORIAN WOODWORK & GOTHIC FURNITURE PROCEDURAL HELPERS
// ============================================================================

// Lathe-turned furniture leg (table leg, desk leg, chair leg)
static void drawTurnedFurnitureLeg(float height, float width, bool broken = false) {
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.19f, 0.15f, 1.0f);

    if (broken) {
        // Jagged splintered stump
        glPushMatrix();
        glTranslatef(0.0f, height * 0.25f, 0.0f);
        drawBox(width, height * 0.5f, width);
        glTranslatef(0.0f, height * 0.25f, 0.0f);
        drawBox(width * 0.45f, height * 0.18f, width * 0.45f);
        glPopMatrix();
        return;
    }

    glPushMatrix();
    // 1. Turned bun foot on floor (y = 0)
    float footR = width * 0.42f;
    glTranslatef(0.0f, footR, 0.0f);
    drawSphere(footR, 10, 8);

    // 2. Slender lower column (grows upward along +Y)
    float shaftH = height * 0.36f;
    drawCylinder(width * 0.30f, width * 0.34f, shaftH, 10);

    // 3. Middle turned ring
    glTranslatef(0.0f, shaftH, 0.0f);
    drawSphere(width * 0.48f, 10, 8);

    // 4. Upper baluster vase (grows upward along +Y)
    float vaseH = height * 0.28f;
    drawCylinder(width * 0.48f, width * 0.36f, vaseH, 10);

    // 5. Upper turned collar bead
    glTranslatef(0.0f, vaseH, 0.0f);
    drawSphere(width * 0.52f, 10, 8);

    // 6. Top square mounting block
    float blockH = height - (footR + shaftH + vaseH);
    if (blockH < height * 0.12f) blockH = height * 0.12f;
    glTranslatef(0.0f, blockH * 0.5f, 0.0f);
    drawBox(width, blockH, width);
    glPopMatrix();
}

// Lathe-turned Victorian staircase and balustrade spindle
static void drawTurnedBalusterSpindle(float height, float width) {
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.19f, 0.15f, 1.0f);

    glPushMatrix();
    // 1. Lower square plinth block (resting on step tread at y = 0)
    float plinthH = height * 0.18f;
    glTranslatef(0.0f, plinthH * 0.5f, 0.0f);
    drawBox(width, plinthH, width);

    // 2. Lower turned transition bead
    glTranslatef(0.0f, plinthH * 0.5f, 0.0f);
    drawSphere(width * 0.56f, 8, 6);

    // 3. Lower baluster vase/bulb (grows upward along +Y)
    float bulbH = height * 0.24f;
    drawCylinder(width * 0.38f, width * 0.48f, bulbH, 8);

    // 4. Center turned ring
    glTranslatef(0.0f, bulbH, 0.0f);
    drawSphere(width * 0.48f, 8, 6);

    // 5. Slender tapered upper column (grows upward along +Y)
    float colH = height * 0.32f;
    drawCylinder(width * 0.32f, width * 0.28f, colH, 8);

    // 6. Upper collar & top square block meeting the handrail
    float topH = height - (plinthH + bulbH + colH);
    if (topH < height * 0.12f) topH = height * 0.12f;
    glTranslatef(0.0f, colH + topH * 0.5f, 0.0f);
    drawBox(width, topH, width);
    glPopMatrix();
}

// Master Victorian Newel Post (Heavy carved post with fluting and finial)
static void drawMasterNewelPost(float height, float width) {
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.22f, 0.17f, 0.13f, 1.0f);

    glPushMatrix();
    // 1. Heavy stepped plinth base
    float baseH = height * 0.26f;
    glTranslatef(0.0f, baseH * 0.5f, 0.0f);
    drawBox(width * 1.15f, baseH, width * 1.15f);
    glTranslatef(0.0f, baseH * 0.5f + 0.02f, 0.0f);
    drawBox(width * 1.25f, 0.04f, width * 1.25f); // Base torus molding

    // 2. Main shaft with recessed fluting relief
    float shaftH = height * 0.56f;
    glTranslatef(0.0f, shaftH * 0.5f + 0.02f, 0.0f);
    drawBox(width, shaftH, width);
    // Subtle front/side panel recesses
    glColor4f(0.18f, 0.14f, 0.10f, 1.0f);
    glPushMatrix(); glTranslatef(0.0f, 0.0f, width * 0.51f); drawBox(width * 0.65f, shaftH * 0.82f, 0.015f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 0.0f, -width * 0.51f); drawBox(width * 0.65f, shaftH * 0.82f, 0.015f); glPopMatrix();
    glPushMatrix(); glTranslatef(width * 0.51f, 0.0f, 0.0f); drawBox(0.015f, shaftH * 0.82f, width * 0.65f); glPopMatrix();
    glPushMatrix(); glTranslatef(-width * 0.51f, 0.0f, 0.0f); drawBox(0.015f, shaftH * 0.82f, width * 0.65f); glPopMatrix();

    // 3. Molded neck & stepped cap
    glColor4f(0.22f, 0.17f, 0.13f, 1.0f);
    glTranslatef(0.0f, shaftH * 0.5f + 0.03f, 0.0f);
    drawBox(width * 1.18f, 0.06f, width * 1.18f);
    glTranslatef(0.0f, 0.05f, 0.0f);
    drawBox(width * 1.30f, 0.05f, width * 1.30f);

    // 4. Carved Acorn / Spherical Finial on collar
    glTranslatef(0.0f, 0.04f, 0.0f);
    drawBox(width * 0.75f, 0.03f, width * 0.75f);
    glTranslatef(0.0f, width * 0.45f, 0.0f);
    drawSphere(width * 0.48f, 12, 10);
    glTranslatef(0.0f, width * 0.38f, 0.0f);
    drawSphere(width * 0.22f, 8, 6); // Acorn tip
    glPopMatrix();
}

// Realistic Victorian Spindle Dining Chair
static void drawRealisticDiningChair(bool overturned, float tiltAngle = 0.0f) {
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.25f, 0.20f, 0.16f, 1.0f);

    glPushMatrix();
    if (overturned) {
        glTranslatef(0.0f, 0.22f, 0.0f);
        glRotatef(88.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(-34.0f, 0.0f, 0.0f, 1.0f);
    } else if (std::abs(tiltAngle) > 0.1f) {
        glRotatef(tiltAngle, 0.0f, 0.0f, 1.0f);
    }

    // 1. Contoured Saddle Seat Pan with beveled rim
    glPushMatrix();
    glTranslatef(0.0f, 0.45f, 0.0f);
    drawBeveledBox(0.48f, 0.045f, 0.46f, 0.015f);
    // Beveled rim trim
    glColor4f(0.20f, 0.16f, 0.12f, 1.0f);
    glTranslatef(0.0f, -0.03f, 0.0f);
    drawBox(0.44f, 0.035f, 0.42f); // Seat apron under-skirt
    glPopMatrix();

    // 2. Front Turned Legs
    glPushMatrix();
    glTranslatef(-0.19f, 0.0f, 0.17f);
    drawTurnedFurnitureLeg(0.43f, 0.044f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.19f, 0.0f, 0.17f);
    drawTurnedFurnitureLeg(0.43f, 0.044f, overturned); // Broken leg if overturned
    glPopMatrix();

    // 3. Rear Saber Legs (Slightly splayed backward for authentic Victorian posture)
    glPushMatrix();
    glTranslatef(-0.18f, 0.21f, -0.17f);
    glRotatef(5.0f, 1.0f, 0.0f, 0.0f);
    drawBox(0.044f, 0.43f, 0.044f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.18f, 0.21f, -0.17f);
    glRotatef(5.0f, 1.0f, 0.0f, 0.0f);
    drawBox(0.044f, 0.43f, 0.044f);
    glPopMatrix();

    // 4. Leg Stretchers (H-Stretcher connecting the legs for stability)
    glColor4f(0.20f, 0.16f, 0.12f, 1.0f);
    glPushMatrix();
    // Left side stretcher
    glTranslatef(-0.185f, 0.16f, 0.0f);
    drawBox(0.024f, 0.024f, 0.34f);
    // Right side stretcher
    glTranslatef(0.37f, 0.0f, 0.0f);
    drawBox(0.024f, 0.024f, 0.34f);
    // Center cross stretcher
    glTranslatef(-0.185f, 0.0f, 0.0f);
    drawBox(0.35f, 0.024f, 0.024f);
    glPopMatrix();

    // 5. Backrest (Gracefully curved crest rail + 5 turned vertical spindles)
    glPushMatrix();
    glTranslatef(0.0f, 0.47f, -0.17f);
    // Two outer stiles
    glTranslatef(-0.19f, 0.25f, 0.0f);
    glRotatef(-3.5f, 0.0f, 0.0f, 1.0f);
    drawBox(0.038f, 0.50f, 0.038f);
    glRotatef(3.5f, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.38f, 0.0f, 0.0f);
    glRotatef(3.5f, 0.0f, 0.0f, 1.0f);
    drawBox(0.038f, 0.50f, 0.038f);
    glRotatef(-3.5f, 0.0f, 0.0f, 1.0f);
    glTranslatef(-0.19f, 0.0f, 0.0f);

    // Carved Top Crest Rail (Arched curve)
    glTranslatef(0.0f, 0.26f, 0.0f);
    drawBox(0.46f, 0.075f, 0.042f);
    glTranslatef(0.0f, 0.045f, 0.0f);
    drawBox(0.24f, 0.035f, 0.038f); // Center carved crown crest

    // 4 Turned backrest spindles
    glTranslatef(0.0f, -0.28f, 0.0f);
    float spindleXs[4] = { -0.12f, -0.04f, 0.04f, 0.12f };
    for (int sp = 0; sp < 4; ++sp) {
        glPushMatrix();
        glTranslatef(spindleXs[sp], 0.0f, 0.0f);
        drawTurnedBalusterSpindle(0.42f, 0.024f);
        glPopMatrix();
    }
    glPopMatrix();

    glPopMatrix();
}

// Antique Leather-bound Book with Ribbed Spine
static void drawAntiqueBook(float width, float height, float thickness, float r, float g, float b, float tiltZ = 0.0f) {
    glPushMatrix();
    if (std::abs(tiltZ) > 0.01f) {
        glRotatef(tiltZ, 0.0f, 0.0f, 1.0f);
    }

    // 1. Text Block (Aged yellowed pages)
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    glColor4f(0.85f, 0.81f, 0.68f, 1.0f);
    glPushMatrix();
    glTranslatef(0.012f, height * 0.5f, 0.0f);
    drawBox(width - 0.02f, height - 0.02f, thickness - 0.012f);
    glPopMatrix();

    // 2. Leather Hardcover Boards (Front, back, and spine)
    applyMaterial(MAT_DARK_WOOD);
    glColor4f(r, g, b, 1.0f);
    // Back cover
    glPushMatrix();
    glTranslatef(0.0f, height * 0.5f, -thickness * 0.5f + 0.005f);
    drawBox(width, height, 0.01f);
    // Front cover
    glTranslatef(0.0f, 0.0f, thickness - 0.01f);
    drawBox(width, height, 0.01f);
    glPopMatrix();

    // 3. Rounded Leather Spine with 4 embossed horizontal ribs
    glPushMatrix();
    glTranslatef(-width * 0.5f + 0.005f, height * 0.5f, 0.0f);
    drawBox(0.012f, height, thickness);
    // Embossed gold/leather ribs on spine
    glColor4f(r * 1.25f, g * 1.2f, b * 1.15f, 1.0f);
    for (int rib = 0; rib < 4; ++rib) {
        float ribY = -height * 0.35f + (float)rib * (height * 0.24f);
        glPushMatrix();
        glTranslatef(-0.005f, ribY, 0.0f);
        drawBox(0.008f, 0.012f, thickness * 0.95f);
        glPopMatrix();
    }
    glPopMatrix();

    glPopMatrix();
}

// Ancient Weathered Human Skull prop
static void drawAntiqueSkull() {
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    glColor4f(0.80f, 0.77f, 0.67f, 1.0f);

    glPushMatrix();
    // Cranium
    drawSphere(0.085f, 12, 10);
    // Facial block / maxilla
    glTranslatef(0.0f, -0.038f, 0.045f);
    drawBox(0.082f, 0.055f, 0.055f);

    // Deep Dark Eye Sockets
    glDisable(GL_LIGHTING);
    glColor3f(0.06f, 0.05f, 0.04f);
    glPushMatrix(); glTranslatef(-0.026f, 0.015f, 0.030f); drawSphere(0.018f, 6, 6); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.026f, 0.015f, 0.030f); drawSphere(0.018f, 6, 6); glPopMatrix();
    // Nasal aperture
    glPushMatrix(); glTranslatef(0.0f, -0.006f, 0.030f); drawSphere(0.011f, 5, 5); glPopMatrix();

    // Teeth row
    glColor3f(0.88f, 0.85f, 0.75f);
    glTranslatef(0.0f, -0.022f, 0.028f);
    drawBox(0.058f, 0.014f, 0.012f);
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

// ----------------------------------------------------------------------------
// HAUNTED HOUSE INTERIOR (Intensely Dilapidated Old Abandoned Interior)
// ----------------------------------------------------------------------------
void drawHouseInterior() {    // 1. Weathered, Rotten Floorboards with Missing Planks & Cracks
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.20f, 0.16f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.8f, 0.74f, 0.5f);
    drawBox(8.2f, 0.08f, 8.4f, 4.0f, 3.0f);
    glPopMatrix();

    // Dark underfloor hole / broken floor gap
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_NONE);
    glColor4f(0.04f, 0.03f, 0.02f, 1.0f);
    glPushMatrix();
    glTranslatef(-3.2f, 0.77f, -0.6f);
    drawBox(1.2f, 0.02f, 0.9f);
    glPopMatrix();

    // Broken loose wooden floor planks scattered on floor
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.28f, 0.24f, 0.20f, 1.0f);
    glPushMatrix();
    glTranslatef(-3.5f, 0.80f, -0.4f);
    glRotatef(25.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(4.0f, 1.0f, 0.0f, 0.0f);
    drawBox(1.10f, 0.04f, 0.20f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.8f, 0.80f, -0.8f);
    glRotatef(-38.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(-3.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.95f, 0.04f, 0.18f);
    glPopMatrix();

    // 2. Interior Walls (Weathered, water-stained rotting wallpaper & decay)
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_WALL);
    glColor4f(0.30f, 0.28f, 0.26f, 1.0f);
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
    glColor4f(0.22f, 0.18f, 0.15f, 1.0f);
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

    // 5. Heavy Exposed Dark Ceiling Timber Beams (One fallen & splintered!)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.18f, 0.15f, 0.12f, 1.0f);
    float beamZs[3] = { 3.5f, 1.5f, -2.5f };
    for (int b = 0; b < 3; ++b) {
        glPushMatrix();
        glTranslatef(-4.8f, 4.08f, beamZs[b]);
        drawBox(8.2f, 0.22f, 0.20f, 2.5f, 0.2f);
        glPopMatrix();
    }
    // Collapsed splintered ceiling beam hanging down across room
    glPushMatrix();
    glTranslatef(-6.2f, 2.6f, -0.4f);
    glRotatef(28.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(12.0f, 0.0f, 1.0f, 0.0f);
    drawBox(4.5f, 0.20f, 0.18f, 2.0f, 0.2f);
    glPopMatrix();

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

    // 7. REALISTIC DILAPIDATED VICTORIAN DINING TABLE (Turned Legs, Plank Grooves, Splintered Break & Melted Wax)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.26f, 0.22f, 0.18f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.8f, 0.74f, 1.7f);
    glRotatef(8.0f, 0.0f, 0.0f, 1.0f); // Tilted due to collapsed right legs

    // Table Apron / Perimeter Skirt Frame
    glPushMatrix();
    glTranslatef(-0.02f, 0.60f, 0.0f);
    // Left & Right apron rails
    glPushMatrix(); glTranslatef(-0.66f, 0.0f, 0.0f); drawBox(0.045f, 0.08f, 0.82f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.62f, 0.0f, 0.0f); drawBox(0.045f, 0.08f, 0.78f); glPopMatrix();
    // Front & Back apron rails
    glPushMatrix(); glTranslatef(0.0f, 0.0f,  0.40f); drawBox(1.32f, 0.08f, 0.045f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 0.0f, -0.40f); drawBox(1.32f, 0.08f, 0.045f); glPopMatrix();
    glPopMatrix();

    // Tabletop (Two fractured halves with plank seam lines)
    // Left intact section
    glPushMatrix();
    glTranslatef(-0.36f, 0.67f, 0.0f);
    drawBeveledBox(0.78f, 0.055f, 0.92f, 0.015f);
    // Plank grooves
    glColor4f(0.18f, 0.15f, 0.12f, 1.0f);
    glPushMatrix(); glTranslatef(0.0f, 0.028f, -0.22f); drawBox(0.78f, 0.005f, 0.012f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 0.028f,  0.22f); drawBox(0.78f, 0.005f, 0.012f); glPopMatrix();
    glPopMatrix();

    // Right fractured tilted section
    glColor4f(0.25f, 0.21f, 0.17f, 1.0f);
    glPushMatrix();
    glTranslatef(0.38f, 0.63f, 0.0f);
    glRotatef(6.5f, 1.0f, 0.0f, 0.0f);
    drawBeveledBox(0.72f, 0.055f, 0.88f, 0.015f);
    // Splintered fracture edges at seam
    glColor4f(0.30f, 0.24f, 0.18f, 1.0f);
    glPushMatrix(); glTranslatef(-0.35f, 0.0f, 0.0f); drawBox(0.04f, 0.05f, 0.84f); glPopMatrix();
    glPopMatrix();

    // Victorian Lathe-Turned Table Legs
    // Intact Left Front Leg
    glPushMatrix();
    glTranslatef(-0.66f, 0.0f, 0.38f);
    drawTurnedFurnitureLeg(0.64f, 0.075f);
    glPopMatrix();

    // Intact Left Rear Leg
    glPushMatrix();
    glTranslatef(-0.66f, 0.0f, -0.38f);
    drawTurnedFurnitureLeg(0.64f, 0.075f);
    glPopMatrix();

    // Broken Snapped Right Legs (splintered stumps)
    glPushMatrix();
    glTranslatef(0.64f, -0.05f, 0.38f);
    drawTurnedFurnitureLeg(0.36f, 0.075f, true);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.64f, -0.05f, -0.38f);
    drawTurnedFurnitureLeg(0.30f, 0.075f, true);
    glPopMatrix();

    glPopMatrix(); // End Table

    // Broken Red Clay Bricks under collapsed table leg
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    glColor4f(0.58f, 0.26f, 0.18f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.1f, 0.79f, 1.4f);
    drawBox(0.24f, 0.09f, 0.14f);
    glTranslatef(0.04f, 0.08f, 0.04f);
    glRotatef(18.0f, 0.0f, 1.0f, 0.0f);
    drawBox(0.22f, 0.08f, 0.13f);
    // Crushed brick shards
    glTranslatef(0.14f, -0.04f, 0.16f);
    drawBox(0.08f, 0.04f, 0.07f);
    glPopMatrix();

    // 8. Melting Wax Candle, Pool of Drippings & Shattered Wine Bottle
    glPushMatrix();
    glTranslatef(-4.95f, 1.43f, 1.55f);
    // Antique brass saucer base with finger loop
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_NONE);
    glColor4f(0.55f, 0.44f, 0.22f, 1.0f);
    drawCylinder(0.07f, 0.045f, 0.025f, 10);
    // Finger loop handle
    glPushMatrix();
    glTranslatef(-0.065f, 0.02f, 0.0f);
    drawCylinder(0.022f, 0.022f, 0.015f, 8);
    glPopMatrix();

    // Melted Wax Pool on table
    glColor4f(0.88f, 0.85f, 0.74f, 0.95f);
    glPushMatrix();
    glTranslatef(0.0f, 0.005f, 0.0f);
    drawCylinder(0.12f, 0.10f, 0.01f, 8);
    // Wax drips running over edge
    glTranslatef(0.08f, -0.02f, 0.0f);
    drawSphere(0.018f, 6, 6);
    glTranslatef(0.02f, -0.03f, 0.0f);
    drawSphere(0.012f, 5, 5);
    glPopMatrix();

    // White wax candle column with melted drips
    glTranslatef(0.0f, 0.025f, 0.0f);
    drawCylinder(0.024f, 0.021f, 0.14f, 8);
    // Wax drip ridges along side of candle
    glPushMatrix();
    glTranslatef(0.016f, 0.05f, 0.005f); drawSphere(0.012f, 6, 6);
    glTranslatef(-0.032f, 0.04f, -0.008f); drawSphere(0.010f, 6, 6);
    glPopMatrix();

    // Flickering candle flame
    glDisable(GL_LIGHTING);
    glTranslatef(0.0f, 0.14f, 0.0f);
    glColor4f(1.0f, 0.65f, 0.15f, 0.95f * intBulbFlicker);
    drawSphere(0.026f, 8, 6);
    glColor4f(1.0f, 0.94f, 0.48f, 1.0f);
    drawSphere(0.013f, 6, 6);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Fallen shattered bottle with dark liquid spill
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-4.2f, 0.80f, 2.1f);
    // Dark liquid puddle stain on floorboards
    glColor4f(0.12f, 0.08f, 0.08f, 0.85f);
    glPushMatrix();
    glTranslatef(0.0f, 0.005f, 0.0f);
    drawCylinder(0.24f, 0.20f, 0.008f, 8);
    glPopMatrix();

    // Green glass bottle body
    glColor4f(0.16f, 0.26f, 0.15f, 1.0f);
    glRotatef(82.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(25.0f, 0.0f, 1.0f, 0.0f);
    drawCylinder(0.042f, 0.042f, 0.18f, 8);
    // Shattered jagged neck
    glTranslatef(0.0f, 0.18f, 0.0f);
    drawCylinder(0.042f, 0.018f, 0.05f, 8);
    // Glass shards
    glTranslatef(0.04f, 0.04f, 0.0f);
    drawBox(0.03f, 0.015f, 0.025f);
    glPopMatrix();

    // 9. REALISTIC VICTORIAN SPINDLE CHAIRS (Contoured Saddle Seat, Turned Legs, H-Stretcher & Spindles)
    // Chair 1: Completely overturned on floor
    glPushMatrix();
    glTranslatef(-3.6f, 0.76f, 1.65f);
    drawRealisticDiningChair(true);
    glPopMatrix();

    // Chair 2: Tilted askew on opposite side
    glPushMatrix();
    glTranslatef(-5.95f, 0.74f, 1.85f);
    glRotatef(82.0f, 0.0f, 1.0f, 0.0f);
    drawRealisticDiningChair(false, 11.5f);
    glPopMatrix();

    // 10. REALISTIC ANTIQUE BOOKSHELF WITH DETAILED MULTI-COLORED BOOKS, SCROLLS & SKULL
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.19f, 0.15f, 1.0f);
    glPushMatrix();
    glTranslatef(-8.20f, 0.74f, -2.20f);
    glRotatef(7.5f, 0.0f, 0.0f, 1.0f); // Leaning precariously against wall
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    // Bookcase Outer Carcase & Architectural Moldings
    glPushMatrix();
    glTranslatef(0.0f, 1.20f, 0.0f);
    // Backing panel with beadboard groove texture
    drawBox(1.42f, 2.40f, 0.035f);
    // Fluted side pilasters
    glTranslatef(-0.69f, 0.0f, 0.20f); drawBox(0.06f, 2.40f, 0.40f);
    glTranslatef( 1.38f, 0.0f, 0.0f);  drawBox(0.06f, 2.40f, 0.40f);
    // Molded base plinth
    glTranslatef(-0.69f, -1.16f, 0.02f); drawBox(1.50f, 0.12f, 0.44f);
    // Top Crown Molding Cornice
    glTranslatef(0.0f, 2.34f, 0.0f); drawBox(1.54f, 0.09f, 0.46f);
    glTranslatef(0.0f, -0.05f, 0.0f); drawBox(1.46f, 0.04f, 0.42f);
    // Fixed Solid Shelves
    glTranslatef(0.0f, -0.55f, -0.02f); drawBox(1.34f, 0.045f, 0.38f); // Shelf 4 (Upper)
    glTranslatef(0.0f, -0.55f,  0.00f); drawBox(1.34f, 0.045f, 0.38f); // Shelf 3 (Middle upper)
    glTranslatef(0.0f, -0.55f,  0.00f); // Shelf 2 (Middle lower - tilted/broken)
    glPushMatrix();
    glRotatef(9.5f, 0.0f, 0.0f, 1.0f);
    drawBox(1.30f, 0.045f, 0.38f);
    glPopMatrix();
    glTranslatef(0.0f, -0.55f, 0.00f); drawBox(1.34f, 0.045f, 0.38f); // Shelf 1 (Bottom)
    glPopMatrix();

    // RICH MULTI-COLORED ANTIQUE BOOKS & OCCULT PROPS ON SHELVES
    // Shelf 1 (Bottom shelf): Heavy tomes, leather folios
    glPushMatrix();
    glTranslatef(-0.48f, 0.24f, 0.18f);
    drawAntiqueBook(0.24f, 0.32f, 0.08f, 0.38f, 0.12f, 0.10f); // Dark red folio
    glTranslatef(0.10f, 0.0f, 0.0f);
    drawAntiqueBook(0.22f, 0.30f, 0.06f, 0.14f, 0.22f, 0.32f); // Deep navy book
    glTranslatef(0.08f, 0.0f, 0.0f);
    drawAntiqueBook(0.23f, 0.28f, 0.07f, 0.15f, 0.25f, 0.16f); // Dark forest green
    glTranslatef(0.12f, 0.0f, 0.0f);
    drawAntiqueBook(0.20f, 0.26f, 0.06f, 0.32f, 0.22f, 0.12f, 16.0f); // Leaning leather book
    // Stack of horizontal books
    glTranslatef(0.28f, 0.0f, 0.0f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    drawAntiqueBook(0.22f, 0.06f, 0.26f, 0.26f, 0.14f, 0.10f);
    glTranslatef(0.07f, 0.0f, 0.0f);
    drawAntiqueBook(0.20f, 0.05f, 0.24f, 0.18f, 0.22f, 0.28f);
    glPopMatrix();
    glPopMatrix();

    // Shelf 2 (Middle broken shelf): ANCIENT HUMAN SKULL & Aged Parchment Scroll
    glPushMatrix();
    glTranslatef(-0.25f, 0.88f, 0.20f);
    glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
    drawAntiqueSkull(); // Creepy Weathered Skull!
    glPopMatrix();

    // Rolled Parchment Scroll tied with string
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    glColor4f(0.85f, 0.80f, 0.62f, 1.0f);
    glPushMatrix();
    glTranslatef(0.28f, 0.84f, 0.20f);
    glRotatef(35.0f, 0.0f, 1.0f, 0.0f);
    drawCylinder(0.035f, 0.035f, 0.26f, 8);
    // Dark ribbon tie
    glColor4f(0.35f, 0.10f, 0.10f, 1.0f);
    glTranslatef(0.0f, 0.12f, 0.0f);
    drawCylinder(0.037f, 0.037f, 0.02f, 8);
    glPopMatrix();

    // Shelf 3: Row of occult grimoires & vintage brass magnifying glass
    glPushMatrix();
    glTranslatef(-0.45f, 1.34f, 0.18f);
    drawAntiqueBook(0.20f, 0.27f, 0.06f, 0.32f, 0.10f, 0.18f);
    glTranslatef(0.08f, 0.0f, 0.0f);
    drawAntiqueBook(0.21f, 0.25f, 0.05f, 0.18f, 0.16f, 0.28f);
    glTranslatef(0.07f, 0.0f, 0.0f);
    drawAntiqueBook(0.22f, 0.28f, 0.08f, 0.22f, 0.18f, 0.12f);
    // Antique brass magnifying glass
    applyMaterial(MAT_RUSTY_METAL);
    glColor4f(0.62f, 0.50f, 0.22f, 1.0f);
    glTranslatef(0.28f, 0.04f, 0.0f);
    glRotatef(75.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.055f, 0.055f, 0.015f, 10); // Brass rim
    glTranslatef(0.0f, -0.08f, 0.0f);
    drawCylinder(0.012f, 0.010f, 0.10f, 6);   // Turned handle
    glPopMatrix();

    // Books fallen on floor in front of bookcase
    glPushMatrix();
    glTranslatef(0.35f, 0.06f, 0.55f);
    drawAntiqueBook(0.24f, 0.06f, 0.18f, 0.35f, 0.15f, 0.10f, 38.0f);
    glTranslatef(0.12f, 0.04f, -0.06f);
    drawAntiqueBook(0.22f, 0.05f, 0.16f, 0.12f, 0.20f, 0.26f, -18.0f);
    glPopMatrix();

    glPopMatrix(); // End Bookcase

    // 11. REALISTIC ANTIQUE GRANDFATHER CLOCK (Swan-neck Pediment, Brass Weights, Dial & Pendulum)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.22f, 0.18f, 0.14f, 1.0f);
    glPushMatrix();
    glTranslatef(-8.35f, 0.74f, 4.20f);
    glRotatef(-40.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(3.5f, 0.0f, 0.0f, 1.0f); // Tilted into haunted corner

    // Stepped Molded Plinth Base with bracket feet
    glPushMatrix();
    glTranslatef(0.0f, 0.16f, 0.0f);
    drawBox(0.64f, 0.32f, 0.44f);
    glTranslatef(0.0f, 0.18f, 0.0f);
    drawBox(0.58f, 0.06f, 0.40f); // Base torus molding
    glPopMatrix();

    // Waist Section (Trunk) with Recessed Door Frame
    glPushMatrix();
    glTranslatef(0.0f, 0.88f, 0.0f);
    drawBox(0.48f, 1.02f, 0.34f);
    // Door molding border
    glColor4f(0.18f, 0.14f, 0.11f, 1.0f);
    glTranslatef(0.0f, 0.0f, 0.165f);
    drawBox(0.36f, 0.88f, 0.02f);
    // Antique dark glass aperture in door
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_NONE);
    glColor4f(0.08f, 0.10f, 0.12f, 0.85f);
    drawBox(0.26f, 0.76f, 0.015f);
    glPopMatrix();

    // Twin Polished Brass Driving Weights hanging on chains inside waist
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_NONE);
    glColor4f(0.68f, 0.54f, 0.24f, 1.0f);
    glPushMatrix();
    // Left driving weight
    glTranslatef(-0.07f, 0.82f, 0.08f);
    drawCylinder(0.030f, 0.030f, 0.24f, 8);
    // Right driving weight
    glTranslatef(0.14f, -0.06f, 0.0f);
    drawCylinder(0.030f, 0.030f, 0.24f, 8);
    glPopMatrix();

    // Crooked Brass Pendulum in waist
    glPushMatrix();
    glTranslatef(0.02f, 1.08f, 0.07f);
    glRotatef(16.0f, 0.0f, 0.0f, 1.0f); // Stuck askew
    drawCylinder(0.012f, 0.012f, 0.62f, 6);
    glTranslatef(0.0f, 0.62f, 0.0f);
    drawSphere(0.078f, 10, 8); // Brass pendulum bob
    glPopMatrix();

    // Clock Bonnet / Hood (Head)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.22f, 0.18f, 0.14f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 1.76f, 0.0f);
    drawBox(0.60f, 0.56f, 0.40f);
    // Fluted side colonnettes flanking dial
    glTranslatef(-0.26f, 0.0f, 0.18f);
    drawCylinder(0.022f, 0.022f, 0.52f, 6);
    glTranslatef( 0.52f, 0.0f, 0.0f);
    drawCylinder(0.022f, 0.022f, 0.52f, 6);
    glPopMatrix();

    // Swan-Neck Broken Arch Pediment with 3 Turned Brass Finials
    glPushMatrix();
    glTranslatef(0.0f, 2.08f, 0.0f);
    drawBox(0.56f, 0.08f, 0.38f);
    glTranslatef(0.0f, 0.07f, 0.0f);
    drawBox(0.44f, 0.06f, 0.36f); // Upper crest step
    // 3 Turned Brass Urn Finials
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_NONE);
    glColor4f(0.68f, 0.54f, 0.24f, 1.0f);
    // Center finial
    glPushMatrix();
    glTranslatef(0.0f, 0.06f, 0.12f);
    drawSphere(0.038f, 8, 6);
    glTranslatef(0.0f, 0.04f, 0.0f);
    drawSphere(0.018f, 6, 6);
    glPopMatrix();
    // Left finial
    glPushMatrix();
    glTranslatef(-0.25f, 0.02f, 0.12f);
    drawSphere(0.030f, 8, 6);
    glPopMatrix();
    // Right finial
    glPushMatrix();
    glTranslatef(0.25f, 0.02f, 0.12f);
    drawSphere(0.030f, 8, 6);
    glPopMatrix();
    glPopMatrix();

    // Clock Face Dial with Brass Bezel, Roman Numeral Ticks & Frozen Hands at Midnight
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    glColor4f(0.86f, 0.82f, 0.68f, 1.0f); // Yellowed parchment face
    glPushMatrix();
    glTranslatef(0.0f, 1.76f, 0.205f);
    drawSphere(0.165f, 14, 10);

    // Circular Brass Bezel Rim
    applyMaterial(MAT_RUSTY_METAL);
    glColor4f(0.65f, 0.52f, 0.22f, 1.0f);
    glPushMatrix();
    drawCylinder(0.175f, 0.175f, 0.018f, 16);
    glPopMatrix();

    // 12 Roman Numeral Hour Marks around perimeter
    glDisable(GL_LIGHTING);
    glColor3f(0.12f, 0.10f, 0.08f);
    glLineWidth(1.8f);
    glBegin(GL_LINES);
    for (int h = 0; h < 12; ++h) {
        float hAngle = (float)h * (2.0f * (float)M_PI / 12.0f);
        float r1 = 0.115f;
        float r2 = 0.145f;
        glVertex3f(r1 * std::sin(hAngle), r1 * std::cos(hAngle), 0.025f);
        glVertex3f(r2 * std::sin(hAngle), r2 * std::cos(hAngle), 0.025f);
    }
    // Broken Brass Clock Hands stopped at Midnight (12:00)!
    glVertex3f(0.0f, 0.0f, 0.03f); glVertex3f(0.005f, 0.11f, 0.03f); // Minute hand
    glVertex3f(0.0f, 0.0f, 0.03f); glVertex3f(0.015f, 0.075f, 0.03f); // Hour hand
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glPopMatrix(); // End Grandfather Clock

    // 12. MASTER REALISTIC WOODEN STAIRCASE (Side Stringers, Bullnose Treads, Turned Balusters, Newel Posts & Under-Stair Panelling)
    int numSteps = 15;
    float stairStartX = -1.35f;
    float stairStartZ =  3.60f;
    float stairEndZ   = -1.40f;
    float stairStartY =  0.74f;
    float stairEndY   =  4.20f;
    float stepWidth   =  1.05f;

    // A. Closed Stringer Carriage Beams (Supporting the steps with positive slope angle)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.20f, 0.16f, 0.12f, 1.0f);

    float totalRunZ   = stairStartZ - stairEndZ;                              // 5.00f (positive distance in Z)
    float totalRiseY  = stairEndY - stairStartY;                              // 3.46f (positive rise in Y)
    float stepDepth   = totalRunZ / (float)numSteps;                          // ~0.3333f per step
    float stepHeight  = totalRiseY / (float)numSteps;                         // ~0.2307f per step
    float stairAngle  = std::atan2(totalRiseY, totalRunZ) * 180.0f / (float)M_PI; // POSITIVE angle (+34.68 deg)
    float stairHyp    = std::sqrt(totalRunZ * totalRunZ + totalRiseY * totalRiseY); // ~6.08f span
    float midY        = (stairStartY + stairEndY) * 0.5f;
    float midZ        = (stairStartZ + stairEndZ) * 0.5f;

    // Outer open-side stringer beam (along x = stairStartX - stepWidth * 0.50f)
    glPushMatrix();
    glTranslatef(stairStartX - stepWidth * 0.50f, midY + stepHeight * 0.5f, midZ);
    glRotatef(stairAngle, 1.0f, 0.0f, 0.0f);
    drawBox(0.06f, 0.28f, stairHyp + 0.15f);
    glPopMatrix();

    // Inner wall-side stringer baseboard trim (along x = stairStartX + stepWidth * 0.50f)
    glPushMatrix();
    glTranslatef(stairStartX + stepWidth * 0.50f, midY + stepHeight * 0.5f, midZ);
    glRotatef(stairAngle, 1.0f, 0.0f, 0.0f);
    drawBox(0.04f, 0.22f, stairHyp + 0.15f);
    glPopMatrix();

    // Smooth under-stair soffit board (Underside ceiling cleanly enclosing stair bottom)
    glPushMatrix();
    glTranslatef(stairStartX, midY - 0.08f, midZ);
    glRotatef(stairAngle, 1.0f, 0.0f, 0.0f);
    drawBox(stepWidth, 0.035f, stairHyp + 0.10f);
    glPopMatrix();

    // B. Steps: Solid Step Blocks, Overhanging Bullnose Treads, and Vertically Aligned Spindles
    for (int s = 0; s < numSteps; ++s) {
        float t = (float)s / (float)(numSteps - 1);
        float sy = stairStartY + (float)s * stepHeight;
        float sz = stairStartZ + t * (stairEndZ - stairStartZ);
        float sx = stairStartX;

        // 1. Solid Step Body Block (Fills entire step volume with zero hollow gaps)
        applyMaterial(MAT_DARK_WOOD);
        bindTexture(TEX_WALL);
        glColor4f(0.24f, 0.20f, 0.16f, 1.0f);
        glPushMatrix();
        glTranslatef(sx, sy + stepHeight * 0.5f, sz);
        drawBox(stepWidth, stepHeight, stepDepth, 1.0f, 0.5f);
        glPopMatrix();

        // 2. Horizontal Tread Board with Bullnose Overhang Nosing
        glColor4f(0.28f, 0.23f, 0.18f, 1.0f);
        glPushMatrix();
        glTranslatef(sx, sy + stepHeight + 0.015f, sz);
        drawBox(stepWidth + 0.05f, 0.035f, stepDepth + 0.05f, 1.0f, 0.4f);
        // Rounded Bullnose front overhang trim
        glColor4f(0.20f, 0.16f, 0.12f, 1.0f);
        glTranslatef(0.0f, -0.01f, (stepDepth + 0.05f) * 0.5f);
        drawBox(stepWidth + 0.06f, 0.02f, 0.03f);
        glPopMatrix();

        // 3. Single Upright Lathe-Turned Spindle resting firmly on the step tread
        glPushMatrix();
        glTranslatef(sx - stepWidth * 0.46f, sy + stepHeight + 0.035f, sz);
        drawTurnedBalusterSpindle(0.78f, 0.035f);
        glPopMatrix();
    }

    // C. Master Victorian Newel Posts at Bottom and Top Landings
    // Bottom Starting Newel Post
    glPushMatrix();
    glTranslatef(stairStartX - stepWidth * 0.46f, stairStartY, stairStartZ + stepDepth * 0.35f);
    drawMasterNewelPost(1.15f, 0.11f);
    glPopMatrix();

    // Top Landing Newel Post
    glPushMatrix();
    glTranslatef(stairStartX - stepWidth * 0.46f, stairEndY, stairEndZ - stepDepth * 0.35f);
    drawMasterNewelPost(1.15f, 0.11f);
    glPopMatrix();

    // D. Continuous Molded Handrail running smoothly at positive stair angle
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.22f, 0.17f, 0.13f, 1.0f);
    glPushMatrix();
    float railMidX = stairStartX - stepWidth * 0.46f;
    float railMidY = midY + stepHeight + 0.035f + 0.78f;
    float railMidZ = midZ;

    glTranslatef(railMidX, railMidY, railMidZ);
    glRotatef(stairAngle, 1.0f, 0.0f, 0.0f);
    // Main handrail body
    drawBox(0.075f, 0.055f, stairHyp + 0.15f);
    // Rounded top crown cap
    glTranslatef(0.0f, 0.025f, 0.0f);
    drawBox(0.055f, 0.020f, stairHyp + 0.15f);
    glPopMatrix();

    // 13. Creepy Vintage Portrait Painting on Left Wall (Hanging crookedly)
    glPushMatrix();
    glTranslatef(-8.78f, 2.40f, 1.60f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glRotatef( 7.5f, 0.0f, 0.0f, 1.0f); // Hanging crookedly from single loose nail
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_NONE);
    glColor4f(0.22f, 0.18f, 0.12f, 1.0f);
    drawBox(0.85f, 1.15f, 0.04f);
    glTranslatef(0.0f, 0.0f, 0.022f);
    glColor4f(0.10f, 0.09f, 0.08f, 1.0f);
    drawBox(0.70f, 1.00f, 0.01f);
    // Glowing eerie eyes in portrait
    glColor4f(0.85f, 0.35f, 0.15f, 0.70f * intBulbFlicker);
    glPushMatrix(); glTranslatef(-0.06f, 0.15f, 0.01f); drawSphere(0.016f, 6, 4); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.06f, 0.15f, 0.01f); drawSphere(0.016f, 6, 4); glPopMatrix();
    glPopMatrix();

    // 14. Dense Cobwebs in Ceiling & Floor Corners
    drawCobweb(-8.75f, 4.10f,  4.65f, 0.95f,   0.0f);
    drawCobweb(-0.85f, 4.10f,  4.65f, 0.85f,  90.0f);
    drawCobweb(-8.75f, 4.10f, -3.65f, 1.10f, -90.0f);
    drawCobweb(-4.60f, 3.50f, -1.15f, 0.65f,   0.0f);
    drawCobweb(-8.20f, 1.80f, -2.00f, 0.55f,  45.0f);
    drawCobweb(-8.35f, 1.50f,  3.90f, 0.60f, -45.0f);
}

// ----------------------------------------------------------------------------
// FULL SECOND FLOOR (DOTOLA) INTERIOR (Walk-in Attic Bedroom & Witchcraft Study)
// ----------------------------------------------------------------------------
void drawSecondFloorInterior() {    // 1. Weathered Second-Floor Floorboards (y = 4.24f) with Stairwell Opening Cutout
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.26f, 0.22f, 0.18f, 1.0f);

    // Main 2nd floor room floor (Left section from left wall to stairwell boundary)
    glPushMatrix();
    glTranslatef(-5.45f, 4.24f, 0.50f);
    drawBox(7.00f, 0.08f, 8.40f, 3.5f, 3.0f);
    glPopMatrix();

    // Rear 2nd floor floor extension (Behind stairwell)
    glPushMatrix();
    glTranslatef(-1.30f, 4.24f, -2.65f);
    drawBox(1.30f, 0.08f, 2.10f, 0.8f, 0.8f);
    glPopMatrix();

    // 2. MASTER REALISTIC BALUSTRADE & GUARDRAIL AROUND 2ND FLOOR STAIRWELL OPENING
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.20f, 0.16f, 1.0f);

    // Left Guardrail Handrail & Base Shoe Rail (along x = -1.95f, from z = 3.80f to z = -1.50f)
    glPushMatrix();
    // Base Shoe Rail on floor
    glTranslatef(-1.95f, 4.29f, 1.15f);
    drawBox(0.07f, 0.035f, 5.30f);
    // Molded Handrail at top
    glTranslatef(0.0f, 0.82f, 0.0f);
    drawBox(0.08f, 0.045f, 5.30f);
    glTranslatef(0.0f, -0.035f, 0.0f);
    drawBox(0.06f, 0.035f, 5.30f); // Sub-rail
    glPopMatrix();

    // Turned Victorian Baluster Spindles along left guardrail edge
    for (float bz = 3.65f; bz >= -1.35f; bz -= 0.32f) {
        glPushMatrix();
        glTranslatef(-1.95f, 4.31f, bz);
        drawTurnedBalusterSpindle(0.78f, 0.034f);
        glPopMatrix();
    }

    // Corner Master Newel Post at front of stair opening (x = -1.95f, z = 3.80f)
    glPushMatrix();
    glTranslatef(-1.95f, 4.24f, 3.80f);
    drawMasterNewelPost(1.22f, 0.125f);
    glPopMatrix();

    // Rear Guardrail Handrail & Base Shoe Rail (across z = -1.50f from x = -1.95f to x = -0.65f)
    glPushMatrix();
    // Base Shoe Rail
    glTranslatef(-1.30f, 4.29f, -1.50f);
    drawBox(1.30f, 0.035f, 0.07f);
    // Molded Handrail
    glTranslatef(0.0f, 0.82f, 0.0f);
    drawBox(1.30f, 0.045f, 0.08f);
    glTranslatef(0.0f, -0.035f, 0.0f);
    drawBox(1.30f, 0.035f, 0.06f);
    glPopMatrix();

    // Turned Baluster Spindles along rear guardrail
    for (float bx = -1.82f; bx <= -0.78f; bx += 0.26f) {
        glPushMatrix();
        glTranslatef(bx, 4.31f, -1.50f);
        drawTurnedBalusterSpindle(0.78f, 0.034f);
        glPopMatrix();
    }

    // 3. Exposed Heavy Timber Roof Trusses & Collar Beams in the Vaulted Ceiling
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.20f, 0.16f, 0.12f, 1.0f);
    float trussZs[3] = { 3.2f, 0.5f, -2.2f };
    for (int t = 0; t < 3; ++t) {
        // Horizontal collar tie beam
        glPushMatrix();
        glTranslatef(-4.8f, 7.20f, trussZs[t]);
        drawBox(6.8f, 0.20f, 0.18f, 2.0f, 0.2f);
        // Vertical king post
        glTranslatef(0.0f, 1.40f, 0.0f);
        drawBox(0.18f, 2.60f, 0.18f);
        // Left sloped rafter
        glPushMatrix();
        glTranslatef(-2.2f, -0.2f, 0.0f);
        glRotatef(48.0f, 0.0f, 0.0f, 1.0f);
        drawBox(0.18f, 3.8f, 0.18f);
        glPopMatrix();
        // Right sloped rafter
        glPushMatrix();
        glTranslatef(2.2f, -0.2f, 0.0f);
        glRotatef(-48.0f, 0.0f, 0.0f, 1.0f);
        drawBox(0.18f, 3.8f, 0.18f);
        glPopMatrix();
        glPopMatrix();
    }

    // 4. REALISTIC GOTHIC ANTIQUE FOUR-POSTER BED (Turned Posts, Arch Tracery, Hanging Drapes & Rumpled Bedding)
    glPushMatrix();
    glTranslatef(-6.80f, 4.24f, 1.80f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.22f, 0.18f, 0.14f, 1.0f);

    // Bed Frame Base Platform with molded perimeter plinth
    glPushMatrix();
    glTranslatef(0.0f, 0.28f, 0.0f);
    drawBeveledBox(1.92f, 0.24f, 2.42f, 0.02f);
    glPopMatrix();

    // 4 Majestic Lathe-Turned Mahogany Bedposts (2.35m tall) with Gothic Spire Finials
    float postBX[4] = { -0.92f,  0.92f, -0.92f,  0.92f };
    float postBZ[4] = { -1.18f, -1.18f,  1.18f,  1.18f };
    for (int p = 0; p < 4; ++p) {
        glPushMatrix();
        glTranslatef(postBX[p], 0.0f, postBZ[p]);

        // Base square plinth
        glTranslatef(0.0f, 0.35f, 0.0f);
        drawBox(0.11f, 0.70f, 0.11f);
        // Turned ring transition
        glTranslatef(0.0f, 0.35f + 0.03f, 0.0f);
        drawSphere(0.065f, 10, 8);
        // Fluted column lower shaft
        glTranslatef(0.0f, 0.40f, 0.0f);
        glPushMatrix();
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, -0.40f);
        drawCylinder(0.048f, 0.048f, 0.80f, 10);
        glPopMatrix();
        // Upper turned baluster bulb
        glTranslatef(0.0f, 0.40f + 0.04f, 0.0f);
        drawSphere(0.062f, 10, 8);
        // Upper column to canopy
        glTranslatef(0.0f, 0.24f, 0.0f);
        drawBox(0.085f, 0.48f, 0.085f);
        // Pointed Gothic Acorn/Spire Finial atop canopy
        glTranslatef(0.0f, 0.28f, 0.0f);
        drawSphere(0.052f, 10, 8);
        glTranslatef(0.0f, 0.06f, 0.0f);
        drawSphere(0.024f, 8, 6);
        glPopMatrix();
    }

    // Heavy Molded Wooden Canopy Tester Rails connecting the 4 bedposts
    glPushMatrix();
    glTranslatef(0.0f, 2.30f, 0.0f);
    glPushMatrix(); glTranslatef( 0.0f, 0.0f, -1.18f); drawBox(1.94f, 0.08f, 0.08f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.0f, 0.0f,  1.18f); drawBox(1.94f, 0.08f, 0.08f); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.92f, 0.0f,   0.0f); drawBox(0.08f, 0.08f, 2.44f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.92f, 0.0f,   0.0f); drawBox(0.08f, 0.08f, 2.44f); glPopMatrix();
    glPopMatrix();

    // Tattered Gothic Velvet Canopy Drapes hanging from top corners
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_NONE);
    glColor4f(0.24f, 0.08f, 0.10f, 0.92f); // Deep tattered burgundy velvet
    // Left rear corner drapes
    glPushMatrix();
    glTranslatef(-0.90f, 1.70f, -1.16f);
    drawBox(0.14f, 1.15f, 0.14f);
    glTranslatef(0.0f, -0.65f, 0.0f);
    drawBox(0.12f, 0.25f, 0.12f); // Frayed tail
    glPopMatrix();
    // Right rear corner drapes
    glPushMatrix();
    glTranslatef( 0.90f, 1.70f, -1.16f);
    drawBox(0.14f, 1.15f, 0.14f);
    glPopMatrix();
    // Front corner drapes
    glPushMatrix();
    glTranslatef(-0.90f, 1.75f, 1.16f);
    drawBox(0.12f, 1.05f, 0.12f);
    glPopMatrix();

    // Carved Pointed Gothic Arch Tracery Headboard
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.20f, 0.16f, 0.12f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.92f, -1.16f);
    drawBox(1.82f, 0.90f, 0.06f);
    // Pointed Arch Tracery crest
    glTranslatef(0.0f, 0.52f, 0.0f);
    drawBox(0.96f, 0.26f, 0.06f);
    glTranslatef(0.0f, 0.16f, 0.0f);
    drawBox(0.42f, 0.14f, 0.06f); // Arch apex
    glPopMatrix();

    // Carved Low Footboard
    glPushMatrix();
    glTranslatef(0.0f, 0.62f, 1.16f);
    drawBox(1.82f, 0.44f, 0.06f);
    glTranslatef(0.0f, 0.24f, 0.0f);
    drawBox(0.60f, 0.10f, 0.06f);
    glPopMatrix();

    // Velvet Burgundy Quilt, Wrinkled Duvet & Rumpled Bedding
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_NONE);
    glColor4f(0.38f, 0.11f, 0.14f, 1.0f); // Gothic crimson velvet
    glPushMatrix();
    glTranslatef(0.0f, 0.52f, 0.10f);
    drawBeveledBox(1.74f, 0.22f, 2.18f, 0.03f);
    // Rumpled Duvet Fold at top
    glTranslatef(0.0f, 0.12f, -0.40f);
    drawBeveledBox(1.70f, 0.08f, 0.50f, 0.02f);
    // Turned-down pale bedsheet lip
    glColor4f(0.72f, 0.70f, 0.64f, 1.0f);
    glTranslatef(0.0f, 0.02f, -0.28f);
    drawBox(1.68f, 0.04f, 0.18f);
    glPopMatrix();

    // Dusty Antique Pillows with Piped Seam Borders
    glColor4f(0.74f, 0.72f, 0.65f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.46f, 0.68f, -0.78f);
    glRotatef(8.0f, 1.0f, 0.0f, 0.0f);
    glScalef(0.68f, 0.18f, 0.44f);
    drawSphere(0.5f, 12, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef( 0.46f, 0.68f, -0.78f);
    glRotatef(8.0f, 1.0f, 0.0f, 0.0f);
    glScalef(0.68f, 0.18f, 0.44f);
    drawSphere(0.5f, 12, 10);
    glPopMatrix();

    glPopMatrix(); // End Four-Poster Bed

    // 5. REALISTIC ALCHEMIST STUDY DESK, CARVED GOTHIC CHAIR & OCCULT ARTIFACTS
    glPushMatrix();
    glTranslatef(-7.20f, 4.24f, -2.40f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.19f, 0.15f, 1.0f);

    // Desktop Solid Slab with Beveled Rim
    glPushMatrix();
    glTranslatef(0.0f, 0.75f, 0.0f);
    drawBeveledBox(1.64f, 0.07f, 0.88f, 0.015f);

    // 3-Drawer Frieze Box underneath desktop
    glTranslatef(0.0f, -0.12f, 0.0f);
    drawBox(1.52f, 0.16f, 0.82f);
    // Antique Brass Drop Handles on the 3 drawers
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_NONE);
    glColor4f(0.65f, 0.52f, 0.22f, 1.0f);
    float handleXs[3] = { -0.50f, 0.0f, 0.50f };
    for (int h = 0; h < 3; ++h) {
        glPushMatrix();
        glTranslatef(handleXs[h], 0.0f, 0.42f);
        drawCylinder(0.018f, 0.018f, 0.015f, 8); // Knob mount
        glTranslatef(0.0f, -0.02f, 0.005f);
        drawBox(0.05f, 0.035f, 0.01f);            // Bail drop handle
        glPopMatrix();
    }
    glPopMatrix();

    // 4 Turned Desk Legs with Cross Stretchers
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.19f, 0.15f, 1.0f);
    float legDX[4] = { -0.70f,  0.70f, -0.70f,  0.70f };
    float legDZ[4] = { -0.36f, -0.36f,  0.36f,  0.36f };
    for (int l = 0; l < 4; ++l) {
        glPushMatrix();
        glTranslatef(legDX[l], 0.0f, legDZ[l]);
        drawTurnedFurnitureLeg(0.66f, 0.065f);
        glPopMatrix();
    }
    // Side and Back Stretchers connecting the desk legs
    glPushMatrix();
    glTranslatef( 0.0f, 0.18f, -0.36f); drawBox(1.40f, 0.03f, 0.03f); glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.70f, 0.18f,  0.0f); drawBox(0.03f, 0.03f, 0.72f); glPopMatrix();
    glPushMatrix();
    glTranslatef( 0.70f, 0.18f,  0.0f); drawBox(0.03f, 0.03f, 0.72f); glPopMatrix();

    // Raised Desktop Letter Gallery / Pigeonhole Cubbies along back of desk
    glPushMatrix();
    glTranslatef(0.0f, 0.94f, -0.30f);
    drawBox(1.58f, 0.32f, 0.24f);
    // Cubby dividers
    glColor4f(0.18f, 0.14f, 0.11f, 1.0f);
    glTranslatef(0.0f, 0.0f, 0.08f);
    drawBox(1.52f, 0.02f, 0.16f); // Horizontal shelf in gallery
    glPopMatrix();

    // Carved Gothic High-Back Armchair at Desk
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.68f);
    glRotatef(12.0f, 0.0f, 1.0f, 0.0f);
    // Tufted Padded Seat Cushion
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_NONE);
    glColor4f(0.32f, 0.12f, 0.14f, 1.0f); // Aged velvet/leather
    glPushMatrix();
    glTranslatef(0.0f, 0.46f, 0.0f);
    drawBeveledBox(0.50f, 0.065f, 0.50f, 0.02f);
    glPopMatrix();
    // 4 Turned Chair Legs
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.19f, 0.15f, 1.0f);
    glPushMatrix(); glTranslatef(-0.21f, 0.0f, -0.21f); drawTurnedFurnitureLeg(0.44f, 0.044f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.21f, 0.0f, -0.21f); drawTurnedFurnitureLeg(0.44f, 0.044f); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.21f, 0.0f,  0.21f); drawTurnedFurnitureLeg(0.44f, 0.044f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.21f, 0.0f,  0.21f); drawTurnedFurnitureLeg(0.44f, 0.044f); glPopMatrix();
    // Carved Gothic Arched Backrest with spires
    glPushMatrix();
    glTranslatef(0.0f, 0.78f, -0.22f);
    drawBox(0.46f, 0.62f, 0.045f);
    glTranslatef(0.0f, 0.35f, 0.0f);
    drawBox(0.30f, 0.16f, 0.045f); // Pointed arch peak
    glPopMatrix();
    // Scrolled Curved Armrests
    glPushMatrix();
    glTranslatef(-0.23f, 0.62f, 0.0f); drawBox(0.045f, 0.035f, 0.44f);
    glTranslatef( 0.46f, 0.00f, 0.0f); drawBox(0.045f, 0.035f, 0.44f);
    glPopMatrix();
    glPopMatrix(); // End Desk Chair

    // OPEN ANCIENT SPELLBOOK / GRIMOIRE with Gilded Brass Corners & Glowing Runes
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    // Heavy leather book cover
    glColor4f(0.36f, 0.14f, 0.10f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.25f, 0.80f, 0.05f);
    glRotatef(-14.0f, 0.0f, 1.0f, 0.0f);
    drawBox(0.48f, 0.035f, 0.36f);

    // Open Parchment Pages Spread (Curved page arch)
    glColor4f(0.86f, 0.81f, 0.66f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.11f, 0.026f, 0.0f);
    glRotatef(5.5f, 0.0f, 0.0f, 1.0f);
    drawBeveledBox(0.22f, 0.024f, 0.32f, 0.008f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef( 0.11f, 0.026f, 0.0f);
    glRotatef(-5.5f, 0.0f, 0.0f, 1.0f);
    drawBeveledBox(0.22f, 0.024f, 0.32f, 0.008f);
    glPopMatrix();

    // Glowing Animated Arcane Glyphs & Pentagram on open pages
    glDisable(GL_LIGHTING);
    float runeGlow = 0.70f + 0.30f * std::sin(g_time * 4.0f);
    glColor4f(0.35f, 0.88f, 1.0f, runeGlow);
    glBegin(GL_LINES);
    // Page runes left
    glVertex3f(-0.18f, 0.045f, -0.10f); glVertex3f(-0.04f, 0.045f, -0.10f);
    glVertex3f(-0.18f, 0.045f, -0.04f); glVertex3f(-0.04f, 0.045f, -0.04f);
    glVertex3f(-0.18f, 0.045f,  0.02f); glVertex3f(-0.04f, 0.045f,  0.02f);
    glVertex3f(-0.18f, 0.045f,  0.08f); glVertex3f(-0.04f, 0.045f,  0.08f);
    // Page runes right & occult pentagram lines
    glVertex3f( 0.04f, 0.045f, -0.10f); glVertex3f( 0.18f, 0.045f, -0.10f);
    glVertex3f( 0.04f, 0.045f, -0.04f); glVertex3f( 0.18f, 0.045f, -0.04f);
    glVertex3f( 0.11f, 0.045f,  0.08f); glVertex3f( 0.15f, 0.045f, -0.02f);
    glVertex3f( 0.15f, 0.045f, -0.02f); glVertex3f( 0.07f, 0.045f,  0.04f);
    glVertex3f( 0.07f, 0.045f,  0.04f); glVertex3f( 0.15f, 0.045f,  0.04f);
    glVertex3f( 0.15f, 0.045f,  0.04f); glVertex3f( 0.07f, 0.045f, -0.02f);
    glVertex3f( 0.07f, 0.045f, -0.02f); glVertex3f( 0.11f, 0.045f,  0.08f);
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix(); // End Grimoire

    // Inkpot with Curved Feather Quill
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    glColor4f(0.18f, 0.16f, 0.14f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.52f, 0.79f, 0.24f);
    drawCylinder(0.035f, 0.025f, 0.05f, 8); // Inkpot
    // White feather quill sticking out at angle
    glColor4f(0.92f, 0.90f, 0.85f, 1.0f);
    glTranslatef(0.0f, 0.04f, 0.0f);
    glRotatef(28.0f, 1.0f, 0.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.22f, 0.0f);
    glEnd();
    // Quill feather vane
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.08f, 0.0f);
    glVertex3f(0.025f, 0.18f, 0.0f);
    glVertex3f(0.0f, 0.22f, 0.0f);
    glEnd();
    glPopMatrix();

    // ORNATE 3-ARM BRASS CANDELABRA WITH WAX DRIPS & 3 FLICKERING CANDLES
    glPushMatrix();
    glTranslatef(0.42f, 0.79f, -0.10f);
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_NONE);
    glColor4f(0.62f, 0.50f, 0.22f, 1.0f); // Antique brass
    // Molded pedestal base
    drawCylinder(0.08f, 0.04f, 0.04f, 8);
    glTranslatef(0.0f, 0.04f, 0.0f);
    drawCylinder(0.02f, 0.015f, 0.22f, 6); // Center column

    // Left curved branch
    glPushMatrix();
    glTranslatef(-0.10f, 0.16f, 0.0f);
    drawBox(0.12f, 0.018f, 0.018f);
    drawCylinder(0.025f, 0.025f, 0.03f, 6);
    glColor4f(0.92f, 0.90f, 0.80f, 1.0f);
    glTranslatef(0.0f, 0.03f, 0.0f);
    drawCylinder(0.016f, 0.015f, 0.10f, 6);
    glDisable(GL_LIGHTING);
    glTranslatef(0.0f, 0.10f, 0.0f);
    float cFlick1 = 0.85f + 0.15f * std::sin(g_time * 8.0f);
    glColor4f(1.0f, 0.65f, 0.10f, 0.95f * cFlick1);
    drawSphere(0.022f * cFlick1, 6, 6);
    glColor4f(1.0f, 0.95f, 0.40f, 1.0f);
    drawSphere(0.010f * cFlick1, 6, 6);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Right curved branch
    glPushMatrix();
    glTranslatef(0.10f, 0.16f, 0.0f);
    applyMaterial(MAT_RUSTY_METAL);
    glColor4f(0.62f, 0.50f, 0.22f, 1.0f);
    drawBox(0.12f, 0.018f, 0.018f);
    drawCylinder(0.025f, 0.025f, 0.03f, 6);
    glColor4f(0.92f, 0.90f, 0.80f, 1.0f);
    glTranslatef(0.0f, 0.03f, 0.0f);
    drawCylinder(0.016f, 0.015f, 0.10f, 6);
    glDisable(GL_LIGHTING);
    glTranslatef(0.0f, 0.10f, 0.0f);
    float cFlick2 = 0.85f + 0.15f * std::cos(g_time * 9.5f);
    glColor4f(1.0f, 0.65f, 0.10f, 0.95f * cFlick2);
    drawSphere(0.022f * cFlick2, 6, 6);
    glColor4f(1.0f, 0.95f, 0.40f, 1.0f);
    drawSphere(0.010f * cFlick2, 6, 6);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Center tall candle
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.0f);
    glColor4f(0.92f, 0.90f, 0.80f, 1.0f);
    drawCylinder(0.016f, 0.015f, 0.14f, 6);
    glDisable(GL_LIGHTING);
    glTranslatef(0.0f, 0.14f, 0.0f);
    float cFlick3 = 0.88f + 0.12f * std::sin(g_time * 11.0f);
    glColor4f(1.0f, 0.65f, 0.10f, 0.95f * cFlick3);
    drawSphere(0.025f * cFlick3, 6, 6);
    glColor4f(1.0f, 0.95f, 0.40f, 1.0f);
    drawSphere(0.012f * cFlick3, 6, 6);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glPopMatrix(); // End Candelabra

    // Glowing Alchemical Potions & Crystal Scrying Orb on Brass Tripod
    glPushMatrix();
    glTranslatef(0.48f, 0.79f, 0.20f);
    // Emerald Potion Bottle
    glDisable(GL_LIGHTING);
    glColor4f(0.20f, 0.95f, 0.35f, 0.85f);
    drawSphere(0.045f, 8, 8);
    glTranslatef(0.0f, 0.05f, 0.0f);
    drawCylinder(0.015f, 0.015f, 0.04f, 6);
    // Ruby Red Elixir Phial
    glTranslatef(0.12f, -0.05f, -0.04f);
    glColor4f(0.95f, 0.18f, 0.25f, 0.85f);
    drawCylinder(0.020f, 0.020f, 0.08f, 6);
    // Mystical Scrying Orb resting on brass tripod claw
    glTranslatef(-0.28f, 0.02f, 0.04f);
    applyMaterial(MAT_RUSTY_METAL);
    glColor4f(0.55f, 0.42f, 0.20f, 1.0f);
    drawCylinder(0.04f, 0.025f, 0.035f, 6); // Tripod claw base
    glDisable(GL_LIGHTING);
    glTranslatef(0.0f, 0.05f, 0.0f);
    float orbPulse = 0.80f + 0.20f * std::sin(g_time * 3.5f);
    glColor4f(0.75f, 0.35f, 0.98f, 0.90f * orbPulse);
    drawSphere(0.058f, 12, 10);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glPopMatrix(); // End Study Desk

    // 6. REALISTIC VINTAGE ROCKING ARMCHAIR (True Curved Rocker Runners, Bentwood Arms & Spindles)
    glPushMatrix();
    glTranslatef(-5.20f, 4.24f, 3.60f);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f); // Facing upper window

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.20f, 0.16f, 1.0f);

    // True Curved Rocker Blades on floorboards (segmented arc runners)
    float runnerXs[2] = { -0.24f, 0.24f };
    for (int r = 0; r < 2; ++r) {
        glPushMatrix();
        glTranslatef(runnerXs[r], 0.0f, 0.0f);
        // Segmented smooth rocker curve
        for (int seg = -5; seg <= 5; ++seg) {
            float z0 = (float)seg * 0.085f;
            float arcY = 0.025f + 0.0035f * (float)(seg * seg);
            glPushMatrix();
            glTranslatef(0.0f, arcY, z0);
            drawBox(0.035f, 0.045f, 0.09f);
            glPopMatrix();
        }
        glPopMatrix();
    }

    // Turned Legs angled into rocker runners with cross stretchers
    glPushMatrix(); glTranslatef(-0.21f, 0.04f, -0.20f); drawTurnedFurnitureLeg(0.38f, 0.042f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.21f, 0.04f, -0.20f); drawTurnedFurnitureLeg(0.38f, 0.042f); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.21f, 0.04f,  0.20f); drawTurnedFurnitureLeg(0.38f, 0.042f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.21f, 0.04f,  0.20f); drawTurnedFurnitureLeg(0.38f, 0.042f); glPopMatrix();
    // Stretchers
    glPushMatrix(); glTranslatef( 0.0f, 0.14f, -0.20f); drawBox(0.42f, 0.025f, 0.025f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.0f, 0.14f,  0.20f); drawBox(0.42f, 0.025f, 0.025f); glPopMatrix();

    // Contoured Chair Seat & Tufted Velvet Cushion
    glPushMatrix();
    glTranslatef(0.0f, 0.42f, 0.0f);
    drawBeveledBox(0.52f, 0.05f, 0.48f, 0.015f);
    // Worn burgundy velvet seat cushion
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_NONE);
    glColor4f(0.35f, 0.12f, 0.15f, 1.0f);
    glTranslatef(0.0f, 0.04f, 0.0f);
    drawBeveledBox(0.46f, 0.04f, 0.42f, 0.02f);
    glPopMatrix();

    // Spindle Backrest with Curved Steam-Bent Crest Rail
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.20f, 0.16f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.45f, -0.19f);
    // Two outer curved back stiles
    glPushMatrix(); glTranslatef(-0.22f, 0.30f, 0.0f); drawBox(0.04f, 0.60f, 0.04f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.22f, 0.30f, 0.0f); drawBox(0.04f, 0.60f, 0.04f); glPopMatrix();
    // Top Arched Crest Rail
    glTranslatef(0.0f, 0.60f, 0.0f);
    drawBox(0.48f, 0.07f, 0.04f);
    // 5 Turned vertical spindles in backrest
    glTranslatef(0.0f, -0.32f, 0.0f);
    float rSpindleX[5] = { -0.15f, -0.075f, 0.0f, 0.075f, 0.15f };
    for (int s = 0; s < 5; ++s) {
        glPushMatrix();
        glTranslatef(rSpindleX[s], 0.0f, 0.0f);
        drawTurnedBalusterSpindle(0.54f, 0.024f);
        glPopMatrix();
    }
    glPopMatrix();

    // Bentwood Scrolled Armrests with Turned Arm Supports
    glPushMatrix();
    // Left armrest
    glTranslatef(-0.23f, 0.64f, 0.02f);
    drawBox(0.045f, 0.035f, 0.42f);
    glTranslatef(0.0f, -0.11f, 0.16f);
    drawTurnedBalusterSpindle(0.22f, 0.030f); // Arm post support
    // Right armrest
    glTranslatef(0.46f, 0.11f, -0.16f);
    drawBox(0.045f, 0.035f, 0.42f);
    glTranslatef(0.0f, -0.11f, 0.16f);
    drawTurnedBalusterSpindle(0.22f, 0.030f);
    glPopMatrix();

    glPopMatrix(); // End Rocking Chair

    // 7. REALISTIC ANTIQUE STORAGE CHEST WITH BARREL-DOMED LID & STUDDED IRON STRAPS
    glPushMatrix();
    glTranslatef(-3.40f, 4.24f, -2.60f);
    glRotatef(25.0f, 0.0f, 1.0f, 0.0f);

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.26f, 0.20f, 0.15f, 1.0f);

    // Chest Body Box with Plank Texture
    glPushMatrix();
    glTranslatef(0.0f, 0.30f, 0.0f);
    drawBeveledBox(1.12f, 0.60f, 0.66f, 0.02f);

    // Authentic Barrel-Vaulted Segmented Domed Lid
    glTranslatef(0.0f, 0.32f, 0.0f);
    for (int arc = -3; arc <= 3; ++arc) {
        float arcFrac = (float)arc / 3.0f;
        float lidZ = arcFrac * 0.31f;
        float lidY = 0.06f * (1.0f - arcFrac * arcFrac);
        glPushMatrix();
        glTranslatef(0.0f, lidY, lidZ);
        drawBox(1.14f, 0.045f, 0.12f);
        glPopMatrix();
    }

    // Heavy Blackened Iron Reinforcing Straps with Raised Rivet Studs
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glColor4f(0.35f, 0.32f, 0.30f, 1.0f);
    float strapXs[2] = { -0.36f, 0.36f };
    for (int st = 0; st < 2; ++st) {
        glPushMatrix();
        glTranslatef(strapXs[st], -0.16f, 0.0f);
        drawBox(0.065f, 0.72f, 0.69f);
        // Stud rivets
        glColor4f(0.45f, 0.40f, 0.36f, 1.0f);
        glTranslatef(0.0f, 0.20f, 0.35f); drawSphere(0.015f, 6, 6);
        glTranslatef(0.0f, -0.40f, 0.0f); drawSphere(0.015f, 6, 6);
        glPopMatrix();
    }

    // Center Iron Hasp & Rusted Padlock
    glPushMatrix();
    glTranslatef(0.0f, -0.05f, 0.345f);
    drawBox(0.07f, 0.14f, 0.03f); // Hasp plate
    // Padlock
    glTranslatef(0.0f, -0.06f, 0.015f);
    drawBox(0.065f, 0.08f, 0.025f);
    // Shackle ring
    glTranslatef(0.0f, 0.045f, 0.0f);
    drawCylinder(0.025f, 0.025f, 0.015f, 8);
    glPopMatrix();

    // Side Drop Carry Handles (Left & Right sides)
    glPushMatrix();
    glTranslatef(-0.57f, -0.12f, 0.0f);
    drawCylinder(0.045f, 0.045f, 0.02f, 8); // Left handle
    glTranslatef(1.14f, 0.0f, 0.0f);
    drawCylinder(0.045f, 0.045f, 0.02f, 8); // Right handle
    glPopMatrix();
    glPopMatrix();

    glPopMatrix(); // End Storage Chest

    // 8. HAUNTED VICTORIAN ARMOIRE / WARDROBE (Slightly Ajar Door Revealing Pitch Darkness)
    glPushMatrix();
    glTranslatef(-2.70f, 4.24f, -3.20f);
    glRotatef(15.0f, 0.0f, 1.0f, 0.0f);

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.22f, 0.17f, 0.13f, 1.0f);

    // Armoire Outer Carcase (1.30m wide, 2.30m tall, 0.55m deep)
    glPushMatrix();
    glTranslatef(0.0f, 1.15f, 0.0f);
    // Backing panel
    drawBox(1.30f, 2.30f, 0.04f);
    // Side pilaster walls
    glTranslatef(-0.62f, 0.0f, 0.25f); drawBox(0.06f, 2.30f, 0.50f);
    glTranslatef( 1.24f, 0.0f, 0.0f);  drawBox(0.06f, 2.30f, 0.50f);
    // Plinth base
    glTranslatef(-0.62f, -1.10f, 0.0f); drawBox(1.38f, 0.12f, 0.56f);
    // Top Arch Crown Molding Cornice
    glTranslatef(0.0f, 2.24f, 0.0f); drawBox(1.42f, 0.12f, 0.58f);
    glTranslatef(0.0f, 0.08f, 0.0f); drawBox(0.80f, 0.08f, 0.56f); // Center crest arch
    glPopMatrix();

    // Pitch Black Void inside wardrobe
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    glColor3f(0.02f, 0.015f, 0.01f);
    glPushMatrix();
    glTranslatef(0.0f, 1.15f, 0.10f);
    drawBox(1.16f, 2.10f, 0.35f);
    glPopMatrix();

    // Left Door (Fully Closed with Carved Panels)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.22f, 0.17f, 0.13f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.29f, 1.15f, 0.50f);
    drawBox(0.58f, 2.05f, 0.04f);
    // Carved panels relief
    glColor4f(0.18f, 0.14f, 0.10f, 1.0f);
    glTranslatef(0.0f, 0.35f, 0.022f); drawBox(0.44f, 0.85f, 0.012f);
    glTranslatef(0.0f, -0.70f, 0.0f);   drawBox(0.44f, 0.65f, 0.012f);
    glPopMatrix();

    // Right Door (CREAKED SLIGHTLY AJAR ~14 DEGREES - Haunted Look!)
    glPushMatrix();
    glTranslatef(0.58f, 1.15f, 0.50f); // Hinge at outer right frame
    glRotatef(14.0f, 0.0f, 1.0f, 0.0f);  // Swung open slightly
    glTranslatef(-0.29f, 0.0f, 0.0f);
    glColor4f(0.22f, 0.17f, 0.13f, 1.0f);
    drawBox(0.58f, 2.05f, 0.04f);
    // Door panel
    glColor4f(0.18f, 0.14f, 0.10f, 1.0f);
    glTranslatef(0.0f, 0.35f, 0.022f); drawBox(0.44f, 0.85f, 0.012f);
    glTranslatef(0.0f, -0.70f, 0.0f);   drawBox(0.44f, 0.65f, 0.012f);
    // Antique Brass Keyhole Escutcheon
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_NONE);
    glColor4f(0.65f, 0.52f, 0.22f, 1.0f);
    glTranslatef(-0.24f, 0.35f, 0.015f);
    drawBox(0.025f, 0.045f, 0.008f);
    glPopMatrix();

    glPopMatrix(); // End Armoire

    // 9. HANGING VINTAGE BRASS LANTERN FROM RIDGE TIMBER BEAM
    glPushMatrix();
    glTranslatef(-4.80f, 7.20f, 0.50f);
    // Wire
    bindTexture(TEX_NONE);
    glDisable(GL_LIGHTING);
    glColor3f(0.12f, 0.12f, 0.12f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, -0.80f, 0.0f);
    glEnd();
    glEnable(GL_LIGHTING);
    // Lantern Body
    glTranslatef(0.0f, -0.80f, 0.0f);
    applyMaterial(MAT_RUSTY_METAL);
    glColor4f(0.55f, 0.42f, 0.20f, 1.0f);
    drawCylinder(0.07f, 0.05f, 0.06f, 8);
    // Glowing Warm Core
    glDisable(GL_LIGHTING);
    glTranslatef(0.0f, -0.06f, 0.0f);
    glColor4f(1.0f, 0.82f, 0.35f, 0.95f);
    drawSphere(0.06f, 8, 6);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // 10. Cobwebs in 2nd Floor Ceiling Rafter Angles
    drawCobweb(-8.75f, 6.80f,  4.60f, 0.85f,   0.0f);
    drawCobweb(-0.85f, 6.80f,  4.60f, 0.75f,  90.0f);
    drawCobweb(-8.75f, 6.80f, -3.60f, 0.95f, -90.0f);
    drawCobweb(-4.80f, 7.20f, -2.10f, 0.65f,   0.0f);
}

// 4. Multi-Section Victorian Gothic Haunted House (Complete with Accurate Windows & Collision)
void drawHouse() {    glPushMatrix();
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

    // Render Full Walk-in Haunted Dilapidated Interior (1st Floor)
    drawHouseInterior();

    // Render Full Walk-in 2nd Floor (Dotola) Interior (Bed, Spellbook, Candelabra, Guardrails)
    drawSecondFloorInterior();

    // ========================================================================
    // ========================================================================
    // 2. SECTION A: MAIN LEFT WING (Hollow Room with Doorway & Clear Window Cutouts)
    // ========================================================================
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_WALL);
    glColor4f(0.38f, 0.36f, 0.34f, 1.0f);

    // Left outer side wall (Constructed with window cutouts at z = 2.2f, z = -1.5f, and 2nd floor z = 0.5f)
    // Ground floor bottom sill
    glPushMatrix(); glTranslatef(-8.95f, 1.45f, 0.5f); drawBox(0.24f, 0.90f, 8.8f, 1.0f, 0.3f); glPopMatrix();
    // Ground floor piers
    glPushMatrix(); glTranslatef(-8.95f, 2.70f, -2.98f); drawBox(0.24f, 1.60f, 1.85f, 0.3f, 0.6f); glPopMatrix(); // Rear
    glPushMatrix(); glTranslatef(-8.95f, 2.70f,  0.35f); drawBox(0.24f, 1.60f, 2.60f, 0.4f, 0.6f); glPopMatrix(); // Middle
    glPushMatrix(); glTranslatef(-8.95f, 2.70f,  3.83f); drawBox(0.24f, 1.60f, 2.15f, 0.3f, 0.6f); glPopMatrix(); // Front
    // 2nd floor sill band
    glPushMatrix(); glTranslatef(-8.95f, 4.05f, 0.5f); drawBox(0.24f, 1.10f, 8.8f, 1.0f, 0.4f); glPopMatrix();
    // 2nd floor piers
    glPushMatrix(); glTranslatef(-8.95f, 5.40f, -1.95f); drawBox(0.24f, 1.60f, 3.90f, 0.5f, 0.6f); glPopMatrix(); // 2nd fl rear
    glPushMatrix(); glTranslatef(-8.95f, 5.40f,  2.95f); drawBox(0.24f, 1.60f, 3.90f, 0.5f, 0.6f); glPopMatrix(); // 2nd fl front
    // Top eaves wall
    glPushMatrix(); glTranslatef(-8.95f, 6.40f, 0.5f); drawBox(0.24f, 0.80f, 8.8f, 1.0f, 0.3f); glPopMatrix();

    // Right interior partition wall (separating wing from central tower)
    glPushMatrix();
    glTranslatef(-0.65f, 3.8f, 0.5f);
    drawBox(0.24f, 5.6f, 8.8f, 1.0f, 2.0f);
    glPopMatrix();

    // Back wall (Constructed with window cutouts at x = -7.2f, x = -2.4f, and 2nd floor x = -4.8f)
    // Bottom sill
    glPushMatrix(); glTranslatef(-4.8f, 1.45f, -3.78f); drawBox(8.5f, 0.90f, 0.24f, 3.0f, 0.3f); glPopMatrix();
    // Ground piers
    glPushMatrix(); glTranslatef(-8.45f, 2.70f, -3.78f); drawBox(1.20f, 1.60f, 0.24f); glPopMatrix();
    glPushMatrix(); glTranslatef(-4.80f, 2.70f, -3.78f); drawBox(3.70f, 1.60f, 0.24f); glPopMatrix();
    glPushMatrix(); glTranslatef(-1.15f, 2.70f, -3.78f); drawBox(1.20f, 1.60f, 0.24f); glPopMatrix();
    // 2nd floor sill band
    glPushMatrix(); glTranslatef(-4.8f, 4.05f, -3.78f); drawBox(8.5f, 1.10f, 0.24f, 3.0f, 0.4f); glPopMatrix();
    // 2nd floor piers
    glPushMatrix(); glTranslatef(-7.10f, 5.40f, -3.78f); drawBox(3.90f, 1.60f, 0.24f); glPopMatrix();
    glPushMatrix(); glTranslatef(-2.50f, 5.40f, -3.78f); drawBox(3.90f, 1.60f, 0.24f); glPopMatrix();
    // Top gable header
    glPushMatrix(); glTranslatef(-4.8f, 6.40f, -3.78f); drawBox(8.5f, 0.80f, 0.24f, 3.0f, 0.3f); glPopMatrix();

    // Front wall: Left section (with window cutout at x = -7.4f, y = 2.6f)
    glPushMatrix(); glTranslatef(-7.40f, 1.35f, 4.78f); drawBox(1.60f, 0.70f, 0.24f); glPopMatrix(); // under window
    glPushMatrix(); glTranslatef(-8.50f, 2.60f, 4.78f); drawBox(0.90f, 1.80f, 0.24f); glPopMatrix(); // left pier
    glPushMatrix(); glTranslatef(-6.15f, 2.60f, 4.78f); drawBox(1.30f, 1.80f, 0.24f); glPopMatrix(); // right pier
    glPushMatrix(); glTranslatef(-7.30f, 4.00f, 4.78f); drawBox(3.30f, 1.00f, 0.24f); glPopMatrix(); // over window

    // Front wall: Right section (with window cutout at x = -1.8f, y = 2.6f)
    glPushMatrix(); glTranslatef(-1.80f, 1.35f, 4.78f); drawBox(1.60f, 0.70f, 0.24f); glPopMatrix(); // under window
    glPushMatrix(); glTranslatef(-3.05f, 2.60f, 4.78f); drawBox(1.30f, 1.80f, 0.24f); glPopMatrix(); // left pier
    glPushMatrix(); glTranslatef(-0.95f, 2.60f, 4.78f); drawBox(0.60f, 1.80f, 0.24f); glPopMatrix(); // right pier
    glPushMatrix(); glTranslatef(-2.30f, 4.00f, 4.78f); drawBox(3.10f, 1.00f, 0.24f); glPopMatrix(); // over window

    // Front wall: Top lintel wall above doorway
    glPushMatrix();
    glTranslatef(-4.6f, 5.55f, 4.78f);
    drawBox(1.8f, 2.1f, 0.24f, 0.8f, 0.8f);
    glPopMatrix();

    // Front wall: 2nd floor upper facade band
    glPushMatrix();
    glTranslatef(-4.8f, 5.55f, 4.78f);
    drawBox(8.5f, 2.1f, 0.24f, 3.0f, 0.8f);
    glPopMatrix();

    // Second-floor ceiling slab with Dedicated Stairwell Opening Cutout!
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    // Main 2nd floor ceiling slab (from left wall up to stairwell opening)
    glPushMatrix();
    glTranslatef(-5.45f, 4.20f, 0.50f);
    drawBox(7.00f, 0.18f, 8.80f, 2.0f, 2.0f);
    glPopMatrix();
    // Rear ceiling slab behind stairwell
    glPushMatrix();
    glTranslatef(-1.30f, 4.20f, -2.69f);
    drawBox(1.30f, 0.18f, 2.18f, 0.5f, 0.8f);
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

    // Left Front Dormer with Peaked Roof & Window Cutout
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_WALL);
    glColor4f(0.38f, 0.36f, 0.34f, 1.0f);
    glPushMatrix();
    glTranslatef(-5.2f, 6.8f, 4.2f);
    glPushMatrix(); glTranslatef(-0.95f, 0.0f, 0.0f); drawBox(0.18f, 2.0f, 2.0f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.95f, 0.0f, 0.0f); drawBox(0.18f, 2.0f, 2.0f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.0f, -0.85f, 0.90f); drawBox(1.8f, 0.30f, 0.20f); glPopMatrix();
    applyMaterial(MAT_ROOF_SHINGLE);
    bindTexture(TEX_ROOF);
    glColor4f(0.38f, 0.42f, 0.50f, 1.0f);
    glTranslatef(0.0f, 1.0f, 0.0f);
    drawPrismRoof(2.5f, 1.6f, 2.2f, 1.0f, 1.0f);
    glPopMatrix();
    drawHouseWindow(-5.2f, 7.0f, 5.25f, 1.2f, 1.4f, 0.0f, true, true);

    // Second Dormer (right side of left wing)
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_WALL);
    glColor4f(0.38f, 0.36f, 0.34f, 1.0f);
    glPushMatrix();
    glTranslatef(-3.2f, 6.8f, 4.2f);
    glPushMatrix(); glTranslatef(-0.80f, 0.0f, 0.0f); drawBox(0.16f, 1.8f, 1.8f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.80f, 0.0f, 0.0f); drawBox(0.16f, 1.8f, 1.8f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.0f, -0.75f, 0.80f); drawBox(1.5f, 0.30f, 0.20f); glPopMatrix();
    applyMaterial(MAT_ROOF_SHINGLE);
    bindTexture(TEX_ROOF);
    glColor4f(0.38f, 0.42f, 0.50f, 1.0f);
    glTranslatef(0.0f, 0.9f, 0.0f);
    drawPrismRoof(2.1f, 1.3f, 2.0f, 1.0f, 1.0f);
    glPopMatrix();
    drawHouseWindow(-3.2f, 6.9f, 5.15f, 1.0f, 1.2f, 0.0f, true, true);

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

    // Tower narrow slit windows
    drawHouseWindow(1.2f, 8.6f, 4.35f, 1.0f, 1.5f, 0.0f, true, true);
    drawHouseWindow(1.2f, 4.8f, 4.35f, 1.0f, 1.5f, 0.0f, true, true);
    drawHouseWindow(1.2f, 6.6f, 4.35f, 0.8f, 1.2f, 0.0f, true, false);

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

    // Upper gable window (Front)
    drawHouseWindow(3.8f, 6.2f, 4.55f, 1.4f, 1.4f, 0.0f, false, true);

    // Ground-floor right wing front windows
    drawHouseWindow(2.6f, 2.6f, 4.35f, 1.1f, 1.5f, 0.0f, true, true);
    drawHouseWindow(4.8f, 2.6f, 4.35f, 1.1f, 1.5f, 0.0f, true, true);

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

    // Broken ajar wooden panel door leaf (always wide open swung inwards along interior wall at 82 deg)
    glPushMatrix();
    glTranslatef(-0.68f, -0.05f, 0.0f);
    glRotatef(82.0f, 0.0f, 1.0f, 0.0f);
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

    // Main-floor front windows under porch / left wing
    drawHouseWindow(-7.4f, 2.6f, 4.90f, 1.15f, 1.55f, 0.0f, true, true);
    drawHouseWindow(-1.8f, 2.6f, 4.90f, 1.15f, 1.55f, 0.0f, true, true);

    // ========================================================================
    // 6. ACCURATELY PLACED SIDE & BACK WINDOWS (FIXED PLACEMENT & ORIENTATIONS)
    // ========================================================================
    
    // LEFT SIDE WINDOWS (Wall at X = -8.95f, rotY = -90.0f facing -X)
    drawHouseWindow(-9.00f, 2.7f,  2.2f, 1.10f, 1.50f, -90.0f, true, true);
    drawHouseWindow(-9.00f, 2.7f, -1.5f, 1.10f, 1.50f, -90.0f, true, true);
    drawHouseWindow(-9.00f, 5.4f,  0.5f, 1.00f, 1.35f, -90.0f, true, true);

    // RIGHT SIDE WINDOWS (Wall at X = 6.05f, rotY = 90.0f facing +X)
    drawHouseWindow(6.05f, 2.7f,  2.2f, 1.10f, 1.50f, 90.0f, true, true);
    drawHouseWindow(6.05f, 2.7f, -1.2f, 1.10f, 1.50f, 90.0f, true, true);
    drawHouseWindow(6.05f, 5.0f,  0.5f, 1.00f, 1.30f, 90.0f, true, true);

    // BACK FACADE WINDOWS (Walls at Z = -3.78f and Z = -3.35f, rotY = 180.0f facing -Z)
    // Left Wing Back Windows
    drawHouseWindow(-7.2f, 2.7f, -3.85f, 1.10f, 1.50f, 180.0f, true, true);
    drawHouseWindow(-2.4f, 2.7f, -3.85f, 1.10f, 1.50f, 180.0f, true, true);
    drawHouseWindow(-4.8f, 5.4f, -3.85f, 1.20f, 1.40f, 180.0f, true, true);
    // Right Wing Back Windows
    drawHouseWindow(3.8f, 2.7f, -3.38f, 1.10f, 1.50f, 180.0f, true, true);
    drawHouseWindow(3.8f, 5.0f, -3.38f, 1.00f, 1.30f, 180.0f, true, true);

    // ========================================================================
    // 7. ADDITIONAL ARCHITECTURAL DETAILS
    // ========================================================================
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

    // Small rear extension / lean-to (Positioned on the exterior behind the back wall)
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_WALL);
    glColor4f(0.35f, 0.33f, 0.30f, 1.0f);
    glPushMatrix();
    glTranslatef(-6.5f, 1.8f, -5.50f);
    drawBox(3.2f, 2.4f, 3.4f, 1.5f, 1.0f);
    applyMaterial(MAT_ROOF_SHINGLE);
    bindTexture(TEX_ROOF);
    glColor4f(0.36f, 0.40f, 0.48f, 1.0f);
    glTranslatef(0.0f, 1.2f, 0.0f);
    drawPrismRoof(3.4f, 1.5f, 3.5f, 1.5f, 1.5f);
    glPopMatrix();

    glPopMatrix(); // End House
}

// ----------------------------------------------------------------------------
// INDIVIDUAL GRAVE / TOMBSTONE STYLES (Reference Panel 12)
// ----------------------------------------------------------------------------

