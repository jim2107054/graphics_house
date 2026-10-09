#include "Particles.h"
#include "Terrain.h"
#include "../core/Camera.h"
#include "../graphics/Material.h"
#include "../graphics/TextureManager.h"
#include "../graphics/Primitives.h"
#include "../graphics/Lighting.h"
#include <cmath>
#include <cstdlib>

const int MAX_FALLING_LEAVES = 15;
std::vector<FallingLeafParticle> g_fallingLeaves;

void initFallingLeaves() {    g_fallingLeaves.resize(MAX_FALLING_LEAVES);
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



void updateFallingLeaves(float dt) {    // Wind direction: Right → Left (negative X) with slight forward drift
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
void drawFallingLeaves() {    applyMaterial(MAT_FALLEN_LEAF);
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



// ============================================================================
// REALISTIC CHIROPTERAN ANATOMY (True 3D Articulated Bats)
// Modeled with authentic biological fidelity:
// - Skeletal Forelimb (Humerus, Radius, Hooked Thumb, Digits III, IV, V)
// - Cambered Wing Membranes (Propatagium, Plagiopatagium, Dactylopatagium)
// - Catenary Scalloped Trailing Edges
// - Cranium with Fleshy Noseleaf, Concave Acoustic Ears with Tragus, & Fangs
// - Keeled Sternum & Triangular Tail Membrane (Uropatagium)
// ============================================================================

static const Material MAT_BAT_FUR = {
    { 0.05f, 0.04f, 0.04f, 1.0f },
    { 0.14f, 0.11f, 0.10f, 1.0f },
    { 0.16f, 0.13f, 0.11f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    24.0f // Soft velvety fur sheen
};

static const Material MAT_BAT_MEMBRANE = {
    { 0.04f, 0.03f, 0.03f, 1.0f },
    { 0.18f, 0.14f, 0.12f, 1.0f },
    { 0.35f, 0.28f, 0.24f, 1.0f }, // Leathery skin specular highlights under moonlight
    { 0.00f, 0.00f, 0.00f, 1.0f },
    48.0f
};

static const Material MAT_BAT_BONE = {
    { 0.06f, 0.05f, 0.04f, 1.0f },
    { 0.22f, 0.18f, 0.15f, 1.0f },
    { 0.25f, 0.20f, 0.18f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    32.0f
};

static const Material MAT_BAT_EYE = {
    { 0.40f, 0.08f, 0.08f, 1.0f },
    { 0.85f, 0.15f, 0.12f, 1.0f },
    { 0.95f, 0.80f, 0.70f, 1.0f },
    { 0.30f, 0.05f, 0.05f, 1.0f }, // Subtle nocturnal eye glow
    90.0f
};

// Helper: Surface Normal from 3 3D Vertices
static void computeTriangleNormal(float x0, float y0, float z0,
                                  float x1, float y1, float z1,
                                  float x2, float y2, float z2,
                                  float& nx, float& ny, float& nz) {
    float ax = x1 - x0, ay = y1 - y0, az = z1 - z0;
    float bx = x2 - x0, by = y2 - y0, bz = z2 - z0;
    nx = ay * bz - az * by;
    ny = az * bx - ax * bz;
    nz = ax * by - ay * bx;
    float len = std::sqrt(nx * nx + ny * ny + nz * nz);
    if (len > 1e-5f) {
        nx /= len; ny /= len; nz /= len;
    } else {
        nx = 0.0f; ny = 1.0f; nz = 0.0f;
    }
}

// Helper: Slender Bone Segment Cylinder between two 3D joints
static void drawBoneSegment(float x0, float y0, float z0, float x1, float y1, float z1, float r) {
    float dx = x1 - x0, dy = y1 - y0, dz = z1 - z0;
    float len = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (len < 1e-4f) return;
    glPushMatrix();
    glTranslatef(x0, y0, z0);
    float yaw = std::atan2(dx, dz) * (180.0f / 3.14159265f);
    float pitch = -std::asin(std::max(-1.0f, std::min(1.0f, dy / len))) * (180.0f / 3.14159265f);
    glRotatef(yaw, 0.0f, 1.0f, 0.0f);
    glRotatef(pitch, 1.0f, 0.0f, 0.0f);
    drawCylinder(r, r * 0.85f, len, 6);
    glPopMatrix();
}

void drawBatWing(float side, float flapAngle) {
    // Backward compatibility stub - modern bat handles both wings in drawRealisticBat
}

void drawRealisticBat(float x, float y, float z, float yaw, float pitch, float roll, float flapAngle, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(yaw,   0.0f, 1.0f, 0.0f); // Flight heading
    glRotatef(pitch, 1.0f, 0.0f, 0.0f); // Climb / dive pitch
    glRotatef(roll,  0.0f, 0.0f, 1.0f); // Aerodynamic banking roll
    glScalef(scale, scale, scale);

    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    bindTexture(TEX_NONE);

    // Dynamic wing flap angle in radians
    float flapRad = flapAngle * (3.14159265f / 180.0f);
    float tipLag = std::sin(flapRad - 0.35f) * 0.16f;
    float camberY = std::sin(flapRad) * 0.022f;

    // --- 1. SKELETAL CRANIUM & SNOUT ---
    applyMaterial(MAT_BAT_FUR);
    // Rounded mammalian skull
    glPushMatrix();
    glTranslatef(0.0f, 0.022f, 0.095f);
    glScalef(0.92f, 0.88f, 1.05f);
    drawSphere(0.044f, 10, 8);
    glPopMatrix();

    // Protruding mammalian snout
    glPushMatrix();
    glTranslatef(0.0f, 0.014f, 0.138f);
    glScalef(1.0f, 0.75f, 1.25f);
    drawBox(0.026f, 0.020f, 0.032f);
    glPopMatrix();

    // Fleshy Leaf-Nose (Acoustic flap for echolocation)
    applyMaterial(MAT_BAT_MEMBRANE);
    glPushMatrix();
    glTranslatef(0.0f, 0.030f, 0.146f);
    glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);
    drawPrismRoof(0.016f, 0.020f, 0.014f);
    glPopMatrix();

    // Tiny Ivory Vampire Fangs
    glDisable(GL_LIGHTING);
    glColor3f(0.92f, 0.90f, 0.85f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex3f(-0.009f, 0.010f, 0.145f);
    glVertex3f(-0.009f, 0.000f, 0.147f);
    glVertex3f( 0.009f, 0.010f, 0.145f);
    glVertex3f( 0.009f, 0.000f, 0.147f);
    glEnd();
    glEnable(GL_LIGHTING);

    // Large Sculpted Bat Ears (Pinnae with Tragus)
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * 0.026f, 0.046f, 0.090f);
        glRotatef(side * -32.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(14.0f, 1.0f, 0.0f, 0.0f);

        // Outer Ear Shell
        applyMaterial(MAT_BAT_FUR);
        drawPrismRoof(0.036f, 0.075f, 0.026f);

        // Inner Concave Acoustic Cavity
        applyMaterial(MAT_BAT_MEMBRANE);
        glTranslatef(0.0f, 0.005f, 0.008f);
        drawPrismRoof(0.026f, 0.058f, 0.014f);

        // Fleshy Tragus Spike
        glTranslatef(0.0f, 0.006f, 0.004f);
        drawBox(0.008f, 0.024f, 0.008f);
        glPopMatrix();

        // Glinting Nocturnal Eye
        glPushMatrix();
        glTranslatef(side * 0.017f, 0.024f, 0.122f);
        applyMaterial(MAT_BAT_EYE);
        drawSphere(0.0065f, 6, 6);
        glPopMatrix();
    }

    // --- 2. THORACIC BARREL, KEELED STERNUM & PELVIS ---
    applyMaterial(MAT_BAT_FUR);
    // Main Torso / Thorax
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(1.0f, 1.15f, 1.45f);
    drawSphere(0.055f, 10, 8);
    glPopMatrix();

    // Keeled Breastbone (Sternum flight muscle ridge)
    glPushMatrix();
    glTranslatef(0.0f, -0.025f, 0.020f);
    glScalef(0.65f, 1.20f, 1.20f);
    drawSphere(0.035f, 8, 6);
    glPopMatrix();

    // Abdomen & Pelvis
    glPushMatrix();
    glTranslatef(0.0f, -0.006f, -0.065f);
    glScalef(0.80f, 0.75f, 1.10f);
    drawSphere(0.040f, 8, 6);
    glPopMatrix();

    // --- 3. HINDLIMBS & UROPATAGIUM (TAIL MEMBRANE) ---
    applyMaterial(MAT_BAT_BONE);
    for (int side = -1; side <= 1; side += 2) {
        // Thigh & Knee
        drawBoneSegment(side * 0.025f, -0.010f, -0.085f,
                        side * 0.042f, -0.018f, -0.125f, 0.008f);
        // Shin & Ankle
        drawBoneSegment(side * 0.042f, -0.018f, -0.125f,
                        side * 0.055f, -0.020f, -0.160f, 0.006f);
        // Hooked Foot
        glPushMatrix();
        glTranslatef(side * 0.055f, -0.020f, -0.160f);
        drawBox(0.012f, 0.008f, 0.016f);
        glPopMatrix();
    }

    // Slender Tail Vertebra
    drawBoneSegment(0.0f, -0.010f, -0.090f, 0.0f, -0.015f, -0.155f, 0.005f);

    // Uropatagium (Triangular Tail Membrane)
    applyMaterial(MAT_BAT_MEMBRANE);
    float nx, ny, nz;
    glBegin(GL_TRIANGLES);
    computeTriangleNormal(0.0f, -0.010f, -0.090f,
                          -0.055f, -0.020f, -0.160f,
                          0.0f, -0.015f, -0.155f, nx, ny, nz);
    glNormal3f(nx, ny, nz);
    glVertex3f(0.0f, -0.010f, -0.090f);
    glVertex3f(-0.055f, -0.020f, -0.160f);
    glVertex3f(0.0f, -0.015f, -0.155f);

    computeTriangleNormal(0.0f, -0.010f, -0.090f,
                          0.0f, -0.015f, -0.155f,
                          0.055f, -0.020f, -0.160f, nx, ny, nz);
    glNormal3f(nx, ny, nz);
    glVertex3f(0.0f, -0.010f, -0.090f);
    glVertex3f(0.0f, -0.015f, -0.155f);
    glVertex3f(0.055f, -0.020f, -0.160f);
    glEnd();

    // --- 4. ARTICULATED FORELIMBS & CAMBERED WING PATAGIUM ---
    for (int side = -1; side <= 1; side += 2) {
        float s = (float)side;

        // Shoulder origin
        float sx = s * 0.040f, sy = 0.015f, sz = 0.035f;

        // Elbow: sweeps with primary shoulder flap
        float ex = s * 0.160f;
        float ey = sy + std::sin(flapRad) * 0.130f;
        float ez = 0.045f;

        // Wrist: extends forearm with elbow flexion
        float wx = s * 0.360f;
        float wy = ey + std::sin(flapRad + 0.20f) * 0.150f;
        float wz = 0.020f;

        // Hooked Thumb Claw (Pollex)
        float tx = wx + s * 0.010f;
        float ty = wy + 0.024f;
        float tz = wz + 0.022f;

        // Elongated Finger Strut Tips (Digits III, IV, V)
        // Digit III (Wingtip leading edge point)
        float p3x = s * 0.650f;
        float p3y = wy + tipLag;
        float p3z = -0.060f;

        // Digit IV (Middle strut tip)
        float p4x = s * 0.490f;
        float p4y = wy + tipLag * 0.85f;
        float p4z = -0.190f;

        // Digit V (Inner strut tip)
        float p5x = s * 0.310f;
        float p5y = wy + tipLag * 0.65f;
        float p5z = -0.170f;

        // Hind Ankle attachment point
        float ax = s * 0.055f;
        float ay = -0.020f;
        float az = -0.160f;

        // Catenary Scallop Midpoints (Curved parabolic indentations)
        // Mid-scallop between Digit III and Digit IV
        float m34x = s * 0.550f;
        float m34y = (p3y + p4y) * 0.5f + camberY;
        float m34z = -0.110f;

        // Mid-scallop between Digit IV and Digit V
        float m45x = s * 0.385f;
        float m45y = (p4y + p5y) * 0.5f + camberY;
        float m45z = -0.165f;

        // Mid-scallop between Digit V and Ankle
        float m5ax = s * 0.170f;
        float m5ay = (p5y + ay) * 0.5f + camberY;
        float m5az = -0.175f;

        // Neck and Flank anchor points
        float nx_pt = s * 0.025f, ny_pt = 0.015f, nz_pt = 0.070f;
        float fx_pt = s * 0.038f, fy_pt = 0.000f, fz_pt = -0.040f;

        // --- Render Wing Bones (Humerus, Radius, Thumb, Digits) ---
        applyMaterial(MAT_BAT_BONE);
        // Humerus (Upper arm)
        drawBoneSegment(sx, sy, sz, ex, ey, ez, 0.010f);
        // Radius / Ulna (Forearm)
        drawBoneSegment(ex, ey, ez, wx, wy, wz, 0.008f);
        // Thumb (Pollex claw)
        drawBoneSegment(wx, wy, wz, tx, ty, tz, 0.004f);
        // Digit III Strut (Metacarpal + Phalanges)
        drawBoneSegment(wx, wy, wz, p3x, p3y, p3z, 0.0055f);
        // Digit IV Strut
        drawBoneSegment(wx, wy, wz, p4x, p4y, p4z, 0.0045f);
        // Digit V Strut
        drawBoneSegment(wx, wy, wz, p5x, p5y, p5z, 0.0045f);

        // --- Render Wing Membrane (Patagium with Catenary Scalloping) ---
        applyMaterial(MAT_BAT_MEMBRANE);
        glBegin(GL_TRIANGLES);

        // Propatagium (Leading edge between neck, shoulder, elbow, wrist)
        computeTriangleNormal(nx_pt, ny_pt, nz_pt, sx, sy, sz, ex, ey, ez, nx, ny, nz);
        glNormal3f(s * nx, ny, s * nz);
        glVertex3f(nx_pt, ny_pt, nz_pt);
        glVertex3f(sx, sy, sz);
        glVertex3f(ex, ey, ez);

        computeTriangleNormal(sx, sy, sz, ex, ey, ez, wx, wy, wz, nx, ny, nz);
        glNormal3f(s * nx, ny, s * nz);
        glVertex3f(sx, sy, sz);
        glVertex3f(ex, ey, ez);
        glVertex3f(wx, wy, wz);

        // Dactylopatagium Cell 1 (Digit III to Digit IV with scallop)
        computeTriangleNormal(wx, wy, wz, p3x, p3y, p3z, m34x, m34y, m34z, nx, ny, nz);
        glNormal3f(s * nx, ny, s * nz);
        glVertex3f(wx, wy, wz);
        glVertex3f(p3x, p3y, p3z);
        glVertex3f(m34x, m34y, m34z);

        computeTriangleNormal(wx, wy, wz, m34x, m34y, m34z, p4x, p4y, p4z, nx, ny, nz);
        glNormal3f(s * nx, ny, s * nz);
        glVertex3f(wx, wy, wz);
        glVertex3f(m34x, m34y, m34z);
        glVertex3f(p4x, p4y, p4z);

        // Dactylopatagium Cell 2 (Digit IV to Digit V with scallop)
        computeTriangleNormal(wx, wy, wz, p4x, p4y, p4z, m45x, m45y, m45z, nx, ny, nz);
        glNormal3f(s * nx, ny, s * nz);
        glVertex3f(wx, wy, wz);
        glVertex3f(p4x, p4y, p4z);
        glVertex3f(m45x, m45y, m45z);

        computeTriangleNormal(wx, wy, wz, m45x, m45y, m45z, p5x, p5y, p5z, nx, ny, nz);
        glNormal3f(s * nx, ny, s * nz);
        glVertex3f(wx, wy, wz);
        glVertex3f(m45x, m45y, m45z);
        glVertex3f(p5x, p5y, p5z);

        // Plagiopatagium (Digit V to Ankle with scallop)
        computeTriangleNormal(wx, wy, wz, p5x, p5y, p5z, m5ax, m5ay, m5az, nx, ny, nz);
        glNormal3f(s * nx, ny, s * nz);
        glVertex3f(wx, wy, wz);
        glVertex3f(p5x, p5y, p5z);
        glVertex3f(m5ax, m5ay, m5az);

        computeTriangleNormal(wx, wy, wz, m5ax, m5ay, m5az, ax, ay, az, nx, ny, nz);
        glNormal3f(s * nx, ny, s * nz);
        glVertex3f(wx, wy, wz);
        glVertex3f(m5ax, m5ay, m5az);
        glVertex3f(ax, ay, az);

        // Inner Body Web (Wrist, Ankle, Flank, Shoulder)
        computeTriangleNormal(wx, wy, wz, ax, ay, az, fx_pt, fy_pt, fz_pt, nx, ny, nz);
        glNormal3f(s * nx, ny, s * nz);
        glVertex3f(wx, wy, wz);
        glVertex3f(ax, ay, az);
        glVertex3f(fx_pt, fy_pt, fz_pt);

        computeTriangleNormal(wx, wy, wz, fx_pt, fy_pt, fz_pt, sx, sy, sz, nx, ny, nz);
        glNormal3f(s * nx, ny, s * nz);
        glVertex3f(wx, wy, wz);
        glVertex3f(fx_pt, fy_pt, fz_pt);
        glVertex3f(sx, sy, sz);

        glEnd();
    }

    glPopAttrib();
    glPopMatrix();
}

// Backward compatible shim
void drawBat(float x, float y, float z, float roll, float flapAngle, float scale) {
    drawRealisticBat(x, y, z, -90.0f, 0.0f, roll, flapAngle, scale);
}

void drawAllBats() {
    // ------------------------------------------------------------------------
    // COLONY OF NOCTURNAL BATS: DYNAMIC HORROR FLIGHT
    // Flying continuously across the moonlit sky with varied swoops,
    // altitudes, banking rolls, and true aerodynamic flight orientation!
    // ------------------------------------------------------------------------
    struct BatColonyMember {
        float startX;      // Spawn X
        float endX;        // Despawn X
        float baseY;       // Nominal altitude
        float baseZ;       // Yard depth
        float scale;       // Scale
        float flapFreq;    // Flapping frequency
        float swoopAmp;    // Vertical swoop amplitude
        float swoopFreq;   // Vertical swoop frequency
        float timeOffset;  // Staggered launch offset
    };

    static const BatColonyMember COLONY[6] = {
        // 1. Alpha Leader Bat (Graceful broad wingbeats across mid-yard)
        {  32.0f, -32.0f, 5.9f, 13.2f, 0.55f, 13.0f, 0.45f, 2.2f, 0.0f },
        // 2. High Moonlit Sentry (High altitude crossing the glowing full moon disk)
        {  34.0f, -34.0f, 8.4f, 10.5f, 0.48f, 14.5f, 0.35f, 1.8f, 2.8f },
        // 3. Low Graveyard Hunter (Swooping low over the tomb crosses and pumpkins)
        {  30.0f, -30.0f, 3.8f, 14.8f, 0.50f, 15.0f, 0.85f, 2.8f, 5.2f },
        // 4. Manor Turret Scout (Gliding near the Gothic roof ridge)
        {  33.0f, -33.0f, 7.2f, 11.8f, 0.46f, 12.5f, 0.40f, 2.0f, 7.5f },
        // 5. Agile Follower (Staggered trailing formation)
        {  31.0f, -31.0f, 6.4f, 13.8f, 0.44f, 14.0f, 0.55f, 2.4f, 9.8f },
        // 6. Low Wrecked-Car Scout (Dipping near the old rusted truck)
        {  35.0f, -35.0f, 4.4f, 12.2f, 0.48f, 15.5f, 0.70f, 2.6f, 11.6f }
    };

    float cycleDuration = 14.0f; // Each wave takes 14s to cross

    for (int i = 0; i < 6; ++i) {
        const BatColonyMember& b = COLONY[i];
        float t = std::fmod(g_time + b.timeOffset, cycleDuration) / cycleDuration;

        // Position
        float bx = b.startX + t * (b.endX - b.startX);
        float by = b.baseY + b.swoopAmp * std::sin(g_time * b.swoopFreq + b.timeOffset);
        float bz = b.baseZ + 0.35f * std::cos(g_time * 1.5f + b.timeOffset);

        // True flight dynamics:
        // Heading towards -X is yaw = -90.0f
        // Pitch calculated from vertical velocity
        float vy = b.swoopAmp * b.swoopFreq * std::cos(g_time * b.swoopFreq + b.timeOffset);
        float vx = -((b.startX - b.endX) / cycleDuration);
        float pitch = -std::atan2(vy, std::abs(vx)) * (180.0f / 3.14159265f);

        // Banking roll when swooping
        float roll = -15.0f + 12.0f * std::sin(g_time * 1.8f + b.timeOffset);

        // Flapping angle with downstroke power
        float flapAngle = std::sin(g_time * b.flapFreq + b.timeOffset) * 38.0f;

        drawRealisticBat(bx, by, bz, -90.0f, pitch, roll, flapAngle, b.scale);
    }
}

// ----------------------------------------------------------------------------
// LAYERED GROUND MIST & ROLLING HORIZON FOG WISPS
// ----------------------------------------------------------------------------
void drawGroundMist() {    if (!g_fogEnabled) return;

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
        {  -4.5f,  13.5f, 4.5f, 3.2f, 0.18f,  0.18f,  0.08f, 0.0f, 0.14f }, // Front lawn wisp
        {   5.0f,  10.5f, 4.2f, 3.0f, 0.22f,  0.14f, -0.06f, 1.2f, 0.13f }, // Car knoll wisp
        {  -9.5f,   8.2f, 3.8f, 2.8f, 0.16f, -0.12f,  0.10f, 2.4f, 0.12f }, // Left porch lawn wisp
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

