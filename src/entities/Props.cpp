#include "Props.h"
#include "Terrain.h"
#include "../graphics/Material.h"
#include "../graphics/TextureManager.h"
#include "../graphics/Primitives.h"
#include <cmath>

void drawTelephonePole(float x, float z, float rotY) {    float gy = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, gy, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(2.4f, 0.0f, 0.0f, 1.0f); // Weathered slight lean

    // Main tall timber pole
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.26f, 0.22f, 0.18f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 4.2f, 0.0f);
    drawBox(0.26f, 8.4f, 0.26f, 1.0f, 4.0f);
    glPopMatrix();

    // Horizontal crossarm near top (6.8m height)
    glPushMatrix();
    glTranslatef(0.0f, 7.2f, 0.0f);
    drawBox(2.6f, 0.18f, 0.16f, 2.0f, 0.5f);

    // Diagonal metal support struts
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(-0.75f, -0.42f, 0.0f);
    glRotatef(-38.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.05f, 0.95f, 0.04f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.75f, -0.42f, 0.0f);
    glRotatef(38.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.05f, 0.95f, 0.04f);
    glPopMatrix();

    // 4 Ceramic / glass electrical insulators with metal mounting pins
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    glColor4f(0.70f, 0.74f, 0.78f, 1.0f);
    float insX[4] = { -1.10f, -0.45f, 0.45f, 1.10f };
    for (int p = 0; p < 4; ++p) {
        glPushMatrix();
        glTranslatef(insX[p], 0.10f, 0.0f);
        drawCylinder(0.045f, 0.03f, 0.22f, 8);
        glTranslatef(0.0f, 0.16f, 0.0f);
        drawCylinder(0.055f, 0.04f, 0.08f, 8); // Flanged top cap
        glPopMatrix();
    }
    glPopMatrix();

    // Sagging electrical cables & loose dangling broken wire (Image 16)
    bindTexture(TEX_NONE);
    applyMaterial(MAT_BLACK_IRON);
    glColor4f(0.12f, 0.12f, 0.12f, 1.0f);
    glLineWidth(2.2f);

    // Loose wire dangling from right insulator all the way down towards the ground
    glBegin(GL_LINE_STRIP);
    glVertex3f(1.10f, 7.35f, 0.0f);
    glVertex3f(1.25f, 6.20f, 0.15f);
    glVertex3f(1.40f, 4.50f, 0.35f);
    glVertex3f(1.30f, 2.80f, 0.20f);
    glVertex3f(1.45f, 1.20f, 0.40f);
    glVertex3f(1.35f, 0.15f, 0.30f);
    glEnd();

    // Cross-catenary hanging loop under crossarm
    glBegin(GL_LINE_STRIP);
    glVertex3f(-1.10f, 7.35f, 0.0f);
    glVertex3f(-0.60f, 6.60f, 0.10f);
    glVertex3f(-0.10f, 6.45f, 0.15f);
    glVertex3f( 0.45f, 7.35f, 0.0f);
    glEnd();

    glPopMatrix();
}

// ----------------------------------------------------------------------------
// HIGH-FIDELITY BROKEN FENCE (Matching Reference Image 14)
// Features: Staggered posts, split-rails, snapped/hanging pickets & ground debris


void drawPumpkin(float x, float y, float z, float scale, float rotY, int faceStyle) {    float groundY = y + getTerrainHeight(x, z);

    glPushMatrix();
    // Position body firmly on the ground surface
    glTranslatef(x, groundY + 0.40f * scale, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glScalef(scale, scale * 0.90f, scale);

    // 1. Ribbed Pumpkin Body (Segmented vertical lobes for authentic organic shape)
    glDisable(GL_CULL_FACE);
    applyMaterial(MAT_PUMPKIN_SKIN);
    bindTexture(TEX_NONE);
    glColor4f(0.95f, 0.46f, 0.12f, 1.0f);

    int numLobes = 10;
    for (int l = 0; l < numLobes; ++l) {
        float lobeAngle = (float)l * (360.0f / numLobes);
        glPushMatrix();
        glRotatef(lobeAngle, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.18f, 0.0f, 0.0f);
        glScalef(0.48f, 0.52f, 0.45f);
        drawSphere(1.0f, 12, 10);
        glPopMatrix();
    }

    // Central pumpkin core to seal body
    glPushMatrix();
    glScalef(0.60f, 0.50f, 0.60f);
    drawSphere(1.0f, 16, 12);
    glPopMatrix();

    // 2. Curled Pumpkin Stem / Stalk on Top
    applyMaterial(MAT_BARK);
    bindTexture(TEX_BARK);
    glPushMatrix();
    glTranslatef(0.0f, 0.48f, 0.0f);
    glRotatef(22.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.065f, 0.035f, 0.28f, 8, 1.0f, 1.0f);
    glPopMatrix();

    // 3. Glowing Carved Face & Localized Ground Halo (if carved Jack-o'-Lantern)
    if (faceStyle >= 0) {
        // Individualized candle flame flicker with slight phase offset
        float localFlicker = g_pumpkinFlicker * (0.88f + 0.12f * std::sin(g_time * 5.2f + x * 2.1f + z * 1.7f));

        if (g_pumpkinLightsOn) {
            // Glowing internal candle flame inside the pumpkin body cavity
            glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT);
            glDisable(GL_LIGHTING);
            bindTexture(TEX_NONE);
            glColor4f(1.0f * localFlicker, 0.95f * localFlicker, 0.50f * localFlicker, 1.0f);
            glPushMatrix();
            glTranslatef(0.0f, -0.05f, 0.05f);
            glScalef(0.14f, 0.22f, 0.14f);
            drawSphere(1.0f, 8, 8);
            glPopMatrix();
            glPopAttrib();

            // Vibrant glowing carved face (fiery yellow-gold)
            glDisable(GL_LIGHTING);
            bindTexture(TEX_NONE);
            glColor4f(1.0f * localFlicker, 0.86f * localFlicker, 0.22f * localFlicker, 1.0f);
        } else {
            // Unlit carved face (dark hollow inside)
            glDisable(GL_LIGHTING);
            bindTexture(TEX_NONE);
            glColor4f(0.06f, 0.03f, 0.01f, 1.0f);
        }

        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.06f); // Slight forward projection to guarantee visibility over ribs

        if (faceStyle == 0) {
            // Classic jagged wicked grin
            glBegin(GL_TRIANGLES);
            // Left Eye (triangular slanted)
            glNormal3f(-0.25f, 0.2f, 0.95f);
            glVertex3f(-0.24f, 0.12f, 0.58f);
            glVertex3f(-0.06f, 0.16f, 0.63f);
            glVertex3f(-0.16f, 0.30f, 0.56f);

            // Right Eye
            glNormal3f(0.25f, 0.2f, 0.95f);
            glVertex3f( 0.06f, 0.16f, 0.63f);
            glVertex3f( 0.24f, 0.12f, 0.58f);
            glVertex3f( 0.16f, 0.30f, 0.56f);

            // Nose
            glNormal3f(0.0f, 0.1f, 1.0f);
            glVertex3f(-0.06f, 0.02f, 0.66f);
            glVertex3f( 0.06f, 0.02f, 0.66f);
            glVertex3f( 0.00f, 0.12f, 0.65f);
            glEnd();

            // Sinister toothy grin
            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0.0f, -0.2f, 0.98f);
            glVertex3f( 0.00f, -0.10f, 0.65f);
            glVertex3f(-0.38f, -0.01f, 0.46f);
            glVertex3f(-0.28f, -0.16f, 0.58f);
            glVertex3f(-0.18f, -0.05f, 0.63f);
            glVertex3f(-0.09f, -0.18f, 0.65f);
            glVertex3f( 0.00f, -0.06f, 0.66f);
            glVertex3f( 0.09f, -0.18f, 0.65f);
            glVertex3f( 0.18f, -0.05f, 0.63f);
            glVertex3f( 0.28f, -0.16f, 0.58f);
            glVertex3f( 0.38f, -0.01f, 0.46f);
            glEnd();
        } else if (faceStyle == 1) {
            // Angled menacing grin
            glBegin(GL_TRIANGLES);
            // Slanted eyes
            glNormal3f(-0.35f, 0.2f, 0.93f);
            glVertex3f(-0.22f, 0.14f, 0.59f);
            glVertex3f(-0.08f, 0.20f, 0.63f);
            glVertex3f(-0.20f, 0.28f, 0.57f);

            glNormal3f(0.35f, 0.2f, 0.93f);
            glVertex3f( 0.08f, 0.20f, 0.63f);
            glVertex3f( 0.22f, 0.14f, 0.59f);
            glVertex3f( 0.20f, 0.28f, 0.57f);

            // Nose
            glNormal3f(0.0f, 0.1f, 1.0f);
            glVertex3f(-0.05f, 0.04f, 0.66f);
            glVertex3f( 0.05f, 0.04f, 0.66f);
            glVertex3f( 0.00f, 0.13f, 0.65f);
            glEnd();

            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0.0f, -0.2f, 0.98f);
            glVertex3f( 0.00f, -0.12f, 0.65f);
            glVertex3f(-0.32f, -0.02f, 0.54f);
            glVertex3f(-0.22f, -0.15f, 0.60f);
            glVertex3f(-0.11f, -0.07f, 0.64f);
            glVertex3f( 0.00f, -0.16f, 0.65f);
            glVertex3f( 0.11f, -0.07f, 0.64f);
            glVertex3f( 0.22f, -0.15f, 0.60f);
            glVertex3f( 0.32f, -0.02f, 0.54f);
            glEnd();
        } else if (faceStyle == 2) {
            // Screaming / haunting ghost face ("O" mouth + tall slanted eyes)
            glBegin(GL_TRIANGLES);
            // Left Eye
            glNormal3f(-0.25f, 0.2f, 0.95f);
            glVertex3f(-0.24f, 0.10f, 0.58f);
            glVertex3f(-0.06f, 0.14f, 0.63f);
            glVertex3f(-0.15f, 0.32f, 0.56f);

            // Right Eye
            glNormal3f(0.25f, 0.2f, 0.95f);
            glVertex3f( 0.06f, 0.14f, 0.63f);
            glVertex3f( 0.24f, 0.10f, 0.58f);
            glVertex3f( 0.15f, 0.32f, 0.56f);

            // Small triangular nose
            glNormal3f(0.0f, 0.1f, 1.0f);
            glVertex3f(-0.04f, 0.03f, 0.66f);
            glVertex3f( 0.04f, 0.03f, 0.66f);
            glVertex3f( 0.00f, 0.11f, 0.65f);
            glEnd();

            // Large hollow tall screaming "O" mouth
            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0.0f, -0.1f, 0.99f);
            glVertex3f(0.00f, -0.12f, 0.65f);
            int mSegs = 12;
            for (int m = 0; m <= mSegs; ++m) {
                float ang = (float)m * (2.0f * (float)M_PI / (float)mSegs);
                glVertex3f(0.14f * std::cos(ang), -0.12f + 0.15f * std::sin(ang), 0.64f);
            }
            glEnd();
        } else if (faceStyle == 3) {
            // Vampire fangs & sinister eyes
            glBegin(GL_TRIANGLES);
            glNormal3f(-0.35f, 0.2f, 0.93f);
            glVertex3f(-0.25f, 0.20f, 0.58f);
            glVertex3f(-0.08f, 0.14f, 0.63f);
            glVertex3f(-0.19f, 0.29f, 0.57f);

            glNormal3f(0.35f, 0.2f, 0.93f);
            glVertex3f( 0.08f, 0.14f, 0.63f);
            glVertex3f( 0.25f, 0.20f, 0.58f);
            glVertex3f( 0.19f, 0.29f, 0.57f);

            // Center nose
            glNormal3f(0.0f, 0.1f, 1.0f);
            glVertex3f(-0.05f, 0.04f, 0.66f);
            glVertex3f( 0.05f, 0.04f, 0.66f);
            glVertex3f( 0.00f, 0.13f, 0.65f);

            // Left upper fang
            glVertex3f(-0.16f, -0.04f, 0.64f);
            glVertex3f(-0.08f, -0.04f, 0.65f);
            glVertex3f(-0.12f, -0.18f, 0.64f);

            // Right upper fang
            glVertex3f( 0.08f, -0.04f, 0.65f);
            glVertex3f( 0.16f, -0.04f, 0.64f);
            glVertex3f( 0.12f, -0.18f, 0.64f);
            glEnd();

            // Wide mouth slit
            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0.0f, -0.2f, 0.98f);
            glVertex3f( 0.00f, -0.08f, 0.65f);
            glVertex3f(-0.34f, -0.02f, 0.52f);
            glVertex3f(-0.22f, -0.13f, 0.60f);
            glVertex3f( 0.00f, -0.15f, 0.65f);
            glVertex3f( 0.22f, -0.13f, 0.60f);
            glVertex3f( 0.34f, -0.02f, 0.52f);
            glEnd();
        } else if (faceStyle == 4) {
            // Crescent smirk
            glBegin(GL_TRIANGLES);
            glNormal3f(-0.25f, 0.2f, 0.95f);
            glVertex3f(-0.22f, 0.15f, 0.60f);
            glVertex3f(-0.06f, 0.19f, 0.63f);
            glVertex3f(-0.14f, 0.27f, 0.58f);

            glNormal3f(0.25f, 0.2f, 0.95f);
            glVertex3f( 0.06f, 0.19f, 0.63f);
            glVertex3f( 0.22f, 0.15f, 0.60f);
            glVertex3f( 0.14f, 0.27f, 0.58f);

            glNormal3f(0.0f, 0.1f, 1.0f);
            glVertex3f(-0.05f, 0.05f, 0.66f);
            glVertex3f( 0.05f, 0.05f, 0.66f);
            glVertex3f( 0.00f, 0.13f, 0.65f);
            glEnd();

            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0.0f, -0.2f, 0.98f);
            glVertex3f( 0.00f, -0.08f, 0.65f);
            glVertex3f(-0.30f,  0.02f, 0.55f);
            glVertex3f(-0.20f, -0.12f, 0.61f);
            glVertex3f(-0.08f, -0.18f, 0.64f);
            glVertex3f( 0.08f, -0.18f, 0.64f);
            glVertex3f( 0.22f, -0.10f, 0.60f);
            glVertex3f( 0.32f,  0.06f, 0.53f);
            glEnd();
        }
        glPopMatrix();
        glEnable(GL_LIGHTING);

        // 4. Localized Warm Radiant Ground Light Halos under Pumpkin
        if (g_pumpkinLightsOn) {
            glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT);
            glDisable(GL_LIGHTING);
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);

            // Wide soft outer ground glow
            glBegin(GL_TRIANGLE_FAN);
            glColor4f(1.0f, 0.55f, 0.08f, 0.38f * localFlicker);
            glVertex3f(0.0f, -0.38f, 0.25f);
            int haloSegs = 16;
            float haloR = 1.30f;
            for (int h = 0; h <= haloSegs; ++h) {
                float ang = (float)h * (2.0f * (float)M_PI / (float)haloSegs);
                glColor4f(1.0f, 0.35f, 0.02f, 0.0f);
                glVertex3f(haloR * std::cos(ang), -0.38f, 0.25f + haloR * std::sin(ang) * 0.85f);
            }
            glEnd();

            // Intense inner warm spot
            glBegin(GL_TRIANGLE_FAN);
            glColor4f(1.0f, 0.78f, 0.22f, 0.55f * localFlicker);
            glVertex3f(0.0f, -0.38f, 0.25f);
            float innerR = 0.55f;
            for (int h = 0; h <= haloSegs; ++h) {
                float ang = (float)h * (2.0f * (float)M_PI / (float)haloSegs);
                glColor4f(1.0f, 0.50f, 0.08f, 0.0f);
                glVertex3f(innerR * std::cos(ang), -0.38f, 0.25f + innerR * std::sin(ang) * 0.85f);
            }
            glEnd();
            glPopAttrib();
        }
    }
    glPopMatrix();
}

// Master Array of Halloween Jack-o'-Lanterns (Lining the Road towards the house, yard & porch)
void drawPumpkinArray() {    // ------------------------------------------------------------------------
    // 1. Road Foreground / Camera Entrance (Z ~ 21.0 to 18.5)
    // ------------------------------------------------------------------------
    drawPumpkin(-1.02f, 0.0f, 20.5f, 0.46f,  35.0f, 0); // Left road entrance (large classic grin)
    drawPumpkin(-1.45f, 0.0f, 20.8f, 0.28f,  60.0f, -1); // Left companion uncarved gourd
    drawPumpkin( 2.53f, 0.0f, 19.2f, 0.42f, -40.0f, 1); // Right road entrance (menacing angled)

    // ------------------------------------------------------------------------
    // 2. Foreground to Mid-Way along Cobblestone Road (Z ~ 17.0 to 13.5)
    // ------------------------------------------------------------------------
    drawPumpkin(-5.50f, 0.0f, 16.2f, 0.46f,  18.0f, 0); // Beside Abandoned House sign
    drawPumpkin(-5.90f, 0.0f, 16.5f, 0.26f,  45.0f, -1); // Gourd beside sign
    drawPumpkin(-1.77f, 0.0f, 16.5f, 0.40f,  28.0f, 2); // Left road edge (ghost screaming face)
    drawPumpkin( 1.42f, 0.0f, 14.8f, 0.44f, -35.0f, 3); // Right road edge (vampire fangs)
    drawPumpkin( 1.78f, 0.0f, 14.5f, 0.26f, -70.0f, -1); // Right road edge companion gourd

    // ------------------------------------------------------------------------
    // 3. Mid-Way S-Curve along Cobblestone Road (Z ~ 12.5 to 7.5)
    // ------------------------------------------------------------------------
    drawPumpkin(-2.70f, 0.0f, 12.2f, 0.43f,  40.0f, 1); // Left curve edge (menacing grin)
    drawPumpkin( 0.20f, 0.0f, 10.5f, 0.40f, -25.0f, 0); // Right curve edge (classic grin)
    drawPumpkin(-3.39f, 0.0f,  8.5f, 0.43f,  32.0f, 4); // Left curve edge (crescent smirk)
    drawPumpkin(-3.75f, 0.0f,  8.2f, 0.27f,  15.0f, -1); // Left companion gourd

    // ------------------------------------------------------------------------
    // 4. Approach to Yard & Porch Walkway (Z ~ 6.5 to 2.5)
    // ------------------------------------------------------------------------
    drawPumpkin(-0.66f, 0.0f,  6.2f, 0.41f, -35.0f, 2); // Right walkway edge (screaming)
    drawPumpkin(-3.91f, 0.0f,  4.2f, 0.45f,  24.0f, 3); // Left walkway edge (vampire fangs)
    drawPumpkin(-1.26f, 0.0f,  2.8f, 0.38f, -20.0f, 0); // Right walkway edge (classic grin)

    // ------------------------------------------------------------------------
    // 5. Porch Steps & House Approach (Z ~ 1.0 to -3.0)
    // ------------------------------------------------------------------------
    drawPumpkin(-4.08f, 0.0f,  0.8f, 0.42f,  20.0f, 1); // Left steps approach (menacing)
    drawPumpkin(-1.54f, 0.0f, -0.5f, 0.39f, -18.0f, 4); // Right steps approach (smirk)
    drawPumpkin(-4.20f, 0.0f, -2.2f, 0.40f,  25.0f, 0); // Left base porch step threshold
    drawPumpkin(-1.65f, 0.0f, -2.2f, 0.38f, -25.0f, 2); // Right base porch step threshold
    drawPumpkin(-1.35f, 0.0f, -2.5f, 0.25f, -45.0f, -1); // Small gourd near steps

    // ------------------------------------------------------------------------
    // 6. Porch Deck Corners
    // ------------------------------------------------------------------------
    drawPumpkin(-6.80f, 0.40f, 5.6f, 0.42f,  35.0f, 0); // Left porch deck corner
    drawPumpkin(-2.40f, 0.40f, 5.6f, 0.38f, -25.0f, 1); // Right porch deck corner
}

// ----------------------------------------------------------------------------
// 3. NATURAL ORGANIC TREES (Small, Compact, Lush Deciduous & Conifer Evergreens)
// ----------------------------------------------------------------------------



void drawRustedCar(float x, float z, float rotY) {    float groundY = getTerrainHeight(x, z);

    glPushMatrix();
    // Partially sunk into mud, with authentic deflated tyre listing & forward pitch
    glTranslatef(x, groundY - 0.10f, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(-5.0f, 0.0f, 0.0f, 1.0f); // Roll listing to the right (flat tyre side)
    glRotatef( 3.4f, 1.0f, 0.0f, 0.0f); // Pitch dipped down at front-right

    // ------------------------------------------------------------------------
    // A. WET MUD RUT & SUNKEN GROUND DEPRESSION UNDER FLAT TYRE
    // ------------------------------------------------------------------------
    applyMaterial(MAT_WET_GROUND);
    bindTexture(TEX_GROUND);
    glPushMatrix();
    glTranslatef(1.02f, 0.04f, 1.35f); // Directly under front-right deflated tyre
    drawBox(0.95f, 0.05f, 1.10f, 1.0f, 1.0f);
    // Surrounding splashed mud ridge
    glTranslatef(0.0f, 0.03f, 0.0f);
    drawBox(1.15f, 0.03f, 1.30f, 1.0f, 1.0f);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // B. LOWER CHASSIS, UNDERCARRIAGE & RUSTY EXHAUST SYSTEM
    // ------------------------------------------------------------------------
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);

    // Lower Chassis Frame / Rocker panels (Beveled to catch light)
    glPushMatrix();
    glTranslatef(0.0f, 0.40f, 0.0f);
    drawBeveledBox(1.95f, 0.26f, 4.40f, 0.04f, 2.0f, 1.5f);
    glPopMatrix();

    // Undercarriage Transmission Tunnel
    glPushMatrix();
    glTranslatef(0.0f, 0.50f, 0.0f);
    drawBox(0.45f, 0.16f, 3.40f);
    glPopMatrix();

    // Rusted Exhaust Pipe & Muffler trailing underneath to the rear
    glPushMatrix();
    glTranslatef(-0.48f, 0.28f, 0.60f);
    // Exhaust pipe from engine bay
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.032f, 0.032f, 1.60f, 8);
    // Rusted oval muffler box
    glTranslatef(0.0f, 1.60f, 0.0f);
    drawBeveledBox(0.32f, 0.16f, 0.65f, 0.03f);
    // Tailpipe leading past rear bumper
    glTranslatef(0.0f, 0.65f, 0.0f);
    drawCylinder(0.030f, 0.030f, 0.65f, 8);
    // Slanted down-turned tailpipe tip
    glTranslatef(0.0f, 0.65f, 0.0f);
    glRotatef(-25.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.030f, 0.028f, 0.18f, 8);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // C. MAIN BODY PANELS, SCULPTED FENDERS, HOOD & TRUNK
    // ------------------------------------------------------------------------
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);

    // Main Lower Body Tub (Wheel wells, lower doors, quarter panels)
    glPushMatrix();
    glTranslatef(0.0f, 0.68f, 0.0f);
    drawBeveledBox(2.18f, 0.38f, 4.62f, 0.06f, 2.0f, 1.8f);
    glPopMatrix();

    // 4 Curved Flared Wheel Well Arches (Fender Flares with mud splatter)
    float archX[2] = { -1.10f, 1.10f };
    float archZ[2] = { -1.35f, 1.35f };
    for (int ix = 0; ix < 2; ++ix) {
        for (int iz = 0; iz < 2; ++iz) {
            glPushMatrix();
            glTranslatef(archX[ix], 0.72f, archZ[iz]);
            drawBeveledBox(0.12f, 0.28f, 1.12f, 0.03f, 0.5f, 0.5f);
            glPopMatrix();
        }
    }

    // Upper Body Waistline / Shoulder Crease (Beveled transition)
    glPushMatrix();
    glTranslatef(0.0f, 0.94f, 0.0f);
    drawBeveledBox(2.08f, 0.22f, 4.42f, 0.04f, 2.0f, 1.2f);
    glPopMatrix();

    // Sloped Front Engine Hood with Raised Central Power Crease
    glPushMatrix();
    glTranslatef(0.0f, 0.98f, 1.35f);
    glRotatef(-4.8f, 1.0f, 0.0f, 0.0f);
    drawBeveledBox(1.98f, 0.14f, 1.72f, 0.04f, 1.5f, 1.0f); // Hood main plate
    // Central raised power bulge / crease line
    glTranslatef(0.0f, 0.05f, 0.0f);
    drawBeveledBox(0.65f, 0.04f, 1.62f, 0.02f);
    // Chrome Hood Center Ornament / Emblem base
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glTranslatef(0.0f, 0.03f, 0.76f);
    drawBox(0.06f, 0.05f, 0.14f);
    glPopMatrix();

    // Sloped Rear Trunk Deck Lid
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(0.0f, 0.98f, -1.55f);
    glRotatef(3.2f, 1.0f, 0.0f, 0.0f);
    drawBeveledBox(1.92f, 0.14f, 1.32f, 0.04f, 1.5f, 1.0f);
    // Chrome Trunk Keyhole Cylinder
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glTranslatef(0.0f, -0.04f, -0.66f);
    drawCylinder(0.025f, 0.025f, 0.03f, 8);
    glPopMatrix();

    // Rear Quarter Panel Fuel Filler Door Flap
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(1.05f, 0.92f, -1.45f);
    drawBox(0.02f, 0.14f, 0.14f);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // D. FRONT GRILLE, HEADLIGHTS, BUMPERS & CRUMPLED ACCENTS
    // ------------------------------------------------------------------------
    // Front Radiator Grille Shell Housing
    glPushMatrix();
    glTranslatef(0.0f, 0.72f, 2.34f);
    drawBeveledBox(1.88f, 0.44f, 0.10f, 0.03f, 1.0f, 0.5f);

    // Deep Dark Radiator Core Mesh behind grille
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.02f);
    drawBox(1.70f, 0.36f, 0.02f);
    glPopMatrix();

    // Chrome Grille Matrix: Vertical Slats & Horizontal Crossbars
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    for (int i = -4; i <= 4; ++i) {
        glPushMatrix();
        glTranslatef(i * 0.18f, 0.0f, 0.055f);
        drawBox(0.028f, 0.34f, 0.035f);
        glPopMatrix();
    }
    for (int j = -1; j <= 1; ++j) {
        glPushMatrix();
        glTranslatef(0.0f, j * 0.11f, 0.055f);
        drawBox(1.68f, 0.025f, 0.035f);
        glPopMatrix();
    }
    // Center Vintage Insignia Emblem Badge
    glPushMatrix();
    glTranslatef(0.0f, 0.06f, 0.075f);
    drawSphere(0.055f, 10, 8);
    glPopMatrix();

    glPopMatrix();

    // Dual Round Headlights (Left Intact with Glass, Right Broken with Exposed Bulb!)
    float headLightX[2] = { -0.74f, 0.74f };
    // 1. Left Headlight (Intact, chrome bezel with fluted reflective glass lens)
    glPushMatrix();
    glTranslatef(headLightX[0], 0.78f, 2.34f);
    // Chrome Bezel Housing
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawCylinder(0.165f, 0.165f, 0.06f, 14, 1.0f, 0.2f);
    // Chrome Reflector Bowl inside
    glTranslatef(0.0f, 0.0f, 0.02f);
    drawSphere(0.13f, 10, 8);
    // Glass Convex Lens (High specular glint)
    glTranslatef(0.0f, 0.0f, 0.04f);
    applyMaterial(MAT_CAR_GLASS);
    drawSphere(0.145f, 12, 10);
    // Soft specular lens flare halo
    drawBillboardHalo(0.0f, 0.0f, 0.08f, 0.45f, 0.85f, 0.92f, 1.0f, 0.35f);
    glPopMatrix();

    // 2. Right Headlight (Broken/Abandoned: dented chrome rim, shattered shards, exposed bulb!)
    glPushMatrix();
    glTranslatef(headLightX[1], 0.78f, 2.34f);
    glRotatef(6.0f, 0.0f, 1.0f, 0.2f); // Askew/dented
    // Dented Chrome Bezel
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawCylinder(0.165f, 0.150f, 0.05f, 12, 1.0f, 0.2f);
    // Dark Empty Lamp Bucket
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glTranslatef(0.0f, 0.0f, 0.015f);
    drawSphere(0.12f, 8, 6);
    // Tiny Exposed Tungsten Filament Bulb on Wire
    applyMaterial(MAT_BULB_EMISSIVE);
    bindTexture(TEX_NONE);
    glTranslatef(0.0f, 0.0f, 0.025f);
    drawSphere(0.035f, 8, 6);
    drawBillboardHalo(0.0f, 0.0f, 0.02f, 0.22f, 1.0f, 0.70f, 0.25f, 0.30f);
    // Broken Glass Shards on rim edge
    applyMaterial(MAT_CAR_GLASS);
    glTranslatef(0.08f, -0.06f, 0.01f);
    drawBox(0.04f, 0.06f, 0.015f);
    glPopMatrix();

    // Heavy Front Bumper Bar with Overriders (Bumperettes) & Frame Brackets
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(0.0f, 0.44f, 2.42f);
    drawBeveledBox(2.28f, 0.14f, 0.12f, 0.03f, 2.0f, 0.3f);
    // Frame Mounting Brackets
    glTranslatef(-0.55f, 0.0f, -0.10f);
    drawBox(0.08f, 0.10f, 0.12f);
    glTranslatef(1.10f, 0.0f, 0.0f);
    drawBox(0.08f, 0.10f, 0.12f);
    // Chrome / Rusted Overrider Guards with Rubber Buffer Pads
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glTranslatef(0.0f, 0.06f, 0.14f);
    drawBeveledBox(0.08f, 0.28f, 0.08f, 0.02f);
    glTranslatef(-1.10f, 0.0f, 0.0f);
    drawBeveledBox(0.08f, 0.28f, 0.08f, 0.02f);
    glPopMatrix();

    // Heavy Rear Bumper Bar & Bent Rusted License Plate
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(0.0f, 0.44f, -2.36f);
    drawBeveledBox(2.22f, 0.14f, 0.12f, 0.03f, 2.0f, 0.3f);
    // Rear Overriders
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glTranslatef(-0.55f, 0.05f, -0.04f);
    drawBeveledBox(0.08f, 0.26f, 0.08f, 0.02f);
    glTranslatef(1.10f, 0.0f, 0.0f);
    drawBeveledBox(0.08f, 0.26f, 0.08f, 0.02f);

    // Vintage License Plate hanging askew by one loose bolt ("H0RR0R-70")
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    glTranslatef(-0.55f, 0.02f, -0.05f);
    glRotatef(14.0f, 0.0f, 0.0f, 1.0f); // Tilted askew
    drawBox(0.42f, 0.20f, 0.015f);
    // Dark stamp border on plate
    applyMaterial(MAT_DARK_WOOD);
    drawBox(0.38f, 0.16f, 0.018f);
    glPopMatrix();

    // Red Glass Tail Light Lenses with Chrome Bezels
    for (int i = 0; i < 2; ++i) {
        // Chrome Bezel
        applyMaterial(MAT_CHROME_TRIM);
        bindTexture(TEX_NONE);
        glPushMatrix();
        glTranslatef(headLightX[i], 0.82f, -2.34f);
        drawBox(0.20f, 0.14f, 0.04f);
        // Red Glass Lens
        applyMaterial(MAT_PUMPKIN_SKIN);
        glTranslatef(0.0f, 0.0f, -0.02f);
        drawBox(0.16f, 0.10f, 0.03f);
        glPopMatrix();
    }

    // ------------------------------------------------------------------------
    // E. CABIN, BEVELED ROOF, PILLARS & DARK REFLECTIVE WINDOWS
    // ------------------------------------------------------------------------
    // Tapered Cabin Roof with Projecting Rain Gutters / Drip Rails
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(0.0f, 1.68f, -0.32f);
    drawBeveledBox(1.74f, 0.06f, 2.10f, 0.025f, 1.5f, 1.5f);
    // Left & Right Rain Gutters
    glTranslatef(-0.88f, -0.02f, 0.0f);
    drawBox(0.04f, 0.04f, 2.12f);
    glTranslatef(1.76f, 0.0f, 0.0f);
    drawBox(0.04f, 0.04f, 2.12f);
    glPopMatrix();

    // A-Pillars (Front windshield frame struts, sloped at 34 deg)
    glPushMatrix();
    glTranslatef(-0.84f, 1.34f, 0.44f);
    glRotatef(34.0f, 1.0f, 0.0f, 0.0f);
    drawBeveledBox(0.06f, 0.74f, 0.06f, 0.015f);
    glTranslatef(1.68f, 0.0f, 0.0f);
    drawBeveledBox(0.06f, 0.74f, 0.06f, 0.015f);
    glPopMatrix();

    // B-Pillars (Middle vertical side frame)
    glPushMatrix();
    glTranslatef(-0.84f, 1.34f, -0.32f);
    drawBeveledBox(0.06f, 0.66f, 0.06f, 0.015f);
    glTranslatef(1.68f, 0.0f, 0.0f);
    drawBeveledBox(0.06f, 0.66f, 0.06f, 0.015f);
    glPopMatrix();

    // C-Pillars (Rear window frame struts / sail panels, sloped at -28 deg)
    glPushMatrix();
    glTranslatef(-0.84f, 1.34f, -1.06f);
    glRotatef(-28.0f, 1.0f, 0.0f, 0.0f);
    drawBeveledBox(0.08f, 0.72f, 0.08f, 0.02f);
    glTranslatef(1.68f, 0.0f, 0.0f);
    drawBeveledBox(0.08f, 0.72f, 0.08f, 0.02f);
    glPopMatrix();

    // Front Windshield (Dark reflective tinted glass)
    applyMaterial(MAT_CAR_GLASS);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 1.34f, 0.46f);
    glRotatef(34.0f, 1.0f, 0.0f, 0.0f);
    drawBox(1.60f, 0.65f, 0.035f);

    // Cracked Windshield Spiderweb Fractures (Intricate etched impact lines)
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glColor4f(0.88f, 0.92f, 1.00f, 0.82f);
    glBegin(GL_LINES);
    // Impact epicenter at driver's eye level (-0.36, 0.12)
    float cx = -0.36f, cy = 0.12f, cz = 0.024f;
    // 8 Long Radial Shatter Cracks
    glVertex3f(cx, cy, cz); glVertex3f(cx - 0.42f, cy + 0.24f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx + 0.48f, cy + 0.20f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx - 0.32f, cy - 0.28f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx + 0.38f, cy - 0.26f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx - 0.52f, cy - 0.06f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx + 0.62f, cy - 0.10f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx + 0.18f, cy + 0.30f, cz);
    glVertex3f(cx, cy, cz); glVertex3f(cx - 0.18f, cy - 0.32f, cz);

    // Inner Concentric Shatter Rings (Shockwave ripples)
    glVertex3f(cx - 0.08f, cy + 0.04f, cz); glVertex3f(cx + 0.06f, cy + 0.08f, cz);
    glVertex3f(cx + 0.06f, cy + 0.08f, cz); glVertex3f(cx + 0.09f, cy - 0.05f, cz);
    glVertex3f(cx + 0.09f, cy - 0.05f, cz); glVertex3f(cx - 0.06f, cy - 0.08f, cz);
    glVertex3f(cx - 0.06f, cy - 0.08f, cz); glVertex3f(cx - 0.08f, cy + 0.04f, cz);

    // Outer Concentric Shatter Ring
    glVertex3f(cx - 0.18f, cy + 0.08f, cz); glVertex3f(cx + 0.14f, cy + 0.16f, cz);
    glVertex3f(cx + 0.14f, cy + 0.16f, cz); glVertex3f(cx + 0.20f, cy - 0.12f, cz);
    glVertex3f(cx + 0.20f, cy - 0.12f, cz); glVertex3f(cx - 0.14f, cy - 0.16f, cz);
    glVertex3f(cx - 0.14f, cy - 0.16f, cz); glVertex3f(cx - 0.18f, cy + 0.08f, cz);
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Windshield Wipers: Driver's wiper frozen mid-sweep, passenger wiper at cowl
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_NONE);
    // Driver Wiper Arm (Frozen halfway up windshield at 48 deg)
    glPushMatrix();
    glTranslatef(-0.38f, 1.14f, 0.72f);
    glRotatef(34.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-48.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.022f, 0.42f, 0.022f); // Arm
    glTranslatef(0.015f, 0.18f, 0.015f);
    drawBox(0.012f, 0.36f, 0.018f); // Blade
    glPopMatrix();

    // Passenger Wiper Arm (Bent/resting on lower cowl)
    glPushMatrix();
    glTranslatef(0.38f, 1.10f, 0.72f);
    glRotatef(34.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-15.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.022f, 0.38f, 0.022f);
    glPopMatrix();

    // Rear Window Glass (Dark reflective sloped glass)
    applyMaterial(MAT_CAR_GLASS);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 1.34f, -1.05f);
    glRotatef(-28.0f, 1.0f, 0.0f, 0.0f);
    drawBox(1.58f, 0.64f, 0.035f);
    glPopMatrix();

    // Right Side Windows (Front passenger & rear quarter glass)
    glPushMatrix();
    glTranslatef(0.85f, 1.34f, 0.02f);
    drawBox(0.035f, 0.58f, 0.62f); // Front right window
    glTranslatef(0.0f, 0.0f, -0.68f);
    drawBox(0.035f, 0.58f, 0.62f); // Rear right window
    glPopMatrix();

    // Left Rear Quarter Window
    glPushMatrix();
    glTranslatef(-0.85f, 1.34f, -0.66f);
    drawBox(0.035f, 0.58f, 0.62f);
    glPopMatrix();

    // Interior Rear-View Mirror hanging from center roof header
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 1.62f, 0.32f);
    drawCylinder(0.015f, 0.015f, 0.08f, 6); // Stem
    glTranslatef(0.0f, -0.06f, 0.0f);
    glRotatef(12.0f, 1.0f, 0.0f, 0.0f);
    drawBeveledBox(0.18f, 0.06f, 0.03f, 0.01f); // Housing
    applyMaterial(MAT_CAR_GLASS);
    glTranslatef(0.0f, 0.0f, 0.016f);
    drawBox(0.16f, 0.045f, 0.01f); // Mirror face
    glPopMatrix();

    // ------------------------------------------------------------------------
    // F. OPEN DRIVER'S DOOR (Ajar at 38 deg) & SNAPPED DANGLING SIDE MIRROR
    // ------------------------------------------------------------------------
    // Open Driver's Door (Swung open at 38 degrees on rusty hinges)
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(-0.95f, 0.68f, 0.44f); // Door hinge pivot on A-pillar
    glRotatef(38.0f, 0.0f, 1.0f, 0.0f);  // Swung outward into the yard
    glTranslatef(0.0f, 0.0f, -0.44f);

    // Upper and Lower Heavy Door Hinges
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.42f);
    drawCylinder(0.03f, 0.03f, 0.06f, 8);
    glTranslatef(0.0f, -0.44f, 0.0f);
    drawCylinder(0.03f, 0.03f, 0.06f, 8);
    glPopMatrix();

    // Outer Lower Door Panel (Beveled sheet metal)
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    drawBeveledBox(0.08f, 0.58f, 0.88f, 0.02f, 0.5f, 1.0f);

    // Chrome Outer Push-Button Door Handle
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.055f, 0.18f, -0.32f);
    drawBox(0.035f, 0.04f, 0.14f);
    drawSphere(0.018f, 8, 6); // Push button
    glPopMatrix();

    // Door Window Frame Border
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glTranslatef(0.0f, 0.52f, 0.0f);
    drawBox(0.06f, 0.48f, 0.06f); // Front vertical post
    glTranslatef(0.0f, 0.0f, -0.82f);
    drawBox(0.06f, 0.48f, 0.06f); // Rear vertical post
    glTranslatef(0.0f, 0.22f, 0.41f);
    drawBox(0.06f, 0.06f, 0.88f); // Top header sash

    // Partially Rolled-Down / Broken Driver's Glass
    applyMaterial(MAT_CAR_GLASS);
    bindTexture(TEX_NONE);
    glTranslatef(0.0f, -0.20f, 0.0f);
    drawBox(0.025f, 0.24f, 0.74f);

    // Inner Door Trim Card (Armrest & interior chrome handle)
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.048f, -0.32f, 0.0f);
    drawBox(0.025f, 0.44f, 0.80f); // Trim panel
    // Molded Armrest
    glTranslatef(0.02f, -0.05f, 0.0f);
    drawBeveledBox(0.05f, 0.08f, 0.35f, 0.015f);
    // Inner Chrome Door Latch Handle & Window Crank
    applyMaterial(MAT_CHROME_TRIM);
    glTranslatef(0.02f, 0.12f, 0.15f);
    drawBox(0.03f, 0.035f, 0.08f);
    glPopMatrix();

    // Snapped Dangling Side Mirror (Torn from bracket, hanging by twisted wire)
    glPushMatrix();
    glTranslatef(-0.06f, -0.24f, 0.40f);
    // Broken mounting bracket stub
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_NONE);
    drawBox(0.03f, 0.04f, 0.04f);

    // Dangling wire lines
    glDisable(GL_LIGHTING);
    glColor3f(0.85f, 0.45f, 0.20f); // Copper wire
    glLineWidth(1.8f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.05f, -0.14f, 0.06f);
    glEnd();
    glEnable(GL_LIGHTING);

    // Dangling Mirror Housing (Hanging askew at 52 deg)
    glTranslatef(-0.05f, -0.14f, 0.06f);
    glRotatef(52.0f, 1.0f, 0.2f, 0.8f);
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawBeveledBox(0.025f, 0.13f, 0.19f, 0.01f); // Chrome housing
    // Mirror Glass Face (High specular reflection)
    applyMaterial(MAT_CAR_GLASS);
    glTranslatef(-0.015f, 0.0f, 0.0f);
    drawBox(0.01f, 0.11f, 0.17f);
    glPopMatrix();

    glPopMatrix(); // End Open Driver's Door

    // ------------------------------------------------------------------------
    // G. DETAILED VINTAGE INTERIOR (Viewable through open door & windows)
    // ------------------------------------------------------------------------
    // Dusty Dashboard with Gauge Cluster & Glovebox
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 1.08f, 0.38f);
    drawBeveledBox(1.68f, 0.22f, 0.42f, 0.02f);
    // Instrument Gauge Binnacle (Speedometer dial & fuel/temp gauge)
    applyMaterial(MAT_DARK_WOOD);
    glPushMatrix();
    glTranslatef(-0.42f, 0.04f, -0.18f);
    drawCylinder(0.07f, 0.07f, 0.04f, 12); // Speedometer housing
    glTranslatef(0.20f, 0.0f, 0.0f);
    drawCylinder(0.05f, 0.05f, 0.04f, 10); // Aux gauge
    // Speedometer needle
    applyMaterial(MAT_PUMPKIN_SKIN);
    glTranslatef(-0.20f, 0.0f, 0.042f);
    drawBox(0.008f, 0.05f, 0.008f);
    glPopMatrix();
    // Glovebox Door with Chrome Button
    applyMaterial(MAT_CAR_INTERIOR);
    glPushMatrix();
    glTranslatef(0.45f, -0.04f, -0.18f);
    drawBox(0.42f, 0.14f, 0.02f);
    applyMaterial(MAT_CHROME_TRIM);
    glTranslatef(0.14f, 0.0f, -0.015f);
    drawSphere(0.015f, 6, 6);
    glPopMatrix();
    glPopMatrix();

    // 3-Spoke Classic Dished Steering Wheel on Tilted Column
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.42f, 1.14f, 0.16f);
    glRotatef(-35.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.028f, 0.028f, 0.28f, 8); // Column
    glTranslatef(0.0f, 0.28f, 0.0f);
    // Wheel Rim
    applyMaterial(MAT_RUSTY_METAL);
    drawSphere(0.19f, 14, 10);
    // 3 Chrome Spokes
    applyMaterial(MAT_CHROME_TRIM);
    for (int s = 0; s < 3; ++s) {
        glPushMatrix();
        glRotatef(s * 120.0f, 0.0f, 1.0f, 0.0f);
        drawBox(0.018f, 0.012f, 0.16f);
        glPopMatrix();
    }
    // Center Horn Button
    drawSphere(0.04f, 8, 8);
    glPopMatrix();

    // Floor Shifter Lever & Foot Pedals
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.08f, 0.58f, 0.05f);
    // Wrinkled Rubber Shift Boot
    applyMaterial(MAT_RUBBER_TYRE);
    drawCylinder(0.08f, 0.03f, 0.08f, 8);
    // Chrome Shifter Lever
    applyMaterial(MAT_CHROME_TRIM);
    glTranslatef(0.0f, 0.08f, 0.0f);
    glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.015f, 0.015f, 0.26f, 6);
    // Spherical Shift Knob
    glTranslatef(0.0f, 0.26f, 0.0f);
    applyMaterial(MAT_DARK_WOOD);
    drawSphere(0.035f, 8, 8);
    glPopMatrix();

    // Suspended Foot Pedals (Clutch, Brake, Accelerator)
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_NONE);
    for (int p = -1; p <= 1; ++p) {
        glPushMatrix();
        glTranslatef(-0.42f + p * 0.10f, 0.62f, 0.35f);
        drawBox(0.045f, 0.065f, 0.02f);
        glPopMatrix();
    }

    // Torn Split-Bench Front Seat (Worn upholstery, exposed yellow foam & rusted springs!)
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_BARK);
    glPushMatrix();
    glTranslatef(0.0f, 0.78f, -0.15f);
    drawBeveledBox(1.64f, 0.24f, 0.56f, 0.03f); // Seat bottom cushion
    glTranslatef(0.0f, 0.28f, -0.24f);
    drawBeveledBox(1.64f, 0.46f, 0.16f, 0.03f); // Backrest

    // Severe Tear on Driver's Seat Cushion (Exposed foam & rusted spring wire)
    applyMaterial(MAT_SEAT_FOAM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.42f, -0.20f, 0.18f);
    drawBox(0.38f, 0.12f, 0.26f); // Exposed foam core
    // Rusted coiled seat springs poking out
    applyMaterial(MAT_RUSTY_METAL);
    for (int sp = 0; sp < 3; ++sp) {
        glPushMatrix();
        glTranslatef((sp - 1) * 0.09f, 0.08f, 0.0f);
        drawCylinder(0.025f, 0.025f, 0.06f, 6);
        glPopMatrix();
    }
    glPopMatrix();
    glPopMatrix();

    // Rear Passenger Bench Seat
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_BARK);
    glPushMatrix();
    glTranslatef(0.0f, 0.78f, -0.92f);
    drawBeveledBox(1.64f, 0.24f, 0.52f, 0.03f); // Rear cushion
    glTranslatef(0.0f, 0.28f, -0.22f);
    drawBeveledBox(1.64f, 0.44f, 0.14f, 0.03f); // Rear backrest
    glPopMatrix();

    // ------------------------------------------------------------------------
    // H. WHEELS & DEFLATED SQUASHED FRONT-RIGHT TYRE
    // ------------------------------------------------------------------------
    float wheelX = 1.02f;
    float wheelZ_front = 1.35f;
    float wheelZ_rear  = -1.35f;

    // 1. Rear-Left Wheel (Inflated)
    glPushMatrix();
    glTranslatef(-wheelX - 0.12f, 0.42f, wheelZ_rear);
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_BARK);
    drawCylinder(0.42f, 0.42f, 0.24f, 16, 1.0f, 1.0f);
    // Rusted steel deep-dish rim & chrome hubcap
    glTranslatef(0.0f, -0.01f, 0.0f);
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    drawCylinder(0.24f, 0.24f, 0.04f, 12, 0.5f, 0.5f);
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawSphere(0.12f, 10, 8);
    glPopMatrix();

    // 2. Rear-Right Wheel (Inflated)
    glPushMatrix();
    glTranslatef(wheelX - 0.12f, 0.42f, wheelZ_rear);
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_BARK);
    drawCylinder(0.42f, 0.42f, 0.24f, 16, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.21f, 0.0f);
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    drawCylinder(0.24f, 0.24f, 0.04f, 12, 0.5f, 0.5f);
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawSphere(0.12f, 10, 8);
    glPopMatrix();

    // 3. Front-Left Wheel (Inflated)
    glPushMatrix();
    glTranslatef(-wheelX - 0.12f, 0.42f, wheelZ_front);
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_BARK);
    drawCylinder(0.42f, 0.42f, 0.24f, 16, 1.0f, 1.0f);
    glTranslatef(0.0f, -0.01f, 0.0f);
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    drawCylinder(0.24f, 0.24f, 0.04f, 12, 0.5f, 0.5f);
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawSphere(0.12f, 10, 8);
    glPopMatrix();

    // 4. Front-Right Wheel (Deflated, Severely Flat & Squashed into Mud Rut)
    glPushMatrix();
    glTranslatef(wheelX - 0.14f, 0.22f, wheelZ_front);
    glScalef(1.36f, 0.44f, 1.28f); // Severely flattened oval pancake squashed tyre
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_BARK);
    drawCylinder(0.42f, 0.42f, 0.28f, 16, 1.0f, 1.0f);
    // Sunken rusted rim resting on flattened rubber
    glTranslatef(0.0f, 0.24f, 0.0f);
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    drawCylinder(0.24f, 0.24f, 0.04f, 12, 0.5f, 0.5f);
    // Dented tarnished hubcap
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawSphere(0.10f, 8, 6);
    glPopMatrix();

    glPopMatrix(); // End Car
}

