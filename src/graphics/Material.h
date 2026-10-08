#pragma once
#include "../core/Config.h"

// ============================================================================
struct Material {
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float emission[4];
    float shininess;
};

inline void applyMaterial(const Material& m) {
    glColor4fv(m.diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   m.ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   m.diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  m.specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  m.emission);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, m.shininess);
}

const Material MAT_DARK_WOOD = {
    { 0.020f, 0.018f, 0.016f, 1.0f },
    { 0.050f, 0.045f, 0.040f, 1.0f },
    { 0.015f, 0.015f, 0.012f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    6.0f
};

const Material MAT_WEATHERED_WALL = {
    { 0.06f, 0.06f, 0.07f, 1.0f },
    { 0.28f, 0.26f, 0.24f, 1.0f },
    { 0.08f, 0.08f, 0.09f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    10.0f
};

const Material MAT_ROOF_SHINGLE = {
    { 0.06f, 0.07f, 0.09f, 1.0f },
    { 0.26f, 0.28f, 0.35f, 1.0f },
    { 0.16f, 0.18f, 0.22f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    16.0f
};

const Material MAT_STONE = {
    { 0.040f, 0.045f, 0.055f, 1.0f },
    { 0.110f, 0.120f, 0.135f, 1.0f },
    { 0.060f, 0.070f, 0.080f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    20.0f
};

const Material MAT_WET_GROUND = {
    { 0.025f, 0.028f, 0.032f, 1.0f },
    { 0.065f, 0.072f, 0.080f, 1.0f },
    { 0.000f, 0.000f, 0.000f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    0.0f
};

const Material MAT_PUDDLE_WATER = {
    { 0.00f, 0.00f, 0.00f, 0.0f },
    { 0.00f, 0.00f, 0.00f, 0.0f },
    { 0.00f, 0.00f, 0.00f, 0.0f },
    { 0.00f, 0.00f, 0.00f, 0.0f },
    0.0f
};

const Material MAT_DEAD_GRASS = {
    { 0.030f, 0.035f, 0.028f, 1.0f },
    { 0.075f, 0.085f, 0.065f, 1.0f },
    { 0.020f, 0.025f, 0.018f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    8.0f
};

const Material MAT_FALLEN_LEAF = {
    { 0.06f, 0.04f, 0.02f, 1.0f },
    { 0.22f, 0.13f, 0.07f, 1.0f },
    { 0.02f, 0.02f, 0.01f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    6.0f
};

const Material MAT_CLAY_BRICK = {
    { 0.26f, 0.14f, 0.10f, 1.0f },
    { 0.64f, 0.34f, 0.22f, 1.0f },
    { 0.10f, 0.08f, 0.05f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    12.0f
};

const Material MAT_MOSS_STONE = {
    { 0.16f, 0.20f, 0.14f, 1.0f },
    { 0.40f, 0.50f, 0.34f, 1.0f },
    { 0.12f, 0.14f, 0.10f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    14.0f
};

const Material MAT_RUSTY_METAL = {
    { 0.24f, 0.16f, 0.12f, 1.0f },
    { 0.58f, 0.40f, 0.28f, 1.0f },
    { 0.88f, 0.82f, 0.72f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    78.0f
};

const Material MAT_BLACK_IRON = {
    { 0.08f, 0.08f, 0.08f, 1.0f },
    { 0.18f, 0.18f, 0.18f, 1.0f },
    { 0.45f, 0.45f, 0.45f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    45.0f
};

const Material MAT_CAR_GLASS = {
    { 0.06f, 0.08f, 0.12f, 0.88f },
    { 0.14f, 0.18f, 0.26f, 0.88f },
    { 0.98f, 0.98f, 1.00f, 0.88f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    128.0f
};

const Material MAT_RUBBER_TYRE = {
    { 0.10f, 0.10f, 0.10f, 1.0f },
    { 0.22f, 0.22f, 0.22f, 1.0f },
    { 0.08f, 0.08f, 0.08f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    10.0f
};

const Material MAT_CHROME_TRIM = {
    { 0.34f, 0.36f, 0.40f, 1.0f },
    { 0.82f, 0.85f, 0.90f, 1.0f },
    { 1.00f, 1.00f, 1.00f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    128.0f
};

const Material MAT_CAR_INTERIOR = {
    { 0.14f, 0.12f, 0.10f, 1.0f },
    { 0.36f, 0.30f, 0.25f, 1.0f },
    { 0.16f, 0.14f, 0.12f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    16.0f
};

const Material MAT_SEAT_FOAM = {
    { 0.28f, 0.24f, 0.12f, 1.0f },
    { 0.70f, 0.62f, 0.32f, 1.0f },
    { 0.06f, 0.06f, 0.03f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    8.0f
};

const Material MAT_PUMPKIN_SKIN = {
    { 0.50f, 0.22f, 0.06f, 1.0f },
    { 0.98f, 0.46f, 0.12f, 1.0f },
    { 0.40f, 0.22f, 0.08f, 1.0f },
    { 0.18f, 0.08f, 0.02f, 1.0f },
    28.0f
};

const Material MAT_PUMPKIN_GLOW = {
    { 1.00f, 0.72f, 0.22f, 1.0f },
    { 1.00f, 0.80f, 0.28f, 1.0f },
    { 1.00f, 0.95f, 0.60f, 1.0f },
    { 1.00f, 0.75f, 0.22f, 1.0f },
    60.0f
};

const Material MAT_WINDOW_GLOW = {
    { 0.45f, 0.28f, 0.10f, 1.0f },
    { 0.85f, 0.55f, 0.20f, 1.0f },
    { 0.25f, 0.15f, 0.05f, 1.0f },
    { 1.00f, 0.72f, 0.26f, 1.0f },
    40.0f
};

const Material MAT_BULB_EMISSIVE = {
    { 1.00f, 0.88f, 0.35f, 1.0f },
    { 1.00f, 0.95f, 0.55f, 1.0f },
    { 1.00f, 1.00f, 0.90f, 1.0f },
    { 1.00f, 0.90f, 0.40f, 1.0f },
    75.0f
};

const Material MAT_MOON = {
    { 0.85f, 0.88f, 0.94f, 1.0f },
    { 0.95f, 0.97f, 1.00f, 1.0f },
    { 0.60f, 0.65f, 0.75f, 1.0f },
    { 0.88f, 0.90f, 0.95f, 1.0f },
    35.0f
};

const Material MAT_BARK = {
    { 0.08f, 0.08f, 0.10f, 1.0f },
    { 0.18f, 0.18f, 0.22f, 1.0f },
    { 0.04f, 0.04f, 0.06f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    4.0f
};

const Material MAT_FOLIAGE_DARK = {
    { 0.08f, 0.14f, 0.08f, 1.0f },
    { 0.14f, 0.28f, 0.14f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    0.0f
};

const Material MAT_FOLIAGE_LUSH = {
    { 0.10f, 0.18f, 0.10f, 1.0f },
    { 0.18f, 0.36f, 0.18f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    0.0f
};

const Material MAT_FOLIAGE_AUTUMN = {
    { 0.14f, 0.10f, 0.05f, 1.0f },
    { 0.35f, 0.22f, 0.08f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    0.0f
};

const Material MAT_PINE_NEEDLES = {
    { 0.08f, 0.15f, 0.09f, 1.0f },
    { 0.15f, 0.30f, 0.16f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    0.0f
};

