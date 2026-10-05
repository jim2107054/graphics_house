#pragma once
#include "../core/Config.h"

void drawNaturalFoliageCluster(float radius, TreeRNG& rng, int foliageType = 0);
void drawRootFlare(float trunkR, TreeRNG& rng);
void drawNaturalDeciduousTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed, int foliageType = 0, float colorTint = 1.0f);
void drawNaturalPineTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed);
void drawOrganicCreepyTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed, float colorTint = 1.0f);
void drawGothicForegroundTree(float x, float z, float trunkRadius, float height, float rotY, unsigned int seed);
void drawAllTrees();
