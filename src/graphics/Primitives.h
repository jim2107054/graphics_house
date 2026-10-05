#pragma once
#include "../core/Config.h"
#include "TextureManager.h"

void drawBox(float width, float height, float depth, float tileU = 1.0f, float tileV = 1.0f);
void drawBeveledBox(float width, float height, float depth, float bevel = 0.04f, float tileU = 1.0f, float tileV = 1.0f);
void drawCylinder(float baseRadius, float topRadius, float height, int slices, float tileU = 1.0f, float tileV = 1.0f);
void drawSphere(float radius, int slices, int stacks, float tileU = 1.0f, float tileV = 1.0f);
void drawPrismRoof(float width, float height, float length, float tileU = 2.0f, float tileV = 2.0f);
void drawSteepleSpire(float baseRadius, float height, int facets, float tileU = 2.0f, float tileV = 3.0f);
void drawBillboardHalo(float x, float y, float z, float radius, float r, float g, float b, float maxAlpha);
void drawVolumetricFlashlightBeam(float posX, float posY, float posZ, float dirX, float dirY, float dirZ);
