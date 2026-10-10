#include "Primitives.h"
#include <cmath>
#include <algorithm>

void drawBox(float width, float height, float depth, float tileU, float tileV) {    float x = width * 0.5f;
    float y = height * 0.5f;
    float z = depth * 0.5f;

    glBegin(GL_QUADS);
    // Front (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-x, -y,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( x, -y,  z);
    glTexCoord2f(tileU, tileV); glVertex3f( x,  y,  z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-x,  y,  z);

    // Back (-Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( x, -y, -z);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-x, -y, -z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-x,  y, -z);
    glTexCoord2f(tileU, tileV); glVertex3f( x,  y, -z);

    // Top (+Y)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-x,  y,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( x,  y,  z);
    glTexCoord2f(tileU, tileV); glVertex3f( x,  y, -z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-x,  y, -z);

    // Bottom (-Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-x, -y, -z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( x, -y, -z);
    glTexCoord2f(tileU, tileV); glVertex3f( x, -y,  z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-x, -y,  z);

    // Right (+X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( x, -y,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( x, -y, -z);
    glTexCoord2f(tileU, tileV); glVertex3f( x,  y, -z);
    glTexCoord2f(0.0f, tileV);  glVertex3f( x,  y,  z);

    // Left (-X)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-x, -y, -z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-x, -y,  z);
    glTexCoord2f(tileU, tileV); glVertex3f(-x,  y,  z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-x,  y, -z);
    glEnd();
}

void drawBeveledBox(float width, float height, float depth, float bevel, float tileU, float tileV) {    float x = width * 0.5f;
    float y = height * 0.5f;
    float z = depth * 0.5f;
    float b = std::min(bevel, std::min(x * 0.45f, std::min(y * 0.45f, z * 0.45f)));
    if (b <= 0.001f) {
        drawBox(width, height, depth, tileU, tileV);
        return;
    }

    float bx = x - b;
    float by = y - b;
    float bz = z - b;
    float invSqrt2 = 0.70710678f;
    float invSqrt3 = 0.57735027f;

    glBegin(GL_QUADS);
    // 1. Primary 6 Main Faces
    // Front (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx, -by,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx, -by,  z);
    glTexCoord2f(tileU, tileV); glVertex3f( bx,  by,  z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-bx,  by,  z);

    // Back (-Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx, -by, -z);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx, -by, -z);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-bx,  by, -z);
    glTexCoord2f(tileU, tileV); glVertex3f( bx,  by, -z);

    // Top (+Y)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx,  y,  bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx,  y,  bz);
    glTexCoord2f(tileU, tileV); glVertex3f( bx,  y, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-bx,  y, -bz);

    // Bottom (-Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx, -y, -bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx, -y, -bz);
    glTexCoord2f(tileU, tileV); glVertex3f( bx, -y,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-bx, -y,  bz);

    // Right (+X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( x, -by,  bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( x, -by, -bz);
    glTexCoord2f(tileU, tileV); glVertex3f( x,  by, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( x,  by,  bz);

    // Left (-X)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-x, -by, -bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-x, -by,  bz);
    glTexCoord2f(tileU, tileV); glVertex3f(-x,  by,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-x,  by, -bz);

    // 2. 12 Beveled Edge Quads
    // Top-Front (+Y, +Z)
    glNormal3f(0.0f, invSqrt2, invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx,  by,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx,  by,  z);
    glTexCoord2f(tileU, tileV); glVertex3f( bx,   y,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-bx,   y,  bz);

    // Top-Back (+Y, -Z)
    glNormal3f(0.0f, invSqrt2, -invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( bx,  by, -z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-bx,  by, -z);
    glTexCoord2f(tileU, tileV); glVertex3f(-bx,   y, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( bx,   y, -bz);

    // Top-Right (+Y, +X)
    glNormal3f(invSqrt2, invSqrt2, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( bx,   y,  bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx,   y, -bz);
    glTexCoord2f(tileU, tileV); glVertex3f(  x,  by, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(  x,  by,  bz);

    // Top-Left (+Y, -X)
    glNormal3f(-invSqrt2, invSqrt2, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx,   y, -bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-bx,   y,  bz);
    glTexCoord2f(tileU, tileV); glVertex3f( -x,  by,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( -x,  by, -bz);

    // Bottom-Front (-Y, +Z)
    glNormal3f(0.0f, -invSqrt2, invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( bx, -by,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-bx, -by,  z);
    glTexCoord2f(tileU, tileV); glVertex3f(-bx,  -y,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( bx,  -y,  bz);

    // Bottom-Back (-Y, -Z)
    glNormal3f(0.0f, -invSqrt2, -invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx, -by, -z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx, -by, -z);
    glTexCoord2f(tileU, tileV); glVertex3f( bx,  -y, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-bx,  -y, -bz);

    // Bottom-Right (-Y, +X)
    glNormal3f(invSqrt2, -invSqrt2, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( bx,  -y, -bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx,  -y,  bz);
    glTexCoord2f(tileU, tileV); glVertex3f(  x, -by,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(  x, -by, -bz);

    // Bottom-Left (-Y, -X)
    glNormal3f(-invSqrt2, -invSqrt2, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx,  -y,  bz);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-bx,  -y, -bz);
    glTexCoord2f(tileU, tileV); glVertex3f( -x, -by, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( -x, -by,  bz);

    // Front-Right (+Z, +X)
    glNormal3f(invSqrt2, 0.0f, invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( bx, -by,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx,  by,  z);
    glTexCoord2f(tileU, tileV); glVertex3f(  x,  by,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(  x, -by,  bz);

    // Front-Left (+Z, -X)
    glNormal3f(-invSqrt2, 0.0f, invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx,  by,  z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-bx, -by,  z);
    glTexCoord2f(tileU, tileV); glVertex3f( -x, -by,  bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( -x,  by,  bz);

    // Back-Right (-Z, +X)
    glNormal3f(invSqrt2, 0.0f, -invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( bx,  by, -z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( bx, -by, -z);
    glTexCoord2f(tileU, tileV); glVertex3f(  x, -by, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f(  x,  by, -bz);

    // Back-Left (-Z, -X)
    glNormal3f(-invSqrt2, 0.0f, -invSqrt2);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-bx, -by, -z);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(-bx,  by, -z);
    glTexCoord2f(tileU, tileV); glVertex3f( -x,  by, -bz);
    glTexCoord2f(0.0f, tileV);  glVertex3f( -x, -by, -bz);
    glEnd();

    // 3. 8 Corner Triangles
    glBegin(GL_TRIANGLES);
    // Top-Front-Right (+X, +Y, +Z)
    glNormal3f(invSqrt3, invSqrt3, invSqrt3);
    glVertex3f( bx,  by,   z); glVertex3f(  x,  by,  bz); glVertex3f( bx,   y,  bz);

    // Top-Front-Left (-X, +Y, +Z)
    glNormal3f(-invSqrt3, invSqrt3, invSqrt3);
    glVertex3f(-bx,  by,   z); glVertex3f(-bx,   y,  bz); glVertex3f( -x,  by,  bz);

    // Top-Back-Right (+X, +Y, -Z)
    glNormal3f(invSqrt3, invSqrt3, -invSqrt3);
    glVertex3f( bx,  by,  -z); glVertex3f( bx,   y, -bz); glVertex3f(  x,  by, -bz);

    // Top-Back-Left (-X, +Y, -Z)
    glNormal3f(-invSqrt3, invSqrt3, -invSqrt3);
    glVertex3f(-bx,  by,  -z); glVertex3f( -x,  by, -bz); glVertex3f(-bx,   y, -bz);

    // Bottom-Front-Right (+X, -Y, +Z)
    glNormal3f(invSqrt3, -invSqrt3, invSqrt3);
    glVertex3f( bx, -by,   z); glVertex3f( bx,  -y,  bz); glVertex3f(  x, -by,  bz);

    // Bottom-Front-Left (-X, -Y, +Z)
    glNormal3f(-invSqrt3, -invSqrt3, invSqrt3);
    glVertex3f(-bx, -by,   z); glVertex3f( -x, -by,  bz); glVertex3f(-bx,  -y,  bz);

    // Bottom-Back-Right (+X, -Y, -Z)
    glNormal3f(invSqrt3, -invSqrt3, -invSqrt3);
    glVertex3f( bx, -by,  -z); glVertex3f(  x, -by, -bz); glVertex3f( bx,  -y, -bz);

    // Bottom-Back-Left (-X, -Y, -Z)
    glNormal3f(-invSqrt3, -invSqrt3, -invSqrt3);
    glVertex3f(-bx, -by,  -z); glVertex3f(-bx,  -y, -bz); glVertex3f( -x, -by, -bz);
    glEnd();
}

void drawCylinder(float baseRadius, float topRadius, float height, int slices, float tileU, float tileV) {    float angleStep = 2.0f * (float)M_PI / (float)slices;
    float slope = (baseRadius - topRadius) / height;

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
        float a = i * angleStep;
        float u = (float)i / slices * tileU;
        float cosA = std::cos(a);
        float sinA = std::sin(a);

        float nx = cosA;
        float ny = slope;
        float nz = sinA;
        float len = std::sqrt(nx*nx + ny*ny + nz*nz);
        glNormal3f(nx/len, ny/len, nz/len);

        glTexCoord2f(u, 0.0f);
        glVertex3f(baseRadius * cosA, 0.0f, baseRadius * sinA);

        glTexCoord2f(u, tileV);
        glVertex3f(topRadius * cosA, height, topRadius * sinA);
    }
    glEnd();

    // Bottom Cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = slices; i >= 0; --i) {
        float a = i * angleStep;
        glTexCoord2f(0.5f + 0.5f * std::cos(a), 0.5f + 0.5f * std::sin(a));
        glVertex3f(baseRadius * std::cos(a), 0.0f, baseRadius * std::sin(a));
    }
    glEnd();

    // Top Cap
    if (topRadius > 0.001f) {
        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.5f, 0.5f);
        glVertex3f(0.0f, height, 0.0f);
        for (int i = 0; i <= slices; ++i) {
            float a = i * angleStep;
            glTexCoord2f(0.5f + 0.5f * std::cos(a), 0.5f + 0.5f * std::sin(a));
            glVertex3f(topRadius * std::cos(a), height, topRadius * std::sin(a));
        }
        glEnd();
    }
}

void drawSphere(float radius, int slices, int stacks, float tileU, float tileV) {    for (int i = 0; i < stacks; ++i) {
        float phi0 = (float)M_PI * (float)i / (float)stacks;
        float phi1 = (float)M_PI * (float)(i + 1) / (float)stacks;

        float y0 = radius * std::cos(phi0);
        float r0 = radius * std::sin(phi0);
        float v0 = (float)i / (float)stacks * tileV;

        float y1 = radius * std::cos(phi1);
        float r1 = radius * std::sin(phi1);
        float v1 = (float)(i + 1) / (float)stacks * tileV;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * (float)M_PI * (float)j / (float)slices;
            float u = (float)j / (float)slices * tileU;

            float cosT = std::cos(theta);
            float sinT = std::sin(theta);

            float x0 = r0 * cosT;
            float z0 = r0 * sinT;
            float x1 = r1 * cosT;
            float z1 = r1 * sinT;

            // Correct CCW quad-strip winding (bottom vertex first, then top vertex)
            glNormal3f(x1 / radius, y1 / radius, z1 / radius);
            glTexCoord2f(u, v1);
            glVertex3f(x1, y1, z1);

            glNormal3f(x0 / radius, y0 / radius, z0 / radius);
            glTexCoord2f(u, v0);
            glVertex3f(x0, y0, z0);
        }
        glEnd();
    }
}

void drawPrismRoof(float width, float height, float length, float tileU, float tileV) {    float hw = width * 0.5f;
    float hl = length * 0.5f;

    glBegin(GL_TRIANGLES);
    // Front (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);        glVertex3f(-hw, 0.0f,  hl);
    glTexCoord2f(tileU, 0.0f);       glVertex3f( hw, 0.0f,  hl);
    glTexCoord2f(tileU * 0.5f, tileV); glVertex3f(0.0f, height, hl);

    // Back (-Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(tileU, 0.0f);       glVertex3f( hw, 0.0f, -hl);
    glTexCoord2f(0.0f, 0.0f);        glVertex3f(-hw, 0.0f, -hl);
    glTexCoord2f(tileU * 0.5f, tileV); glVertex3f(0.0f, height, -hl);
    glEnd();

    glBegin(GL_QUADS);
    // Right (+X)
    float nx = height; float ny = hw; float nlen = std::sqrt(nx*nx + ny*ny);
    glNormal3f(nx/nlen, ny/nlen, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f( hw, 0.0f,  hl);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( hw, 0.0f, -hl);
    glTexCoord2f(tileU, tileV); glVertex3f(0.0f, height, -hl);
    glTexCoord2f(0.0f, tileV);  glVertex3f(0.0f, height,  hl);

    // Left (-X)
    glNormal3f(-nx/nlen, ny/nlen, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(0.0f, height,  hl);
    glTexCoord2f(tileU, 0.0f);  glVertex3f(0.0f, height, -hl);
    glTexCoord2f(tileU, tileV); glVertex3f(-hw, 0.0f, -hl);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-hw, 0.0f,  hl);

    // Bottom (-Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);   glVertex3f(-hw, 0.0f, -hl);
    glTexCoord2f(tileU, 0.0f);  glVertex3f( hw, 0.0f, -hl);
    glTexCoord2f(tileU, tileV); glVertex3f( hw, 0.0f,  hl);
    glTexCoord2f(0.0f, tileV);  glVertex3f(-hw, 0.0f,  hl);
    glEnd();
}

void drawSteepleSpire(float baseRadius, float height, int facets, float tileU, float tileV) {    float angleStep = 2.0f * (float)M_PI / (float)facets;
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < facets; ++i) {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;
        float x1 = baseRadius * std::cos(a1);
        float z1 = baseRadius * std::sin(a1);
        float x2 = baseRadius * std::cos(a2);
        float z2 = baseRadius * std::sin(a2);

        float mx = (x1 + x2) * 0.5f;
        float mz = (z1 + z2) * 0.5f;
        float nx = mx;
        float ny = baseRadius / height;
        float nz = mz;
        float nl = std::sqrt(nx*nx + ny*ny + nz*nz);

        glNormal3f(nx/nl, ny/nl, nz/nl);
        glTexCoord2f(0.0f, 0.0f);        glVertex3f(x1, 0.0f, z1);
        glTexCoord2f(tileU, 0.0f);       glVertex3f(x2, 0.0f, z2);
        glTexCoord2f(tileU * 0.5f, tileV); glVertex3f(0.0f, height, 0.0f);
    }
    glEnd();
}

void drawBillboardHalo(float x, float y, float z, float radius, float r, float g, float b, float maxAlpha) {    bindTexture(TEX_NONE);
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive luminous bloom

    glPushMatrix();
    glTranslatef(x, y, z);

    // Billboarding: Extract camera orientation to keep halo billboarded towards screen
    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    modelview[0] = 1.0f; modelview[1] = 0.0f; modelview[2] = 0.0f;
    modelview[4] = 0.0f; modelview[5] = 1.0f; modelview[6] = 0.0f;
    modelview[8] = 0.0f; modelview[9] = 0.0f; modelview[10] = 1.0f;
    glLoadMatrixf(modelview);

    int segments = 28;
    float coreRadius = radius * 0.18f;

    // 1. Intense Overbright Core Glare Disc (Hotspot)
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 1.0f, 1.0f, maxAlpha * 0.92f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * (float)M_PI * (float)i / segments;
        glColor4f(r * 1.05f, g * 1.05f, b * 1.05f, maxAlpha * 0.45f);
        glVertex3f(coreRadius * std::cos(theta), coreRadius * std::sin(theta), 0.0f);
    }
    glEnd();

    // 2. Multi-tier Soft Gaussian Bloom Rings (Smooth cubic falloff)
    int rings = 9;
    for (int ring = 0; ring < rings; ++ring) {
        float t0 = (float)ring / rings;
        float t1 = (float)(ring + 1) / rings;
        float r0 = coreRadius + (radius - coreRadius) * t0;
        float r1 = coreRadius + (radius - coreRadius) * t1;

        // Smooth cubic Gaussian decay curve
        float a0 = maxAlpha * (1.0f - t0) * (1.0f - t0) * (1.0f - t0);
        float a1 = maxAlpha * (1.0f - t1) * (1.0f - t1) * (1.0f - t1);

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segments; ++i) {
            float theta = 2.0f * (float)M_PI * (float)i / segments;
            float ct = std::cos(theta);
            float st = std::sin(theta);

            glColor4f(r, g, b, a1);
            glVertex3f(r1 * ct, r1 * st, 0.0f);

            glColor4f(r, g, b, a0);
            glVertex3f(r0 * ct, r0 * st, 0.0f);
        }
        glEnd();
    }

    glPopMatrix();
    glPopAttrib();
}

// Volumetric Flashlight Beam Cone & Drifting Dust Motes in the Fog
void drawVolumetricFlashlightBeam(float posX, float posY, float posZ, float dirX, float dirY, float dirZ) {    if (!g_light2SpotOn) return;

    bindTexture(TEX_NONE);
    glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_POINT_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive soft volumetric glow

    // Forward direction vector
    float fLen = std::sqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
    if (fLen < 0.0001f) { glPopAttrib(); return; }
    float fx = dirX / fLen;
    float fy = dirY / fLen;
    float fz = dirZ / fLen;

    // Right and Up orthogonal coordinate vectors
    float ux = 0.0f, uy = 1.0f, uz = 0.0f;
    if (std::abs(fy) > 0.95f) { ux = 1.0f; uy = 0.0f; uz = 0.0f; }
    float rx = fy * uz - fz * uy;
    float ry = fz * ux - fx * uz;
    float rz = fx * uy - fy * ux;
    float rLen = std::sqrt(rx * rx + ry * ry + rz * rz);
    rx /= rLen; ry /= rLen; rz /= rLen;

    ux = ry * fz - rz * fy;
    uy = rz * fx - rx * fz;
    uz = rx * fy - ry * fx;

    // Multi-segment soft translucent beam cone
    int slices = 18;
    int rings = 8;
    float maxDist = 28.0f;
    float spreadAngle = 24.5f * (float)M_PI / 180.0f; // matches spot cutoff (25.0 deg)
    float tanSpread = std::tan(spreadAngle);

    for (int ring = 0; ring < rings; ++ring) {
        float d0 = 0.35f + maxDist * ((float)ring / rings);
        float d1 = 0.35f + maxDist * ((float)(ring + 1) / rings);
        float coneR0 = d0 * tanSpread;
        float coneR1 = d1 * tanSpread;

        float t0 = (float)ring / rings;
        float t1 = (float)(ring + 1) / rings;
        float a0 = 0.032f * (1.0f - t0 * t0);
        float a1 = 0.032f * (1.0f - t1 * t1);

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= slices; ++i) {
            float theta = 2.0f * (float)M_PI * (float)i / slices;
            float ct = std::cos(theta);
            float st = std::sin(theta);

            float cx1 = posX + fx * d1 + (rx * ct + ux * st) * coneR1;
            float cy1 = posY + fy * d1 + (ry * ct + uy * st) * coneR1;
            float cz1 = posZ + fz * d1 + (rz * ct + uz * st) * coneR1;

            float cx0 = posX + fx * d0 + (rx * ct + ux * st) * coneR0;
            float cy0 = posY + fy * d0 + (ry * ct + uy * st) * coneR0;
            float cz0 = posZ + fz * d0 + (rz * ct + uz * st) * coneR0;

            glColor4f(0.85f, 0.90f, 0.98f, a1);
            glVertex3f(cx1, cy1, cz1);

            glColor4f(0.85f, 0.90f, 0.98f, a0);
            glVertex3f(cx0, cy0, cz0);
        }
        glEnd();
    }

    // Floating dust motes illuminated in the beam
    glPointSize(2.4f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 28; ++i) {
        float seed = (float)i * 137.5f;
        float speed = 0.12f + 0.08f * std::sin(seed * 0.3f);
        float drift = std::fmod(g_time * speed + seed, 12.0f) + 0.6f;

        float spread = drift * tanSpread * 0.65f;
        float angle = seed + g_time * 0.25f;
        float dr = std::sin(seed * 1.7f) * spread;

        float mx = posX + fx * drift + (rx * std::cos(angle) + ux * std::sin(angle)) * dr;
        float my = posY + fy * drift + (ry * std::cos(angle) + uy * std::sin(angle)) * dr + 0.05f * std::sin(g_time + seed);
        float mz = posZ + fz * drift + (rz * std::cos(angle) + uz * std::sin(angle)) * dr;

        float moteAlpha = (0.28f + 0.22f * std::sin(g_time * 3.5f + seed)) * (1.0f - drift / 13.0f);
        if (moteAlpha > 0.0f) {
            glColor4f(0.95f, 0.96f, 0.90f, moteAlpha);
            glVertex3f(mx, my, mz);
        }
    }
    glEnd();

    glPopAttrib();
}
