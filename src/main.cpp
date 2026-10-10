#include "core/Config.h"
#include "core/Camera.h"
#include "core/Input.h"
#include "audio/AudioSystem.h"
#include "graphics/Material.h"
#include "graphics/TextureManager.h"
#include "graphics/Primitives.h"
#include "graphics/Lighting.h"
#include "graphics/Shadow.h"
#include "entities/SkyAndStars.h"
#include "entities/Terrain.h"
#include "entities/Vegetation.h"
#include "entities/Graveyard.h"
#include "entities/Props.h"
#include "entities/Particles.h"
#include "entities/Creatures.h"
#include "entities/House.h"
#include "entities/Clouds.h"
#include "ui/HUD.h"
#include "ui/Screenshot.h"
#include <iostream>

static int g_autoScreenshotTargetFrames = 3;

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    // Light 0: Porch Point Light Attenuation
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  0.40f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.14f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.045f);

    // Light 2: Spot Light Flashlight Cone & Cutoff (Player Tactical Flashlight on key '3')
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF,   25.0f);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 5.0f);
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION,  0.35f);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION,    0.020f);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.0025f);
    float flashDiff[4] = { 2.6f, 2.5f, 2.2f, 1.0f }; // Bright diffuse light illuminating all objects!
    float flashSpec[4] = { 1.5f, 1.5f, 1.5f, 1.0f };
    float flashAmb[4]  = { 0.10f, 0.10f, 0.08f, 1.0f };
    glLightfv(GL_LIGHT2, GL_DIFFUSE,  flashDiff);
    glLightfv(GL_LIGHT2, GL_SPECULAR, flashSpec);
    glLightfv(GL_LIGHT2, GL_AMBIENT,  flashAmb);

    // Light 3: Area Light Window Glow Attenuation
    glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION,  0.40f);
    glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION,    0.08f);
    glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.025f);
    float winDiff[4] = { 0.95f, 0.65f, 0.22f, 1.0f };
    glLightfv(GL_LIGHT3, GL_DIFFUSE, winDiff);

    // Light 4: 2nd Floor Alchemist Candelabra & Candlelight Attenuation
    glLightf(GL_LIGHT4, GL_CONSTANT_ATTENUATION,  0.22f);
    glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION,    0.035f);
    glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, 0.007f);

    // Light 5: 2nd Floor Hanging Brass Lantern Room Attenuation
    glLightf(GL_LIGHT5, GL_CONSTANT_ATTENUATION,  0.25f);
    glLightf(GL_LIGHT5, GL_LINEAR_ATTENUATION,    0.035f);
    glLightf(GL_LIGHT5, GL_QUADRATIC_ATTENUATION, 0.008f);

    // No Fog (Fog permanently disabled per user request)
    glDisable(GL_FOG);

    initAllTextures();
    initStars();
    initFallingLeaves();
}

void render3DScene() {    // 1. UPDATE LIGHTS & ATMOSPHERE
    float flash = g_lightning.flashIntensity;

    // No Fog (Fog permanently disabled per user request)
    glDisable(GL_FOG);

    // Global Ambient (Spikes during lightning strike)
    float ambR = 0.05f + 0.25f * flash;
    float ambG = 0.06f + 0.28f * flash;
    float ambB = 0.09f + 0.38f * flash;
    float curGlobalAmbient[4] = { ambR, ambG, ambB, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, curGlobalAmbient);

    // Clear Color (Sky backdrop flash)
    glClearColor(0.020f + 0.18f * flash, 0.030f + 0.20f * flash, 0.065f + 0.28f * flash, 1.0f);

    // Light 0: Point Light (House Front Porch Bulb)
    if (g_light0PointOn) {
        glEnable(GL_LIGHT0);
        float pPos[4] = { g_bulbCurX, g_bulbCurY, g_bulbCurZ, 1.0f };
        float pDiff[4] = { 
            0.78f * g_bulbFlickerFactor, 
            0.60f * g_bulbFlickerFactor, 
            0.28f * g_bulbFlickerFactor, 
            1.0f 
        };
        float pAmb[4] = {
            0.12f * g_bulbFlickerFactor,
            0.08f * g_bulbFlickerFactor,
            0.03f * g_bulbFlickerFactor,
            1.0f
        };
        float pSpec[4] = {
            0.45f * g_bulbFlickerFactor,
            0.35f * g_bulbFlickerFactor,
            0.15f * g_bulbFlickerFactor,
            1.0f
        };
        glLightfv(GL_LIGHT0, GL_POSITION, pPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT,  pAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  pDiff);
        glLightfv(GL_LIGHT0, GL_SPECULAR, pSpec);
    } else {
        glDisable(GL_LIGHT0);
    }

    // Light 1: Directional Moonlight / Overhead Lightning Bolt
    if (g_light1DirectionalOn || flash > 0.05f) {
        glEnable(GL_LIGHT1);
        if (flash > 0.05f) {
            // Intense overhead lightning illumination
            float lDir[4] = { 0.25f, 0.92f, 0.30f, 0.0f };
            float lDiff[4] = { 0.45f + 1.25f * flash, 0.55f + 1.30f * flash, 0.75f + 1.55f * flash, 1.0f };
            glLightfv(GL_LIGHT1, GL_POSITION, lDir);
            glLightfv(GL_LIGHT1, GL_DIFFUSE, lDiff);
        } else {
            float mDiff[4] = { 0.28f, 0.35f, 0.48f, 1.0f };
            glLightfv(GL_LIGHT1, GL_POSITION, g_moonDir);
            glLightfv(GL_LIGHT1, GL_DIFFUSE, mDiff);
        }
    } else {
        glDisable(GL_LIGHT1);
    }

    float radYaw   = g_cam.yaw * (float)M_PI / 180.0f;
    float radPitch = g_cam.pitch * (float)M_PI / 180.0f;
    float fx = std::cos(radYaw) * std::cos(radPitch);
    float fy = std::sin(radPitch);
    float fz = std::sin(radYaw) * std::cos(radPitch);

    float rx = -std::sin(radYaw);
    float rz =  std::cos(radYaw);

    float flashPosX = g_cam.x + rx * 0.25f;
    float flashPosY = g_cam.y - 0.15f;
    float flashPosZ = g_cam.z + rz * 0.25f;

    if (g_light2SpotOn) {
        glEnable(GL_LIGHT2);
        // Position light at camera eye for centered illumination
        float flashPos[4]  = { g_cam.x, g_cam.y, g_cam.z, 1.0f };
        float flashDir[3]  = { fx, fy, fz };
        float flashDiff[4] = { 2.6f, 2.5f, 2.2f, 1.0f };
        float flashSpec[4] = { 1.5f, 1.5f, 1.5f, 1.0f };
        float flashAmb[4]  = { 0.10f, 0.10f, 0.08f, 1.0f };

        glLightfv(GL_LIGHT2, GL_POSITION, flashPos);
        glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, flashDir);
        glLightfv(GL_LIGHT2, GL_DIFFUSE,  flashDiff);
        glLightfv(GL_LIGHT2, GL_SPECULAR, flashSpec);
        glLightfv(GL_LIGHT2, GL_AMBIENT,  flashAmb);
    } else {
        glDisable(GL_LIGHT2);
    }

    if (g_light3AreaOn) {
        glEnable(GL_LIGHT3);
        float winPos[4] = { -4.6f + g_houseShiftX, 4.6f, 4.85f + g_houseShiftZ, 1.0f };
        glLightfv(GL_LIGHT3, GL_POSITION, winPos);
    } else {
        glDisable(GL_LIGHT3);
    }

    // Light 4: 2nd Floor Alchemist Candelabra & Candlelight (GL_LIGHT4)
    if (g_light4CandleOn) {
        glEnable(GL_LIGHT4);
        float radH = g_houseRotY * (float)M_PI / 180.0f;
        // Desk candelabra local pos on 2nd floor: (-7.10f, 5.35f, -1.98f)
        float cLocalX = -7.10f;
        float cLocalY =  5.35f;
        float cLocalZ = -1.98f;
        float cWorldX = g_houseShiftX + cLocalX * std::cos(radH) + cLocalZ * std::sin(radH);
        float cWorldY = cLocalY;
        float cWorldZ = g_houseShiftZ - cLocalX * std::sin(radH) + cLocalZ * std::cos(radH);

        float cFlick = 0.90f + 0.10f * std::sin(g_time * 7.5f) + 0.05f * std::cos(g_time * 12.0f);
        float cPos[4]  = { cWorldX, cWorldY, cWorldZ, 1.0f };
        float cDiff[4] = { 1.35f * cFlick, 0.92f * cFlick, 0.42f * cFlick, 1.0f };
        float cAmb[4]  = { 0.22f * cFlick, 0.16f * cFlick, 0.07f * cFlick, 1.0f };
        float cSpec[4] = { 0.75f * cFlick, 0.55f * cFlick, 0.25f * cFlick, 1.0f };

        glLightfv(GL_LIGHT4, GL_POSITION, cPos);
        glLightfv(GL_LIGHT4, GL_AMBIENT,  cAmb);
        glLightfv(GL_LIGHT4, GL_DIFFUSE,  cDiff);
        glLightfv(GL_LIGHT4, GL_SPECULAR, cSpec);
    } else {
        glDisable(GL_LIGHT4);
    }

    // Light 5: 2nd Floor Hanging Brass Lantern Room Illumination (GL_LIGHT5)
    if (g_light5LanternOn) {
        glEnable(GL_LIGHT5);
        float radH = g_houseRotY * (float)M_PI / 180.0f;
        // Hanging lantern local pos: (-4.80f, 6.34f, 0.50f)
        float lLocalX = -4.80f;
        float lLocalY =  6.34f;
        float lLocalZ =  0.50f;
        float lWorldX = g_houseShiftX + lLocalX * std::cos(radH) + lLocalZ * std::sin(radH);
        float lWorldY = lLocalY;
        float lWorldZ = g_houseShiftZ - lLocalX * std::sin(radH) + lLocalZ * std::cos(radH);

        float lFlick = 0.94f + 0.06f * std::sin(g_time * 5.0f);
        float lPos[4]  = { lWorldX, lWorldY, lWorldZ, 1.0f };
        float lDiff[4] = { 1.20f * lFlick, 0.92f * lFlick, 0.50f * lFlick, 1.0f };
        float lAmb[4]  = { 0.20f * lFlick, 0.15f * lFlick, 0.08f * lFlick, 1.0f };
        float lSpec[4] = { 0.60f * lFlick, 0.45f * lFlick, 0.22f * lFlick, 1.0f };

        glLightfv(GL_LIGHT5, GL_POSITION, lPos);
        glLightfv(GL_LIGHT5, GL_AMBIENT,  lAmb);
        glLightfv(GL_LIGHT5, GL_DIFFUSE,  lDiff);
        glLightfv(GL_LIGHT5, GL_SPECULAR, lSpec);
    } else {
        glDisable(GL_LIGHT5);
    }

    // 2. RENDER 3D SCENE OBJECTS
    drawMoonAndStars();
    drawDynamicClouds();
    drawGround();
    drawHouse();
    drawRustedCar(8.2f, 6.8f, -22.0f);
    drawEnvironmentalClutter();
    drawGraveyardCrosses();
    drawHangingBulb();
    drawBulbLightPool();
    drawPumpkinArray();
    drawAllTrees();
    drawFallingLeaves();
    drawAllBats();
    drawAllCreatures();

    // 3. RENDER SHADOWS
    renderPlanarShadows();

    // 4. VOLUMETRIC FLASHLIGHT BEAM & DUST MOTES
    if (g_light2SpotOn) {
        drawVolumetricFlashlightBeam(flashPosX, flashPosY, flashPosZ, fx, fy, fz);
    }
}


void displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (g_appState == STATE_TITLE) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, g_windowWidth, g_windowHeight, 0.0);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        renderTitleScreen();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    } else {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective((double)g_cam.fov, (double)g_windowWidth / (double)g_windowHeight, 0.2, 350.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        float radYaw   = g_cam.yaw * (float)M_PI / 180.0f;
        float radPitch = g_cam.pitch * (float)M_PI / 180.0f;
        float targetX = g_cam.x + std::cos(radYaw) * std::cos(radPitch);
        float targetY = g_cam.y + std::sin(radPitch);
        float targetZ = g_cam.z + std::sin(radYaw) * std::cos(radPitch);

        gluLookAt(g_cam.x, g_cam.y, g_cam.z,
                  targetX, targetY, targetZ,
                  0.0, 1.0, 0.0);

        static int debugFrame = 0;
        if (++debugFrame % 60 == 0) {
            std::cout << "[DEBUG] Camera: (" << g_cam.x << ", " << g_cam.y << ", " << g_cam.z 
                      << ") | Yaw: " << g_cam.yaw << " deg, Pitch: " << g_cam.pitch 
                      << " deg" << std::endl;
        }

        render3DScene();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, g_windowWidth, g_windowHeight, 0.0);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        renderSceneHUD();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    if (!g_autoScreenshotFile.empty()) {
        g_autoScreenshotFrames++;
        if (g_autoScreenshotFrames >= g_autoScreenshotTargetFrames) {
            saveScreenshot(g_autoScreenshotFile.c_str());
            std::cout << "[AUTO SCREENSHOT] Saved to " << g_autoScreenshotFile << std::endl;
            exit(0);
        }
    }

    glutSwapBuffers();
}

void reshapeCallback(int w, int h) {    if (h == 0) h = 1;
    g_windowWidth  = w;
    g_windowHeight = h;
    glViewport(0, 0, w, h);
}

void idleCallback() {    int curTimeMs = glutGet(GLUT_ELAPSED_TIME);
    if (g_prevTimeMs == 0) g_prevTimeMs = curTimeMs;
    g_deltaTime = (curTimeMs - g_prevTimeMs) * 0.001f;
    g_prevTimeMs = curTimeMs;
    if (g_deltaTime > 0.1f) g_deltaTime = 0.1f;

    g_time += g_deltaTime;

    g_frameCount++;
    g_fpsTimer += g_deltaTime;
    if (g_fpsTimer >= 0.5f) {
        g_fps = g_frameCount / g_fpsTimer;
        g_frameCount = 0;
        g_fpsTimer = 0.0f;
    }

    // 1. Porch Bulb Sway & Flicker
    if (g_bulbAnimEnabled) {
        float f1 = std::sin(g_time * 18.0f);
        float f2 = std::cos(g_time * 33.0f);
        float noise = ((float)(rand() % 100) / 100.0f) * 0.12f;
        g_bulbFlickerFactor = 0.88f + 0.08f * f1 * f2 + noise;
        if (rand() % 95 == 0) g_bulbFlickerFactor = 0.25f;
    } else {
        g_bulbFlickerFactor = 1.0f;
    }

    // 2. Pumpkin Warm Candle Flicker
    g_pumpkinFlicker = 0.85f + 0.15f * std::sin(g_time * 4.5f) * std::cos(g_time * 2.8f);

    // 2.1 Dynamic Falling Leaves Animation
    updateFallingLeaves(g_deltaTime);

    // Smooth camera FOV zooming (game-style mouse scroll)
    g_cam.fov += (g_cam.targetFov - g_cam.fov) * std::min(1.0f, g_deltaTime * 14.0f);

    // 3. Lightning State Machine & Dynamic Flashes
    g_lightning.timer += g_deltaTime;
    if (!g_lightning.active && g_lightning.timer >= g_lightning.nextStrikeInterval) {
        triggerLightning();
    }

    if (g_lightning.active) {
        g_lightning.strikeProgress += g_deltaTime;
        float prog = g_lightning.strikeProgress;
        float dur = g_lightning.strikeDuration;

        if (prog < 0.08f) {
            g_lightning.flashIntensity = (prog / 0.08f) * 0.70f;
        } else if (prog < 0.13f) {
            g_lightning.flashIntensity = 0.15f;
        } else if (prog < 0.28f) {
            float tMain = (prog - 0.13f) / 0.15f;
            g_lightning.flashIntensity = 1.0f - tMain * 0.35f;
        } else if (prog < 0.36f) {
            g_lightning.flashIntensity = 0.35f;
        } else if (prog < 0.48f) {
            float tSec = (prog - 0.36f) / 0.12f;
            g_lightning.flashIntensity = 0.75f - tSec * 0.40f;
        } else if (prog < dur) {
            float tTail = (prog - 0.48f) / (dur - 0.48f);
            g_lightning.flashIntensity = 0.35f * (1.0f - tTail) * (1.0f - tTail);
        } else {
            g_lightning.active = false;
            g_lightning.flashIntensity = 0.0f;
            g_lightning.timer = 0.0f;
        }

#ifdef _WIN32
        if (g_lightning.thunderPending) {
            g_lightning.thunderCountdown -= g_deltaTime;
            if (g_lightning.thunderCountdown <= 0.0f) {
                g_lightning.thunderPending = false;
            }
        }
#endif
    } else {
        g_lightning.flashIntensity = 0.0f;
    }

    if (g_cinematicMode) {
        updateCinematicCamera(g_deltaTime);
    } else {
        processKeyboardInput(g_deltaTime);
    }

    glutPostRedisplay();
}


int main(int argc, char** argv) {    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--screenshot" && i + 1 < argc) {
            g_autoScreenshotFile = argv[i + 1];
            g_appState = STATE_SCENE;
            g_showHUD = false;
            i++;
        } else if (std::string(argv[i]) == "--interior") {
            g_cam.x = -7.50f; g_cam.y = 1.60f; g_cam.z = -9.20f;
            g_cam.yaw = -100.0f; g_cam.pitch = -4.0f;
        } else if (std::string(argv[i]) == "--frames" && i + 1 < argc) {
            g_autoScreenshotTargetFrames = std::stoi(argv[i + 1]);
            i++;
        } else if (std::string(argv[i]) == "--cam" && i + 5 < argc) {
            g_cam.x = std::stof(argv[i + 1]);
            g_cam.y = std::stof(argv[i + 2]);
            g_cam.z = std::stof(argv[i + 3]);
            g_cam.yaw = std::stof(argv[i + 4]);
            g_cam.pitch = std::stof(argv[i + 5]);
            i += 5;
        } else if (std::string(argv[i]) == "--hud") {
            g_showHUD = true;
        } else if (std::string(argv[i]) == "--no-tex") {
            g_texturesEnabled = false;
        } else if (std::string(argv[i]) == "--flashlight") {
            g_light2SpotOn = true;
        } else if (std::string(argv[i]) == "--lightning") {
            g_lightning.active = true;
            g_lightning.flashIntensity = 1.0f;
        } else if (std::string(argv[i]) == "--no-moon") {
            g_light1DirectionalOn = false;
        } else if (std::string(argv[i]) == "--no-lights") {
            g_light0PointOn = false;
            g_light1DirectionalOn = false;
            g_light2SpotOn = false;
            g_light3AreaOn = false;
            g_pumpkinLightsOn = false;
        } else if (std::string(argv[i]) == "--fov" && i + 1 < argc) {
            g_cam.fov = std::stof(argv[i + 1]);
            g_cam.targetFov = g_cam.fov;
            i++;
        } else if (std::string(argv[i]) == "--tour-time" && i + 1 < argc) {
            g_cinematicMode = true;
            g_cinematicTime = std::stof(argv[i + 1]);
            updateCinematicCamera(0.0f);
            i++;
        }
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(WINDOW_INIT_WIDTH, WINDOW_INIT_HEIGHT);
    glutInitWindowPosition(80, 50);
    glutCreateWindow("Horror House at Night - 3D 4-Light Scene (Jim 2107054)");

    initOpenGL();

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutIdleFunc(idleCallback);
    glutKeyboardFunc(keyboardDownCallback);
    glutKeyboardUpFunc(keyboardUpCallback);
    glutSpecialFunc(specialKeyDownCallback);
    glutSpecialUpFunc(specialKeyUpCallback);
    glutMotionFunc(mouseMotionCallback);
    glutPassiveMotionFunc(mousePassiveMotionCallback);
    glutMouseFunc(mouseButtonCallback);

    std::cout << "==========================================================" << std::endl;
    std::cout << "  HORROR HOUSE AT NIGHT - COMPUTER GRAPHICS PROJECT       " << std::endl;
    std::cout << "  Developer: MD Jahid Hasan Jim (Roll: 2107054)           " << std::endl;
    std::cout << "==========================================================" << std::endl;
    std::cout << "  [1] Toggle House Front Light (Porch Bulb)               " << std::endl;
    std::cout << "  [2] Toggle Directional Light (Moonlight)                " << std::endl;
    std::cout << "  [3/F] Toggle Spot Light (Flashlight)                    " << std::endl;
    std::cout << "  [4] Toggle Area Light (Window Interior Glow)            " << std::endl;
    std::cout << "  [5/K] Toggle Pumpkin Candle Lights                      " << std::endl;
    std::cout << "  [0] Master Toggle All Lights                            " << std::endl;
    std::cout << "  [T] Toggle Texture Mapping ON / OFF                     " << std::endl;
    std::cout << "  [G] Toggle Fog                                          " << std::endl;
    std::cout << "  [B] Toggle Bulb Sway & Flicker                          " << std::endl;
    std::cout << "  [C] Toggle Cinematic Auto-Tour Presentation             " << std::endl;
    std::cout << "  [L] Trigger Lightning Strike (Random + Manual)          " << std::endl;
    std::cout << "  [H] Toggle HUD Overlay                                  " << std::endl;
    std::cout << "  [P] Take Screenshot (.bmp)                              " << std::endl;
    std::cout << "  [R] Reset Camera to Reference Image Vantage Point       " << std::endl;
    std::cout << "  [W/A/S/D + Mouse] First-Person Exploration              " << std::endl;
    std::cout << "  [Mouse Scroll Wheel] Zoom In / Zoom Out (Dynamic FOV)   " << std::endl;
    std::cout << "==========================================================" << std::endl;

    glutMainLoop();
    return 0;
}
