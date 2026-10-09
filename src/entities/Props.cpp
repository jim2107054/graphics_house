#include "Props.h"
#include "Terrain.h"
#include "House.h"
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
            glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
            glDisable(GL_LIGHTING);
            bindTexture(TEX_NONE);
            glColor4f(1.0f * localFlicker, 0.86f * localFlicker, 0.22f * localFlicker, 1.0f);
        } else {
            // Unlit carved face (dark hollow inside)
            glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
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
        glPopAttrib();

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
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
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



// ============================================================================
// REALISTIC 1957 VINTAGE CRUISER WRECK ("VANGLA CAR")
// Fully sculpted vintage American cruiser featuring authentic crash damage:
// - Popped-ajar crumpled hood revealing fully exposed detailed V8 engine bay
// - Smashed collision-twisted front chrome bumper & bent radiator grille
// - Realistic panoramic wrap-around safety windshield with intricate fractures
// - Authentic hollow dished 2-spoke vintage steering wheel with chrome horn ring
// - Ripped two-tone tuck-and-roll bench seat with yellow foam & coiled steel springs
// - Authentic wide whitewall wheels, severed tie-rod, and flattened pancake tire
// - Splintered fence rails, fallen hubcap, oil slick puddle & mud depression rut
// ============================================================================

static const Material MAT_CAR_PAINT_TURQUOISE = {
    { 0.16f, 0.28f, 0.30f, 1.0f },
    { 0.34f, 0.58f, 0.62f, 1.0f },
    { 0.42f, 0.54f, 0.56f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    32.0f
};

static const Material MAT_CAR_RUST_WEATHERED = {
    { 0.24f, 0.15f, 0.10f, 1.0f },
    { 0.52f, 0.32f, 0.20f, 1.0f },
    { 0.18f, 0.14f, 0.10f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    14.0f
};

static const Material MAT_WHITEWALL_RUBBER = {
    { 0.50f, 0.48f, 0.45f, 1.0f },
    { 0.88f, 0.86f, 0.82f, 1.0f },
    { 0.20f, 0.20f, 0.20f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    24.0f
};

static const Material MAT_ENGINE_IRON = {
    { 0.12f, 0.12f, 0.14f, 1.0f },
    { 0.28f, 0.28f, 0.30f, 1.0f },
    { 0.35f, 0.35f, 0.38f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    28.0f
};

static const Material MAT_VALVE_RED = {
    { 0.35f, 0.08f, 0.06f, 1.0f },
    { 0.82f, 0.18f, 0.14f, 1.0f },
    { 0.50f, 0.25f, 0.20f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    45.0f
};

static const Material MAT_BRASS_CORE = {
    { 0.28f, 0.22f, 0.08f, 1.0f },
    { 0.68f, 0.54f, 0.22f, 1.0f },
    { 0.55f, 0.46f, 0.18f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    48.0f
};

static const Material MAT_OIL_SLICK = {
    { 0.04f, 0.05f, 0.05f, 0.88f },
    { 0.10f, 0.12f, 0.12f, 0.88f },
    { 0.45f, 0.38f, 0.48f, 0.88f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    75.0f
};

// Helper: Hollow Vintage Dished Steering Wheel (Replaces solid sphere!)
static void drawVintageSteeringWheel() {
    glPushMatrix();
    // Steering column angled at 36 degrees from dashboard firewall
    applyMaterial(MAT_BLACK_IRON);
    bindTexture(TEX_NONE);
    drawCylinder(0.022f, 0.022f, 0.34f, 10);

    // Turn signal lever wand on column
    glPushMatrix();
    glTranslatef(-0.025f, 0.22f, 0.0f);
    glRotatef(75.0f, 0.0f, 0.0f, 1.0f);
    drawCylinder(0.005f, 0.005f, 0.11f, 6);
    glTranslatef(0.0f, 0.11f, 0.0f);
    applyMaterial(MAT_STONE);
    drawSphere(0.012f, 6, 6);
    glPopMatrix();

    // Wheel Hub Base
    glTranslatef(0.0f, 0.34f, 0.0f);
    applyMaterial(MAT_CHROME_TRIM);
    drawCylinder(0.048f, 0.044f, 0.025f, 12);

    // Center Horn Button with vintage emblem
    glTranslatef(0.0f, 0.02f, 0.0f);
    applyMaterial(MAT_CAR_PAINT_TURQUOISE);
    drawSphere(0.038f, 10, 8);
    applyMaterial(MAT_CHROME_TRIM);
    drawSphere(0.020f, 8, 6);

    // Two Horizontal Spokes
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    drawBox(0.36f, 0.012f, 0.024f);
    glPopMatrix();

    // Lower Chrome Half-Horn Ring
    glPushMatrix();
    glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);
    int hornSegs = 10;
    float hornRadius = 0.125f;
    for (int i = 0; i < hornSegs; ++i) {
        float a1 = 3.14159f * 0.25f + (float)i * (3.14159f * 0.50f / hornSegs);
        float a2 = 3.14159f * 0.25f + (float)(i + 1) * (3.14159f * 0.50f / hornSegs);
        float x1 = hornRadius * cosf(a1), z1 = -hornRadius * sinf(a1);
        float x2 = hornRadius * cosf(a2), z2 = -hornRadius * sinf(a2);
        glPushMatrix();
        glTranslatef((x1 + x2) * 0.5f, 0.01f, (z1 + z2) * 0.5f);
        drawBox(fabsf(x2 - x1) + 0.010f, 0.008f, fabsf(z2 - z1) + 0.010f);
        glPopMatrix();
    }
    glPopMatrix();

    // Hollow Outer Steering Wheel Ring (16 smooth tube segments - NOT a solid sphere!)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_NONE);
    float rimRadius = 0.205f;
    int rimSegs = 16;
    for (int i = 0; i < rimSegs; ++i) {
        float theta1 = (float)i * (2.0f * 3.14159265f / rimSegs);
        float theta2 = (float)(i + 1) * (2.0f * 3.14159265f / rimSegs);
        float x1 = rimRadius * cosf(theta1);
        float z1 = rimRadius * sinf(theta1);
        float x2 = rimRadius * cosf(theta2);
        float z2 = rimRadius * sinf(theta2);

        float mx = (x1 + x2) * 0.5f;
        float mz = (z1 + z2) * 0.5f;
        float dx = x2 - x1;
        float dz = z2 - z1;
        float segLen = sqrtf(dx * dx + dz * dz);
        float segAngle = atan2f(dz, dx) * (180.0f / 3.14159265f);

        glPushMatrix();
        glTranslatef(mx, 0.018f, mz);
        glRotatef(-segAngle, 0.0f, 1.0f, 0.0f);
        drawBox(segLen + 0.006f, 0.024f, 0.024f);
        glPopMatrix();
    }
    glPopMatrix();
}

// Helper: Coiled wire seat spring poking out of torn upholstery
static void drawSeatSpring(float radius, float totalHeight, int coils) {
    glDisable(GL_LIGHTING);
    glColor3f(0.68f, 0.42f, 0.26f); // Oxidized rusty spring wire
    glLineWidth(2.6f);
    glBegin(GL_LINE_STRIP);
    int steps = coils * 14;
    for (int i = 0; i <= steps; ++i) {
        float t = (float)i / steps;
        float angle = t * coils * 2.0f * 3.14159265f;
        float sx = radius * cosf(angle);
        float sz = radius * sinf(angle);
        float sy = t * totalHeight;
        glVertex3f(sx, sy, sz);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

// Helper: Authentic 1950s Whitewall Wheel with deep dish rim & chrome hubcap
static void drawWhitewallWheel(bool isFlat, bool hasHubcap, bool isLeftSide) {
    glPushMatrix();
    if (isFlat) {
        // Severely deflated flattened tire pressed into mud rut
        glScalef(1.0f, 0.44f, 1.25f);
    }

    // Cylinder along X axis (axle direction):
    // For left side, outer face points to -X; for right side, outer face points to +X.
    glRotatef(isLeftSide ? 90.0f : -90.0f, 0.0f, 0.0f, 1.0f);

    float tireR = 0.42f;
    float tireW = 0.22f;

    // Outer Tire Tread (Black Rubber)
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_BARK);
    drawCylinder(tireR, tireR, tireW, 18, 1.0f, 1.0f);

    // Outer Sidewall Face (at y = tireW)
    glPushMatrix();
    glTranslatef(0.0f, tireW + 0.002f, 0.0f);

    // Outer Whitewall Annulus Ring (r = 0.22f to 0.36f in XZ plane)
    applyMaterial(MAT_WHITEWALL_RUBBER);
    bindTexture(TEX_NONE);
    int ringSegs = 20;
    glBegin(GL_QUADS);
    for (int s = 0; s < ringSegs; ++s) {
        float a1 = (float)s * (2.0f * 3.14159265f / ringSegs);
        float a2 = (float)(s + 1) * (2.0f * 3.14159265f / ringSegs);
        float rIn = 0.22f, rOut = 0.36f;
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(rIn * cosf(a1), 0.0f, rIn * sinf(a1));
        glVertex3f(rOut * cosf(a1), 0.0f, rOut * sinf(a1));
        glVertex3f(rOut * cosf(a2), 0.0f, rOut * sinf(a2));
        glVertex3f(rIn * cosf(a2), 0.0f, rIn * sinf(a2));
    }
    glEnd();

    // Recessed Steel Rim (dish)
    applyMaterial(MAT_CAR_RUST_WEATHERED);
    bindTexture(TEX_RUST);
    drawCylinder(0.22f, 0.22f, 0.04f, 14, 0.5f, 0.5f);

    if (hasHubcap) {
        // Polished Chrome Baby-Moon / Dog-Dish Hubcap
        glTranslatef(0.0f, 0.02f, 0.0f);
        applyMaterial(MAT_CHROME_TRIM);
        bindTexture(TEX_NONE);
        drawSphere(0.13f, 12, 10);
        // Center vintage embossed star emblem
        glTranslatef(0.0f, 0.03f, 0.0f);
        applyMaterial(MAT_CAR_PAINT_TURQUOISE);
        drawSphere(0.04f, 8, 6);
    } else {
        // Missing hubcap on crash wheel: Exposed rusty axle snout & 5 lug nuts
        applyMaterial(MAT_BLACK_IRON);
        bindTexture(TEX_NONE);
        drawCylinder(0.065f, 0.065f, 0.045f, 10);
        // 5 Lug Nuts
        applyMaterial(MAT_CHROME_TRIM);
        for (int l = 0; l < 5; ++l) {
            float la = (float)l * (2.0f * 3.14159265f / 5.0f);
            glPushMatrix();
            glTranslatef(0.12f * cosf(la), 0.02f, 0.12f * sinf(la));
            drawBox(0.022f, 0.025f, 0.022f);
            glPopMatrix();
        }
    }
    glPopMatrix(); // End outer sidewall face
    glPopMatrix();
}

// Master Rusted Car Render Routine
void drawRustedCar(float x, float z, float rotY) {
    float groundY = getTerrainHeight(x, z);

    glPushMatrix();
    // Partially sunk into mud with deflated tyre listing & front-right pitch
    glTranslatef(x, groundY - 0.12f, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(-5.4f, 0.0f, 0.0f, 1.0f); // Roll tilt towards flat front-right tyre
    glRotatef( 3.8f, 1.0f, 0.0f, 0.0f); // Pitch dipped down at front bumper

    // ------------------------------------------------------------------------
    // 1. GROUND DEBRIS, MUD DEPRESSION, OIL SLICK & SPLINTERED FENCE
    // ------------------------------------------------------------------------
    // Deep Mud Depression under flat front-right tire
    applyMaterial(MAT_WET_GROUND);
    bindTexture(TEX_GROUND);
    glPushMatrix();
    glTranslatef(1.04f, 0.03f, 1.35f);
    drawBox(1.10f, 0.05f, 1.25f, 1.0f, 1.0f);
    glTranslatef(0.0f, 0.03f, 0.0f);
    drawBox(1.30f, 0.03f, 1.45f, 1.0f, 1.0f);
    glPopMatrix();

    // Motor Oil Puddle under cracked engine oil pan
    applyMaterial(MAT_OIL_SLICK);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.35f, 0.045f, 1.15f);
    drawBox(1.15f, 0.02f, 1.45f);
    glPopMatrix();

    // Fallen Chrome Hubcap (knocked off in the crash, lying half in the mud rut)
    glPushMatrix();
    glTranslatef(1.55f, 0.06f, 1.85f);
    glRotatef(28.0f, 0.2f, 0.0f, 1.0f);
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawSphere(0.13f, 10, 8);
    glPopMatrix();

    // Splintered Wooden Fence Post wedged right into the crushed front bumper
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glPushMatrix();
    glTranslatef(0.88f, 0.52f, 2.52f);
    glRotatef(38.0f, 1.0f, 0.2f, -0.4f); // Smashed backwards by impact
    drawBox(0.18f, 1.35f, 0.18f, 0.5f, 2.0f);
    // Pointed jagged splinter break at top
    glTranslatef(0.0f, 0.72f, 0.0f);
    drawBox(0.10f, 0.28f, 0.08f);
    glPopMatrix();

    // Snapped Horizontal Fence Rail wedged under front-right fender
    glPushMatrix();
    glTranslatef(0.72f, 0.24f, 2.10f);
    glRotatef(-18.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(12.0f, 0.0f, 0.0f, 1.0f);
    drawBox(1.45f, 0.11f, 0.06f, 2.0f, 0.5f);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // 2. LADDER CHASSIS, SUSPENSION & RUSTED DUAL EXHAUST
    // ------------------------------------------------------------------------
    applyMaterial(MAT_BLACK_IRON);
    bindTexture(TEX_NONE);

    // Twin Heavy Steel Ladder Frame Rails
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * 0.65f, 0.38f, 0.0f);
        drawBox(0.14f, 0.16f, 4.30f);
        glPopMatrix();
    }
    // Chassis Crossmembers
    for (int c = -2; c <= 2; ++c) {
        glPushMatrix();
        glTranslatef(0.0f, 0.38f, c * 0.95f);
        drawBox(1.35f, 0.14f, 0.14f);
        glPopMatrix();
    }

    // Rear Solid Axle & Differential "Pumpkin"
    glPushMatrix();
    glTranslatef(0.0f, 0.42f, -1.35f);
    drawCylinder(0.045f, 0.045f, 1.95f, 8); // Axle tube
    drawSphere(0.16f, 10, 8);               // Differential pumpkin
    glPopMatrix();

    // Severed Front-Right Steering Tie-Rod hanging down
    glPushMatrix();
    glTranslatef(0.70f, 0.26f, 1.25f);
    glRotatef(35.0f, 1.0f, 0.0f, 0.5f);
    drawCylinder(0.016f, 0.016f, 0.35f, 6);
    glPopMatrix();

    // Rusted Dual Exhaust System (twin mufflers + drooping tailpipes)
    applyMaterial(MAT_CAR_RUST_WEATHERED);
    bindTexture(TEX_RUST);
    for (int ex = -1; ex <= 1; ex += 2) {
        glPushMatrix();
        glTranslatef(ex * 0.42f, 0.28f, 0.40f);
        // Header pipe
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        drawCylinder(0.030f, 0.030f, 1.45f, 8);
        // Oval Muffler
        glTranslatef(0.0f, 1.45f, 0.0f);
        drawBeveledBox(0.28f, 0.15f, 0.62f, 0.03f);
        // Tailpipe
        glTranslatef(0.0f, 0.62f, 0.0f);
        drawCylinder(0.028f, 0.028f, 0.85f, 8);
        // Slanted tailpipe tip drooping under rear bumper
        glTranslatef(0.0f, 0.85f, 0.0f);
        glRotatef(ex * 8.0f - 18.0f, 1.0f, 0.0f, 0.0f);
        drawCylinder(0.028f, 0.025f, 0.20f, 8);
        glPopMatrix();
    }

    // ------------------------------------------------------------------------
    // 3. SCULPTED 1957 CRUISER BODY PANELS, TAILFINS & CHROME SPEAR
    // ------------------------------------------------------------------------
    // Main Lower Body Tub (Two-tone weathered turquoise paint with rust sills)
    applyMaterial(MAT_CAR_PAINT_TURQUOISE);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 0.68f, 0.0f);
    drawBeveledBox(2.18f, 0.38f, 4.65f, 0.06f, 2.0f, 1.8f);
    glPopMatrix();

    // Rusted Rocker Panel Skirts along lower perimeter
    applyMaterial(MAT_CAR_RUST_WEATHERED);
    bindTexture(TEX_RUST);
    glPushMatrix();
    glTranslatef(0.0f, 0.52f, 0.0f);
    drawBeveledBox(2.20f, 0.12f, 4.60f, 0.02f);
    glPopMatrix();

    // Upper Waistline Body Panels: Rear Tub & Left/Right Front Fenders
    applyMaterial(MAT_CAR_PAINT_TURQUOISE);
    bindTexture(TEX_NONE);
    // Rear Cabin & Quarter Panel Waistline (Behind firewall)
    glPushMatrix();
    glTranslatef(0.0f, 0.94f, -0.82f);
    drawBeveledBox(2.08f, 0.22f, 2.80f, 0.04f, 2.0f, 1.2f);
    glPopMatrix();

    // Left Front Fender (Flanking the engine bay)
    glPushMatrix();
    glTranslatef(-0.95f, 0.94f, 1.45f);
    drawBeveledBox(0.24f, 0.26f, 1.72f, 0.03f, 1.0f, 1.0f);
    glPopMatrix();

    // Right Front Fender (Crumpled slightly inward from collision)
    glPushMatrix();
    glTranslatef(0.93f, 0.92f, 1.45f);
    glRotatef(3.5f, 0.0f, 1.0f, 0.2f); // Collision warp
    drawBeveledBox(0.24f, 0.26f, 1.70f, 0.03f, 1.0f, 1.0f);
    glPopMatrix();

    // 1957 Iconic Chrome "Sweep-Spear" Side Trim Moldings & Cream Two-Tone Inserts
    for (int side = -1; side <= 1; side += 2) {
        // Horizontal chrome spear running down each car flank
        applyMaterial(MAT_CHROME_TRIM);
        bindTexture(TEX_NONE);
        glPushMatrix();
        glTranslatef(side * 1.10f, 0.82f, 0.15f);
        drawBox(0.025f, 0.035f, 4.10f);

        // Downward swooping spear fin at rear quarter panel
        glTranslatef(0.0f, -0.06f, -1.45f);
        drawBox(0.028f, 0.035f, 1.25f);

        // Cream white two-tone accent panel inside rear spear pocket!
        applyMaterial(MAT_WHITEWALL_RUBBER);
        glTranslatef(side * -0.01f, 0.04f, 0.0f);
        drawBox(0.015f, 0.08f, 1.15f);
        glPopMatrix();
    }

    // 4 Curved Flared Wheel Well Arches
    float archX[2] = { -1.10f, 1.10f };
    float archZ[2] = { -1.35f, 1.35f };
    for (int ix = 0; ix < 2; ++ix) {
        for (int iz = 0; iz < 2; ++iz) {
            glPushMatrix();
            glTranslatef(archX[ix], 0.72f, archZ[iz]);
            applyMaterial(MAT_CAR_PAINT_TURQUOISE);
            bindTexture(TEX_NONE);
            drawBeveledBox(0.12f, 0.28f, 1.12f, 0.03f, 0.5f, 0.5f);
            // Chrome Wheel Opening Lip Molding
            applyMaterial(MAT_CHROME_TRIM);
            glTranslatef(archX[ix] > 0 ? 0.06f : -0.06f, -0.10f, 0.0f);
            drawBox(0.020f, 0.025f, 1.05f);
            glPopMatrix();
        }
    }

    // ICONIC 1957 REAR TAILFINS with Chrome Edge Caps
    for (int side = -1; side <= 1; side += 2) {
        // Sculpted fin blade rising towards rear
        glPushMatrix();
        glTranslatef(side * 0.98f, 1.16f, -1.65f);
        glRotatef(side * -3.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(8.5f, 1.0f, 0.0f, 0.0f); // Sloped fin wedge
        applyMaterial(MAT_CAR_PAINT_TURQUOISE);
        bindTexture(TEX_NONE);
        drawBeveledBox(0.16f, 0.32f, 1.35f, 0.03f, 1.0f, 1.0f);
        // Chrome Fin Top Edge Molding
        applyMaterial(MAT_CHROME_TRIM);
        glTranslatef(0.0f, 0.17f, 0.0f);
        drawBox(0.045f, 0.030f, 1.38f);

        // Rocket Bullet Ruby Red Taillight Pods embedded at rear fin tip!
        glTranslatef(0.0f, -0.08f, -0.72f);
        drawCylinder(0.075f, 0.075f, 0.06f, 12); // Chrome Bezel
        applyMaterial(MAT_PUMPKIN_SKIN);
        glTranslatef(0.0f, 0.0f, -0.05f);
        drawCylinder(0.060f, 0.015f, 0.10f, 10); // Ruby Bullet Lens
        glPopMatrix();
    }

    // Rear Trunk Deck Lid (slightly popped ajar on its latch)
    applyMaterial(MAT_CAR_PAINT_TURQUOISE);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 1.04f, -1.55f);
    glRotatef(5.5f, 1.0f, 0.0f, 0.0f); // Popped ajar ~5 degrees
    drawBeveledBox(1.88f, 0.12f, 1.32f, 0.04f, 1.5f, 1.0f);
    // Chrome Trunk Script & Keyhole Lock Cylinder
    applyMaterial(MAT_CHROME_TRIM);
    glTranslatef(0.0f, -0.04f, -0.66f);
    drawCylinder(0.025f, 0.025f, 0.03f, 8);
    drawBox(0.24f, 0.02f, 0.02f); // Chrome script bar
    glPopMatrix();

    // ------------------------------------------------------------------------
    // 4. CRASH-DAMAGED FRONT BUMPER, MANGLED GRILLE & HEADLIGHTS
    // ------------------------------------------------------------------------
    // Front Radiator Grille Bulkhead Support
    applyMaterial(MAT_ENGINE_IRON);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 0.72f, 2.30f);
    drawBox(1.82f, 0.44f, 0.10f);
    glPopMatrix();

    // Classic Chrome Radiator Grille: Intact on left, crushed & mangled on right!
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    for (int i = -5; i <= 5; ++i) {
        float gx = i * 0.16f;
        glPushMatrix();
        if (gx > 0.15f) {
            // Right side of grille: violently bent and crushed backwards by crash!
            glTranslatef(gx, 0.72f, 2.35f - (gx - 0.15f) * 0.40f);
            glRotatef((gx - 0.15f) * 38.0f, 0.0f, 1.0f, 0.2f);
            drawBox(0.024f, 0.32f, 0.035f);
        } else {
            // Left side: straight vertical grille bars
            glTranslatef(gx, 0.72f, 2.35f);
            drawBox(0.024f, 0.34f, 0.035f);
        }
        glPopMatrix();
    }
    // Horizontal Grille Ribs
    for (int j = -1; j <= 1; ++j) {
        glPushMatrix();
        glTranslatef(-0.45f, 0.72f + j * 0.11f, 2.36f);
        drawBox(0.95f, 0.024f, 0.030f);
        glPopMatrix();
    }

    // Heavy Front Wraparound Chrome Bumper:
    // Left side is straight; Right side is violently bent back ~24 degrees!
    // Left Half of Front Bumper (Intact)
    glPushMatrix();
    glTranslatef(-0.58f, 0.45f, 2.45f);
    drawBeveledBox(1.25f, 0.16f, 0.12f, 0.03f);
    // Left Dagmar / Bumper Bullet Overrider
    glTranslatef(-0.15f, 0.06f, 0.12f);
    drawCylinder(0.075f, 0.025f, 0.22f, 10);
    glPopMatrix();

    // Right Half of Front Bumper (Bent backwards, crumpled by fence impact!)
    glPushMatrix();
    glTranslatef(0.04f, 0.44f, 2.44f);
    glRotatef(24.0f, 0.0f, 1.0f, 0.0f); // Bent back 24 degrees
    glRotatef(-8.0f, 0.0f, 0.0f, 1.0f); // Drooping down from crushed bracket
    glTranslatef(0.58f, 0.0f, 0.0f);
    drawBeveledBox(1.22f, 0.16f, 0.12f, 0.03f);
    // Right Dagmar Overrider (Knocked sideways & dented)
    glTranslatef(-0.15f, 0.05f, 0.10f);
    glRotatef(-28.0f, 0.0f, 1.0f, 0.5f);
    drawCylinder(0.070f, 0.020f, 0.18f, 8);
    glPopMatrix();

    // Dual Round Headlights:
    // 1. Left Headlight: Intact cold glass lens & chrome bezel
    glPushMatrix();
    glTranslatef(-0.76f, 0.78f, 2.34f);
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawCylinder(0.165f, 0.165f, 0.06f, 14); // Bezel
    glTranslatef(0.0f, 0.0f, 0.03f);
    drawSphere(0.13f, 10, 8);                // Reflector bowl
    glTranslatef(0.0f, 0.0f, 0.03f);
    applyMaterial(MAT_CAR_GLASS);
    drawSphere(0.145f, 12, 10);              // Fluted convex glass lens
    glPopMatrix();

    // 2. Right Headlight: Smashed collision point! Shattered glass & dangling filament bulb
    glPushMatrix();
    glTranslatef(0.76f, 0.78f, 2.30f);
    glRotatef(12.0f, 0.0f, 1.0f, 0.3f);
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    drawCylinder(0.165f, 0.145f, 0.05f, 10); // Dented bezel
    // Empty rusted lamp bucket
    applyMaterial(MAT_CAR_RUST_WEATHERED);
    bindTexture(TEX_RUST);
    glTranslatef(0.0f, 0.0f, 0.015f);
    drawSphere(0.12f, 8, 6);
    // Tiny exposed tungsten bulb dangling on bent wire
    applyMaterial(MAT_BULB_EMISSIVE);
    bindTexture(TEX_NONE);
    glTranslatef(0.02f, -0.04f, 0.035f);
    drawSphere(0.032f, 8, 6);
    // Broken glass shards clinging to lower bezel rim
    applyMaterial(MAT_CAR_GLASS);
    glTranslatef(0.06f, -0.05f, 0.01f);
    drawBox(0.045f, 0.055f, 0.015f);
    glPopMatrix();

    // Heavy Rear Chrome Bumper & Askew License Plate
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 0.44f, -2.36f);
    drawBeveledBox(2.26f, 0.16f, 0.12f, 0.03f);
    // Rear Overriders
    glTranslatef(-0.62f, 0.06f, -0.04f);
    drawBeveledBox(0.08f, 0.26f, 0.08f, 0.02f);
    glTranslatef(1.24f, 0.0f, 0.0f);
    drawBeveledBox(0.08f, 0.26f, 0.08f, 0.02f);
    // Vintage Rusted License Plate hanging crooked by one screw
    applyMaterial(MAT_STONE);
    glTranslatef(-0.62f, 0.02f, -0.06f);
    glRotatef(16.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.42f, 0.20f, 0.015f);
    applyMaterial(MAT_CAR_RUST_WEATHERED);
    drawBox(0.38f, 0.16f, 0.018f);
    glPopMatrix();

    // ------------------------------------------------------------------------
    // 5. POPPED-AJAR CRUMPLED HOOD & FULLY EXPOSED V8 ENGINE BAY
    // ------------------------------------------------------------------------
    // Engine Bay Enclosure (Inner fender walls & Firewall)
    applyMaterial(MAT_ENGINE_IRON);
    bindTexture(TEX_NONE);
    // Rear Firewall
    glPushMatrix();
    glTranslatef(0.0f, 0.88f, 0.58f);
    drawBox(1.72f, 0.52f, 0.06f);
    glPopMatrix();
    // Inner Fender Aprons
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * 0.86f, 0.84f, 1.45f);
        drawBox(0.06f, 0.46f, 1.70f);
        glPopMatrix();
    }

    // --- FULLY DETAILED EXPOSED V8 ENGINE BAY ---
    // Cast Iron V8 Engine Block
    applyMaterial(MAT_ENGINE_IRON);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 0.74f, 1.35f);
    drawBox(0.44f, 0.38f, 0.68f); // Central Block

    // Twin Angled Cylinder Heads & Ribbed Vintage Red Valve Covers
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * 0.24f, 0.16f, 0.0f);
        glRotatef(side * -45.0f, 0.0f, 0.0f, 1.0f);
        // Cylinder head
        applyMaterial(MAT_ENGINE_IRON);
        drawBox(0.18f, 0.14f, 0.62f);
        // Vintage Red Valve Cover
        applyMaterial(MAT_VALVE_RED);
        glTranslatef(0.0f, 0.09f, 0.0f);
        drawBeveledBox(0.16f, 0.08f, 0.60f, 0.02f);
        // Chrome Oil Fill Breather Cap on left valve cover
        if (side == -1) {
            applyMaterial(MAT_CHROME_TRIM);
            glTranslatef(0.0f, 0.05f, 0.16f);
            drawCylinder(0.032f, 0.032f, 0.045f, 8);
        }
        glPopMatrix();
    }

    // Aluminum Intake Manifold & 4-Barrel Carburetor
    applyMaterial(MAT_STONE);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.0f);
    drawBox(0.28f, 0.08f, 0.48f); // Intake manifold
    glTranslatef(0.0f, 0.07f, 0.02f);
    applyMaterial(MAT_CAR_RUST_WEATHERED);
    drawBox(0.16f, 0.09f, 0.16f); // Carburetor body
    glPopMatrix();

    // Classic Round Pancake Chrome Air Cleaner with Wing Nut
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 0.38f, 0.02f);
    drawCylinder(0.22f, 0.22f, 0.065f, 18); // Pancake filter housing
    glTranslatef(0.0f, 0.065f, 0.0f);
    drawCylinder(0.012f, 0.012f, 0.030f, 6);  // Center stud & wingnut
    drawBox(0.06f, 0.012f, 0.015f);
    glPopMatrix();

    // Front Timing Cover & Engine Crankshaft Pulley
    applyMaterial(MAT_BLACK_IRON);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.35f);
    drawCylinder(0.08f, 0.08f, 0.05f, 10);
    glPopMatrix();

    // Engine Cooling Fan with 4 steel blades (one bent back from crash)
    glPushMatrix();
    glTranslatef(0.0f, 0.08f, 0.43f);
    for (int b = 0; b < 4; ++b) {
        glPushMatrix();
        glRotatef(b * 90.0f, 0.0f, 0.0f, 1.0f);
        if (b == 1) glRotatef(-24.0f, 1.0f, 0.0f, 0.0f); // Bent fan blade
        glTranslatef(0.0f, 0.12f, 0.0f);
        drawBox(0.06f, 0.16f, 0.010f);
        glPopMatrix();
    }
    glPopMatrix();

    // Heavy Brass Radiator Core (Front bulkhead)
    applyMaterial(MAT_BRASS_CORE);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 0.10f, 0.70f);
    // Left half of radiator core
    glTranslatef(-0.35f, 0.0f, 0.0f);
    drawBox(0.68f, 0.46f, 0.08f);
    // Right half of radiator core (dented & pushed back by crash impact)
    glTranslatef(0.70f, -0.02f, -0.06f);
    glRotatef(18.0f, 0.0f, 1.0f, 0.0f);
    drawBox(0.68f, 0.44f, 0.08f);
    glPopMatrix();

    // Curved Upper Black Rubber Radiator Hose
    applyMaterial(MAT_RUBBER_TYRE);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.16f, 0.32f, 0.48f);
    drawCylinder(0.032f, 0.032f, 0.32f, 8); // Hose section
    // Chrome Hose Clamps
    applyMaterial(MAT_CHROME_TRIM);
    drawCylinder(0.036f, 0.036f, 0.020f, 8);
    glTranslatef(0.0f, 0.0f, 0.28f);
    drawCylinder(0.036f, 0.036f, 0.020f, 8);
    glPopMatrix();

    // 12V Vintage Battery Box on inner driver fender tray
    applyMaterial(MAT_BLACK_IRON);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.62f, 0.16f, 0.15f);
    drawBox(0.24f, 0.22f, 0.32f); // Hard rubber case
    // Lead Terminals with green corrosion wash
    applyMaterial(MAT_CHROME_TRIM);
    glTranslatef(-0.06f, 0.12f, 0.08f);
    drawCylinder(0.016f, 0.016f, 0.030f, 6); // Positive post
    glTranslatef(0.12f, 0.0f, -0.16f);
    drawCylinder(0.016f, 0.016f, 0.030f, 6); // Negative post
    glPopMatrix();
    glPopMatrix(); // End V8 Engine Bay

    // THE POPPED-AJAR CRUMPLED HOOD (Buckled latch, popped open at 35 degrees!)
    // Tilted high so the entire V8 engine, red valve covers & radiator are visible!
    applyMaterial(MAT_CAR_PAINT_TURQUOISE);
    bindTexture(TEX_NONE);
    glPushMatrix();
    // Pivot at cowl hinge line
    glTranslatef(0.0f, 1.06f, 0.58f);
    glRotatef(-28.0f, 1.0f, 0.0f, 0.0f); // Popped UPWARDS 28 degrees!
    glRotatef(-4.8f, 0.0f, 0.0f, 1.0f); // Twisted askew by broken right latch
    glTranslatef(0.0f, 0.0f, 0.88f);

    // Left Half of Hood (Flatter)
    glPushMatrix();
    glTranslatef(-0.48f, 0.0f, 0.0f);
    drawBeveledBox(0.98f, 0.06f, 1.76f, 0.03f, 1.5f, 1.0f);
    glPopMatrix();

    // Right Half of Hood (Crumpled & buckled upward in middle from crash)
    glPushMatrix();
    glTranslatef(0.48f, 0.03f, 0.0f);
    glRotatef(6.5f, 0.0f, 1.0f, 0.0f);
    drawBeveledBox(0.96f, 0.06f, 1.74f, 0.03f, 1.5f, 1.0f);
    glPopMatrix();

    // Center Raised Hood Ridge & Chrome Jet Airplane Ornament
    applyMaterial(MAT_CHROME_TRIM);
    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.0f);
    drawBox(0.06f, 0.03f, 1.70f); // Center chrome ridge
    // Winged Jet Hood Ornament at front prow
    glTranslatef(0.0f, 0.04f, 0.82f);
    drawBox(0.04f, 0.06f, 0.16f); // Fuselage
    drawBox(0.24f, 0.015f, 0.05f); // Wings
    glPopMatrix();
    glPopMatrix(); // End Popped Hood

    // ------------------------------------------------------------------------
    // 6. CABIN, WRAP-AROUND WINDSHIELD & SHATTERED SAFETY GLASS
    // ------------------------------------------------------------------------
    // Tapered Cabin Roof with Drip Rails
    applyMaterial(MAT_CAR_PAINT_TURQUOISE);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 1.68f, -0.32f);
    drawBeveledBox(1.74f, 0.06f, 2.12f, 0.025f, 1.5f, 1.5f);
    // Drip Rails along roof gutters
    applyMaterial(MAT_CHROME_TRIM);
    glTranslatef(-0.88f, -0.02f, 0.0f);
    drawBox(0.035f, 0.035f, 2.14f);
    glTranslatef(1.76f, 0.0f, 0.0f);
    drawBox(0.035f, 0.035f, 2.14f);
    glPopMatrix();

    // Roof Pillars (A, B, C)
    applyMaterial(MAT_CAR_PAINT_TURQUOISE);
    bindTexture(TEX_NONE);
    // A-Pillars (Wrap-around panoramic windshield frame)
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * 0.84f, 1.34f, 0.44f);
        glRotatef(34.0f, 1.0f, 0.0f, 0.0f);
        drawBeveledBox(0.06f, 0.74f, 0.06f, 0.015f);
        glPopMatrix();

        // B-Pillars
        glPushMatrix();
        glTranslatef(side * 0.84f, 1.34f, -0.32f);
        drawBeveledBox(0.06f, 0.66f, 0.06f, 0.015f);
        glPopMatrix();

        // C-Pillars (Swept rear sail panels)
        glPushMatrix();
        glTranslatef(side * 0.84f, 1.34f, -1.06f);
        glRotatef(-28.0f, 1.0f, 0.0f, 0.0f);
        drawBeveledBox(0.08f, 0.72f, 0.08f, 0.02f);
        glPopMatrix();
    }

    // Front Panoramic Safety Glass Windshield
    applyMaterial(MAT_CAR_GLASS);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 1.34f, 0.46f);
    glRotatef(34.0f, 1.0f, 0.0f, 0.0f);
    drawBox(1.60f, 0.65f, 0.035f);

    // INTRICATE REALISTIC SPIDERWEB IMPACT CRACKS
    // Delicate thin hairline fractures & concentric shockwave rings
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.85f, 0.92f, 1.00f, 0.75f);
    glLineWidth(1.4f);

    float cx = -0.32f, cy = 0.08f, cz = 0.022f; // Impact epicenter
    glBegin(GL_LINES);
    // 14 Radial Fracture Rays radiating across windshield
    for (int r = 0; r < 14; ++r) {
        float angle = (float)r * (2.0f * 3.14159265f / 14.0f);
        float rLen = 0.35f + 0.18f * sinf((float)r * 1.7f);
        glVertex3f(cx, cy, cz);
        glVertex3f(cx + rLen * cosf(angle), cy + rLen * sinf(angle) * 0.65f, cz);

        // Branching sub-fractures
        if (r % 2 == 0) {
            float bx = cx + rLen * 0.55f * cosf(angle);
            float by = cy + rLen * 0.55f * sinf(angle) * 0.65f;
            float bAngle = angle + 0.45f;
            glVertex3f(bx, by, cz);
            glVertex3f(bx + 0.16f * cosf(bAngle), by + 0.16f * sinf(bAngle) * 0.65f, cz);
        }
    }
    glEnd();

    // 3 Concentric Shockwave Rings (Smooth ripple arcs)
    for (int ring = 1; ring <= 3; ++ring) {
        float ringRad = (float)ring * 0.085f;
        glBegin(GL_LINE_LOOP);
        for (int a = 0; a < 14; ++a) {
            float ang = (float)a * (2.0f * 3.14159265f / 14.0f);
            glVertex3f(cx + ringRad * cosf(ang), cy + ringRad * sinf(ang) * 0.65f, cz);
        }
        glEnd();
    }
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Windshield Wipers: Driver's frozen mid-sweep, Passenger's broken on cowl
    applyMaterial(MAT_CAR_RUST_WEATHERED);
    bindTexture(TEX_NONE);
    // Driver Wiper
    glPushMatrix();
    glTranslatef(-0.38f, 1.14f, 0.72f);
    glRotatef(34.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-46.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.020f, 0.42f, 0.020f);
    glTranslatef(0.015f, 0.18f, 0.015f);
    drawBox(0.012f, 0.36f, 0.016f);
    glPopMatrix();
    // Passenger Wiper
    glPushMatrix();
    glTranslatef(0.38f, 1.10f, 0.72f);
    glRotatef(34.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-12.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.020f, 0.38f, 0.020f);
    glPopMatrix();

    // Rear Window Glass with Dusty Vintage Cobweb
    applyMaterial(MAT_CAR_GLASS);
    glPushMatrix();
    glTranslatef(0.0f, 1.34f, -1.05f);
    glRotatef(-28.0f, 1.0f, 0.0f, 0.0f);
    drawBox(1.58f, 0.64f, 0.035f);
    glPopMatrix();
    drawCobweb(0.60f, 1.38f, -0.92f, 0.45f, 30.0f);

    // Right Side Passenger Window: Completely Smashed Out!
    // Open void with jagged glass teeth along lower sill
    applyMaterial(MAT_CAR_GLASS);
    for (int tooth = -3; tooth <= 3; ++tooth) {
        glPushMatrix();
        glTranslatef(0.85f, 1.08f, tooth * 0.10f);
        drawBox(0.015f, 0.04f + 0.02f * (tooth % 2), 0.06f);
        glPopMatrix();
    }

    // ------------------------------------------------------------------------
    // 7. OPEN DRIVER'S DOOR (Ajar at 36 deg) & SNAPPED DANGLING SIDE MIRROR
    // ------------------------------------------------------------------------
    applyMaterial(MAT_CAR_PAINT_TURQUOISE);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.95f, 0.68f, 0.44f); // Door hinge pivot on A-pillar
    glRotatef(36.0f, 0.0f, 1.0f, 0.0f);  // Swung open into the yard
    glTranslatef(0.0f, 0.0f, -0.44f);

    // Heavy Rusty Door Hinges
    applyMaterial(MAT_BLACK_IRON);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.42f);
    drawCylinder(0.028f, 0.028f, 0.06f, 8);
    glTranslatef(0.0f, -0.44f, 0.0f);
    drawCylinder(0.028f, 0.028f, 0.06f, 8);
    glPopMatrix();

    // Outer Lower Door Panel
    applyMaterial(MAT_CAR_PAINT_TURQUOISE);
    bindTexture(TEX_NONE);
    drawBeveledBox(0.08f, 0.58f, 0.88f, 0.02f, 0.5f, 1.0f);

    // Exterior Push-Button Chrome Door Handle
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.055f, 0.18f, -0.32f);
    drawBox(0.035f, 0.04f, 0.14f);
    drawSphere(0.018f, 8, 6);
    glPopMatrix();

    // Window Frame Arch & Rolled-Down Window Pane
    applyMaterial(MAT_CAR_PAINT_TURQUOISE);
    glTranslatef(0.0f, 0.52f, 0.0f);
    drawBox(0.055f, 0.48f, 0.055f); // Front post
    glTranslatef(0.0f, 0.0f, -0.82f);
    drawBox(0.055f, 0.48f, 0.055f); // Rear post
    glTranslatef(0.0f, 0.22f, 0.41f);
    drawBox(0.055f, 0.055f, 0.88f); // Top header

    // Inner Door Trim Panel (Molded armrest & manual window crank)
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.048f, -0.52f, 0.0f);
    drawBox(0.025f, 0.44f, 0.80f); // Trim card
    // Armrest
    glTranslatef(0.02f, -0.05f, 0.0f);
    drawBeveledBox(0.05f, 0.08f, 0.35f, 0.015f);
    // Chrome Door Latch & Window Crank
    applyMaterial(MAT_CHROME_TRIM);
    glTranslatef(0.02f, 0.14f, 0.15f);
    drawBox(0.03f, 0.035f, 0.08f);
    // Crank handle
    glTranslatef(0.0f, -0.16f, -0.30f);
    drawCylinder(0.016f, 0.016f, 0.020f, 6);
    drawBox(0.020f, 0.070f, 0.015f);
    glPopMatrix();

    // Snapped Dangling Side Mirror (Torn from bracket, hanging by twin wires)
    glPushMatrix();
    glTranslatef(-0.06f, -0.24f, 0.40f);
    applyMaterial(MAT_BLACK_IRON);
    drawBox(0.03f, 0.04f, 0.04f); // Broken mounting stub

    // Dangling Copper Wires
    glDisable(GL_LIGHTING);
    glColor3f(0.85f, 0.48f, 0.22f);
    glLineWidth(1.8f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.06f, -0.16f, 0.05f);
    glVertex3f(0.01f, 0.0f, -0.01f);
    glVertex3f(-0.05f, -0.17f, 0.04f);
    glEnd();
    glEnable(GL_LIGHTING);

    // Dangling Round Chrome Mirror Housing
    glTranslatef(-0.06f, -0.16f, 0.05f);
    glRotatef(55.0f, 1.0f, 0.2f, 0.8f);
    applyMaterial(MAT_CHROME_TRIM);
    drawCylinder(0.085f, 0.085f, 0.022f, 12);
    applyMaterial(MAT_CAR_GLASS);
    glTranslatef(0.0f, 0.0f, 0.012f);
    drawSphere(0.080f, 10, 8); // Convex mirror face
    glPopMatrix();
    glPopMatrix(); // End Open Driver's Door

    // ------------------------------------------------------------------------
    // 8. DETAILED VINTAGE INTERIOR & TORN BENCH SEATS WITH SPRINGS
    // ------------------------------------------------------------------------
    // Sculpted 1957 Dashboard
    applyMaterial(MAT_CAR_PAINT_TURQUOISE);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, 1.08f, 0.38f);
    drawBeveledBox(1.68f, 0.22f, 0.42f, 0.02f);

    // Arched Instrument Speedometer Binnacle
    applyMaterial(MAT_CAR_INTERIOR);
    glPushMatrix();
    glTranslatef(-0.42f, 0.04f, -0.18f);
    drawBox(0.38f, 0.12f, 0.05f);
    // Chrome Bezel & Speedometer Needle
    applyMaterial(MAT_CHROME_TRIM);
    drawBox(0.35f, 0.02f, 0.06f);
    applyMaterial(MAT_VALVE_RED);
    glTranslatef(0.0f, 0.02f, 0.035f);
    drawBox(0.010f, 0.055f, 0.010f); // Speedometer needle
    glPopMatrix();

    // Center Chrome Radio Grille & Vintage Knobs
    applyMaterial(MAT_CHROME_TRIM);
    glPushMatrix();
    glTranslatef(0.0f, 0.02f, -0.18f);
    drawBox(0.26f, 0.08f, 0.03f);
    glTranslatef(-0.08f, 0.0f, 0.02f);
    drawSphere(0.018f, 6, 6);
    glTranslatef(0.16f, 0.0f, 0.0f);
    drawSphere(0.018f, 6, 6);
    glPopMatrix();

    // Glovebox Door with Push Button
    applyMaterial(MAT_CAR_INTERIOR);
    glPushMatrix();
    glTranslatef(0.45f, -0.04f, -0.18f);
    drawBox(0.42f, 0.14f, 0.02f);
    applyMaterial(MAT_CHROME_TRIM);
    glTranslatef(0.14f, 0.0f, -0.015f);
    drawSphere(0.015f, 6, 6);
    glPopMatrix();
    glPopMatrix(); // End Dashboard

    // STEERING WHEEL ASSEMBLY (Hollow dished 2-spoke wheel - NO SOLID SPHERE!)
    glPushMatrix();
    glTranslatef(-0.42f, 0.98f, 0.26f);
    glRotatef(-36.0f, 1.0f, 0.0f, 0.0f); // Tilted towards driver
    drawVintageSteeringWheel();
    glPopMatrix();

    // Floor Gear Shifter Lever & Pedals
    applyMaterial(MAT_CHROME_TRIM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.08f, 0.58f, 0.05f);
    // Accordion rubber shift boot
    applyMaterial(MAT_RUBBER_TYRE);
    drawCylinder(0.08f, 0.03f, 0.08f, 8);
    // Chrome Lever
    applyMaterial(MAT_CHROME_TRIM);
    glTranslatef(0.0f, 0.08f, 0.0f);
    glRotatef(-15.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.014f, 0.014f, 0.28f, 6);
    // Spherical Shift Knob
    glTranslatef(0.0f, 0.28f, 0.0f);
    applyMaterial(MAT_STONE);
    drawSphere(0.032f, 8, 8);
    glPopMatrix();

    // Suspended Foot Pedals (Clutch, Brake, Accelerator)
    applyMaterial(MAT_RUBBER_TYRE);
    for (int p = -1; p <= 1; ++p) {
        glPushMatrix();
        glTranslatef(-0.42f + p * 0.10f, 0.62f, 0.35f);
        drawBox(0.045f, 0.065f, 0.02f);
        glPopMatrix();
    }

    // FRONT SPLIT-BENCH SEAT (Torn Upholstery with Exposed Foam & Coiled Wire Springs!)
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_BARK);
    glPushMatrix();
    glTranslatef(0.0f, 0.78f, -0.15f);
    // Seat base cushion
    drawBeveledBox(1.64f, 0.24f, 0.58f, 0.03f);
    // Backrest
    glTranslatef(0.0f, 0.28f, -0.24f);
    drawBeveledBox(1.64f, 0.46f, 0.16f, 0.03f);

    // Gaping Tear on Driver's Cushion: Spilling Yellow Foam & Coiled Springs
    applyMaterial(MAT_SEAT_FOAM);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(-0.42f, -0.18f, 0.22f);
    drawBox(0.42f, 0.14f, 0.28f); // Exposed polyurethane foam core

    // 4 COILED RUSTY WIRE SPRINGS BURSTING OUT THROUGH THE FABRIC!
    for (int sp = 0; sp < 4; ++sp) {
        glPushMatrix();
        float sx = -0.12f + (float)(sp % 2) * 0.20f;
        float sz = -0.08f + (float)(sp / 2) * 0.18f;
        glTranslatef(sx, 0.06f, sz);
        glRotatef((sp == 1 ? 18.0f : -12.0f), 0.0f, 0.0f, 1.0f);
        drawSeatSpring(0.032f, 0.14f, 4);
        glPopMatrix();
    }
    glPopMatrix();
    glPopMatrix(); // End Front Seat

    // Rear Passenger Bench Seat (Sun-faded cushions & fallen autumn leaves)
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_BARK);
    glPushMatrix();
    glTranslatef(0.0f, 0.78f, -0.92f);
    drawBeveledBox(1.64f, 0.24f, 0.52f, 0.03f); // Base
    glTranslatef(0.0f, 0.28f, -0.22f);
    drawBeveledBox(1.64f, 0.44f, 0.14f, 0.03f); // Backrest

    // Fallen Dead Autumn Leaves scattered on rear bench
    applyMaterial(MAT_FALLEN_LEAF);
    bindTexture(TEX_NONE);
    for (int lf = 0; lf < 6; ++lf) {
        glPushMatrix();
        glTranslatef(-0.55f + lf * 0.22f, -0.12f, 0.14f + (lf % 2) * 0.08f);
        glRotatef(lf * 38.0f, 0.0f, 1.0f, 0.0f);
        drawBox(0.08f, 0.01f, 0.06f);
        glPopMatrix();
    }
    glPopMatrix();

    // ------------------------------------------------------------------------
    // 9. VINTAGE WHITEWALL WHEELS & PUNCTURED COLLAPSED FRONT-RIGHT TYRE
    // ------------------------------------------------------------------------
    float wheelDistX = 1.04f;
    float wheelFrontZ = 1.35f;
    float wheelRearZ  = -1.35f;

    // 1. Rear-Left Wheel (Inflated, Whitewall, Chrome Hubcap)
    glPushMatrix();
    glTranslatef(-wheelDistX, 0.42f, wheelRearZ);
    drawWhitewallWheel(false, true, true);
    glPopMatrix();

    // 2. Rear-Right Wheel (Inflated, Whitewall, Chrome Hubcap)
    glPushMatrix();
    glTranslatef(wheelDistX - 0.22f, 0.42f, wheelRearZ);
    drawWhitewallWheel(false, true, false);
    glPopMatrix();

    // 3. Front-Left Wheel (Inflated, Whitewall, Chrome Hubcap)
    glPushMatrix();
    glTranslatef(-wheelDistX, 0.42f, wheelFrontZ);
    drawWhitewallWheel(false, true, true);
    glPopMatrix();

    // 4. Front-Right Wheel (SEVERELY PUNCTURED, FLATTENED PANCAKE TIRE IN MUD RUT)
    // Hubcap knocked off, broken tie-rod, sitting sunken in depression
    glPushMatrix();
    glTranslatef(wheelDistX - 0.22f, 0.20f, wheelFrontZ);
    drawWhitewallWheel(true, false, false); // isFlat = true, hasHubcap = false, isLeft = false
    glPopMatrix();

    glPopMatrix(); // End Car
}


