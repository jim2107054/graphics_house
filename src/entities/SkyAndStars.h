#pragma once
#include "../core/Config.h"

struct Star {
    float x, y, z;
    float size;
    float brightness;
};

extern std::vector<Star> g_stars;

void initStars();
void drawMoonAndStars();
