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

    // 7. Smashed & Dilapidated Wooden Dining Table (Split & Tilted on Broken Leg)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.26f, 0.22f, 0.18f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.8f, 0.74f, 1.7f);
    glRotatef(8.5f, 0.0f, 0.0f, 1.0f); // Tilted because right legs are broken/collapsed

    // Tabletop (Split into two broken halves)
    glPushMatrix();
    glTranslatef(-0.35f, 0.68f, 0.0f);
    drawBox(0.75f, 0.055f, 0.85f);
    glTranslatef(0.72f, -0.04f, 0.0f);
    glRotatef(6.0f, 1.0f, 0.0f, 0.0f);
    drawBox(0.68f, 0.055f, 0.82f);
    glPopMatrix();

    // Intact Left Legs
    glPushMatrix();
    glTranslatef(-0.65f, 0.32f, -0.32f); drawBox(0.07f, 0.64f, 0.07f);
    glTranslatef( 0.0f,  0.0f,   0.64f); drawBox(0.07f, 0.64f, 0.07f);
    // Broken Snapped Right Legs (short stubs propped on bricks)
    glTranslatef( 1.30f, -0.15f,  0.0f);  drawBox(0.07f, 0.34f, 0.07f);
    glTranslatef( 0.0f,   0.0f,  -0.64f); drawBox(0.07f, 0.28f, 0.07f);
    glPopMatrix();
    glPopMatrix();

    // Broken Red Clay Bricks under collapsed table leg
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    glColor4f(0.55f, 0.25f, 0.18f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.1f, 0.80f, 1.4f);
    drawBox(0.24f, 0.10f, 0.14f);
    glTranslatef(0.08f, 0.08f, 0.18f);
    glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
    drawBox(0.22f, 0.08f, 0.12f);
    glPopMatrix();

    // 8. Melting Wax Candle & Dusty Shattered Wine Bottle on Table
    glPushMatrix();
    glTranslatef(-4.95f, 1.44f, 1.55f);
    // Brass candle holder base
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_NONE);
    glColor4f(0.48f, 0.38f, 0.20f, 1.0f);
    drawCylinder(0.065f, 0.045f, 0.03f, 8);
    // White wax candle column with melted wax drips
    glColor4f(0.88f, 0.85f, 0.75f, 1.0f);
    glTranslatef(0.0f, 0.03f, 0.0f);
    drawCylinder(0.022f, 0.020f, 0.12f, 6);
    // Candle flame (flickering orange-yellow teardrop)
    glDisable(GL_LIGHTING);
    glTranslatef(0.0f, 0.12f, 0.0f);
    glColor4f(1.0f, 0.65f, 0.15f, 0.95f * intBulbFlicker);
    drawSphere(0.025f, 6, 6);
    glColor4f(1.0f, 0.92f, 0.45f, 1.0f);
    drawSphere(0.012f, 6, 6);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Fallen shattered bottle on floor
    applyMaterial(MAT_CAR_INTERIOR);
    bindTexture(TEX_NONE);
    glColor4f(0.18f, 0.25f, 0.16f, 1.0f);
    glPushMatrix();
    glTranslatef(-4.2f, 0.80f, 2.1f);
    glRotatef(82.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(25.0f, 0.0f, 1.0f, 0.0f);
    drawCylinder(0.042f, 0.042f, 0.20f, 8);
    // Broken neck
    glTranslatef(0.0f, 0.20f, 0.0f);
    drawCylinder(0.042f, 0.016f, 0.06f, 8);
    glPopMatrix();

    // 9. Broken & Overturned Chairs
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.28f, 0.24f, 0.20f, 1.0f);
    // Chair 1 (Completely overturned on floor with shattered legs)
    glPushMatrix();
    glTranslatef(-3.6f, 0.88f, 1.65f);
    glRotatef(88.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-35.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.48f, 0.04f, 0.48f); // Seat
    // Broken legs sticking out
    glTranslatef(-0.19f, -0.18f, -0.19f); drawBox(0.045f, 0.36f, 0.045f);
    glTranslatef( 0.38f,  0.0f,   0.0f);  drawBox(0.045f, 0.20f, 0.045f);
    // Backrest posts
    glTranslatef(-0.19f, 0.45f, -0.19f); drawBox(0.045f, 0.52f, 0.045f);
    glTranslatef( 0.38f, 0.0f,   0.0f);  drawBox(0.045f, 0.40f, 0.045f);
    glPopMatrix();

    // Chair 2 (Tilted askew on opposite side)
    glPushMatrix();
    glTranslatef(-5.95f, 0.74f, 1.85f);
    glRotatef(82.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(14.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, 0.45f, 0.0f); drawBox(0.48f, 0.04f, 0.48f);
    glPushMatrix();
    glTranslatef(-0.19f, -0.225f, -0.19f); drawBox(0.045f, 0.45f, 0.045f);
    glTranslatef( 0.38f,  0.0f,    0.0f);  drawBox(0.045f, 0.45f, 0.045f);
    glTranslatef( 0.0f,   0.0f,    0.38f); drawBox(0.045f, 0.45f, 0.045f);
    glTranslatef(-0.38f,  0.10f,   0.0f);  drawBox(0.045f, 0.25f, 0.045f);
    glPopMatrix();
    glTranslatef(-0.19f, 0.26f, -0.19f); drawBox(0.045f, 0.52f, 0.045f);
    glTranslatef( 0.38f, 0.0f,   0.0f);   drawBox(0.045f, 0.52f, 0.045f);
    glTranslatef(-0.19f, 0.24f,  0.0f);   drawBox(0.46f, 0.06f, 0.04f);
    glPopMatrix();

    // 10. Decaying & Leaning Antique Bookshelf with Fallen Dusty Books
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.22f, 0.18f, 0.14f, 1.0f);
    glPushMatrix();
    glTranslatef(-8.20f, 0.74f, -2.20f);
    glRotatef(8.0f, 0.0f, 0.0f, 1.0f); // Leaning precariously against the wall
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    // Outer frame (1.4m wide, 2.4m tall, 0.45m deep)
    glPushMatrix();
    glTranslatef(0.0f, 1.20f, 0.0f);
    // Backing panel
    drawBox(1.40f, 2.40f, 0.04f);
    // Side panels
    glTranslatef(-0.68f, 0.0f, 0.20f); drawBox(0.05f, 2.40f, 0.40f);
    glTranslatef( 1.36f, 0.0f, 0.0f);  drawBox(0.05f, 2.40f, 0.40f);
    // Top & bottom
    glTranslatef(-0.68f, 1.18f, 0.0f); drawBox(1.40f, 0.06f, 0.40f);
    glTranslatef( 0.0f, -2.36f, 0.0f); drawBox(1.40f, 0.06f, 0.40f);
    // Shelves (one broken/tilted)
    glTranslatef( 0.0f, 0.65f, 0.0f); drawBox(1.32f, 0.04f, 0.38f);
    glTranslatef( 0.0f, 0.65f, 0.0f); glRotatef(12.0f, 0.0f, 0.0f, 1.0f); drawBox(1.25f, 0.04f, 0.38f);
    glPopMatrix();

    // Scattered Antique Books on shelves & fallen on floor
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    // Red leather book
    glColor4f(0.48f, 0.18f, 0.14f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.35f, 1.95f, 0.18f);
    glRotatef(18.0f, 0.0f, 1.0f, 0.0f);
    drawBox(0.08f, 0.26f, 0.20f);
    glPopMatrix();
    // Blue leather book
    glColor4f(0.18f, 0.24f, 0.38f, 1.0f);
    glPushMatrix();
    glTranslatef(0.20f, 1.30f, 0.18f);
    glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
    drawBox(0.09f, 0.24f, 0.20f);
    glPopMatrix();
    // Fallen books on floor
    glColor4f(0.35f, 0.28f, 0.18f, 1.0f);
    glPushMatrix();
    glTranslatef(0.45f, 0.06f, 0.55f);
    glRotatef(42.0f, 0.0f, 1.0f, 0.0f);
    drawBox(0.24f, 0.06f, 0.18f);
    glTranslatef(0.10f, 0.05f, -0.05f);
    glRotatef(-20.0f, 0.0f, 1.0f, 0.0f);
    drawBox(0.22f, 0.05f, 0.16f);
    glPopMatrix();

    glPopMatrix();

    // 11. Old Broken Grandfather Clock in Corner (Cracked face & askew pendulum)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.20f, 0.16f, 0.12f, 1.0f);
    glPushMatrix();
    glTranslatef(-8.35f, 0.74f, 4.20f);
    glRotatef(-40.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(4.5f, 0.0f, 0.0f, 1.0f); // Tilted

    // Clock Base & Waist Body
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 0.0f); drawBox(0.60f, 0.70f, 0.40f);
    glTranslatef(0.0f, 0.85f, 0.0f); drawBox(0.48f, 1.00f, 0.34f);
    // Clock Hood / Head
    glTranslatef(0.0f, 0.72f, 0.0f); drawBox(0.58f, 0.48f, 0.38f);
    // Top Arch Cap
    glTranslatef(0.0f, 0.28f, 0.0f); drawBox(0.50f, 0.12f, 0.36f);
    glPopMatrix();

    // Clock Dial Face (Yellowed, cracked parchment)
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    glColor4f(0.78f, 0.74f, 0.60f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 1.92f, 0.20f);
    drawSphere(0.16f, 12, 8);
    // Broken brass clock hands frozen at midnight!
    applyMaterial(MAT_RUSTY_METAL);
    glColor4f(0.22f, 0.18f, 0.10f, 1.0f);
    glTranslatef(0.0f, 0.0f, 0.03f);
    drawBox(0.015f, 0.14f, 0.01f);
    glTranslatef(0.0f, 0.0f, 0.005f);
    drawBox(0.10f, 0.015f, 0.01f);
    glPopMatrix();

    // Crooked brass pendulum in waist aperture
    applyMaterial(MAT_RUSTY_METAL);
    glColor4f(0.55f, 0.45f, 0.22f, 1.0f);
    glPushMatrix();
    glTranslatef(0.04f, 1.05f, 0.08f);
    glRotatef(18.0f, 0.0f, 0.0f, 1.0f);
    drawCylinder(0.012f, 0.012f, 0.65f, 6);
    glTranslatef(0.0f, 0.65f, 0.0f);
    drawSphere(0.075f, 8, 6);
    glPopMatrix();

    glPopMatrix();

    // 12. FULLY COMPLETED WOODEN STAIRCASE (Ascending seamlessly from 1st Floor y=0.74f to 2nd Floor y=4.20f)
    int numSteps = 15;
    float stairStartX = -1.35f;
    float stairStartZ =  3.60f;
    float stairEndZ   = -1.40f;
    float stairStartY =  0.74f;
    float stairEndY   =  4.20f;
    float stepWidth   =  1.05f;
    float stepDepth   =  0.38f;
    float stepHeight  = (stairEndY - stairStartY) / (float)numSteps; // ~0.2307f per step

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.28f, 0.23f, 0.18f, 1.0f);

    for (int s = 0; s < numSteps; ++s) {
        float t = (float)s / (float)(numSteps - 1);
        float sy = stairStartY + (float)s * stepHeight;
        float sz = stairStartZ + t * (stairEndZ - stairStartZ);
        float sx = stairStartX;

        // Step Tread & Riser Box
        glPushMatrix();
        glTranslatef(sx, sy + stepHeight * 0.5f, sz);
        drawBox(stepWidth, stepHeight, stepDepth, 0.8f, 0.4f);

        // Bullnose step overhang trim
        applyMaterial(MAT_DARK_WOOD);
        glColor4f(0.22f, 0.18f, 0.14f, 1.0f);
        glTranslatef(0.0f, stepHeight * 0.45f, stepDepth * 0.48f);
        drawBox(stepWidth + 0.04f, 0.035f, 0.06f);
        glPopMatrix();

        // Vertical Carved Baluster Spindle on open side of step
        glPushMatrix();
        glTranslatef(sx - stepWidth * 0.45f, sy + stepHeight + 0.38f, sz);
        applyMaterial(MAT_DARK_WOOD);
        glColor4f(0.25f, 0.20f, 0.16f, 1.0f);
        drawBox(0.04f, 0.76f, 0.04f);
        glPopMatrix();
    }

    // Carved Newel Posts (Sturdy square posts with pyramid finials at bottom, mid-landing, and top)
    // Bottom Newel Post
    glPushMatrix();
    glTranslatef(stairStartX - stepWidth * 0.45f, stairStartY + 0.55f, stairStartZ + stepDepth * 0.35f);
    drawBox(0.10f, 1.10f, 0.10f);
    glTranslatef(0.0f, 0.58f, 0.0f);
    drawBox(0.13f, 0.06f, 0.13f); // Post cap
    glTranslatef(0.0f, 0.06f, 0.0f);
    drawSphere(0.055f, 8, 6);      // Finial ball
    glPopMatrix();

    // Top 2nd Floor Newel Post
    glPushMatrix();
    glTranslatef(stairStartX - stepWidth * 0.45f, stairEndY + 0.55f, stairEndZ - stepDepth * 0.35f);
    drawBox(0.10f, 1.10f, 0.10f);
    glTranslatef(0.0f, 0.58f, 0.0f);
    drawBox(0.13f, 0.06f, 0.13f);
    glTranslatef(0.0f, 0.06f, 0.0f);
    drawSphere(0.055f, 8, 6);
    glPopMatrix();

    // Continuous Carved Wooden Handrail running smoothly from bottom to top
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.22f, 0.18f, 0.14f, 1.0f);
    glPushMatrix();
    float midX = stairStartX - stepWidth * 0.45f;
    float midY = (stairStartY + stairEndY) * 0.5f + 0.85f;
    float midZ = (stairStartZ + stairEndZ) * 0.5f;
    float totalRunZ = std::abs(stairEndZ - stairStartZ);
    float totalRiseY = (stairEndY - stairStartY);
    float railAngle = -std::atan2(totalRiseY, totalRunZ) * 180.0f / (float)M_PI;
    float railLen = std::sqrt(totalRunZ * totalRunZ + totalRiseY * totalRiseY) + 0.20f;

    glTranslatef(midX, midY, midZ);
    glRotatef(railAngle, 1.0f, 0.0f, 0.0f);
    drawBox(0.065f, 0.075f, railLen);
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

    // 2. Safety Balustrade / Banister Guardrail around the 2nd Floor Stairwell Opening
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.20f, 0.16f, 1.0f);

    // Left Guardrail Handrail (along x = -1.95f, from z = 3.8f to z = -1.5f)
    glPushMatrix();
    glTranslatef(-1.95f, 5.08f, 1.15f);
    drawBox(0.065f, 0.075f, 5.30f);
    glPopMatrix();

    // Vertical Guardrail Balusters along left edge
    for (float bz = 3.70f; bz >= -1.40f; bz -= 0.45f) {
        glPushMatrix();
        glTranslatef(-1.95f, 4.66f, bz);
        drawBox(0.035f, 0.76f, 0.035f);
        glPopMatrix();
    }

    // Corner Newel Post at front of stair opening (x = -1.95f, z = 3.80f)
    glPushMatrix();
    glTranslatef(-1.95f, 4.78f, 3.80f);
    drawBox(0.09f, 1.05f, 0.09f);
    glTranslatef(0.0f, 0.55f, 0.0f);
    drawBox(0.12f, 0.05f, 0.12f);
    drawSphere(0.05f, 8, 6);
    glPopMatrix();

    // Back Guardrail Handrail (across z = -1.50f from x = -1.95f to x = -0.65f)
    glPushMatrix();
    glTranslatef(-1.30f, 5.08f, -1.50f);
    drawBox(1.30f, 0.075f, 0.065f);
    glPopMatrix();

    for (float bx = -1.80f; bx <= -0.80f; bx += 0.35f) {
        glPushMatrix();
        glTranslatef(bx, 4.66f, -1.50f);
        drawBox(0.035f, 0.76f, 0.035f);
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

    // 4. GOTHIC ANTIQUE FOUR-POSTER BED (Placed against Left Wall x = -6.8f, z = 1.8f)
    glPushMatrix();
    glTranslatef(-6.80f, 4.24f, 1.80f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.22f, 0.18f, 0.14f, 1.0f);

    // Bed Frame base platform
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 0.0f);
    drawBox(1.90f, 0.30f, 2.40f);
    glPopMatrix();

    // 4 Tall Turned Mahogany Bedposts (2.3m tall) with Finial Balls
    float postBX[4] = { -0.92f,  0.92f, -0.92f,  0.92f };
    float postBZ[4] = { -1.18f, -1.18f,  1.18f,  1.18f };
    for (int p = 0; p < 4; ++p) {
        glPushMatrix();
        glTranslatef(postBX[p], 1.15f, postBZ[p]);
        drawBox(0.09f, 2.30f, 0.09f);
        // Turned corbel rings
        glTranslatef(0.0f, 1.18f, 0.0f);
        drawSphere(0.065f, 8, 6);
        glPopMatrix();
    }

    // Top Wooden Canopy Rails connecting the 4 bedposts
    glPushMatrix();
    glTranslatef(0.0f, 2.30f, 0.0f);
    glPushMatrix(); glTranslatef( 0.0f, 0.0f, -1.18f); drawBox(1.90f, 0.07f, 0.07f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.0f, 0.0f,  1.18f); drawBox(1.90f, 0.07f, 0.07f); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.92f, 0.0f,   0.0f); drawBox(0.07f, 0.07f, 2.40f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.92f, 0.0f,   0.0f); drawBox(0.07f, 0.07f, 2.40f); glPopMatrix();
    glPopMatrix();

    // Carved Gothic Headboard
    glPushMatrix();
    glTranslatef(0.0f, 0.90f, -1.16f);
    drawBox(1.80f, 0.85f, 0.06f);
    // Pointed arch crest on headboard
    glTranslatef(0.0f, 0.48f, 0.0f);
    drawBox(0.90f, 0.22f, 0.06f);
    glPopMatrix();

    // Footboard
    glPushMatrix();
    glTranslatef(0.0f, 0.65f, 1.16f);
    drawBox(1.80f, 0.45f, 0.06f);
    glPopMatrix();

    // Velvet Burgundy Quilt & Torn Antique Mattress
    applyMaterial(MAT_WEATHERED_WALL);
    bindTexture(TEX_NONE);
    glColor4f(0.42f, 0.12f, 0.15f, 1.0f); // Dark gothic crimson velvet
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.10f);
    drawBox(1.72f, 0.20f, 2.15f);
    glPopMatrix();

    // Dusty Antique Pillows
    glColor4f(0.68f, 0.65f, 0.58f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.45f, 0.70f, -0.80f);
    glScalef(0.65f, 0.18f, 0.45f);
    drawSphere(0.5f, 10, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef( 0.45f, 0.70f, -0.80f);
    glScalef(0.65f, 0.18f, 0.45f);
    drawSphere(0.5f, 10, 8);
    glPopMatrix();

    glPopMatrix(); // End Bed

    // 5. ALCHEMIST / WITCHCRAFT STUDY DESK & OCCULT GRIMOIRE (x = -7.2f, z = -2.4f)
    glPushMatrix();
    glTranslatef(-7.20f, 4.24f, -2.40f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.19f, 0.15f, 1.0f);

    // Desk Top & Drawers
    glPushMatrix();
    glTranslatef(0.0f, 0.75f, 0.0f);
    drawBox(1.60f, 0.08f, 0.85f);
    // Drawers box
    glTranslatef(0.0f, -0.12f, 0.0f);
    drawBox(1.50f, 0.16f, 0.80f);
    // 4 Turned Desk Legs
    glTranslatef(-0.68f, -0.32f, -0.34f); drawBox(0.08f, 0.64f, 0.08f);
    glTranslatef( 1.36f,  0.00f,  0.00f); drawBox(0.08f, 0.64f, 0.08f);
    glTranslatef( 0.00f,  0.00f,  0.68f); drawBox(0.08f, 0.64f, 0.08f);
    glTranslatef(-1.36f,  0.00f,  0.00f); drawBox(0.08f, 0.64f, 0.08f);
    glPopMatrix();

    // Wooden High-Back Desk Chair
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.65f);
    glRotatef(15.0f, 0.0f, 1.0f, 0.0f);
    // Chair Seat
    glTranslatef(0.0f, 0.45f, 0.0f); drawBox(0.48f, 0.05f, 0.48f);
    // Legs
    glTranslatef(-0.19f, -0.225f, -0.19f); drawBox(0.05f, 0.45f, 0.05f);
    glTranslatef( 0.38f,  0.000f,  0.00f); drawBox(0.05f, 0.45f, 0.05f);
    glTranslatef( 0.00f,  0.000f,  0.38f); drawBox(0.05f, 0.45f, 0.05f);
    glTranslatef(-0.38f,  0.000f,  0.00f); drawBox(0.05f, 0.45f, 0.05f);
    // Carved Backrest
    glTranslatef( 0.19f,  0.500f, -0.19f); drawBox(0.44f, 0.55f, 0.04f);
    glPopMatrix();

    // OPEN ANCIENT SPELLBOOK / GRIMOIRE on Desk with visible magical rune pages!
    applyMaterial(MAT_STONE);
    bindTexture(TEX_NONE);
    // Leather book cover
    glColor4f(0.38f, 0.15f, 0.10f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.25f, 0.81f, 0.05f);
    glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
    drawBox(0.46f, 0.03f, 0.34f);

    // Open Parchment Pages (Left & Right halves)
    glColor4f(0.85f, 0.80f, 0.65f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.10f, 0.025f, 0.0f);
    glRotatef( 6.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.20f, 0.02f, 0.30f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef( 0.10f, 0.025f, 0.0f);
    glRotatef(-6.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.20f, 0.02f, 0.30f);
    glPopMatrix();

    // Glowing Arcane Glyphs & Runes on open pages
    glDisable(GL_LIGHTING);
    float runeGlow = 0.70f + 0.30f * std::sin(g_time * 4.0f);
    glColor4f(0.40f, 0.90f, 1.0f, runeGlow);
    glBegin(GL_LINES);
    glVertex3f(-0.16f, 0.045f, -0.08f); glVertex3f(-0.04f, 0.045f, -0.08f);
    glVertex3f(-0.16f, 0.045f, -0.02f); glVertex3f(-0.04f, 0.045f, -0.02f);
    glVertex3f(-0.16f, 0.045f,  0.04f); glVertex3f(-0.04f, 0.045f,  0.04f);
    glVertex3f( 0.04f, 0.045f, -0.08f); glVertex3f( 0.16f, 0.045f, -0.08f);
    glVertex3f( 0.04f, 0.045f, -0.02f); glVertex3f( 0.16f, 0.045f, -0.02f);
    glVertex3f( 0.04f, 0.045f,  0.04f); glVertex3f( 0.16f, 0.045f,  0.04f);
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // ORNATE 3-ARM BRASS CANDELABRA ON DESK WITH 3 BURNING CANDLES
    glPushMatrix();
    glTranslatef(0.42f, 0.79f, -0.10f);
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_NONE);
    glColor4f(0.58f, 0.46f, 0.22f, 1.0f); // Antique brass

    // Base & Center stem
    drawCylinder(0.08f, 0.04f, 0.04f, 8);
    glTranslatef(0.0f, 0.04f, 0.0f);
    drawCylinder(0.02f, 0.015f, 0.22f, 6);

    // Left curved branch
    glPushMatrix();
    glTranslatef(-0.10f, 0.16f, 0.0f);
    drawBox(0.12f, 0.018f, 0.018f);
    drawCylinder(0.025f, 0.025f, 0.03f, 6);
    // Candle wax
    glColor4f(0.92f, 0.90f, 0.80f, 1.0f);
    glTranslatef(0.0f, 0.03f, 0.0f);
    drawCylinder(0.016f, 0.015f, 0.10f, 6);
    // Flickering Flame
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
    glColor4f(0.58f, 0.46f, 0.22f, 1.0f);
    drawBox(0.12f, 0.018f, 0.018f);
    drawCylinder(0.025f, 0.025f, 0.03f, 6);
    // Candle wax
    glColor4f(0.92f, 0.90f, 0.80f, 1.0f);
    glTranslatef(0.0f, 0.03f, 0.0f);
    drawCylinder(0.016f, 0.015f, 0.10f, 6);
    // Flickering Flame
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

    // Glowing Alchemical Potion Bottles & Crystal Scrying Ball
    glPushMatrix();
    glTranslatef(0.48f, 0.79f, 0.20f);
    // Emerald Potion Bottle
    glDisable(GL_LIGHTING);
    glColor4f(0.20f, 0.95f, 0.35f, 0.85f);
    drawSphere(0.045f, 8, 8);
    glTranslatef(0.0f, 0.05f, 0.0f);
    drawCylinder(0.015f, 0.015f, 0.04f, 6);
    // Mystical Scrying Orb
    glTranslatef(-0.16f, -0.01f, 0.0f);
    float orbPulse = 0.80f + 0.20f * std::sin(g_time * 3.5f);
    glColor4f(0.70f, 0.35f, 0.95f, 0.88f * orbPulse);
    drawSphere(0.055f, 10, 8);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glPopMatrix(); // End Study Desk

    // 6. VINTAGE ROCKING ARMCHAIR BY UPPER DORMER WINDOW (x = -5.2f, z = 3.6f)
    glPushMatrix();
    glTranslatef(-5.20f, 4.24f, 3.60f);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f); // Facing toward window

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.20f, 0.16f, 1.0f);

    // Curved Rocker Runners on floor
    glPushMatrix();
    glTranslatef(-0.24f, 0.04f, 0.0f); drawBox(0.04f, 0.04f, 0.85f);
    glTranslatef( 0.48f, 0.00f, 0.0f); drawBox(0.04f, 0.04f, 0.85f);
    glPopMatrix();

    // Chair Seat & Legs
    glPushMatrix();
    glTranslatef(0.0f, 0.42f, 0.0f); drawBox(0.52f, 0.05f, 0.48f);
    glTranslatef(-0.20f, -0.19f, -0.18f); drawBox(0.045f, 0.38f, 0.045f);
    glTranslatef( 0.40f,  0.00f,  0.00f); drawBox(0.045f, 0.38f, 0.045f);
    glTranslatef( 0.00f,  0.00f,  0.36f); drawBox(0.045f, 0.38f, 0.045f);
    glTranslatef(-0.40f,  0.00f,  0.00f); drawBox(0.045f, 0.38f, 0.045f);
    // Spindle Backrest
    glTranslatef( 0.20f,  0.48f, -0.18f); drawBox(0.48f, 0.58f, 0.04f);
    // Armrests
    glTranslatef(-0.22f, -0.22f, 0.18f); drawBox(0.045f, 0.04f, 0.40f);
    glTranslatef( 0.44f,  0.00f, 0.00f); drawBox(0.045f, 0.04f, 0.40f);
    glPopMatrix();

    glPopMatrix(); // End Rocking Chair

    // 7. ANTIQUE WOODEN STORAGE CHEST WITH RUSTED IRON STRAPS (x = -3.4f, z = -2.6f)
    glPushMatrix();
    glTranslatef(-3.40f, 4.24f, -2.60f);
    glRotatef(25.0f, 0.0f, 1.0f, 0.0f);

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.26f, 0.20f, 0.15f, 1.0f);
    // Chest Body Box
    glPushMatrix();
    glTranslatef(0.0f, 0.30f, 0.0f);
    drawBox(1.10f, 0.60f, 0.65f);
    // Curved Lid
    glTranslatef(0.0f, 0.33f, 0.0f);
    drawBox(1.12f, 0.08f, 0.67f);
    // Iron Reinforcing Straps
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    glColor4f(0.35f, 0.32f, 0.30f, 1.0f);
    glPushMatrix(); glTranslatef(-0.35f, -0.15f, 0.0f); drawBox(0.06f, 0.70f, 0.68f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.35f, -0.15f, 0.0f); drawBox(0.06f, 0.70f, 0.68f); glPopMatrix();
    // Front Padlock
    glTranslatef(0.0f, -0.06f, 0.34f);
    drawBox(0.08f, 0.10f, 0.04f);
    glPopMatrix();
    glPopMatrix();

    // 8. HANGING VINTAGE BRASS LANTERN FROM RIDGE TIMBER BEAM
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

    // 9. Cobwebs in 2nd Floor Ceiling Rafter Angles
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

