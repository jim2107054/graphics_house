#include "Terrain.h"
#include "Props.h"
#include "Graveyard.h"
#include "../graphics/Material.h"
#include "../graphics/TextureManager.h"
#include "../graphics/Primitives.h"
#include <cmath>
#include <algorithm>

float getTerrainHeight(float x, float z) {    // Distance to house foundation [-8.5, 6.5] x [-7.0, 5.0] and porch
    float dx = std::max(0.0f, std::abs(x + 1.0f) - 7.5f);
    float dz = std::max(0.0f, std::abs(z - 0.5f) - 6.5f);
    float dHouse = std::sqrt(dx * dx + dz * dz);
    float houseBlend = 1.0f - std::exp(-dHouse * 0.45f);

    // Distance to cobblestone pathway
    float pathX = -0.5f + 1.2f * std::sin((24.0f - z) / 20.0f * (float)M_PI * 1.4f);
    float dPath = (z >= 3.0f && z <= 24.5f) ? std::abs(x - pathX) : 10.0f;
    float pathBlend = (dPath < 2.0f) ? (dPath / 2.0f) : 1.0f;

    float blend = houseBlend * pathBlend;

    // Gentle multi-frequency undulating terrain
    float h1 = 0.30f * std::sin(x * 0.075f + 1.2f) * std::cos(z * 0.065f + 0.5f);
    float h2 = 0.12f * std::sin(x * 0.18f - z * 0.15f) * std::cos(x * 0.12f + z * 0.20f);
    float h3 = 0.05f * std::sin(x * 0.38f + z * 0.32f);

    // Puddle depression 1: (x = -4.5, z = 13.5)
    float dp1 = std::sqrt((x + 4.5f)*(x + 4.5f)*1.0f + (z - 13.5f)*(z - 13.5f)*1.4f);
    float dip1 = (dp1 < 2.5f) ? (-0.08f * (1.0f - dp1 / 2.5f)) : 0.0f;

    // Puddle depression 2: (x = 5.0, z = 10.5)
    float dp2 = std::sqrt((x - 5.0f)*(x - 5.0f)*1.3f + (z - 10.5f)*(z - 10.5f)*1.0f);
    float dip2 = (dp2 < 2.2f) ? (-0.07f * (1.0f - dp2 / 2.2f)) : 0.0f;

    // Puddle depression 3: (x = -9.5, z = 8.2)
    float dp3 = std::sqrt((x + 9.5f)*(x + 9.5f)*1.0f + (z - 8.2f)*(z - 8.2f)*1.2f);
    float dip3 = (dp3 < 2.0f) ? (-0.06f * (1.0f - dp3 / 2.0f)) : 0.0f;

    return (h1 + h2 + h3 + dip1 + dip2 + dip3) * blend;
}

// Compute accurate surface normals using finite differences
void getTerrainNormal(float x, float z, float& nx, float& ny, float& nz) {    const float eps = 0.15f;
    float hL = getTerrainHeight(x - eps, z);
    float hR = getTerrainHeight(x + eps, z);
    float hD = getTerrainHeight(x, z - eps);
    float hU = getTerrainHeight(x, z + eps);

    nx = (hL - hR) / (2.0f * eps);
    ny = 1.0f;
    nz = (hD - hU) / (2.0f * eps);

    float len = std::sqrt(nx * nx + ny * ny + nz * nz);
    if (len > 0.0001f) {
        nx /= len; ny /= len; nz /= len;
    }
}

// Draw a single reflective water puddle with dark wet mud rim
void drawPuddle(float cx, float cz, float radiusX, float radiusZ, float rotAngle) {    float baseY = getTerrainHeight(cx, cz) + 0.02f;
    int segments = 24;

    glPushMatrix();
    glTranslatef(cx, baseY, cz);
    glRotatef(rotAngle, 0.0f, 1.0f, 0.0f);

    // 1. Dark damp saturated mud border fringe around puddle
    applyMaterial(MAT_WET_GROUND);
    bindTexture(TEX_GROUND);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float angle = (float)i * 2.0f * (float)M_PI / segments;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);

        float inX  = radiusX * cosA;
        float inZ  = radiusZ * sinA;
        float outX = (radiusX + 0.55f) * cosA;
        float outZ = (radiusZ + 0.55f) * sinA;

        float nx, ny, nz;
        getTerrainNormal(cx + inX, cz + inZ, nx, ny, nz);
        glNormal3f(nx, ny, nz);

        // Dark soaked mud
        glColor4f(0.22f, 0.22f, 0.25f, 1.0f);
        glTexCoord2f(inX * 0.2f, inZ * 0.2f);
        glVertex3f(inX, 0.002f, inZ);

        // Fading outward to normal terrain
        glColor4f(0.70f, 0.70f, 0.72f, 1.0f);
        glTexCoord2f(outX * 0.2f, outZ * 0.2f);
        glVertex3f(outX, -0.015f, outZ);
    }
    glEnd();

    // 2. Reflective Water Surface Disk (High specular mirror highlight)
    applyMaterial(MAT_PUDDLE_WATER);
    bindTexture(TEX_NONE);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glColor4f(0.12f, 0.16f, 0.22f, 0.92f);
    glVertex3f(0.0f, 0.008f, 0.0f);

    for (int i = 0; i <= segments; ++i) {
        float angle = (float)i * 2.0f * (float)M_PI / segments;
        float px = radiusX * std::cos(angle);
        float pz = radiusZ * std::sin(angle);
        glVertex3f(px, 0.008f, pz);
    }
    glEnd();

    // 3. Dynamic Water Ripple Drip Rings (expanding concentric rings with fading alpha)
    glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive luminous water glint
    glLineWidth(1.6f);

    for (int d = 0; d < 2; ++d) {
        float dripTime = g_time * 1.2f + (float)d * 1.4f + std::abs(cx) * 0.4f;
        float cycle = std::fmod(dripTime, 2.4f);
        float rProgress = cycle / 2.4f;
        float ripRadius = rProgress * (radiusX * 0.72f);
        float ripAlpha = (1.0f - rProgress) * 0.35f;

        float dripOffsetX = (d == 0) ? -0.25f : 0.35f;
        float dripOffsetZ = (d == 0) ? 0.15f : -0.20f;

        glColor4f(0.55f, 0.72f, 0.95f, ripAlpha);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 20; ++i) {
            float theta = 2.0f * (float)M_PI * (float)i / 20.0f;
            float px = dripOffsetX + ripRadius * std::cos(theta);
            float pz = dripOffsetZ + (ripRadius * (radiusZ / radiusX)) * std::sin(theta);
            glVertex3f(px, 0.012f, pz);
        }
        glEnd();
    }
    glPopAttrib();

    glPopMatrix();
}

// Draw all 3 reflective puddles
void drawPuddles() {    // Puddle 1: Front yard pathside puddle (catches moon + porch light)
    drawPuddle(-4.5f, 13.5f, 2.3f, 1.6f, -18.0f);

    // Puddle 2: Near monster tree & rusted car (catches moonlight)
    drawPuddle( 5.0f, 10.5f, 2.0f, 1.4f,  24.0f);

    // Puddle 3: Left side near cemetery / porch corner
    drawPuddle(-9.5f,  8.2f, 1.8f, 1.3f, -10.0f);
}



// Exact Cobblestone Road Centerline Formula (Stretches from foreground to house porch)
inline float getRoadCenterX(float pz) {
    if (pz > 24.0f) return 1.2f;
    if (pz < -3.5f) return -2.8f;
    float t = (22.0f - pz) / 25.5f;
    t = std::max(0.0f, std::min(1.0f, t));
    float ease = t * t * (3.0f - 2.0f * t);
    return (1.0f - ease) * 1.2f + ease * (-2.8f) - 1.1f * std::sin(t * (float)M_PI);
}

// Slender, dense dark wild grass tufts (Dark silhouette tones - NO bright green)
void drawGrassTuft(float x, float z, float width, float height, float rotY) {    float y = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // Subtle gentle night wind sway
    float windAngle = std::sin(g_time * 1.45f + x * 0.35f + z * 0.25f) * 2.8f;
    glRotatef(windAngle, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_DEAD_GRASS);
    bindTexture(TEX_NONE);

    float hw = width * 0.5f;

    glBegin(GL_TRIANGLES);

    // 1. Low Ground-Hugging Undergrowth Base Leaves (Dark decayed charcoal tones)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glColor4f(0.040f, 0.045f, 0.038f, 1.0f);
    glVertex3f(0.0f, 0.01f, 0.0f);
    glVertex3f(-hw * 0.65f, 0.02f, -hw * 0.45f);
    glVertex3f(-hw * 0.20f, 0.03f, -hw * 0.85f);

    glVertex3f(0.0f, 0.01f, 0.0f);
    glVertex3f( hw * 0.20f, 0.03f, -hw * 0.85f);
    glVertex3f( hw * 0.65f, 0.02f, -hw * 0.45f);

    glVertex3f(0.0f, 0.01f, 0.0f);
    glVertex3f( hw * 0.75f, 0.02f,  hw * 0.35f);
    glVertex3f( hw * 0.35f, 0.03f,  hw * 0.75f);

    glVertex3f(0.0f, 0.01f, 0.0f);
    glVertex3f(-hw * 0.35f, 0.03f,  hw * 0.75f);
    glVertex3f(-hw * 0.75f, 0.02f,  hw * 0.35f);

    // 2. Eight Tall Fanning Jungle-Grass Blades (Dark moody night shading)
    // Blade 1 (0 deg)
    glNormal3f(0.0f, 0.3f, 0.95f);
    glColor4f(0.050f, 0.058f, 0.045f, 1.0f);
    glVertex3f(-hw * 0.35f, 0.0f, 0.0f);
    glVertex3f( hw * 0.35f, 0.0f, 0.0f);
    glColor4f(0.105f, 0.120f, 0.090f, 1.0f);
    glVertex3f(0.02f, height, 0.05f);

    // Blade 2 (25 deg)
    glNormal3f(0.4f, 0.3f, 0.85f);
    glColor4f(0.048f, 0.055f, 0.044f, 1.0f);
    glVertex3f(-hw * 0.30f, 0.0f, -hw * 0.15f);
    glVertex3f( hw * 0.30f, 0.0f,  hw * 0.15f);
    glColor4f(0.110f, 0.125f, 0.095f, 1.0f);
    glVertex3f(0.06f, height * 0.96f, -0.02f);

    // Blade 3 (50 deg)
    glNormal3f(0.75f, 0.3f, 0.6f);
    glColor4f(0.046f, 0.052f, 0.042f, 1.0f);
    glVertex3f(-hw * 0.22f, 0.0f, -hw * 0.26f);
    glVertex3f( hw * 0.22f, 0.0f,  hw * 0.26f);
    glColor4f(0.100f, 0.115f, 0.088f, 1.0f);
    glVertex3f(0.05f, height * 0.90f, 0.04f);

    // Blade 4 (75 deg)
    glNormal3f(0.95f, 0.3f, 0.25f);
    glColor4f(0.048f, 0.055f, 0.044f, 1.0f);
    glVertex3f(-hw * 0.10f, 0.0f, -hw * 0.32f);
    glVertex3f( hw * 0.10f, 0.0f,  hw * 0.32f);
    glColor4f(0.112f, 0.128f, 0.096f, 1.0f);
    glVertex3f(-0.02f, height * 0.94f, 0.02f);

    // Blade 5 (100 deg)
    glNormal3f(0.95f, 0.3f, -0.25f);
    glColor4f(0.046f, 0.052f, 0.042f, 1.0f);
    glVertex3f( hw * 0.10f, 0.0f, -hw * 0.32f);
    glVertex3f(-hw * 0.10f, 0.0f,  hw * 0.32f);
    glColor4f(0.098f, 0.112f, 0.086f, 1.0f);
    glVertex3f(-0.04f, height * 0.88f, -0.03f);

    // Blade 6 (125 deg)
    glNormal3f(0.75f, 0.3f, -0.6f);
    glColor4f(0.045f, 0.050f, 0.040f, 1.0f);
    glVertex3f( hw * 0.22f, 0.0f, -hw * 0.26f);
    glVertex3f(-hw * 0.22f, 0.0f,  hw * 0.26f);
    glColor4f(0.102f, 0.118f, 0.090f, 1.0f);
    glVertex3f(-0.06f, height * 0.92f, -0.02f);

    // Blade 7 (150 deg)
    glNormal3f(0.4f, 0.3f, -0.85f);
    glColor4f(0.048f, 0.054f, 0.044f, 1.0f);
    glVertex3f( hw * 0.30f, 0.0f, -hw * 0.15f);
    glVertex3f(-hw * 0.30f, 0.0f,  hw * 0.15f);
    glColor4f(0.108f, 0.124f, 0.094f, 1.0f);
    glVertex3f(-0.04f, height * 0.95f, 0.03f);

    // Blade 8 (175 deg)
    glNormal3f(-0.1f, 0.3f, -0.95f);
    glColor4f(0.045f, 0.050f, 0.040f, 1.0f);
    glVertex3f( hw * 0.35f, 0.0f, 0.0f);
    glVertex3f(-hw * 0.35f, 0.0f, 0.0f);
    glColor4f(0.095f, 0.110f, 0.084f, 1.0f);
    glVertex3f(0.01f, height * 0.87f, -0.04f);

    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// Fallen decayed autumnal leaves on the ground
void drawFallenLeaf(float x, float z, float size, float rotY, float pitch, float r, float g, float b) {    float y = getTerrainHeight(x, z) + 0.012f;
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(pitch, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_FALLEN_LEAF);
    bindTexture(TEX_NONE);
    glColor4f(r, g, b, 1.0f);

    float hs = size * 0.5f;
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.15f);
    glVertex3f(0.0f, 0.008f, 0.0f);
    glVertex3f(-hs, 0.0f, -hs * 0.6f);
    glVertex3f(0.0f, 0.004f, -hs * 1.2f);
    glVertex3f(hs, 0.0f, -hs * 0.6f);
    glVertex3f(hs * 0.5f, 0.004f, hs * 0.8f);
    glVertex3f(-hs * 0.5f, 0.004f, hs * 0.8f);
    glVertex3f(-hs, 0.0f, -hs * 0.6f);
    glEnd();

    glPopMatrix();
}



void drawPebble(float x, float z, float scaleX, float scaleY, float scaleZ, float rotY) {    float y = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, y + scaleY * 0.35f, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glScalef(scaleX, scaleY, scaleZ);

    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    drawSphere(1.0f, 6, 5, 0.5f, 0.5f);
    glPopMatrix();
}

// ----------------------------------------------------------------------------
// IRREGULAR MOSSY ROCKS & ENVIRONMENTAL ABANDONED CLUTTER
// ----------------------------------------------------------------------------

// Irregular Low-Poly Boulder with Deterministic Vertex Perturbation & Moss-Green Top Tint
void drawIrregularRock(float x, float z, float rx, float ry, float rz, float rotY, float rotX, unsigned int seed, float mossFactor) {    float groundY = getTerrainHeight(x, z);
    // Partially sunk into terrain
    float y = groundY - ry * 0.30f;

    TreeRNG rng(seed);

    int stacks = 8;
    int slices = 12;

    struct RockVert {
        float x, y, z;
        float nx, ny, nz;
        float moss;
    };
    std::vector<RockVert> verts((stacks + 1) * (slices + 1));

    for (int i = 0; i <= stacks; ++i) {
        float phi = (float)M_PI * (-0.5f + (float)i / stacks);
        float cosPhi = std::cos(phi);
        float sinPhi = std::sin(phi);

        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * (float)M_PI * (float)j / slices;
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            // Natural organic perturbation per vertex
            float perturb = 1.0f + rng.nextFloat(-0.24f, 0.24f);

            float vx = rx * cosPhi * cosTheta * perturb;
            float vy = ry * sinPhi * perturb;
            float vz = rz * cosPhi * sinTheta * perturb;

            int idx = i * (slices + 1) + j;
            verts[idx].x = vx;
            verts[idx].y = vy;
            verts[idx].z = vz;

            // Approximate vertex normal
            float nx = vx / (rx * rx);
            float ny = vy / (ry * ry);
            float nz = vz / (rz * rz);
            float nlen = std::sqrt(nx*nx + ny*ny + nz*nz);
            if (nlen > 0.001f) { nx /= nlen; ny /= nlen; nz /= nlen; }
            verts[idx].nx = nx;
            verts[idx].ny = ny;
            verts[idx].nz = nz;

            // Moss tint calculation on upward-facing surfaces (ny > 0.15)
            float upMoss = (ny > 0.15f) ? ((ny - 0.15f) / 0.85f * mossFactor) : 0.0f;
            verts[idx].moss = upMoss;
        }
    }

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);

    glBegin(GL_QUADS);
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int idx00 = i * (slices + 1) + j;
            int idx10 = (i + 1) * (slices + 1) + j;
            int idx11 = (i + 1) * (slices + 1) + (j + 1);
            int idx01 = i * (slices + 1) + (j + 1);

            // Facet normal for crisp, rugged, craggy stone edges
            float edge1X = verts[idx10].x - verts[idx00].x;
            float edge1Y = verts[idx10].y - verts[idx00].y;
            float edge1Z = verts[idx10].z - verts[idx00].z;

            float edge2X = verts[idx01].x - verts[idx00].x;
            float edge2Y = verts[idx01].y - verts[idx00].y;
            float edge2Z = verts[idx01].z - verts[idx00].z;

            float fnx = edge1Y * edge2Z - edge1Z * edge2Y;
            float fny = edge1Z * edge2X - edge1X * edge2Z;
            float fnz = edge1X * edge2Y - edge1Y * edge2X;
            float fnlen = std::sqrt(fnx*fnx + fny*fny + fnz*fnz);
            if (fnlen > 0.0001f) { fnx /= fnlen; fny /= fnlen; fnz /= fnlen; }

            glNormal3f(fnx, fny, fnz);

            auto emitVert = [&](int idx, float u, float v) {
                float m = verts[idx].moss;
                // Moss-green tint multiplier on upper faces
                float r = 1.0f - 0.35f * m;
                float g = 1.0f + 0.18f * m;
                float b = 1.0f - 0.45f * m;
                glColor4f(r, g, b, 1.0f);
                glTexCoord2f(u, v);
                glVertex3f(verts[idx].x, verts[idx].y, verts[idx].z);
            };

            emitVert(idx00, (float)j / slices * 2.0f, (float)i / stacks * 2.0f);
            emitVert(idx10, (float)j / slices * 2.0f, (float)(i + 1) / stacks * 2.0f);
            emitVert(idx11, (float)(j + 1) / slices * 2.0f, (float)(i + 1) / stacks * 2.0f);
            emitVert(idx01, (float)(j + 1) / slices * 2.0f, (float)i / stacks * 2.0f);
        }
    }
    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// Broken Wooden Crate with Missing / Cracked Planks & Spilling Boards
void drawBrokenCrate(float x, float z, float rotY, float tilt) {    float groundY = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, groundY + 0.38f, z); // Embedded slightly into dirt
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(tilt, 0.0f, 0.0f, 1.0f);

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);

    float cw = 1.05f, ch = 0.85f, cd = 0.85f;
    float hw = cw * 0.5f, hh = ch * 0.5f, hd = cd * 0.5f;

    // 4 Vertical corner posts
    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            glPushMatrix();
            glTranslatef(i * (hw - 0.04f), 0.0f, j * (hd - 0.04f));
            drawBox(0.08f, ch, 0.08f, 0.3f, 1.0f);
            glPopMatrix();
        }
    }

    // Bottom floor slats
    for (int i = -2; i <= 2; ++i) {
        glPushMatrix();
        glTranslatef(i * 0.20f, -hh + 0.02f, 0.0f);
        drawBox(0.16f, 0.04f, cd - 0.08f, 0.5f, 1.0f);
        glPopMatrix();
    }

    // Back side planks (+Z)
    for (int i = -1; i <= 1; ++i) {
        glPushMatrix();
        glTranslatef(0.0f, i * 0.26f, hd - 0.02f);
        drawBox(cw, 0.20f, 0.04f, 1.0f, 0.4f);
        glPopMatrix();
    }

    // Left side planks (-X)
    for (int i = -1; i <= 1; ++i) {
        glPushMatrix();
        glTranslatef(-hw + 0.02f, i * 0.26f, 0.0f);
        drawBox(0.04f, 0.20f, cd, 0.4f, 1.0f);
        glPopMatrix();
    }

    // Right side planks (+X)
    for (int i = -1; i <= 1; ++i) {
        glPushMatrix();
        glTranslatef(hw - 0.02f, i * 0.26f, 0.0f);
        drawBox(0.04f, 0.20f, cd, 0.4f, 1.0f);
        glPopMatrix();
    }

    // Broken Front side (-Z): bottom plank intact, middle missing, top tilted cracked!
    glPushMatrix();
    glTranslatef(0.0f, -0.26f, -hd + 0.02f);
    drawBox(cw, 0.20f, 0.04f, 1.0f, 0.4f); // Bottom plank
    glPopMatrix();

    // Smashed loose plank dangling out of the front
    glPushMatrix();
    glTranslatef(0.15f, 0.08f, -hd - 0.15f);
    glRotatef(28.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-18.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.75f, 0.18f, 0.04f, 1.0f, 0.3f);
    glPopMatrix();

    // Loose broken board sticking out of the top
    glPushMatrix();
    glTranslatef(-0.10f, hh + 0.12f, 0.05f);
    glRotatef(-35.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(22.0f, 1.0f, 0.0f, 0.0f);
    drawBox(0.85f, 0.04f, 0.18f, 1.0f, 0.3f);
    glPopMatrix();

    glPopMatrix();
}

// Old Wooden Barrel with Rusty Iron Hoops
void drawOldBarrel(float x, float z, float rotY, float tilt) {    float groundY = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, groundY + 0.48f, z); // Embedded base into soil
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(tilt, 1.0f, 0.0f, 0.0f);

    float rEnd = 0.44f;
    float rMid = 0.54f;
    float h = 1.05f;

    // Wooden Staved Body (two tapered halves meeting at bulging belly)
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);

    glPushMatrix();
    glTranslatef(0.0f, -h * 0.5f, 0.0f);
    drawCylinder(rEnd, rMid, h * 0.5f, 16, 2.0f, 1.0f); // Lower half
    glTranslatef(0.0f, h * 0.5f, 0.0f);
    drawCylinder(rMid, rEnd, h * 0.5f, 16, 2.0f, 1.0f); // Upper half
    glPopMatrix();

    // Top & Bottom Recessed Lid Caps
    glPushMatrix();
    glTranslatef(0.0f, h * 0.5f - 0.02f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    drawCylinder(rEnd * 0.95f, 0.01f, 0.02f, 14, 1.0f, 1.0f);
    glPopMatrix();

    // 3 Rusty Metal Bands / Hoops around the barrel
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);

    float hoopPositions[3] = { -h * 0.35f, 0.0f, h * 0.35f };
    float hoopRadii[3]     = { rEnd * 1.08f, rMid * 1.03f, rEnd * 1.08f };

    for (int i = 0; i < 3; ++i) {
        glPushMatrix();
        glTranslatef(0.0f, hoopPositions[i] - 0.025f, 0.0f);
        drawCylinder(hoopRadii[i], hoopRadii[i], 0.05f, 16, 1.0f, 0.2f);
        glPopMatrix();
    }

    glPopMatrix();
}

// Fallen Fence Planks / Pointed Pickets Lying in the Mud
void drawFallenPlank(float x, float z, float rotY, float pitch) {    float y = getTerrainHeight(x, z) + 0.015f;
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(pitch, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);

    drawBox(0.14f, 0.04f, 1.55f, 0.3f, 2.0f);
    // Pointed picket top
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.85f);
    drawPrismRoof(0.14f, 0.16f, 0.04f, 0.3f, 0.3f);
    glPopMatrix();

    glPopMatrix();
}

// Rusty Metal Bucket with Arched Wire Handle
void drawRustyBucket(float x, float z, float rotY, float tilt) {    float y = getTerrainHeight(x, z) + 0.05f;
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(tilt, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);

    float rBase = 0.15f;
    float rTop  = 0.22f;
    float h     = 0.36f;

    // Tapered bucket body
    drawCylinder(rBase, rTop, h, 14, 1.0f, 1.0f);

    // Bottom rim & top rolled rim
    glPushMatrix();
    glTranslatef(0.0f, h - 0.01f, 0.0f);
    drawCylinder(rTop * 1.04f, rTop * 1.04f, 0.025f, 14, 1.0f, 0.2f);
    glPopMatrix();

    // Arched Wire Handle (Bail) draped over the bucket
    bindTexture(TEX_NONE);
    glLineWidth(2.5f);
    glColor3f(0.35f, 0.25f, 0.20f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 12; ++i) {
        float a = (float)i * (float)M_PI / 12.0f;
        float hx = (rTop + 0.02f) * std::cos(a);
        float hy = h + 0.22f * std::sin(a);
        glVertex3f(hx, hy, 0.0f);
    }
    glEnd();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glPopMatrix();
}

// Scattered Clay Bricks around the house foundation & corners
void drawSingleBrick(float x, float z, float rotY, float pitch) {    float y = getTerrainHeight(x, z) + 0.035f;
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(pitch, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_CLAY_BRICK);
    bindTexture(TEX_STONE); // subtle rough stone/brick texture

    drawBox(0.25f, 0.07f, 0.12f, 0.5f, 0.3f);
    glPopMatrix();
}

void drawScatteredBricks() {    // Cluster 1: Near cracked foundation wall corner
    drawSingleBrick( 7.5f, 3.8f,  18.0f);
    drawSingleBrick( 7.9f, 3.2f, -42.0f,  6.0f);
    drawSingleBrick( 7.2f, 4.3f,  65.0f);
    drawSingleBrick( 8.3f, 4.0f, -10.0f, 12.0f);
    drawSingleBrick( 7.8f, 4.8f,  30.0f);

    // Cluster 2: Near porch steps / foundation
    drawSingleBrick(-5.2f, 7.8f,  33.0f);
    drawSingleBrick(-5.6f, 7.2f, -25.0f,  8.0f);
    drawSingleBrick(-4.9f, 8.4f,  70.0f);

    // Cluster 3: Near broken crate
    drawSingleBrick(-6.8f, 10.8f, -15.0f);
    drawSingleBrick(-5.8f, 12.2f,  40.0f, 10.0f);
}

// Tangled Bare Dead Bush / Bramble Shrub (with wind sway)
void drawDeadBush(float x, float z, float scale, float rotY, unsigned int seed) {    float groundY = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, groundY, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // Subtle gentle night wind sway
    float bushWind = std::sin(g_time * 1.25f + x * 0.3f + z * 0.2f) * 2.5f;
    glRotatef(bushWind, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_BARK);
    bindTexture(TEX_BARK);

    TreeRNG rng(seed);
    int numStems = 7;
    for (int i = 0; i < numStems; ++i) {
        float azimuth = (float)i * (360.0f / numStems) + rng.nextFloat(-20.0f, 20.0f);
        float outAngle = rng.nextFloat(35.0f, 65.0f);
        float stemLen = scale * rng.nextFloat(0.70f, 1.15f);

        glPushMatrix();
        glRotatef(azimuth, 0.0f, 1.0f, 0.0f);
        glRotatef(outAngle, 1.0f, 0.0f, 0.0f);

        // Lower stem
        drawCylinder(0.045f * scale, 0.025f * scale, stemLen * 0.5f, 5, 1.0f, 1.0f);
        glTranslatef(0.0f, stemLen * 0.5f, 0.0f);

        // Crooked bend
        glRotatef(rng.nextFloat(-25.0f, 25.0f), 1.0f, 0.0f, 0.0f);
        glRotatef(rng.nextFloat(-25.0f, 25.0f), 0.0f, 0.0f, 1.0f);
        drawCylinder(0.025f * scale, 0.010f * scale, stemLen * 0.5f, 4, 1.0f, 1.0f);
        glTranslatef(0.0f, stemLen * 0.5f, 0.0f);

        // Sub-twigs
        for (int t = 0; t < 2; ++t) {
            glPushMatrix();
            glRotatef(rng.nextFloat(30.0f, 60.0f), 1.0f, 0.0f, 0.0f);
            glRotatef((float)t * 180.0f + rng.nextFloat(-20.0f, 20.0f), 0.0f, 1.0f, 0.0f);
            drawCylinder(0.012f * scale, 0.004f * scale, stemLen * 0.40f, 4);
            glPopMatrix();
        }

        glPopMatrix();
    }

    glPopMatrix();
}

// Leaning Old Wrought-Iron Lamppost with Faint Glowing Lantern
void drawLeaningLamppost(float x, float z, float rotY, float leanAngle) {    float groundY = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, groundY, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(leanAngle, 0.0f, 0.0f, 1.0f); // Haunting historic soil lean

    // Stepped Pedestal Base Plinth (deepened to stay grounded under soil lean)
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);

    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.0f);
    drawBox(0.52f, 0.38f, 0.52f); // Deepened bottom step (penetrates terrain)
    glTranslatef(0.0f, 0.25f, 0.0f);
    drawBox(0.40f, 0.16f, 0.40f); // Middle tier
    glTranslatef(0.0f, 0.14f, 0.0f);
    drawCylinder(0.18f, 0.12f, 0.16f, 10, 1.0f, 0.5f); // Base collar
    glPopMatrix();

    // Fluted Tapered Iron Shaft (3.2m tall)
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.0f);
    drawCylinder(0.10f, 0.065f, 3.1f, 10, 1.0f, 3.0f);

    // Mid-shaft decorative ring collar
    glTranslatef(0.0f, 1.8f, 0.0f);
    drawCylinder(0.09f, 0.09f, 0.06f, 10, 1.0f, 0.2f);

    // Top capital header
    glTranslatef(0.0f, 1.3f, 0.0f);
    drawBox(0.18f, 0.08f, 0.18f);
    glPopMatrix();

    // Decorative Scrollwork Curved Bracket Arm holding the lantern
    glPushMatrix();
    glTranslatef(0.0f, 3.85f, 0.0f);

    // Horizontal bracket arm
    glPushMatrix();
    glTranslatef(0.35f, 0.0f, 0.0f);
    drawBox(0.70f, 0.05f, 0.05f);
    glPopMatrix();

    // Curved lower support strut
    glPushMatrix();
    glTranslatef(0.20f, -0.22f, 0.0f);
    glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.45f, 0.04f, 0.04f);
    glPopMatrix();

    // Lantern Drop Mount
    glTranslatef(0.65f, -0.15f, 0.0f);

    // Antique 4-Sided Carriage Lantern Housing
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);

    // Top roof cap
    glPushMatrix();
    glTranslatef(0.0f, 0.28f, 0.0f);
    drawPrismRoof(0.38f, 0.18f, 0.38f, 0.5f, 0.5f);
    glTranslatef(0.0f, 0.20f, 0.0f);
    drawSphere(0.04f, 8, 6); // Top finial
    glPopMatrix();

    // Bottom finial drop
    glPushMatrix();
    glTranslatef(0.0f, -0.32f, 0.0f);
    drawCylinder(0.06f, 0.01f, 0.12f, 8);
    glPopMatrix();

    // Glass Lantern Panes
    applyMaterial(MAT_CAR_GLASS);
    bindTexture(TEX_NONE);
    glPushMatrix();
    drawBox(0.30f, 0.46f, 0.30f);
    glPopMatrix();

    // Decayed dark filament inside lantern (no bright glare on player's side)
    applyMaterial(MAT_DARK_WOOD);
    glPushMatrix();
    drawSphere(0.05f, 8, 6);
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();
}

// ----------------------------------------------------------------------------
// OLD FURNITURE (Broken Chair & Table - Reference Panel 2)
// ----------------------------------------------------------------------------
void drawBrokenChair(float x, float z, float rotY, float tilt) {    float gy = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, gy, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    if (tilt != 0.0f) {
        glRotatef(tilt, 0.0f, 0.0f, 1.0f);
    }

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.32f, 0.28f, 0.24f, 1.0f);

    // Seat plank
    glPushMatrix();
    glTranslatef(0.0f, 0.45f, 0.0f);
    drawBox(0.48f, 0.04f, 0.48f);
    glPopMatrix();

    // 4 Legs (Front-left leg is broken shorter)
    glPushMatrix();
    glTranslatef(-0.19f, 0.225f, -0.19f); drawBox(0.045f, 0.45f, 0.045f); // Back-left
    glTranslatef( 0.38f, 0.0f,   0.0f);   drawBox(0.045f, 0.45f, 0.045f); // Back-right
    glTranslatef( 0.0f,  0.0f,   0.38f);  drawBox(0.045f, 0.45f, 0.045f); // Front-right
    glTranslatef(-0.38f, 0.10f,  0.0f);   drawBox(0.045f, 0.25f, 0.045f); // Front-left (broken short)
    glPopMatrix();

    // Backrest vertical posts
    glPushMatrix();
    glTranslatef(-0.19f, 0.72f, -0.19f); drawBox(0.045f, 0.52f, 0.045f);
    glTranslatef( 0.38f, 0.0f,   0.0f);   drawBox(0.045f, 0.52f, 0.045f);
    // Backrest top rail
    glTranslatef(-0.19f, 0.24f,  0.0f);   drawBox(0.46f, 0.06f, 0.04f);
    // Backrest middle horizontal slat
    glTranslatef( 0.0f, -0.16f,  0.0f);   drawBox(0.42f, 0.05f, 0.035f);
    // Vertical center spindle (crooked/broken)
    glTranslatef(-0.06f, -0.14f, 0.0f);   glRotatef(12.0f, 0.0f, 0.0f, 1.0f); drawBox(0.03f, 0.24f, 0.03f);
    glPopMatrix();

    glPopMatrix();
}

void drawWoodenTable(float x, float z, float rotY, float tilt) {    float gy = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, gy, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    if (tilt != 0.0f) {
        glRotatef(tilt, 1.0f, 0.0f, 0.0f);
    }

    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.30f, 0.26f, 0.22f, 1.0f);

    // Tabletop slab
    glPushMatrix();
    glTranslatef(0.0f, 0.72f, 0.0f);
    drawBox(1.10f, 0.05f, 0.65f);
    // Apron frame under top
    glTranslatef(0.0f, -0.04f, 0.0f);
    drawBox(1.02f, 0.07f, 0.58f);
    glPopMatrix();

    // 4 Sturdy Legs
    glPushMatrix();
    glTranslatef(-0.48f, 0.35f, -0.26f); drawBox(0.06f, 0.70f, 0.06f);
    glTranslatef( 0.96f, 0.0f,   0.0f);   drawBox(0.06f, 0.70f, 0.06f);
    glTranslatef( 0.0f,  0.0f,   0.52f);  drawBox(0.06f, 0.70f, 0.06f);
    glTranslatef(-0.96f, 0.0f,   0.0f);   drawBox(0.06f, 0.70f, 0.06f);
    glPopMatrix();

    glPopMatrix();
}

void drawOldFurniture() {    // 1. On the Porch (Under the roof near the warm glowing bulb)
    drawWoodenTable(-2.2f + g_houseShiftX, 4.2f + g_houseShiftZ, -12.0f);
    drawBrokenChair(-3.3f + g_houseShiftX, 4.0f + g_houseShiftZ,  22.0f, 4.0f);
    drawBrokenChair(-1.3f + g_houseShiftX, 4.2f + g_houseShiftZ, -60.0f, 0.0f);

    // 2. Inside the House (Visible through open doorway and windows)
    drawWoodenTable(-5.5f + g_houseShiftX, 1.5f + g_houseShiftZ,  15.0f);
    drawBrokenChair(-4.5f + g_houseShiftX, 1.8f + g_houseShiftZ,  45.0f);
    drawBrokenChair(-6.8f + g_houseShiftX, 1.2f + g_houseShiftZ, -30.0f);
}

// ----------------------------------------------------------------------------
// 3D CARVED LETTERING HELPER (For Rustic Signboards & Inscriptions)
// ----------------------------------------------------------------------------
static void drawLetterStroke2D(float x1, float y1, float x2, float y2, float thick, float depth) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.0001f) return;
    float nx = -dy / len * (thick * 0.5f);
    float ny =  dx / len * (thick * 0.5f);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(x1 - nx, y1 - ny, depth);
    glVertex3f(x2 - nx, y2 - ny, depth);
    glVertex3f(x2 + nx, y2 + ny, depth);
    glVertex3f(x1 + nx, y1 + ny, depth);
    glEnd();
}

static void drawSignChar(char c, float cx, float cy, float w, float h, float thick, float depth) {
    float hw = w * 0.5f;
    float hh = h * 0.5f;
    switch (c) {
        case 'A':
            drawLetterStroke2D(cx - hw, cy - hh, cx, cy + hh, thick, depth);
            drawLetterStroke2D(cx, cy + hh, cx + hw, cy - hh, thick, depth);
            drawLetterStroke2D(cx - hw * 0.55f, cy - hh * 0.15f, cx + hw * 0.55f, cy - hh * 0.15f, thick, depth);
            break;
        case 'B':
            drawLetterStroke2D(cx - hw, cy - hh, cx - hw, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh, cx + hw * 0.5f, cy + hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.5f, cy + hh, cx + hw, cy + hh * 0.5f, thick, depth);
            drawLetterStroke2D(cx + hw, cy + hh * 0.5f, cx + hw * 0.5f, cy, thick, depth);
            drawLetterStroke2D(cx - hw, cy, cx + hw * 0.5f, cy, thick, depth);
            drawLetterStroke2D(cx + hw * 0.5f, cy, cx + hw, cy - hh * 0.5f, thick, depth);
            drawLetterStroke2D(cx + hw, cy - hh * 0.5f, cx + hw * 0.5f, cy - hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.5f, cy - hh, cx - hw, cy - hh, thick, depth);
            break;
        case 'N':
            drawLetterStroke2D(cx - hw, cy - hh, cx - hw, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh, cx + hw, cy - hh, thick, depth);
            drawLetterStroke2D(cx + hw, cy - hh, cx + hw, cy + hh, thick, depth);
            break;
        case 'D':
            drawLetterStroke2D(cx - hw, cy - hh, cx - hw, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh, cx + hw * 0.35f, cy + hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.35f, cy + hh, cx + hw, cy, thick, depth);
            drawLetterStroke2D(cx + hw, cy, cx + hw * 0.35f, cy - hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.35f, cy - hh, cx - hw, cy - hh, thick, depth);
            break;
        case 'O':
            drawLetterStroke2D(cx - hw * 0.4f, cy + hh, cx + hw * 0.4f, cy + hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.4f, cy + hh, cx + hw, cy + hh * 0.4f, thick, depth);
            drawLetterStroke2D(cx + hw, cy + hh * 0.4f, cx + hw, cy - hh * 0.4f, thick, depth);
            drawLetterStroke2D(cx + hw, cy - hh * 0.4f, cx + hw * 0.4f, cy - hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.4f, cy - hh, cx - hw * 0.4f, cy - hh, thick, depth);
            drawLetterStroke2D(cx - hw * 0.4f, cy - hh, cx - hw, cy - hh * 0.4f, thick, depth);
            drawLetterStroke2D(cx - hw, cy - hh * 0.4f, cx - hw, cy + hh * 0.4f, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh * 0.4f, cx - hw * 0.4f, cy + hh, thick, depth);
            break;
        case 'E':
            drawLetterStroke2D(cx - hw, cy - hh, cx - hw, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh, cx + hw * 0.85f, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh * 0.05f, cx + hw * 0.6f, cy + hh * 0.05f, thick, depth);
            drawLetterStroke2D(cx - hw, cy - hh, cx + hw * 0.85f, cy - hh, thick, depth);
            break;
        case 'H':
            drawLetterStroke2D(cx - hw, cy - hh, cx - hw, cy + hh, thick, depth);
            drawLetterStroke2D(cx + hw, cy - hh, cx + hw, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw, cy, cx + hw, cy, thick, depth);
            break;
        case 'U':
            drawLetterStroke2D(cx - hw, cy + hh, cx - hw, cy - hh * 0.4f, thick, depth);
            drawLetterStroke2D(cx - hw, cy - hh * 0.4f, cx - hw * 0.4f, cy - hh, thick, depth);
            drawLetterStroke2D(cx - hw * 0.4f, cy - hh, cx + hw * 0.4f, cy - hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.4f, cy - hh, cx + hw, cy - hh * 0.4f, thick, depth);
            drawLetterStroke2D(cx + hw, cy - hh * 0.4f, cx + hw, cy + hh, thick, depth);
            break;
        case 'S':
            drawLetterStroke2D(cx + hw * 0.8f, cy + hh, cx - hw * 0.4f, cy + hh, thick, depth);
            drawLetterStroke2D(cx - hw * 0.4f, cy + hh, cx - hw, cy + hh * 0.6f, thick, depth);
            drawLetterStroke2D(cx - hw, cy + hh * 0.6f, cx - hw * 0.4f, cy + hh * 0.1f, thick, depth);
            drawLetterStroke2D(cx - hw * 0.4f, cy + hh * 0.1f, cx + hw * 0.4f, cy - hh * 0.1f, thick, depth);
            drawLetterStroke2D(cx + hw * 0.4f, cy - hh * 0.1f, cx + hw, cy - hh * 0.6f, thick, depth);
            drawLetterStroke2D(cx + hw, cy - hh * 0.6f, cx + hw * 0.4f, cy - hh, thick, depth);
            drawLetterStroke2D(cx + hw * 0.4f, cy - hh, cx - hw * 0.8f, cy - hh, thick, depth);
            break;
    }
}

// ----------------------------------------------------------------------------
// SIGN BOARD ("ABANDONED HOUSE" - Image 17 & 16)
// ----------------------------------------------------------------------------
void drawSignBoard(float x, float z, float rotY) {    float gy = getTerrainHeight(x, z);
    glPushMatrix();
    glTranslatef(x, gy, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(-2.8f, 0.0f, 0.0f, 1.0f); // Slight weathered crooked tilt

    // 1. Vertical main wooden post with beveled top
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glColor4f(0.24f, 0.19f, 0.14f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 1.15f, 0.0f);
    drawBox(0.18f, 2.30f, 0.18f, 0.5f, 2.0f);
    // Pyramid weather cap on post top
    glTranslatef(0.0f, 1.15f, 0.0f);
    drawPrismRoof(0.20f, 0.12f, 0.20f, 0.5f, 0.5f);
    glPopMatrix();

    // 2. Diagonal wooden support brace struts underneath the signboard
    glPushMatrix();
    glTranslatef(-0.42f, 1.22f, 0.0f);
    glRotatef(-42.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.09f, 0.75f, 0.09f, 0.5f, 1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.42f, 1.22f, 0.0f);
    glRotatef(42.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.09f, 0.75f, 0.09f, 0.5f, 1.0f);
    glPopMatrix();

    // 3. Wooden Sign Board Assembly (3 weathered horizontal planks with chipped irregular ends)
    glPushMatrix();
    glTranslatef(0.0f, 1.82f, 0.12f);

    // Board Plank 1 (Top Plank)
    glPushMatrix();
    glTranslatef(-0.02f, 0.22f, 0.0f);
    drawBox(1.92f, 0.22f, 0.065f, 2.0f, 0.5f);
    glPopMatrix();

    // Board Plank 2 (Middle Plank - slightly offset and chipped)
    glPushMatrix();
    glTranslatef(0.03f, 0.0f, 0.0f);
    drawBox(1.96f, 0.21f, 0.065f, 2.0f, 0.5f);
    glPopMatrix();

    // Board Plank 3 (Bottom Plank)
    glPushMatrix();
    glTranslatef(-0.01f, -0.22f, 0.0f);
    drawBox(1.88f, 0.22f, 0.065f, 2.0f, 0.5f);
    glPopMatrix();

    // Weathered edge chips & notches on the board silhouette
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glPushMatrix();
    glTranslatef(-0.96f, 0.12f, 0.0f);
    glRotatef(25.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.08f, 0.14f, 0.07f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.97f, -0.15f, 0.0f);
    glRotatef(-30.0f, 0.0f, 0.0f, 1.0f);
    drawBox(0.08f, 0.12f, 0.07f);
    glPopMatrix();

    // 4. Heavy Rustic Iron Corner Brackets & Bolt Caps (Image 17)
    applyMaterial(MAT_RUSTY_METAL);
    bindTexture(TEX_RUST);
    float bracketPositions[4][2] = {
        { -0.85f,  0.26f },
        {  0.85f,  0.26f },
        { -0.85f, -0.26f },
        {  0.85f, -0.26f }
    };
    for (int b = 0; b < 4; ++b) {
        glPushMatrix();
        glTranslatef(bracketPositions[b][0], bracketPositions[b][1], 0.038f);
        drawBox(0.08f, 0.08f, 0.015f); // Square washer bracket
        glTranslatef(0.0f, 0.0f, 0.010f);
        drawCylinder(0.022f, 0.022f, 0.025f, 6); // Iron bolt head
        glPopMatrix();
    }

    // 5. Inscribed Dark Weathered Typography: "ABANDONED" (Top Line) & "HOUSE" (Bottom Line)
    applyMaterial(MAT_BLACK_IRON);
    bindTexture(TEX_NONE);
    glColor4f(0.08f, 0.06f, 0.05f, 1.0f); // Dark scorched / carved pigment

    // Line 1: "ABANDONED"
    const char* line1 = "ABANDONED";
    float l1StartX = -0.74f;
    float l1Spacing = 0.185f;
    float l1CharW   = 0.125f;
    float l1CharH   = 0.155f;
    float l1StrokeT = 0.022f;
    float l1Y       = 0.11f;
    for (int c = 0; c < 9; ++c) {
        drawSignChar(line1[c], l1StartX + c * l1Spacing, l1Y, l1CharW, l1CharH, l1StrokeT, 0.038f);
    }

    // Line 2: "HOUSE" (Centered, slightly larger)
    const char* line2 = "HOUSE";
    float l2StartX = -0.46f;
    float l2Spacing = 0.23f;
    float l2CharW   = 0.155f;
    float l2CharH   = 0.18f;
    float l2StrokeT = 0.026f;
    float l2Y       = -0.12f;
    for (int c = 0; c < 5; ++c) {
        drawSignChar(line2[c], l2StartX + c * l2Spacing, l2Y, l2CharW, l2CharH, l2StrokeT, 0.038f);
    }

    glPopMatrix(); // End board assembly

    // 6. Scattered base rocks & splinters at post foot
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);
    glPushMatrix();
    glTranslatef(0.18f, 0.08f, 0.14f);
    drawBox(0.24f, 0.14f, 0.20f);
    glTranslatef(-0.35f, 0.0f, -0.10f);
    drawBox(0.18f, 0.10f, 0.16f);
    glPopMatrix();

    glPopMatrix();
}

// ----------------------------------------------------------------------------
// UTILITY / ELECTRIC TELEPHONE POLE WITH SAGGING CABLES (Image 16)


void drawScatteredRocks() {    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);

    struct RockData {
        float x, z;
        float rx, ry, rz;
        float rotY, rotX;
        unsigned int seed;
    };
    static const RockData ROCKS[10] = {
        { -2.8f, 17.5f, 0.38f, 0.28f, 0.35f,  25.0f, 12.0f, 501 }, // Left path entrance
        {  3.2f, 17.0f, 0.42f, 0.32f, 0.38f, -40.0f,  8.0f, 502 }, // Right path entrance
        { -5.5f, 13.5f, 0.65f, 0.48f, 0.58f,  15.0f, 18.0f, 503 }, // Left knoll boulder
        {  6.2f, 13.8f, 0.55f, 0.40f, 0.50f,  65.0f, -10.0f, 504 }, // Right midground
        { -1.8f,  9.5f, 0.30f, 0.22f, 0.28f, -20.0f,  5.0f, 505 }, // Along path
        {  2.5f,  8.5f, 0.35f, 0.26f, 0.32f,  30.0f, 14.0f, 506 }, // Along path
        {  6.5f,  6.8f, 0.52f, 0.38f, 0.48f, -50.0f,  9.0f, 507 }, // Beside car
        {  9.8f,  5.2f, 0.44f, 0.30f, 0.40f,  10.0f, 12.0f, 508 }, // Behind car
        { -7.8f,  6.0f, 0.48f, 0.35f, 0.42f,  45.0f, -8.0f, 509 }, // Near porch
        { -12.0f, 9.0f, 0.60f, 0.45f, 0.55f, -30.0f, 15.0f, 510 }  // Left fence
    };

    for (int i = 0; i < 10; ++i) {
        const auto& r = ROCKS[i];
        drawIrregularRock(r.x, r.z, r.rx, r.ry, r.rz, r.rotY, r.rotX, r.seed, 0.55f);
    }
}

// Complete Environmental Clutter & Props Master Function
void drawEnvironmentalClutter() {    // 1. Scattered Low-Poly Rocks
    drawScatteredRocks();

    // 2. Broken Wooden Crates
    drawBrokenCrate(-6.2f, 11.5f, 22.0f, 6.0f);
    drawBrokenCrate(-8.2f + g_houseShiftX, 4.2f + g_houseShiftZ, -35.0f, 4.0f);
    drawBrokenCrate( 9.5f, 6.5f, 45.0f, 5.0f);

    // 3. Old Wooden Barrels
    drawOldBarrel(-4.8f, 5.2f, -15.0f, 0.0f);  // Upright near porch
    drawOldBarrel( 8.8f, 9.2f,  48.0f, 72.0f); // Tilted on side in mud near car
    drawOldBarrel(-7.5f + g_houseShiftX, 4.0f + g_houseShiftZ, 20.0f, 0.0f);

    // 4. Fallen Fence Planks
    drawFallenPlank(-13.5f, 18.0f,  35.0f,  4.0f);
    drawFallenPlank(-13.8f, 10.5f, -50.0f, -3.0f);
    drawFallenPlank(-14.2f,  2.0f,  20.0f,  5.0f);

    // 5. Rusty Metal Bucket
    drawRustyBucket(-3.5f, 8.8f, 30.0f, 24.0f);

    // 6. Scattered Clay Bricks
    drawScatteredBricks();

    // 7. Tangled Bare Dead Bushes
    drawDeadBush(-12.5f, 12.0f, 1.10f,  15.0f, 901); // Near fence
    drawDeadBush(  8.2f, 16.5f, 1.20f, -35.0f, 902); // Near cemetery
    drawDeadBush( -7.5f, 19.5f, 0.90f,  45.0f, 903); // Near road entrance
    drawDeadBush( 12.0f, 11.0f, 1.00f, -60.0f, 904); // Behind car

    // 8. Leaning Old Wrought Iron Lamppost
    drawLeaningLamppost(-3.8f, 19.2f, 25.0f, 11.5f);

    // 9. Sign Board & Telephone Pole & Broken Fence & Furniture
    drawSignBoard(-4.6f, 16.5f, 18.0f);
    drawTelephonePole(10.8f, 5.8f, -12.0f);
    drawBrokenFence();
    drawOldFurniture();
}

// Tasteful 3D Wild Grass Clumps along distant yard boundaries (zero clutter on pathway)
void drawDenseGrassField() {    applyMaterial(MAT_DEAD_GRASS);
    bindTexture(TEX_NONE);

    TreeRNG rng(7721);
    for (int i = 0; i < 320; ++i) {
        float gx = rng.nextFloat(-25.0f, 25.0f);
        float gz = rng.nextFloat(-22.0f, 24.0f);

        // Keep cobblestone walkway clear while flanking path edges with overgrown tufts
        if (gz >= -6.0f && gz <= 25.0f) {
            float roadX = getRoadCenterX(gz);
            float pathHalfW = (3.4f * (1.0f - 0.45f * ((23.5f - gz) / 27.0f))) * 0.5f;
            if (std::abs(gx - roadX) < (pathHalfW + 0.15f)) continue;
        }

        // Avoid house footprint
        if (gx >= -12.0f && gx <= 5.0f && gz >= -15.0f && gz <= -3.0f) continue;

        float gw = rng.nextFloat(0.35f, 0.65f);
        float gh = rng.nextFloat(0.30f, 0.60f);
        float rot = rng.nextFloat(0.0f, 180.0f);

        drawGrassTuft(gx, gz, gw, gh, rot);
    }
}

// Scatter dead grass tufts
void drawGroundProps() {    drawDenseGrassField();
}

// 1. Terrain & Wet Cobblestone Pathway leading to house porch
void drawGround() {    applyMaterial(MAT_WET_GROUND);
    bindTexture(TEX_GROUND);

    int gridSize = 64;
    float halfDim = 65.0f;
    float step = (2.0f * halfDim) / gridSize;
    float tileScale = 0.22f;

    glBegin(GL_QUADS);
    for (int i = 0; i < gridSize; ++i) {
        float z0 = -halfDim + i * step;
        float z1 = z0 + step;
        for (int j = 0; j < gridSize; ++j) {
            float x0 = -halfDim + j * step;
            float x1 = x0 + step;

            // Height and analytical surface normals
            float y00 = getTerrainHeight(x0, z0);
            float y10 = getTerrainHeight(x1, z0);
            float y11 = getTerrainHeight(x1, z1);
            float y01 = getTerrainHeight(x0, z1);

            float nx00, ny00, nz00; getTerrainNormal(x0, z0, nx00, ny00, nz00);
            float nx10, ny10, nz10; getTerrainNormal(x1, z0, nx10, ny10, nz10);
            float nx11, ny11, nz11; getTerrainNormal(x1, z1, nx11, ny11, nz11);
            float nx01, ny01, nz01; getTerrainNormal(x0, z1, nx01, ny01, nz01);

            // Ground Contact Darkening / Analytical Ambient Occlusion & Mud Wetness
            auto calcGroundOcclusion = [](float px, float pz) {
                float ao = 1.0f;

                // House Main Foundation Perimeter AO
                float dxMain = std::max(0.0f, std::max((-9.75f + g_houseShiftX) - px, px - (7.75f + g_houseShiftX)));
                float dzMain = std::max(0.0f, std::max((-7.75f + g_houseShiftZ) - pz, pz - (5.75f + g_houseShiftZ)));
                float dHouse = std::sqrt(dxMain * dxMain + dzMain * dzMain);
                if (dHouse < 2.2f) {
                    float houseAO = 0.48f + 0.52f * std::pow(dHouse / 2.2f, 0.70f);
                    ao = std::min(ao, houseAO);
                }

                // Porch Perimeter AO
                float dxPorch = std::max(0.0f, std::max((-6.2f + g_houseShiftX) - px, px - (0.6f + g_houseShiftX)));
                float dzPorch = std::max(0.0f, std::max((3.2f + g_houseShiftZ) - pz, pz - (7.4f + g_houseShiftZ)));
                float dPorch = std::sqrt(dxPorch * dxPorch + dzPorch * dzPorch);
                if (dPorch < 1.4f) {
                    float porchAO = 0.52f + 0.48f * (dPorch / 1.4f);
                    ao = std::min(ao, porchAO);
                }

                return ao;
            };

            float w00 = calcGroundOcclusion(x0, z0);
            float w10 = calcGroundOcclusion(x1, z0);
            float w11 = calcGroundOcclusion(x1, z1);
            float w01 = calcGroundOcclusion(x0, z1);

            // V00
            glNormal3f(nx00, ny00, nz00);
            glColor4f(w00, w00, w00 * 1.04f, 1.0f);
            glTexCoord2f(x0 * tileScale, z0 * tileScale);
            glVertex3f(x0, y00, z0);

            // V10
            glNormal3f(nx10, ny10, nz10);
            glColor4f(w10, w10, w10 * 1.04f, 1.0f);
            glTexCoord2f(x1 * tileScale, z0 * tileScale);
            glVertex3f(x1, y10, z0);

            // V11
            glNormal3f(nx11, ny11, nz11);
            glColor4f(w11, w11, w11 * 1.04f, 1.0f);
            glTexCoord2f(x1 * tileScale, z1 * tileScale);
            glVertex3f(x1, y11, z1);

            // V01
            glNormal3f(nx01, ny01, nz01);
            glColor4f(w01, w01, w01 * 1.04f, 1.0f);
            glTexCoord2f(x0 * tileScale, z1 * tileScale);
            glVertex3f(x0, y01, z1);
        }
    }
    glEnd();

    // Reset base vertex color
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Old Irregular Wet Cobblestone / Flagstone Pathway (Curving from foreground to mansion porch)
    applyMaterial(MAT_STONE);
    bindTexture(TEX_STONE);

    int numRows = 44;
    TreeRNG pathRng(4411);
    for (int i = 0; i < numRows; ++i) {
        float t = (float)i / (float)(numRows - 1);
        float pz = 23.5f - t * 27.0f; // from 23.5 down to -3.5 (porch steps)
        float roadX = getRoadCenterX(pz);
        float pathW = 3.4f * (1.0f - 0.45f * t); // starts ~3.4m wide, converges to ~1.85m
        int stonesInRow = (i % 2 == 0) ? 3 : 4;
        float stoneW = pathW / (float)stonesInRow;

        for (int s = 0; s < stonesInRow; ++s) {
            float offsetU = -pathW * 0.5f + (s + 0.5f) * stoneW + pathRng.nextFloat(-0.06f, 0.06f);
            float sx = roadX + offsetU;
            float sz = pz + pathRng.nextFloat(-0.07f, 0.07f);
            float sy = getTerrainHeight(sx, sz) + 0.035f + pathRng.nextFloat(0.0f, 0.025f);
            float slen = 0.56f + pathRng.nextFloat(-0.05f, 0.06f);
            float swid = stoneW * 0.90f + pathRng.nextFloat(-0.04f, 0.04f);
            float srot = pathRng.nextFloat(-8.0f, 8.0f) - 14.0f * t;

            glPushMatrix();
            glTranslatef(sx, sy, sz);
            glRotatef(srot, 0.0f, 1.0f, 0.0f);
            drawBeveledBox(swid, 0.06f, slen, 0.025f, 1.0f, 1.0f);
            glPopMatrix();
        }
    }

    // Ground details
    drawGroundProps();
}

