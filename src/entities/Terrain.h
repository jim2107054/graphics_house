#pragma once
#include "../core/Config.h"

float getTerrainHeight(float x, float z);
void getTerrainNormal(float x, float z, float& nx, float& ny, float& nz);
float getRoadCenterX(float pz);
void drawPuddle(float cx, float cz, float radiusX, float radiusZ, float rotAngle);
void drawPuddles();
void drawGrassTuft(float x, float z, float width, float height, float rotY);
void drawFallenLeaf(float x, float z, float size, float rotY, float pitch, float r, float g, float b);
void drawPebble(float x, float z, float scaleX, float scaleY, float scaleZ, float rotY);
void drawIrregularRock(float x, float z, float rx, float ry, float rz, float rotY, float rotX, unsigned int seed, float mossFactor = 0.65f);
void drawBrokenCrate(float x, float z, float rotY, float tilt = 6.0f);
void drawOldBarrel(float x, float z, float rotY, float tilt = 0.0f);
void drawFallenPlank(float x, float z, float rotY, float pitch);
void drawRustyBucket(float x, float z, float rotY, float tilt = 22.0f);
void drawSingleBrick(float x, float z, float rotY, float pitch = 0.0f);
void drawScatteredBricks();
void drawDeadBush(float x, float z, float scale, float rotY, unsigned int seed);
void drawLeaningLamppost(float x, float z, float rotY, float leanAngle = 11.5f);
void drawBrokenChair(float x, float z, float rotY, float tilt = 0.0f);
void drawWoodenTable(float x, float z, float rotY, float tilt = 0.0f);
void drawOldFurniture();
void drawSignBoard(float x, float z, float rotY);
void drawScatteredRocks();
void drawDenseGrassField();
void drawGroundProps();
void drawGround();
void drawEnvironmentalClutter();
