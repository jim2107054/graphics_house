#pragma once
#include "../core/Config.h"

void drawArchedHeadstone(float x, float z, float scale, float rotY, float lean, float leanDir, int stoneType = 0);
void drawCelticCrossGrave(float x, float z, float scale, float rotY, float lean, float leanDir);
void drawStoneCrossGrave(float x, float z, float scale, float rotY, float lean, float leanDir);
void drawStoneSarcophagusGrave(float x, float z, float scale, float rotY, float tilt, bool lidAjar = true);
void drawEarthBurialMound(float x, float z, float scale, float rotY);
void drawGraveyardCrosses();
void drawDetailedBrokenFenceSection(float x, float z, float rotY);
void drawBrokenFence();
void drawFenceAndYardProps();
