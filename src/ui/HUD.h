#pragma once
#include "../core/Config.h"

void drawString2D(float x, float y, void* font, const char* str, float r, float g, float b, float a = 1.0f);
void drawUIPanel(float x, float y, float w, float h, float r, float g, float b, float a);
void drawCinematicColorGrade();
void drawScreenVignette();
void renderTitleScreen();
void renderSceneHUD();
