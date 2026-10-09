#include "Clouds.h"
#include "../graphics/Material.h"
#include "../graphics/TextureManager.h"
#include "../graphics/Primitives.h"
#include "../graphics/Lighting.h"
#include <cmath>
#include <algorithm>

// ============================================================================
// DYNAMIC 3D VOLUMETRIC NOCTURNAL CLOUDS
// Realistic cumulus banks with atmospheric moonlit gradients & silver linings
// (Directly modeled on the reference image)
// ============================================================================

struct CloudDef {
    float startX;    // Initial horizontal spawn offset
    float y;         // Altitude in the sky (meters)
    float z;         // Distance/depth in the northern sky (meters)
    float speed;     // Drift velocity from West to East (m/s)
    float scale;     // Overall volumetric scaling factor
    int   variant;   // Lobe configuration variant (0: Hero Reference Cumulus, 1: Stratus Deck, 2: Compact Fluffy)
    float bobSpeed;  // Subtle vertical floating frequency
    float bobAmp;    // Subtle vertical floating amplitude
    float driftSpan; // Horizontal travel distance before seamless wrap (meters)
};

static const CloudDef G_CLOUDS[6] = {
    // Cloud 0: Hero Moon Crosser (Drifts directly across the illuminated lunar face!)
    { -12.0f, 29.5f, -48.0f, 0.28f, 1.25f, 0, 0.08f, 0.25f, 240.0f },
    // Cloud 1: High Zenith Stratocumulus Deck (Above house roofs and spire)
    {  26.0f, 33.5f, -38.0f, 0.42f, 1.05f, 1, 0.10f, 0.20f, 240.0f },
    // Cloud 2: Distant Northern Horizon Cloud Bank (Behind the house & moon)
    { -68.0f, 26.5f, -65.0f, 0.22f, 1.40f, 0, 0.07f, 0.28f, 240.0f },
    // Cloud 3: Mid-Foreground Fluffy Cumulus (Above the front driveway & yard)
    {  72.0f, 30.5f, -32.0f, 0.35f, 0.95f, 2, 0.11f, 0.18f, 240.0f },
    // Cloud 4: Graveyard Sky Bank (Over the cemetery, crosses, and rusted car)
    { -38.0f, 28.0f, -44.0f, 0.30f, 1.15f, 2, 0.09f, 0.22f, 240.0f },
    // Cloud 5: High Fast Stratus Ribbon (Sweeps calmly across upper sky)
    {  96.0f, 36.5f, -54.0f, 0.46f, 0.90f, 1, 0.12f, 0.16f, 240.0f }
};

static void getCloudPosition(int idx, float& outX, float& outY, float& outZ) {
    const CloudDef& c = G_CLOUDS[idx];
    float span = c.driftSpan;
    float raw = c.startX + g_time * c.speed;
    outX = std::fmod(raw + span * 1000.0f, span) - (span * 0.5f);
    outY = c.y + std::sin(g_time * c.bobSpeed + (float)idx * 1.6f) * c.bobAmp;
    outZ = c.z;
}

// Draw a single atmospheric cloud lobe with smooth normals, realistic height gradient, and lunar specular sheen
static void drawAtmosphericCloudLobe(float lx, float ly, float lz,
                                    float sx, float sy, float sz,
                                    float rimBoost, float flash, bool isSoftWisp, bool forShadow) {
    if (forShadow) {
        // Fast shadow caster geometry
        glPushMatrix();
        glTranslatef(lx, ly, lz);
        glScalef(sx, sy, sz);
        drawSphere(1.0f, 10, 8);
        glPopMatrix();
        return;
    }

    glPushMatrix();
    glTranslatef(lx, ly, lz);
    glScalef(sx, sy, sz);

    int stacks = 14;
    int slices = 20;

    for (int i = 0; i < stacks; ++i) {
        float phi0 = (float)M_PI * (-0.5f + (float)i / (float)stacks);
        float phi1 = (float)M_PI * (-0.5f + (float)(i + 1) / (float)stacks);

        float cosP0 = std::cos(phi0), sinP0 = std::sin(phi0);
        float cosP1 = std::cos(phi1), sinP1 = std::sin(phi1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * (float)M_PI * (float)j / (float)slices;
            float cosT = std::cos(theta), sinT = std::sin(theta);

            // Vertex 0
            float nx0 = cosP0 * cosT;
            float ny0 = sinP0; // -1.0 (bottom) to +1.0 (top)
            float nz0 = cosP0 * sinT;

            // Vertex 1
            float nx1 = cosP1 * cosT;
            float ny1 = sinP1;
            float nz1 = cosP1 * sinT;

            // Compute realistic color gradient matching reference image:
            // Top: luminous moonlit slate-cyan (0.38, 0.52, 0.74) + silver lining
            // Bottom: deep dark nocturnal indigo shadow (0.09, 0.14, 0.22)
            auto evalVertexColor = [&](float ny, float& r, float& g, float& b, float& a) {
                float t = (ny + 1.0f) * 0.5f; // Normalized height 0.0 to 1.0
                float s = t * t * (3.0f - 2.0f * t); // Smooth S-curve easing

                // Deep nocturnal bottom shadow
                float botR = 0.08f, botG = 0.13f, botB = 0.20f;
                // Luminous lunar top highlight (from reference image)
                float topR = 0.40f + rimBoost * 0.28f;
                float topG = 0.54f + rimBoost * 0.32f;
                float topB = 0.76f + rimBoost * 0.24f;

                r = botR + s * (topR - botR) + 0.65f * flash;
                g = botG + s * (topG - botG) + 0.65f * flash;
                b = botB + s * (topB - botB) + 0.70f * flash;
                a = isSoftWisp ? 0.38f : 0.94f;
            };

            float r0, g0, b0, a0;
            evalVertexColor(ny0, r0, g0, b0, a0);
            glNormal3f(nx0, ny0, nz0);
            glColor4f(r0, g0, b0, a0);
            glVertex3f(nx0, ny0, nz0);

            float r1, g1, b1, a1;
            evalVertexColor(ny1, r1, g1, b1, a1);
            glNormal3f(nx1, ny1, nz1);
            glColor4f(r1, g1, b1, a1);
            glVertex3f(nx1, ny1, nz1);
        }
        glEnd();
    }

    glPopMatrix();
}

// Master volumetric cluster renderer modeled exactly on user reference image
static void drawVolumetricCloudGeometry(int variant, float scale, float rimBoost, float flash, bool forShadow) {
    glPushMatrix();
    glScalef(scale, scale, scale);

    if (variant == 0) {
        // ====================================================================
        // VARIANT 0: HERO REFERENCE CUMULUS (Exact Match to User Reference Image)
        // - Compact, smooth, billowing cumulus cloud
        // - High rounded central dome
        // - Left-forward bulging lobe
        // - Right elongated lobe
        // - Far-left lower tip
        // - Smooth dark underbelly
        // ====================================================================

        // 1. Central High Crown Dome (Primary moonlit peak)
        drawAtmosphericCloudLobe( 0.0f,  0.6f,  0.0f,  4.4f, 2.6f, 3.5f, rimBoost, flash, false, forShadow);

        // 2. Left-Forward Bulging Lobe (Prominent front swelling puff)
        drawAtmosphericCloudLobe(-2.9f, -0.2f,  0.8f,  3.6f, 2.1f, 2.9f, rimBoost, flash, false, forShadow);

        // 3. Right Elongated Lobe (Rightward extending puff)
        drawAtmosphericCloudLobe( 3.4f,  0.1f, -0.4f,  3.5f, 1.9f, 2.7f, rimBoost, flash, false, forShadow);

        // 4. Far-Left Lower Tip Lobe
        drawAtmosphericCloudLobe(-5.5f, -0.7f,  0.3f,  2.6f, 1.6f, 2.3f, rimBoost, flash, false, forShadow);

        // 5. Back Supporting Lobe (Rear depth and volume)
        drawAtmosphericCloudLobe( 0.9f,  0.2f, -1.6f,  4.0f, 2.1f, 2.9f, rimBoost, flash, false, forShadow);

        // 6. Underbelly Smoothing Core (Connects all lobes with smooth dark shadow underneath)
        drawAtmosphericCloudLobe(-0.6f, -0.7f,  0.1f,  5.2f, 1.8f, 3.8f, rimBoost, flash, false, forShadow);

        // 7. Soft outer wisps (Feathered translucent edges against the sky)
        if (!forShadow) {
            drawAtmosphericCloudLobe( 0.2f,  0.8f,  0.0f,  4.9f, 2.9f, 3.8f, rimBoost, flash, true, false);
            drawAtmosphericCloudLobe(-3.2f, -0.2f,  0.9f,  4.0f, 2.3f, 3.2f, rimBoost, flash, true, false);
            drawAtmosphericCloudLobe( 3.7f,  0.1f, -0.5f,  3.9f, 2.1f, 3.0f, rimBoost, flash, true, false);
            drawAtmosphericCloudLobe(-5.8f, -0.8f,  0.4f,  2.9f, 1.8f, 2.5f, rimBoost, flash, true, false);
        }

    } else if (variant == 1) {
        // ====================================================================
        // VARIANT 1: STRATOCUMULUS DECK (Wider bank with twin rolling crests)
        // ====================================================================
        drawAtmosphericCloudLobe(-2.2f,  0.5f,  0.1f,  4.5f, 2.3f, 3.2f, rimBoost, flash, false, forShadow);
        drawAtmosphericCloudLobe( 2.6f,  0.6f, -0.2f,  4.8f, 2.4f, 3.4f, rimBoost, flash, false, forShadow);
        drawAtmosphericCloudLobe(-6.2f, -0.1f,  0.3f,  3.6f, 1.8f, 2.7f, rimBoost, flash, false, forShadow);
        drawAtmosphericCloudLobe( 6.4f, -0.1f, -0.3f,  3.8f, 1.9f, 2.8f, rimBoost, flash, false, forShadow);
        drawAtmosphericCloudLobe( 0.2f, -0.6f,  0.2f,  6.8f, 1.8f, 3.6f, rimBoost, flash, false, forShadow);
        drawAtmosphericCloudLobe(-0.4f,  0.1f, -1.8f,  4.2f, 1.9f, 2.6f, rimBoost, flash, false, forShadow);

        if (!forShadow) {
            drawAtmosphericCloudLobe(-2.2f,  0.6f,  0.1f,  5.0f, 2.5f, 3.5f, rimBoost, flash, true, false);
            drawAtmosphericCloudLobe( 2.8f,  0.7f, -0.2f,  5.3f, 2.6f, 3.7f, rimBoost, flash, true, false);
        }

    } else {
        // ====================================================================
        // VARIANT 2: PLUMP COMPACT CUMULUS (High fluffy dome)
        // ====================================================================
        drawAtmosphericCloudLobe( 0.1f,  0.8f,  0.1f,  4.0f, 2.8f, 3.3f, rimBoost, flash, false, forShadow);
        drawAtmosphericCloudLobe(-2.8f,  0.1f,  0.4f,  3.2f, 2.1f, 2.8f, rimBoost, flash, false, forShadow);
        drawAtmosphericCloudLobe( 2.9f,  0.0f, -0.3f,  3.3f, 2.0f, 2.8f, rimBoost, flash, false, forShadow);
        drawAtmosphericCloudLobe( 0.4f,  0.2f, -1.5f,  3.2f, 2.0f, 2.6f, rimBoost, flash, false, forShadow);
        drawAtmosphericCloudLobe( 0.0f, -0.6f,  0.0f,  4.6f, 1.9f, 3.5f, rimBoost, flash, false, forShadow);

        if (!forShadow) {
            drawAtmosphericCloudLobe( 0.1f,  0.9f,  0.1f,  4.5f, 3.1f, 3.6f, rimBoost, flash, true, false);
        }
    }

    glPopMatrix();
}

void initClouds() {
}

// Master Visual Render Routine in the Night Sky
void drawDynamicClouds() {
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_NORMALIZE);
    bindTexture(TEX_NONE);

    // Alpha blending for seamless atmospheric feathering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable Color Material so our custom vertex height gradient interacts with dynamic lights
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    float flash = g_lightning.flashIntensity;

    // Set up material with fine specular lunar sheen
    Material matCloud;
    matCloud.ambient[0]  = 0.35f;
    matCloud.ambient[1]  = 0.42f;
    matCloud.ambient[2]  = 0.55f;
    matCloud.ambient[3]  = 1.0f;

    matCloud.diffuse[0]  = 0.65f;
    matCloud.diffuse[1]  = 0.75f;
    matCloud.diffuse[2]  = 0.95f;
    matCloud.diffuse[3]  = 1.0f;

    matCloud.specular[0] = 0.35f;
    matCloud.specular[1] = 0.45f;
    matCloud.specular[2] = 0.60f;
    matCloud.specular[3] = 1.0f;

    matCloud.emission[0] = 0.04f + 0.25f * flash;
    matCloud.emission[1] = 0.06f + 0.25f * flash;
    matCloud.emission[2] = 0.09f + 0.28f * flash;
    matCloud.emission[3] = 1.0f;

    matCloud.shininess   = 32.0f;

    applyMaterial(matCloud);

    // Moon world position: (2.5f, 29.5f, -52.0f)
    const float moonX =   2.5f;
    const float moonY =  29.5f;

    // Draw all 6 drifting clouds in the sky
    for (int i = 0; i < 6; ++i) {
        float cx, cy, cz;
        getCloudPosition(i, cx, cy, cz);

        // Dynamic silver lining calculation:
        // When a cloud approaches or crosses the full moon, back-scattering creates a brilliant silver lunar rim
        float dMoonX = cx - moonX;
        float dMoonY = cy - moonY;
        float distToMoon = std::sqrt(dMoonX * dMoonX + dMoonY * dMoonY);
        float rimBoost = 0.0f;
        if (distToMoon < 22.0f) {
            rimBoost = (1.0f - distToMoon / 22.0f) * 0.55f;
        }

        glPushMatrix();
        glTranslatef(cx, cy, cz);
        drawVolumetricCloudGeometry(G_CLOUDS[i].variant, G_CLOUDS[i].scale, rimBoost, flash, false);
        glPopMatrix();
    }

    glPopAttrib();
}

// Master Shadow Caster Routine (Projected onto ground by Directional Moonlight)
void drawCloudShadowCasters() {
    // Only cast shadows when directional moonlight is active
    if (!g_light1DirectionalOn) return;

    for (int i = 0; i < 6; ++i) {
        float cx, cy, cz;
        getCloudPosition(i, cx, cy, cz);

        glPushMatrix();
        glTranslatef(cx, cy, cz);
        drawVolumetricCloudGeometry(G_CLOUDS[i].variant, G_CLOUDS[i].scale, 0.0f, 0.0f, true);
        glPopMatrix();
    }
}
