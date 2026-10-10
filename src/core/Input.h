#pragma once
#include "Config.h"

void keyboardDownCallback(unsigned char key, int x, int y);
void keyboardUpCallback(unsigned char key, int x, int y);
void specialKeyDownCallback(int key, int x, int y);
void specialKeyUpCallback(int key, int x, int y);
void mouseMotionCallback(int x, int y);
void mousePassiveMotionCallback(int x, int y);
void mouseButtonCallback(int button, int state, int x, int y);
