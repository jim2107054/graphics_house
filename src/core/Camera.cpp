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
    // Stage 1: Exterior Overview & Moonlit Atmosphere (0.0s - 8.0s)
    {  0.0f,   1.20f,  1.35f, 22.0f, -94.0f,   5.0f, "1/10: Exterior Overview & Sky Dome", "Directional Moonlight (GL_LIGHT1)" },
    {  4.0f,   0.20f,  1.45f, 19.5f, -90.0f,   6.0f, "1/10: Lightning Flash & Thunder Demo", "Atmospheric Lightning Flash [L]" },
    
    // Stage 2: Atmospheric Fog & Environmental Depth (8.0s - 14.0s)
    {  8.0f,  -0.50f,  1.50f, 16.5f, -88.0f,   4.0f, "2/10: Atmospheric Fog Demonstration", "GL_FOG Exponential Distance Attenuation" },
    { 11.0f,  -1.20f,  1.40f, 14.0f, -85.0f,   0.0f, "2/10: Night Mist & Ambient Horizon", "GL_FOG Dynamic Density" },

    // Stage 3: Walkway Approach & Jack-o'-Lantern Candle Lights (14.0s - 22.0s)
    { 14.0f,  -1.60f,  1.20f, 11.5f, -65.0f, -12.0f, "3/10: Carved Jack-o'-Lanterns (Close-up)", "Halloween Pumpkins lining Pathway" },
    { 18.0f,   0.50f,  1.20f,  9.5f, -120.0f, -10.0f, "3/10: Candle Point Lights & Ground Halos", "Internal Flame Flicker & Ground Halos" },

    // Stage 4: Vintage 1950s Rusted Car & Spotlight Cone (22.0s - 30.0s)
    { 22.0f,   4.80f,  1.40f, 12.0f, -125.0f,   2.0f, "4/10: Vintage 1950s Rusted Car (Right Yard)", "Weathered Rust & Chrome Shading" },
    { 26.0f,   7.50f,  1.55f,  9.2f, -155.0f,  -2.0f, "4/10: Spot Light (GL_LIGHT2) Demonstration", "Focused 22 deg Soft Cone & Volumetric Beam [3/F]" },

    // Stage 5: Foggy Cemetery & Celtic Crosses (30.0s - 37.0s)
    { 30.0f,  -6.50f,  1.50f,  9.5f,  -45.0f,   3.0f, "5/10: Foggy Graveyard & Celtic Crosses", "Stone Sarcophagi & Gravestones (Left Yard)" },
    { 34.0f, -10.50f,  1.65f,  4.5f,  -55.0f,  -2.0f, "5/10: Ancient Crosses in Drifting Mist", "Stone Material Shading & Ground Mist" },

    // Stage 6: Front Porch & Hanging Bulb Harmonic Motion (37.0s - 46.0s)
    { 37.0f,  -7.40f,  1.75f,  7.8f,  -90.0f,   8.0f, "6/10: Front Porch & Gothic Columns", "Ascending Porch Stone Steps" },
    { 41.5f,  -7.40f,  2.05f,  6.2f,  -85.0f,  18.0f, "6/10: Point Light (GL_LIGHT0) Sway & Light Pool", "Hanging Porch Bulb Sway & Amber Pool [1/B]" },

    // Stage 7: Dilapidated Ground Floor Parlor & Texture Mapping (46.0s - 56.0s)
    { 46.0f,  -7.40f,  1.65f,  3.5f,  -90.0f,  -4.0f, "7/10: Stepping through Gothic Arched Doorway", "Entering Haunted Ground Floor Parlor" },
    { 49.5f,  -5.80f,  1.65f,  0.5f, -135.0f,  -2.0f, "7/10: Grandfather Clock & Dilapidated Furniture", "Texture Mapping vs Solid Phong Shading [T]" },
    { 53.0f,  -7.80f,  1.65f, -0.8f,  -65.0f,   5.0f, "7/10: Dilapidated Fireplace & Archway", "Interior Wallpaper, Floorboards & Cobwebs" },

    // Stage 8: Ascending Completed 15-Step Staircase to 2nd Floor (56.0s - 66.0s)
    { 56.0f,  -4.15f,  1.65f,  2.2f, -180.0f,  16.0f, "8/10: Approaching 15-Step Wooden Staircase", "Looking up the Staircase Rise" },
    { 60.0f,  -4.15f,  3.00f,  0.2f, -180.0f,  15.0f, "8/10: Climbing Completed 15-Step Staircase", "Ascending Stairway with Railing & Balusters" },
    { 63.5f,  -4.15f,  4.60f, -1.2f, -150.0f,  10.0f, "8/10: Stepping onto 2nd Floor (Dotola) Landing", "Reaching Second Floor Opening" },

    // Stage 9: 2nd Floor (Dotola) Attic Bedroom, Grimoire & Dormer Window (66.0s - 76.0s)
    { 66.0f,  -7.20f,  5.85f, -1.5f,  -90.0f,  -5.0f, "9/10: 2nd Floor (Dotola) Attic Bedroom", "Gothic Four-Poster Bed & Occult Desk" },
    { 71.0f,  -7.40f,  5.85f,  1.8f,  -70.0f,   4.0f, "9/10: Upper Dormer Window Moonlit Panorama", "Looking Out Dormer Window at the Moon" },

    // Stage 10: High Aerial Rooftop Panorama (3D Estate Architecture) (76.0s - 85.0s)
    { 76.0f,   2.50f, 13.50f, 16.5f, -115.0f, -28.0f, "10/10: High Aerial Rooftop Panorama", "Bird's-Eye View: Roof Shingles, Tower & Chimneys" },
    { 81.0f,   6.50f, 11.50f, 14.5f, -135.0f, -22.0f, "10/10: 360 Degree Estate Architecture", "Hierarchical 3D Modeling Overview" },

    // Stage 11: Tour Completed & Return to Vantage (85.0s - 90.0s)
    { 85.0f,   1.20f,  1.35f, 22.0f,  -94.0f,   5.0f, "Tour Completed! [Press C to Explore Freely]", "All Systems Verified & Active" },
    { 90.0f,   1.20f,  1.35f, 22.0f,  -94.0f,   5.0f, "Tour Completed! [Press C to Explore Freely]", "All Systems Verified & Active" }
};
const int NUM_TOUR_KEYS = sizeof(TOUR_KEYS) / sizeof(TOUR_KEYS[0]);
const float TOTAL_TOUR_DURATION = 90.0f;

void updateCinematicCamera(float dt) {
    float prevTime = g_cinematicTime;
    g_cinematicTime += dt;
    if (g_cinematicTime >= TOTAL_TOUR_DURATION) {
        g_cinematicTime = std::fmod(g_cinematicTime, TOTAL_TOUR_DURATION);
        prevTime = 0.0f;
        // Restore all default lighting states
        g_light0PointOn = true;
        g_light1DirectionalOn = true;
        g_light2SpotOn = false;
        g_light3AreaOn = true;
        g_pumpkinLightsOn = true;
        g_texturesEnabled = true;
        g_fogEnabled = false;
    }

    // Dynamic Live Demonstrations during Guided Tour:
    // 1. Distant Lightning Strike at t = 2.2s
    if (prevTime < 2.2f && g_cinematicTime >= 2.2f) {
        triggerLightning();
    }
    // 2. Directional Moonlight Demonstration (GL_LIGHT1)
    if (prevTime < 4.2f && g_cinematicTime >= 4.2f) {
        g_light1DirectionalOn = false; // DEMO: Moonlight OFF
    }
    if (prevTime < 6.5f && g_cinematicTime >= 6.5f) {
        g_light1DirectionalOn = true;  // DEMO: Moonlight ON (Silvery blue & planar shadows restored)
    }

    // 4. Jack-o'-Lantern Pumpkin Candle Lights Demonstration
    if (prevTime < 14.8f && g_cinematicTime >= 14.8f) {
        g_pumpkinLightsOn = false; // DEMO: Pumpkin lights OFF (Extinguished)
    }
    if (prevTime < 17.5f && g_cinematicTime >= 17.5f) {
        g_pumpkinLightsOn = true;  // DEMO: Pumpkin lights ON (Fiery faces & ground halos)
    }

    // 5. Spotlight / Flashlight Demonstration (GL_LIGHT2)
    if (prevTime < 22.8f && g_cinematicTime >= 22.8f) {
        g_light2SpotOn = true;  // DEMO: Flashlight ON (Focused 22 deg cone & volumetric beam)
    }
    if (prevTime < 28.5f && g_cinematicTime >= 28.5f) {
        g_light2SpotOn = false; // DEMO: Flashlight OFF
    }

    // 6. House Front Porch Light Demonstration (GL_LIGHT0)
    if (prevTime < 38.0f && g_cinematicTime >= 38.0f) {
        g_light0PointOn = false; // DEMO: Porch Bulb OFF (Dark porch)
    }
    if (prevTime < 41.0f && g_cinematicTime >= 41.0f) {
        g_light0PointOn = true;  // DEMO: Porch Bulb ON (Amber glow & light pool on porch deck)
    }

    // 7. Texture Mapping vs Solid Phong Shading Demonstration
    if (prevTime < 48.0f && g_cinematicTime >= 48.0f) {
        g_texturesEnabled = false; // DEMO: Textures OFF (Solid Phong material colors)
    }
    if (prevTime < 51.5f && g_cinematicTime >= 51.5f) {
        g_texturesEnabled = true;  // DEMO: Textures ON (GL_MODULATE texture mapping)
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
    if (segDur < 0.001f) segDur = 0.001f;
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
