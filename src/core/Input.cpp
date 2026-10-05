#include "Input.h"
#include "Camera.h"
#include "../audio/AudioSystem.h"
#include "../graphics/Lighting.h"
#include "../ui/Screenshot.h"
#include <iostream>

void keyboardDownCallback(unsigned char key, int x, int y) {    (void)x; (void)y;
    g_keyState[key] = true;

    if (g_appState == STATE_TITLE) {
        if (key == 13 || key == ' ') {
            g_appState = STATE_SCENE;
#ifdef _WIN32
            playAmbientAudio();
#endif
        }
        if (key == 27) exit(0);
        return;
    }

    switch (key) {
        case 27: // ESC
            exit(0);
            break;
        case '1':
            g_light0PointOn = !g_light0PointOn;
            std::cout << "[LIGHT 0] Point Light (Porch Bulb) : " << (g_light0PointOn ? "ON" : "OFF") << std::endl;
            break;
        case '2':
            g_light1DirectionalOn = !g_light1DirectionalOn;
            std::cout << "[LIGHT 1] Directional (Moonlight)   : " << (g_light1DirectionalOn ? "ON" : "OFF") << std::endl;
            break;
        case '3':
        case 'f':
        case 'F':
            g_light2SpotOn = !g_light2SpotOn;
            std::cout << "[LIGHT 2] Spot Light (Flashlight)   : " << (g_light2SpotOn ? "ON" : "OFF") << std::endl;
            break;
        case '4':
            g_light3AreaOn = !g_light3AreaOn;
            std::cout << "[LIGHT 3] Area Light (Window Glow)  : " << (g_light3AreaOn ? "ON" : "OFF") << std::endl;
            break;
        case '5':
        case 'k':
        case 'K':
            g_pumpkinLightsOn = !g_pumpkinLightsOn;
            std::cout << "[LIGHT 4] Pumpkin Candles (Jack-o'-Lanterns) : " << (g_pumpkinLightsOn ? "ON (Glowing)" : "OFF (Extinguished)") << std::endl;
            break;
        case '0': {
            bool anyOn = g_light0PointOn || g_light1DirectionalOn || g_light2SpotOn || g_light3AreaOn || g_pumpkinLightsOn;
            g_light0PointOn = g_light1DirectionalOn = g_light2SpotOn = g_light3AreaOn = g_pumpkinLightsOn = !anyOn;
            std::cout << "[LIGHTS] Master Toggle : " << (!anyOn ? "ALL ON" : "ALL OFF") << std::endl;
            break;
        }
        case 't':
        case 'T':
            g_texturesEnabled = !g_texturesEnabled;
            std::cout << "[TEXTURES] Texture Mapping : " << (g_texturesEnabled ? "ON (GL_MODULATE)" : "OFF (Materials Only)") << std::endl;
            break;
        case 'g':
        case 'G':
            g_fogEnabled = !g_fogEnabled;
            std::cout << "[ATMOSPHERE] Fog : " << (g_fogEnabled ? "ON" : "OFF") << std::endl;
            break;
        case 'b':
        case 'B':
            g_bulbAnimEnabled = !g_bulbAnimEnabled;
            std::cout << "[ANIMATION] Bulb Sway & Flicker : " << (g_bulbAnimEnabled ? "ON" : "OFF") << std::endl;
            break;
        case 'c':
        case 'C':
        case 'u':
        case 'U':
            g_cinematicMode = !g_cinematicMode;
            if (g_cinematicMode) g_cinematicTime = 0.0f;
            std::cout << "[CAMERA] Guided Showcase Tour : " << (g_cinematicMode ? "ACTIVE (8-Stage Comprehensive Tour)" : "DISABLED (Manual Exploration)") << std::endl;
            break;
        case 'l':
        case 'L':
            triggerLightning();
            break;
#ifdef _WIN32
        case 'm':
        case 'M':
            toggleAudio();
            break;
#endif
        case 'p':
        case 'P': {
            static int ssCount = 1;
            char ssName[64];
            snprintf(ssName, sizeof(ssName), "horror_house_%03d.bmp", ssCount++);
            saveScreenshot(ssName);
            break;
        }
        case 'h':
        case 'H':
        case 9: // TAB key
            g_showHUD = !g_showHUD;
            std::cout << "[HUD] Controls & Shortcuts Display : " << (g_showHUD ? "EXPANDED (VISIBLE)" : "COLLAPSED (HIDDEN)") << std::endl;
            break;
        case 'r':
        case 'R':
            g_cam.x = 1.2f; g_cam.y = 1.35f; g_cam.z = 22.0f;
            g_cam.yaw = -94.0f; g_cam.pitch = 5.0f;
            g_cam.fov = 52.0f; g_cam.targetFov = 52.0f;
            g_cinematicMode = false;
            g_camFloorState = 0;
            std::cout << "[CAMERA] Reset to Exterior Yard Vantage Point (FOV Reset)" << std::endl;
            break;
        case 'v':
        case 'V':
        case 'i':
        case 'I': {
            g_camFloorState = (g_camFloorState + 1) % 3;
            g_cinematicMode = false;
            if (g_camFloorState == 1) {
                // 1st Floor Ground Parlor Interior
                g_cam.x = -7.50f; g_cam.y = 2.43f; g_cam.z = -9.20f;
                g_cam.yaw = -100.0f; g_cam.pitch = -4.0f;
                std::cout << "[CAMERA] Switched to 1st Floor Parlor Interior" << std::endl;
            } else if (g_camFloorState == 2) {
                // 2nd Floor (Dotola) Attic Bedroom & Study Interior
                g_cam.x = -7.72f; g_cam.y = 5.85f; g_cam.z = -8.85f;
                g_cam.yaw = -95.0f; g_cam.pitch = -4.0f;
                std::cout << "[CAMERA] Switched to 2nd Floor (Dotola) Attic Bedroom & Witchcraft Study" << std::endl;
            } else {
                // Exterior Yard Vantage Point
                g_cam.x = 1.2f; g_cam.y = 1.35f; g_cam.z = 22.0f;
                g_cam.yaw = -94.0f; g_cam.pitch = 5.0f;
                std::cout << "[CAMERA] Reset to Exterior Yard Vantage Point" << std::endl;
            }
            break;
        }
    }
}

void keyboardUpCallback(unsigned char key, int x, int y) {    (void)x; (void)y;
    g_keyState[key] = false;
}

void specialKeyDownCallback(int key, int x, int y) {    (void)x; (void)y;
    int mod = glutGetModifiers();
    if (mod & GLUT_ACTIVE_CTRL) {
        g_isCtrlPressed = true;
    }
    if (key == GLUT_KEY_PAGE_DOWN || key == GLUT_KEY_DOWN) {
        g_isCtrlPressed = true;
    }
}

void specialKeyUpCallback(int key, int x, int y) {    (void)x; (void)y;
    int mod = glutGetModifiers();
    if (!(mod & GLUT_ACTIVE_CTRL)) {
        g_isCtrlPressed = false;
    }
    if (key == GLUT_KEY_PAGE_DOWN || key == GLUT_KEY_DOWN) {
        g_isCtrlPressed = false;
    }
}

void mouseMotionCallback(int x, int y) {    if (g_appState != STATE_SCENE || g_cinematicMode) return;

    if (g_lastMouseX == -1 || g_lastMouseY == -1) {
        g_lastMouseX = x;
        g_lastMouseY = y;
        return;
    }

    float dx = (float)(x - g_lastMouseX);
    float dy = (float)(y - g_lastMouseY);

    g_lastMouseX = x;
    g_lastMouseY = y;

    g_cam.yaw   += dx * g_cam.sens;
    g_cam.pitch -= dy * g_cam.sens;

    if (g_cam.pitch >  85.0f) g_cam.pitch =  85.0f;
    if (g_cam.pitch < -85.0f) g_cam.pitch = -85.0f;
}

void mousePassiveMotionCallback(int x, int y) {    mouseMotionCallback(x, y);
}

void mouseButtonCallback(int button, int state, int x, int y) {    (void)x; (void)y;
    // Mouse Scroll Wheel Zoom (like standard 3D games)
    if (button == 3) { // Wheel Up -> Zoom In
        g_cam.targetFov -= 3.5f;
        if (g_cam.targetFov < 18.0f) g_cam.targetFov = 18.0f;
        glutPostRedisplay();
    } else if (button == 4) { // Wheel Down -> Zoom Out
        g_cam.targetFov += 3.5f;
        if (g_cam.targetFov > 85.0f) g_cam.targetFov = 85.0f;
        glutPostRedisplay();
    }

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (g_appState == STATE_TITLE) {
            g_appState = STATE_SCENE;
#ifdef _WIN32
            playAmbientAudio();
#endif
        } else {
            g_light2SpotOn = !g_light2SpotOn;
        }
    }
    if (state == GLUT_UP) {
        g_lastMouseX = -1;
        g_lastMouseY = -1;
    }
}

