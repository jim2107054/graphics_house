#pragma once
#include "../core/Config.h"

struct LightningSystem {
    bool  active;
    float timer;
    float nextStrikeInterval;
    float strikeProgress;
    float strikeDuration;
    float flashIntensity;
    float thunderCountdown;
    bool  thunderPending;
};

extern LightningSystem g_lightning;

void triggerLightning();
void drawHangingBulb();
void drawBulbLightPool();
