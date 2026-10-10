#pragma once
#include "../core/Config.h"

void drawHouseWindow(float x, float y, float z, float width, float height, float rotY = 0.0f, bool hasArch = true, bool hasCrossMuntin = true);
void drawOutdoorCobweb(float x, float y, float z, float size, float rotX, float rotY, float rotZ);
void drawCobweb(float x, float y, float z, float size, float rotY = 0.0f);
void drawHouseInterior();
void drawSecondFloorInterior();
void drawHouse();
