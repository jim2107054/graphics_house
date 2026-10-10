#include "SkyAndStars.h"
#include "../core/Camera.h"
#include "../graphics/Material.h"
#include "../graphics/TextureManager.h"
#include "../graphics/Primitives.h"
#include "../graphics/Lighting.h"
#include <cmath>

std::vector<Star> g_stars;

void initStars() {    g_stars.clear();
    srand(1337);
    for (int i = 0; i < 350; ++i) {
        float theta = ((float)rand() / RAND_MAX) * 2.0f * (float)M_PI;
        float phi   = ((float)rand() / RAND_MAX) * 0.45f * (float)M_PI;
        float r     = 180.0f;
        Star s;
        s.x = r * std::cos(phi) * std::cos(theta);
        s.y = r * std::sin(phi) + 12.0f;
        s.z = r * std::cos(phi) * std::sin(theta);
        s.size = 1.0f + ((float)rand() / RAND_MAX) * 2.5f;
        s.brightness = 0.4f + ((float)rand() / RAND_MAX) * 0.6f;
        g_stars.push_back(s);
    }
}


void drawMoonAndStars() {    bindTexture(TEX_NONE);
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE);

    float flash = g_lightning.flashIntensity;

    // Horizon: Desaturated atmospheric blue-gray (blends seamlessly into fog)
    float horR = 0.06f + 0.22f * flash;
    float horG = 0.10f + 0.25f * flash;
    float horB = 0.16f + 0.35f * flash;

    // Mid-sky: Dark teal-blue
    float midR = 0.035f + 0.20f * flash;
    float midG = 0.065f + 0.22f * flash;
    float midB = 0.115f + 0.32f * flash;

    // Upper Zenith: Deep navy blue-black
    float topR = 0.020f + 0.18f * flash;
    float topG = 0.030f + 0.20f * flash;
    float topB = 0.065f + 0.28f * flash;

    // 1. Seamless 360-Degree Spherical Sky Dome (Centered on camera - ZERO edges or seams anywhere!)
    glPushMatrix();
    glTranslatef(g_cam.x, g_cam.y, g_cam.z);

    int skyStacks = 16;
    int skySlices = 36;
    float skyRadius = 220.0f;

    for (int i = 0; i < skyStacks; ++i) {
        float f0 = (float)i / (float)skyStacks;
        float f1 = (float)(i + 1) / (float)skyStacks;

        // Phi from 0 (zenith, top) to PI*0.60 (past horizon)
        float phi0 = f0 * (float)M_PI * 0.60f;
        float phi1 = f1 * (float)M_PI * 0.60f;

        float sinP0 = std::sin(phi0);
        float cosP0 = std::cos(phi0);
        float sinP1 = std::sin(phi1);
        float cosP1 = std::cos(phi1);

        // Smooth 3-tier color interpolation
        auto evalSkyColor = [&](float t, float& r, float& g, float& b) {
            if (t <= 0.45f) {
                float u = t / 0.45f;
                float s = u * u * (3.0f - 2.0f * u);
                r = topR + s * (midR - topR);
                g = topG + s * (midG - topG);
                b = topB + s * (midB - topB);
            } else {
                float u = (t - 0.45f) / 0.55f;
                float s = u * u * (3.0f - 2.0f * u);
                r = midR + s * (horR - midR);
                g = midG + s * (horG - midG);
                b = midB + s * (horB - midB);
            }
        };

        float r0, g0, b0, r1, g1, b1;
        evalSkyColor(f0, r0, g0, b0);
        evalSkyColor(f1, r1, g1, b1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= skySlices; ++j) {
            float theta = (float)j / (float)skySlices * 2.0f * (float)M_PI;
            float cosT = std::cos(theta);
            float sinT = std::sin(theta);

            glColor4f(r0, g0, b0, 1.0f);
            glVertex3f(skyRadius * sinP0 * cosT, skyRadius * cosP0, skyRadius * sinP0 * sinT);

            glColor4f(r1, g1, b1, 1.0f);
            glVertex3f(skyRadius * sinP1 * cosT, skyRadius * cosP1, skyRadius * sinP1 * sinT);
        }
        glEnd();
    }
    glPopMatrix();

    // 2. Subtle Faint Distant Stars in Upper Dark Sky (Non-distracting, high above horizon)
    glPushMatrix();
    glTranslatef(g_cam.x, g_cam.y, g_cam.z);
    glPointSize(1.2f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < g_stars.size(); ++i) {
        if (g_stars[i].y < 22.0f) continue; // Keep lower sky completely clear for moon & silhouettes
        float b = g_stars[i].brightness * 0.35f * (0.85f + 0.15f * std::sin(g_time * 2.5f + (float)i));
        glColor4f(0.70f * b, 0.82f * b, 0.95f * b, 0.40f * b);
        glVertex3f(g_stars[i].x, g_stars[i].y, g_stars[i].z);
    }
    glEnd();
    glPopMatrix();

    // 3. Giant Luminous Moon (Elevated higher in the night sky behind the house)
    float moonX =   2.5f;
    float moonY =  29.5f; // Elevated higher in the sky behind the house
    float moonZ = -52.0f; // Screen X ≈ 0.54
    float moonRadius = 10.8f;

    // 3.1 Soft Luminous Atmospheric Halo Disc behind the Moon
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive halo glow
    glPushMatrix();
    glTranslatef(moonX, moonY, moonZ);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.70f, 0.82f, 0.96f, 0.42f);
    glVertex3f(0.0f, 0.0f, -0.2f);
    int haloSegments = 36;
    float haloR = moonRadius * 1.50f;
    for (int i = 0; i <= haloSegments; ++i) {
        float th = (float)i * (2.0f * (float)M_PI / (float)haloSegments);
        glColor4f(0.35f, 0.50f, 0.70f, 0.0f);
        glVertex3f(haloR * std::cos(th), haloR * std::sin(th), -0.2f);
    }
    glEnd();
    glPopMatrix();

    // 3.2 Crisp Luminous Full Moon with Lunar Maria & Craters
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE); // Strict depth write so front gothic house tower cleanly occludes it
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    bindTexture(TEX_MOON);

    glPushMatrix();
    glTranslatef(moonX, moonY, moonZ);
    // Draw planar camera-facing circular disc with orthogonal UV mapping for zero distortion
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    int discSegments = 48;
    for (int i = 0; i <= discSegments; ++i) {
        float th = (float)i * (2.0f * (float)M_PI / (float)discSegments);
        float u = 0.5f + 0.5f * std::cos(th);
        float v = 0.5f + 0.5f * std::sin(th);
        glTexCoord2f(u, v);
        glVertex3f(moonRadius * std::cos(th), moonRadius * std::sin(th), 0.0f);
    }
    glEnd();
    glPopMatrix();

    glPopAttrib();
}
