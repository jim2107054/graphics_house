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



void drawBatWing(float side, float flapAngle) {    glPushMatrix();
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

void drawBat(float x, float y, float z, float roll, float flapAngle, float scale) {    glPushMatrix();
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

void drawAllBats() {    // ------------------------------------------------------------------------
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

