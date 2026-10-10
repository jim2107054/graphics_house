#pragma once
#include "../core/Config.h"

void buildShadowMatrix(float shadowMat[16], const float groundPlane[4], const float lightPos[4]);
void renderShadowCasters();
void renderBulbShadowCasters();
void renderPlanarShadows();
