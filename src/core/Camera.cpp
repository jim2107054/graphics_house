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
    // Stage 1: Exterior Overview & Sky Dome (0.0s - 10.0s)
    {   0.0f,   1.20f,  1.35f, 22.0f,  -94.0f,   5.0f, "1/12: Exterior Overview & Sky Dome", "Directional Moonlight [GL_LIGHT1]" },
    {   5.0f,   0.20f,  1.45f, 19.5f,  -90.0f,   6.0f, "1/12: Distant Lightning Flash & Sky Surge", "Atmospheric Lightning Strike [L]" },
    {  10.0f,  -0.60f,  1.45f, 17.0f,  -88.0f,   4.0f, "1/12: Moonlight Restored & Shadow Casting", "Planar Stencil Shadows on Terrain" },

    // Stage 2: Gothic Landscape & Vegetation (10.0s - 20.0s)
    {  10.0f,  -0.60f,  1.45f, 17.0f,  -88.0f,   4.0f, "2/12: Gothic Landscape & Vegetation", "Winding Cobblestone Road & Warning Sign" },
    {  15.0f,  -2.20f,  1.40f, 14.5f,  -82.0f,  -2.0f, "2/12: Spooky Dead Trees & Pine Canopies", "Natural Root Flares & Organic Branches" },
    {  20.0f,  -1.80f,  1.30f, 12.8f,  -75.0f,  -8.0f, "2/12: Falling Leaves Particle Dynamics", "Wind Drift & Rotational Tumble" },

    // Stage 3: Carved Jack-o'-Lanterns (20.0s - 30.0s)
    {  20.0f,  -1.80f,  1.30f, 12.8f,  -75.0f,  -8.0f, "3/12: Carved Jack-o'-Lanterns (Close-up)", "Halloween Pumpkins Lining Pathway" },
    {  25.0f,  -1.20f,  1.15f, 10.8f,  -60.0f, -14.0f, "3/12: Pumpkin Candle Lights Demonstration", "Candle Point Lights Extinguished & Reignited [K]" },
    {  30.0f,   0.40f,  1.20f,  9.5f, -115.0f, -10.0f, "3/12: Fiery Carved Faces & Ground Halos", "Internal Flame Flicker & Amber Glow" },

    // Stage 4: Vintage 1930s Abandoned Automobile (30.0s - 42.0s)
    {  30.0f,   0.40f,  1.20f,  9.5f, -115.0f, -10.0f, "4/12: Vintage 1930s Abandoned Automobile", "Weathered Chassis, Radiator Grill & Open Door" },
    {  35.0f,   4.80f,  1.40f, 12.0f, -125.0f,   2.0f, "4/12: Tactical Flashlight Spotlight [GL_LIGHT2]", "Focused 25 deg Cone & Volumetric Beam [3/F]" },
    {  42.0f,   7.50f,  1.55f,  9.2f, -155.0f,  -2.0f, "4/12: Whitewall Wheels, Steering Wheel & Engine", "Chrome Highlights & Rusted Metal Shading" },

    // Stage 5: Haunted Cemetery & Crypts (42.0s - 54.0s)
    {  42.0f,   7.50f,  1.55f,  9.2f, -155.0f,  -2.0f, "5/12: Haunted Cemetery & Crypts (Left Yard)", "Stone Sarcophagi & Weathered Picket Fence" },
    {  48.0f,  -6.50f,  1.50f,  9.5f,  -45.0f,   3.0f, "5/12: Celtic Crosses & Carved Headstones", "Ancient Tomb Inscriptions & Crosses" },
    {  54.0f, -10.50f,  1.65f,  4.5f,  -55.0f,  -2.0f, "5/12: Fresh Earth Burial Mounds & Mist", "Translucent Ground Mist Rolling Across Graves" },

    // Stage 6: Front Porch & Prowling Creatures (54.0s - 66.0s)
    {  54.0f, -10.50f,  1.65f,  4.5f,  -55.0f,  -2.0f, "6/12: Front Porch & Prowling Creatures", "Ascending Porch Stone Steps" },
    {  60.0f,  -7.40f,  1.75f,  7.8f,  -90.0f,   8.0f, "6/12: Hanging Porch Bulb Kinematics [GL_LIGHT0]", "Double-Pendulum Sway & Light Pool [1/B]" },
    {  66.0f,  -7.40f,  2.05f,  6.2f,  -85.0f,  18.0f, "6/12: Prowling Black Cat & Perched Horned Owl", "Articulated Walking Cycle & Glowing Raptor Eyes" },

    // Stage 7: Haunted Ground Floor Parlor (66.0s - 78.0s)
    {  66.0f,  -7.40f,  2.05f,  6.2f,  -85.0f,  18.0f, "7/12: Entering Haunted Ground Floor Parlor", "Stepping through Gothic Arched Doorway" },
    {  70.0f,  -7.40f,  1.65f,  3.5f,  -90.0f,  -4.0f, "7/12: Fireplace Hearth & Grandfather Clock", "Ticking Clock Pendulum & Brick Fireplace" },
    {  74.0f,  -5.80f,  1.65f,  0.5f, -135.0f,  -2.0f, "7/12: Texture Mapping vs Solid Phong Shading", "Toggling GL_MODULATE vs Materials Only [T]" },
    {  78.0f,  -7.80f,  1.65f, -0.8f,  -65.0f,   5.0f, "7/12: Antique Dining Furniture & Cobwebs", "Turned Wood Legs, Bookshelf & Wallpapers" },

    // Stage 8: Ascending 15-Step Staircase (78.0s - 88.0s)
    {  78.0f,  -7.80f,  1.65f, -0.8f,  -65.0f,   5.0f, "8/12: Approaching 15-Step Wooden Staircase", "Looking up the Continuous Stairway Rise" },
    {  83.0f,  -4.15f,  1.65f,  2.2f, -180.0f,  16.0f, "8/12: Climbing Completed 15-Step Staircase", "Turned Baluster Spindles & Master Newel Post" },
    {  88.0f,  -4.15f,  4.60f, -1.2f, -150.0f,  10.0f, "8/12: Reaching Second Floor Mezzanine", "Smooth Collision-Aware Stair Ascension" },

    // Stage 9: 2nd Floor Attic & Alchemist Study (88.0s - 98.0s)
    {  88.0f,  -4.15f,  4.60f, -1.2f, -150.0f,  10.0f, "9/12: 2nd Floor (Dotola) Attic Bedroom & Study", "Gothic Four-Poster Bed & Occult Desk" },
    {  93.0f,  -7.20f,  5.85f, -1.5f,  -90.0f,  -5.0f, "9/12: Human Skull, Ancient Grimoire & Candlelight", "Candelabra [GL_LIGHT4] & Wall Sconces" },
    {  98.0f,  -7.40f,  5.85f,  1.8f,  -70.0f,   4.0f, "9/12: Hanging Brass Lantern [GL_LIGHT5]", "Localized Amber Attenuation in Attic Chamber" },

    // Stage 10: Dormer Window & Bat Swarm (98.0s - 108.0s)
    {  98.0f,  -7.40f,  5.85f,  1.8f,  -70.0f,   4.0f, "10/12: Upper Dormer Window Moonlit Panorama", "Looking Out Dormer Window at the Night Sky" },
    { 103.0f,  -4.50f,  7.50f,  4.5f,  -85.0f,  12.0f, "10/12: Animated Bat Swarm & Volumetric Clouds", "Multi-Agent Orbital Flocking Around the Moon" },
    { 108.0f,  -2.00f,  9.50f,  8.0f, -100.0f,   8.0f, "10/12: Secondary Rooftop Lightning Flash", "Dramatic Electric Flash Illuminating Bats [L]" },

    // Stage 11: Aerial Rooftop & 3D Architecture (108.0s - 116.0s)
    { 108.0f,  -2.00f,  9.50f,  8.0f, -100.0f,   8.0f, "11/12: Aerial Rooftop & 3D Architecture", "Steeple Spire, Weather Vane & Double Chimneys" },
    { 112.0f,   2.50f, 13.50f, 16.5f, -115.0f, -28.0f, "11/12: 360 Degree Bird's-Eye Estate Panorama", "Pitched Gable Roofs & Shingle Geometry" },
    { 116.0f,   6.50f, 11.50f, 14.5f, -135.0f, -22.0f, "11/12: Planar Stencil Shadows Across Terrain", "Directional Shadows & Stencil Buffer Masking" },

    // Stage 12: Grand Finale & HUD Overlay (116.0s - 120.0s)
    { 116.0f,   6.50f, 11.50f, 14.5f, -135.0f, -22.0f, "12/12: Grand Finale Return & 2D HUD", "Descending to Front Reference Vantage Point" },
    { 118.5f,   1.20f,  1.35f, 22.0f,  -94.0f,   5.0f, "12/12: Full 2D Orthographic HUD Dashboard [TAB]", "Live FPS, In-Game Time & Controls Overlay" },
    { 120.0f,   1.20f,  1.35f, 22.0f,  -94.0f,   5.0f, "Showcase Complete! All Systems Verified", "Interactive 3D Gothic Environment Developed in OpenGL" }
};
const int NUM_TOUR_KEYS = sizeof(TOUR_KEYS) / sizeof(TOUR_KEYS[0]);
const float TOTAL_TOUR_DURATION = 120.0f;

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
    // 1. Distant Lightning Strike at t = 2.5s
    if (prevTime < 2.5f && g_cinematicTime >= 2.5f) {
        triggerLightning();
    }
    // 2. Directional Moonlight Demonstration (GL_LIGHT1)
    if (prevTime < 5.0f && g_cinematicTime >= 5.0f) {
        g_light1DirectionalOn = false; // DEMO: Moonlight OFF
    }
    if (prevTime < 8.0f && g_cinematicTime >= 8.0f) {
        g_light1DirectionalOn = true;  // DEMO: Moonlight ON (Silvery blue & planar shadows restored)
    }

    // 3. Jack-o'-Lantern Pumpkin Candle Lights Demonstration
    if (prevTime < 23.5f && g_cinematicTime >= 23.5f) {
        g_pumpkinLightsOn = false; // DEMO: Pumpkin lights OFF (Extinguished)
    }
    if (prevTime < 27.0f && g_cinematicTime >= 27.0f) {
        g_pumpkinLightsOn = true;  // DEMO: Pumpkin lights ON (Fiery faces & ground halos)
    }

    // 4. Spotlight / Flashlight Demonstration (GL_LIGHT2)
    if (prevTime < 34.0f && g_cinematicTime >= 34.0f) {
        g_light2SpotOn = true;  // DEMO: Flashlight ON (Focused 25 deg cone & volumetric beam)
    }
    if (prevTime < 40.5f && g_cinematicTime >= 40.5f) {
        g_light2SpotOn = false; // DEMO: Flashlight OFF
    }

    // 5. House Front Porch Light Demonstration (GL_LIGHT0)
    if (prevTime < 58.5f && g_cinematicTime >= 58.5f) {
        g_light0PointOn = false; // DEMO: Porch Bulb OFF (Dark porch)
    }
    if (prevTime < 62.0f && g_cinematicTime >= 62.0f) {
        g_light0PointOn = true;  // DEMO: Porch Bulb ON (Amber glow & light pool on porch deck)
    }

    // 6. Texture Mapping vs Solid Phong Shading Demonstration
    if (prevTime < 71.0f && g_cinematicTime >= 71.0f) {
        g_texturesEnabled = false; // DEMO: Textures OFF (Solid Phong material colors)
    }
    if (prevTime < 75.0f && g_cinematicTime >= 75.0f) {
        g_texturesEnabled = true;  // DEMO: Textures ON (GL_MODULATE texture mapping)
    }

    // 7. Secondary Rooftop Lightning Flash at t = 104.0s
    if (prevTime < 104.0f && g_cinematicTime >= 104.0f) {
        triggerLightning();
    }

    // 8. Full HUD Overlay Dashboard at t = 117.5s
    if (prevTime < 117.5f && g_cinematicTime >= 117.5f) {
        g_showHUD = true; // DEMO: 2D HUD Dashboard
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
