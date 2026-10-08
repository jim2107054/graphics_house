#include "Vegetation.h"
#include "Terrain.h"
#include "../graphics/Material.h"
#include "../graphics/TextureManager.h"
#include "../graphics/Primitives.h"
#include <cmath>

void drawNaturalFoliageCluster(float radius, TreeRNG& rng, int foliageType) {    if (foliageType == 1) {
        applyMaterial(MAT_FOLIAGE_DARK);
        glColor4fv(MAT_FOLIAGE_DARK.diffuse);
    } else if (foliageType == 2) {
        applyMaterial(MAT_FOLIAGE_AUTUMN);
        glColor4fv(MAT_FOLIAGE_AUTUMN.diffuse);
    } else {
        applyMaterial(MAT_FOLIAGE_LUSH);
        glColor4fv(MAT_FOLIAGE_LUSH.diffuse);
    }
    bindTexture(TEX_NONE);

    // Central primary foliage mass (smooth shaded)
    drawSphere(radius * 0.85f, 14, 10);

    // 5 interlocking satellite lobes around the perimeter for rich organic silhouettes
    for (int i = 0; i < 5; ++i) {
        float ang = (float)i * (2.0f * (float)M_PI / 5.0f) + rng.nextFloat(-0.25f, 0.25f);
        float elev = rng.nextFloat(-0.18f, 0.25f);
        float dist = radius * rng.nextFloat(0.32f, 0.48f);
        float rSub = radius * rng.nextFloat(0.55f, 0.70f);

        glPushMatrix();
        glTranslatef(dist * std::cos(ang), dist * elev, dist * std::sin(ang));
        drawSphere(rSub, 12, 8);
        glPopMatrix();
    }

    // Outer subtle leaf accents to soften silhouette edges
    int leafCards = 6;
    for (int l = 0; l < leafCards; ++l) {
        float la = (float)l * (2.0f * (float)M_PI / (float)leafCards) + rng.nextFloat(-0.3f, 0.3f);
        float ld = radius * rng.nextFloat(0.70f, 0.92f);
        float ly = radius * rng.nextFloat(-0.22f, 0.40f);
        float ls = radius * rng.nextFloat(0.18f, 0.28f);

        glPushMatrix();
        glTranslatef(ld * std::cos(la), ly, ld * std::sin(la));
        glRotatef(rng.nextFloat(0.0f, 360.0f), 0.0f, 1.0f, 0.0f);
        glRotatef(rng.nextFloat(-25.0f, 25.0f), 1.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLES);
        glNormal3f(0.0f, 0.8f, 0.6f);
        glVertex3f(0.0f, 0.0f, -ls * 0.5f);
        glVertex3f(-ls * 0.4f, 0.0f, ls * 0.2f);
        glVertex3f(ls * 0.4f, ls * 0.2f, ls * 0.4f);
        glEnd();
        glPopMatrix();
    }

    // Restore bark material & texture
    applyMaterial(MAT_BARK);
    glColor4fv(MAT_BARK.diffuse);
    bindTexture(TEX_BARK);
}

// Flaring root base anchoring trunk into terrain
void drawRootFlare(float trunkR, TreeRNG& rng) {    int numRoots = 5;
    for (int i = 0; i < numRoots; ++i) {
        float angle = (float)i * (360.0f / (float)numRoots) + rng.nextFloat(-15.0f, 15.0f);
        glPushMatrix();
        glRotatef(angle, 0.0f, 1.0f, 0.0f);
        glTranslatef(trunkR * 0.30f, 0.03f, 0.0f);

        glRotatef(rng.nextFloat(55.0f, 68.0f), 0.0f, 0.0f, 1.0f);
        float r1Len = trunkR * rng.nextFloat(1.1f, 1.4f);
        drawCylinder(trunkR * 0.45f, trunkR * 0.22f, r1Len, 6, 1.0f, 1.0f);

        glTranslatef(0.0f, r1Len, 0.0f);
        glRotatef(rng.nextFloat(22.0f, 35.0f), 0.0f, 0.0f, 1.0f);
        float r2Len = trunkR * rng.nextFloat(1.1f, 1.5f);
        drawCylinder(trunkR * 0.22f, trunkR * 0.06f, r2Len, 5, 1.0f, 1.0f);

        glPopMatrix();
    }
}

// 1. Natural Organic Deciduous Tree (Small, compact, beautiful leafy canopy, 100% fully visible)
void drawNaturalDeciduousTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed, int foliageType, float colorTint) {    TreeRNG rng(seed);

    float groundY = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, groundY, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // Natural gentle wind sway in the night breeze
    float treeWindPhase = g_time * 1.15f + (float)(seed % 100) * 0.08f;
    float swayAmp = 0.55f;
    float swayX = std::sin(treeWindPhase) * swayAmp;
    float swayZ = std::cos(treeWindPhase * 0.9f) * (swayAmp * 0.6f);
    glRotatef(swayX * 0.35f, 1.0f, 0.0f, 0.0f);
    glRotatef(swayZ * 0.35f, 0.0f, 0.0f, 1.0f);

    // Bark material with tone variations
    Material treeMat = MAT_BARK;
    treeMat.diffuse[0] *= colorTint;
    treeMat.diffuse[1] *= colorTint * 0.96f;
    treeMat.diffuse[2] *= colorTint * 0.92f;
    applyMaterial(treeMat);
    bindTexture(TEX_BARK);

    // Flared roots into terrain
    drawRootFlare(trunkRadius, rng);

    // Trunk segments with natural curvature
    float seg1H = height * 0.16f;
    float seg2H = height * 0.16f;
    float seg3H = height * 0.14f;

    float r0 = trunkRadius;
    float r1 = trunkRadius * 0.85f;
    float r2 = trunkRadius * 0.70f;
    float r3 = trunkRadius * 0.55f;

    // Segment 1 (Base)
    drawCylinder(r0, r1, seg1H, 8, 1.0f, 1.0f);
    glTranslatef(0.0f, seg1H, 0.0f);

    // Segment 2 (Mid trunk with slight bend)
    float bend1X = rng.nextFloat(-3.5f, 3.5f);
    float bend1Z = rng.nextFloat(-3.5f, 3.5f);
    glRotatef(bend1X, 1.0f, 0.0f, 0.0f);
    glRotatef(bend1Z, 0.0f, 0.0f, 1.0f);
    drawCylinder(r1, r2, seg2H, 8, 1.0f, 1.0f);
    glTranslatef(0.0f, seg2H, 0.0f);

    // Segment 3 (Upper trunk heading into main fork)
    float bend2X = rng.nextFloat(-4.0f, 4.0f);
    float bend2Z = rng.nextFloat(-4.0f, 4.0f);
    glRotatef(bend2X, 1.0f, 0.0f, 0.0f);
    glRotatef(bend2Z, 0.0f, 0.0f, 1.0f);
    drawCylinder(r2, r3, seg3H, 7, 1.0f, 1.0f);
    glTranslatef(0.0f, seg3H, 0.0f);

    // Primary branch boughs (3 main natural limbs spreading outward and upward)
    int numBoughs = 3 + (int)(rng.nextFloat(0.0f, 1.9f));
    for (int b = 0; b < numBoughs; ++b) {
        float azimuth = (float)b * (360.0f / (float)numBoughs) + rng.nextFloat(-18.0f, 18.0f);
        float pitch = rng.nextFloat(30.0f, 45.0f);
        float boughLen = height * rng.nextFloat(0.26f, 0.35f);
        float boughBaseR = r3 * rng.nextFloat(0.65f, 0.80f);
        float boughTopR = boughBaseR * 0.40f;

        glPushMatrix();
        glRotatef(azimuth, 0.0f, 1.0f, 0.0f);
        glRotatef(pitch, 1.0f, 0.0f, 0.0f);

        // Branch wind motion
        glRotatef(swayX * (0.8f + (float)b * 0.2f), 1.0f, 0.0f, 0.0f);

        drawCylinder(boughBaseR, boughTopR, boughLen, 6, 1.0f, 1.0f);
        glTranslatef(0.0f, boughLen, 0.0f);

        // Sub-branches
        int numTwigs = 2;
        for (int t = 0; t < numTwigs; ++t) {
            float twigAz = (float)t * 180.0f + rng.nextFloat(-25.0f, 25.0f);
            float twigPitch = rng.nextFloat(22.0f, 38.0f);
            float twigLen = boughLen * rng.nextFloat(0.40f, 0.60f);

            glPushMatrix();
            glRotatef(twigAz, 0.0f, 1.0f, 0.0f);
            glRotatef(twigPitch, 1.0f, 0.0f, 0.0f);
            drawCylinder(boughTopR * 0.70f, 0.012f, twigLen, 4, 1.0f, 1.0f);
            glTranslatef(0.0f, twigLen, 0.0f);

            // Foliage cluster at tip of sub-branch
            drawNaturalFoliageCluster(height * rng.nextFloat(0.16f, 0.22f), rng, foliageType);
            glPopMatrix();
        }

        // Main bough foliage cluster
        drawNaturalFoliageCluster(height * rng.nextFloat(0.20f, 0.26f), rng, foliageType);
        glPopMatrix();
    }

    // Central crown top foliage dome
    glPushMatrix();
    glTranslatef(0.0f, height * 0.06f, 0.0f);
    drawNaturalFoliageCluster(height * 0.28f, rng, foliageType);
    glPopMatrix();

    glPopMatrix();
}

// 2. Natural Evergreen Conifer / Pine Tree (Small, elegant, tiered needle foliage, 100% fully visible)
void drawNaturalPineTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed) {    TreeRNG rng(seed);

    float groundY = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, groundY, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // Subtle pine tree wind sway
    float treeWindPhase = g_time * 1.10f + (float)(seed % 100) * 0.08f;
    float swayAmp = 0.45f;
    float swayX = std::sin(treeWindPhase) * swayAmp;
    float swayZ = std::cos(treeWindPhase * 0.85f) * (swayAmp * 0.5f);
    glRotatef(swayX * 0.25f, 1.0f, 0.0f, 0.0f);
    glRotatef(swayZ * 0.25f, 0.0f, 0.0f, 1.0f);

    // Bark material & trunk
    applyMaterial(MAT_BARK);
    bindTexture(TEX_BARK);
    drawRootFlare(trunkRadius, rng);

    // Tapered trunk to tree top
    drawCylinder(trunkRadius, trunkRadius * 0.15f, height * 0.92f, 8, 1.0f, 2.0f);

    // Tiered conical evergreen needle foliage layers
    applyMaterial(MAT_PINE_NEEDLES);
    glColor4fv(MAT_PINE_NEEDLES.diffuse);
    bindTexture(TEX_NONE);

    struct PineTier {
        float yFrac;
        float baseRadiusFrac;
        float coneHeightFrac;
    };
    static const PineTier TIERS[5] = {
        { 0.26f, 0.34f, 0.24f },
        { 0.42f, 0.28f, 0.22f },
        { 0.56f, 0.22f, 0.20f },
        { 0.70f, 0.16f, 0.18f },
        { 0.82f, 0.11f, 0.16f }
    };

    for (int t = 0; t < 5; ++t) {
        float tierY = height * TIERS[t].yFrac;
        float tierBaseR = height * TIERS[t].baseRadiusFrac;
        float tierConeH = height * TIERS[t].coneHeightFrac;

        glPushMatrix();
        glTranslatef(0.0f, tierY, 0.0f);

        // Wind flex on higher tiers
        float tierFlex = (float)(t + 1) * 0.18f;
        glRotatef(swayX * tierFlex, 1.0f, 0.0f, 0.0f);
        glRotatef(swayZ * tierFlex, 0.0f, 0.0f, 1.0f);

        // Conical foliage tier
        drawCylinder(tierBaseR, 0.005f, tierConeH, 10, 1.0f, 1.0f);

        // Scalloped undergrowth needles
        int subNeedles = 6;
        for (int n = 0; n < subNeedles; ++n) {
            float ang = (float)n * (360.0f / (float)subNeedles) + rng.nextFloat(-10.0f, 10.0f);
            glPushMatrix();
            glRotatef(ang, 0.0f, 1.0f, 0.0f);
            glTranslatef(tierBaseR * 0.60f, 0.02f, 0.0f);
            glRotatef(rng.nextFloat(40.0f, 55.0f), 0.0f, 0.0f, 1.0f);
            drawSphere(tierBaseR * 0.20f, 8, 5);
            glPopMatrix();
        }

        glPopMatrix();
    }

    // Reset material
    applyMaterial(MAT_BARK);
    glColor4fv(MAT_BARK.diffuse);
    bindTexture(TEX_BARK);

    glPopMatrix();
}

// Compatibility wrapper for any legacy organic tree calls
void drawOrganicCreepyTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed, float colorTint) {    (void)trunkRadius;
    drawNaturalDeciduousTree(x, z, 0.16f, std::min(height, 4.2f), rotY, seed, (seed % 2), colorTint);
}

// Compatibility wrapper for gothic foreground tree
void drawGothicForegroundTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed) {    (void)trunkRadius; (void)height;
    drawNaturalDeciduousTree(x, z, 0.16f, 3.8f, rotY, seed, 0, 1.0f);
}

// Scatter Natural Organic Trees across the entire landscape (Small, natural, 100% fully visible)
void drawAllTrees() {    // 1. FRONT YARD & ENTRANCE TREES (Framing the entrance knolls, small and completely visible in full)
    drawNaturalDeciduousTree(-4.8f, 15.5f, 0.15f, 3.4f,  25.0f, 1001, 0, 0.95f); // Front Left Lawn (In full view)
    drawNaturalPineTree(     13.5f, 14.5f, 0.16f, 3.8f,  40.0f, 1002);         // Far Right Entrance Flank
    drawNaturalDeciduousTree(15.5f, 16.5f, 0.14f, 3.4f, -30.0f, 1003, 1, 0.90f); // Cemetery Far Border

    // 2. MIDGROUND YARD & GRAVEYARD TREES
    drawNaturalDeciduousTree( -7.8f, 12.5f, 0.15f, 3.4f, -15.0f, 1004, 1, 0.92f); // Graveyard Knoll
    drawNaturalPineTree(     -11.5f, 10.0f, 0.16f, 3.7f,  30.0f, 1005);         // Left Fence Line
    drawNaturalDeciduousTree(-13.5f,  5.5f, 0.17f, 3.9f,  48.0f, 1006, 0, 0.94f); // Left Manor Approach
    drawNaturalDeciduousTree( 13.8f,  7.0f, 0.16f, 3.9f, -42.0f, 1007, 0, 0.95f); // Right Lawn behind car
    drawNaturalPineTree(      16.0f,  9.5f, 0.16f, 3.8f,  55.0f, 1008);         // Far Right Manor Approach

    // 3. MANOR FLANKING TREES (Flanking the house on left and right)
    drawNaturalDeciduousTree(-15.0f, -3.5f, 0.17f, 4.1f,  12.0f, 2001, 0, 0.90f); // Left House Flank
    drawNaturalPineTree(      13.5f, -3.0f, 0.17f, 4.1f, -20.0f, 2002);         // Right House Flank

    // 4. BACKGROUND FOREST HORIZON (Small natural trees on the ridge behind the mansion, creating a dense natural backdrop)
    drawNaturalPineTree(     -12.5f, -14.0f, 0.16f, 4.0f,  15.0f, 3001);
    drawNaturalDeciduousTree( -8.0f, -16.0f, 0.16f, 3.8f, -22.0f, 3002, 1, 0.88f);
    drawNaturalPineTree(      -4.0f, -17.5f, 0.17f, 4.0f,  45.0f, 3003);
    drawNaturalDeciduousTree(  0.0f, -18.5f, 0.17f, 3.9f, -28.0f, 3004, 0, 0.88f);
    drawNaturalPineTree(       4.0f, -17.5f, 0.17f, 4.0f,  35.0f, 3005);
    drawNaturalDeciduousTree(  8.0f, -16.0f, 0.16f, 3.8f, -15.0f, 3006, 1, 0.88f);
    drawNaturalPineTree(      12.5f, -14.0f, 0.16f, 4.0f,  60.0f, 3007);
}

