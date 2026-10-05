#include "Camera.h"
#include "../entities/Terrain.h"
#include "../graphics/Lighting.h"
#include <cmath>
#include <iostream>

Camera g_cam = {
    1.2f, 1.35f, 22.0f,
    -94.0f, 5.0f,
    12.0f,
    0.15f,
    52.0f,
    52.0f
};

int g_camFloorState = 0; // 0: Exterior, 1: 1st Floor Parlor, 2: 2nd Floor (Dotola) Attic

// ============================================================================
std::string g_tourActionBadge = "Directional Light (GL_LIGHT1)";

struct TourKeyframe {
    float time;
    float x, y, z;
    float yaw, pitch;
    const char* stageTitle;
    const char* actionBadge;
};

const TourKeyframe TOUR_KEYS[] = {
    // Stage 1: Exterior Overview & Lightning Strike (0s - 7s)
    {  0.0f,   1.20f,  1.35f, 22.0f, -94.0f,   5.0f, "1/8: Exterior Overview & Moonlit Atmosphere", "Directional Light (GL_LIGHT1)" },
    {  3.8f,  -0.50f,  1.80f, 16.5f, -88.0f,   6.0f, "1/8: Distant Lightning Strike & Thunder", "Lightning Strike Triggered [L]" },
    
    // Stage 2: Vintage Rusted 1950s Car & Spotlight (7s - 14s)
    {  7.0f,   5.20f,  1.40f, 13.5f, -115.0f,  2.0f, "2/8: Rusted 1950s Car & Planar Shadows", "Flashlight Spotlight ON [3/F]" },
    { 10.5f,   7.80f,  1.50f,  9.5f, -145.0f, -2.0f, "2/8: Telephone Pole & Gothic Yard Props", "Spotlight Soft Cone (22 deg)" },

    // Stage 3: Foggy Graveyard & Pumpkin Candle Lights (14s - 21s)
    { 14.0f,   8.80f,  1.60f, -3.5f, -160.0f, -4.0f, "3/8: Foggy Cemetery & Celtic Crosses", "Jack-o'-Lanterns Active [K]" },
    { 17.5f,   3.50f,  1.50f, -4.5f, -130.0f,  2.0f, "3/8: Carved Pumpkins & Dynamic Light Pools", "Flickering Candle Point Lights" },

    // Stage 4: Front Porch & Hanging Bulb Harmonic Sway (21s - 28s)
    { 21.0f,  -1.80f,  1.80f,  3.5f,  -70.0f, 12.0f, "4/8: Front Porch & Hanging Incandescent Bulb", "Point Light (GL_LIGHT0) Sway [B]" },
    { 24.8f,  -4.60f,  2.10f,  0.8f,  -65.0f, 15.0f, "4/8: Point Light Distance Attenuation", "1 / (kc + kl*d + kq*d^2)" },

    // Stage 5: Ground Floor Interior & Material Shading (28s - 36s)
    { 28.0f,  -7.20f,  1.65f, -9.2f, -100.0f, -4.0f, "5/8: Dilapidated Ground Floor Parlor", "Texture Mapping [T] (GL_MODULATE)" },
    { 32.0f,  -5.50f,  1.65f, -8.5f, -125.0f, -2.0f, "5/8: Grandfather Clock & Broken Furniture", "Phong Material Shading" },

    // Stage 6: Ground Floor Hallway, Dilapidated Fireplace & Clutter (36s - 44s)
    { 36.0f,  -7.80f,  1.65f, -9.8f,  -70.0f,  4.0f, "6/8: Dilapidated Fireplace & Archway", "Interior Architecture & Props" },
    { 40.0f,  -6.20f,  1.75f, -9.0f,  -85.0f,  2.0f, "6/8: Weathered Floorboards & Cobwebs", "Approaching 15-Step Stairway" },

    // Stage 7: Ascending Completed 15-Step Staircase to 2nd Floor (44s - 52s)
    { 44.0f,  -4.80f,  2.20f, -7.5f,  -55.0f, 18.0f, "7/8: Climbing Completed 15-Step Staircase", "Ascending to 2nd Floor (Dotola)" },
    { 48.0f,  -4.20f,  4.20f, -9.8f,  -68.0f, 15.0f, "7/8: Passing Balusters & Stairwell Opening", "Full Stairway Completed" },

    // Stage 8: 2nd Floor (Dotola) Attic Bedroom, Grimoire & Dormer Window (52s - 64s)
    { 52.0f,  -7.50f,  5.85f, -8.8f,  -95.0f, -5.0f, "8/8: 2nd Floor Four-Poster Bed & Desk", "Occult Grimoire & Candelabra" },
    { 58.0f,  -6.80f,  5.85f, -6.5f,  -75.0f,  4.0f, "8/8: Upper Dormer Window Moonlit View", "Tour Complete! [Press C to Explore]" },
    { 64.0f,   1.20f,  1.35f, 22.0f, -94.0f,   5.0f, "8/8: Grand Exterior Panorama", "Cinematic Tour Finished" }
};
const int NUM_TOUR_KEYS = sizeof(TOUR_KEYS) / sizeof(TOUR_KEYS[0]);
const float TOTAL_TOUR_DURATION = 64.0f;

void updateCinematicCamera(float dt) {    float prevTime = g_cinematicTime;
    g_cinematicTime += dt;
    if (g_cinematicTime >= TOTAL_TOUR_DURATION) {
        g_cinematicTime = std::fmod(g_cinematicTime, TOTAL_TOUR_DURATION);
        prevTime = 0.0f;
    }

    // Dynamic Live Action Demonstrations during Guided Tour
    // 1. Trigger Lightning Strike at t = 2.5s
    if (prevTime < 2.5f && g_cinematicTime >= 2.5f) {
        triggerLightning();
    }
    // 2. Turn Flashlight Spotlight ON at t = 7.0s
    if (prevTime < 7.0f && g_cinematicTime >= 7.0f) {
        g_light2SpotOn = true;
    }
    // 3. Turn Flashlight Spotlight OFF at t = 13.5s
    if (prevTime < 13.5f && g_cinematicTime >= 13.5f) {
        g_light2SpotOn = false;
    }
    // 4. Brief Texture Mapping comparison at t = 30.0s to 32.5s
    if (prevTime < 30.0f && g_cinematicTime >= 30.0f) {
        g_texturesEnabled = false; // Show Phong material albedos
    }
    if (prevTime < 32.5f && g_cinematicTime >= 32.5f) {
        g_texturesEnabled = true;  // Restore GL_MODULATE textures
    }

    int idx = 0;
    for (int i = 0; i < NUM_TOUR_KEYS - 1; ++i) {
        if (g_cinematicTime >= TOUR_KEYS[i].time && g_cinematicTime <= TOUR_KEYS[i + 1].time) {
            idx = i;
            break;
        }
    }

    g_tourStageTitle  = TOUR_KEYS[idx].stageTitle;
    g_tourActionBadge = TOUR_KEYS[idx].actionBadge;

    float segDur = TOUR_KEYS[idx + 1].time - TOUR_KEYS[idx].time;
    float t = (g_cinematicTime - TOUR_KEYS[idx].time) / segDur;
    float s = t * t * (3.0f - 2.0f * t); // Smooth Hermite ease-in ease-out

    g_cam.x = TOUR_KEYS[idx].x + (TOUR_KEYS[idx + 1].x - TOUR_KEYS[idx].x) * s;
    g_cam.y = TOUR_KEYS[idx].y + (TOUR_KEYS[idx + 1].y - TOUR_KEYS[idx].y) * s;
    g_cam.z = TOUR_KEYS[idx].z + (TOUR_KEYS[idx + 1].z - TOUR_KEYS[idx].z) * s;
    g_cam.yaw = TOUR_KEYS[idx].yaw + (TOUR_KEYS[idx + 1].yaw - TOUR_KEYS[idx].yaw) * s;
    g_cam.pitch = TOUR_KEYS[idx].pitch + (TOUR_KEYS[idx + 1].pitch - TOUR_KEYS[idx].pitch) * s;
}

// ============================================================================
// HOUSE COLLISION DETECTION (STRICT SINGLE-DOOR ENTRY/EXIT & 2ND FLOOR ACCESS)
// Only the front doorway (dorja) allows passing between exterior and interior!
// ============================================================================
bool isHouseLocationFree(float worldX, float worldZ) {    // Transform from world space to house local space
    float dx = worldX - g_houseShiftX;
    float dz = worldZ - g_houseShiftZ;
    float rad = -g_houseRotY * (float)M_PI / 180.0f;
    float lx =  dx * std::cos(rad) + dz * std::sin(rad);
    float lz = -dx * std::sin(rad) + dz * std::cos(rad);
    float pr = 0.35f; // Player collision radius

    // 1. Central Tower (Cylinder) - Completely Solid
    float tdx = lx - 1.2f;
    float tdz = lz - 2.2f;
    if (tdx * tdx + tdz * tdz < (2.15f + pr) * (2.15f + pr)) {
        return false;
    }

    // 2. Right Wing (Solid Building Section) - Completely Solid
    if (lx >= (1.55f - pr) && lx <= (6.10f + pr) && lz >= (-3.40f - pr) && lz <= (4.45f + pr)) {
        return false;
    }

    // 3. Back Lean-to Extension - Completely Solid
    if (lx >= (-8.20f - pr) && lx <= (-4.80f + pr) && lz >= (-5.85f - pr) && lz <= (-3.70f + pr)) {
        return false;
    }

    // 4. Main Wing Left Outer Wall (lx = -8.95f) - Completely Solid (No window clipping)
    if (lx <= (-8.85f + pr) && lz >= (-3.90f - pr) && lz <= (4.90f + pr)) {
        return false;
    }

    // 5. Main Wing Right Partition Wall (lx = -0.65f) - Completely Solid
    if (lx >= (-0.75f - pr) && lz >= (-3.90f - pr) && lz <= (4.90f + pr)) {
        return false;
    }

    // 6. Main Wing Back Wall (lz = -3.78f) - Completely Solid
    if (lz <= (-3.65f + pr) && lx >= (-9.10f - pr) && lx <= (-0.55f + pr)) {
        return false;
    }

    // 7. Main Wing Front Wall (lz = 4.78f)
    // ONLY the front doorway (lx in [-5.30f, -3.90f] on ground floor y < 3.4f) allows entry/exit!
    // All left panels, right panels, lintels, and entire 2nd floor front wall are 100% SOLID!
    if (lz >= (4.60f - pr) && lz <= (4.96f + pr)) {
        bool inDoorOpening = (g_cam.y < 3.4f) && (lx >= -5.30f && lx <= -3.90f);
        if (!inDoorOpening) {
            if (lx >= (-9.10f - pr) && lx <= (-0.55f + pr)) {
                return false;
            }
        }
    }

    // 8. Porch Support Posts (Columns)
    float postX[4] = { -7.4f, -5.8f, -3.4f, -1.8f };
    for (int p = 0; p < 4; ++p) {
        float pdx = lx - postX[p];
        float pdz = lz - 5.7f;
        if (pdx * pdx + pdz * pdz < (0.24f + pr) * (0.24f + pr)) {
            return false;
        }
    }

    // 9. Hallway Partition inside ground floor room (lz = -1.20f)
    if (g_cam.y < 3.4f) {
        bool inStairCorridor = (lx >= -2.25f && lx <= -0.65f);
        if (!inStairCorridor && lz >= (-1.35f - pr) && lz <= (-1.05f + pr)) {
            if (lx >= (-9.00f - pr) && lx <= -5.45f) return false;
            if (lx >= -3.75f && lx <= (-0.55f + pr)) return false;
        }
    }

    return true;
}

// ============================================================================
// INPUT PROCESSING & CAMERA MOVEMENT (SEAMLESS STAIR CLIMBING & 2ND FLOOR)
// ============================================================================
void processKeyboardInput(float dt) {    if (g_cinematicMode) return;

    float radYaw = g_cam.yaw * (float)M_PI / 180.0f;
    float fx = std::cos(radYaw);
    float fz = std::sin(radYaw);

    float rx = -std::sin(radYaw);
    float rz =  std::cos(radYaw);

    float moveSpeed = g_cam.speed * dt;
    float deltaX = 0.0f;
    float deltaZ = 0.0f;

    if (g_keyState['w'] || g_keyState['W']) {
        deltaX += fx * moveSpeed;
        deltaZ += fz * moveSpeed;
    }
    if (g_keyState['s'] || g_keyState['S']) {
        deltaX -= fx * moveSpeed;
        deltaZ -= fz * moveSpeed;
    }
    if (g_keyState['a'] || g_keyState['A']) {
        deltaX -= rx * moveSpeed;
        deltaZ -= rz * moveSpeed;
    }
    if (g_keyState['d'] || g_keyState['D']) {
        deltaX += rx * moveSpeed;
        deltaZ += rz * moveSpeed;
    }

    // Apply collision-aware movement with smooth sliding along walls
    if (deltaX != 0.0f || deltaZ != 0.0f) {
        if (isHouseLocationFree(g_cam.x + deltaX, g_cam.z + deltaZ)) {
            g_cam.x += deltaX;
            g_cam.z += deltaZ;
        } else if (isHouseLocationFree(g_cam.x + deltaX, g_cam.z)) {
            g_cam.x += deltaX; // Slide along X
        } else if (isHouseLocationFree(g_cam.x, g_cam.z + deltaZ)) {
            g_cam.z += deltaZ; // Slide along Z
        }
    }

    if (g_keyState[' ']) {
        g_cam.y += moveSpeed;
    }
    if (g_isCtrlPressed || g_keyState['q'] || g_keyState['Q']) {
        g_cam.y -= moveSpeed;
        if (g_cam.y < 0.4f) g_cam.y = 0.4f;
    }

    // Auto-step height tracking for seamless walking on paths, porch steps, staircase, and 2nd floor (Dotola)!
    if (!g_keyState[' '] && !g_isCtrlPressed && !g_keyState['q']) {
        float surfaceY = getTerrainHeight(g_cam.x, g_cam.z);

        // Porch steps (Z: 7.0 to 8.2, X: -4.5 + g_houseShiftX to -1.0 + g_houseShiftX)
        if (g_cam.z >= 7.0f && g_cam.z <= 8.2f && g_cam.x >= (-4.5f + g_houseShiftX) && g_cam.x <= (-1.0f + g_houseShiftX)) {
            float stepFrac = (8.2f - g_cam.z) / 1.2f;
            surfaceY = std::max(surfaceY, stepFrac * 0.72f);
        }
        // Porch deck (Z: 3.2f to 7.0f, X: -6.4 + g_houseShiftX to 0.8 + g_houseShiftX)
        if (g_cam.z >= 3.2f && g_cam.z < 7.0f && g_cam.x >= (-6.4f + g_houseShiftX) && g_cam.x <= (0.8f + g_houseShiftX)) {
            surfaceY = std::max(surfaceY, 0.74f);
        }

        // Inside Main House Section:
        float hdx = g_cam.x - g_houseShiftX;
        float hdz = g_cam.z - g_houseShiftZ;
        float hRad = -g_houseRotY * (float)M_PI / 180.0f;
        float hlx =  hdx * std::cos(hRad) + hdz * std::sin(hRad);
        float hlz = -hdx * std::sin(hRad) + hdz * std::cos(hRad);

        if (hlx >= -8.90f && hlx <= -0.65f && hlz >= -3.75f && hlz <= 4.75f) {
            // Inside left wing
            if (hlx >= -2.25f && hlx <= -0.65f && hlz >= -1.65f && hlz <= 3.85f) {
                // WALKING ON THE 15-STEP COMPLETED STAIRCASE!
                float stairT = (3.60f - hlz) / 5.0f;
                if (stairT < 0.0f) stairT = 0.0f;
                if (stairT > 1.0f) stairT = 1.0f;
                surfaceY = 0.74f + stairT * (4.20f - 0.74f);
            } else if (g_cam.y >= 3.6f) {
                // WALKING ON 2nd FLOOR (DOTOLA) FLOORBOARDS!
                surfaceY = 4.20f;
            } else {
                // GROUND FLOOR INTERIOR!
                surfaceY = 0.78f;
            }
        }

        float targetEyeY = surfaceY + 1.65f;
        // Smoothly interpolate eye height
        g_cam.y += (targetEyeY - g_cam.y) * std::min(1.0f, dt * 8.0f);
    }
}
