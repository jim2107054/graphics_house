#include "Config.h"

AppState g_appState = STATE_TITLE;

int g_windowWidth  = WINDOW_INIT_WIDTH;
int g_windowHeight = WINDOW_INIT_HEIGHT;

float g_time = 0.0f;
float g_deltaTime = 0.016f;
int   g_prevTimeMs = 0;
int   g_frameCount = 0;
float g_fps = 60.0f;
float g_fpsTimer = 0.0f;

bool g_keyState[256] = { false };
bool g_isCtrlPressed = false;
int  g_lastMouseX = -1;
int  g_lastMouseY = -1;

bool g_light0PointOn       = true;  // Porch Bulb
bool g_light1DirectionalOn = true;  // Moonlight
bool g_light2SpotOn        = false; // Flashlight
bool g_light3AreaOn        = true;  // Window Glow
bool g_light4CandleOn      = true;  // 2nd Floor Alchemist Candelabra & Candlelight
bool g_light5LanternOn     = true;  // 2nd Floor Hanging Brass Lantern
bool g_pumpkinLightsOn     = true;  // Jack-o'-Lantern Candle Lights

bool g_texturesEnabled     = true;
bool g_fogEnabled          = false; // Permanently disabled - No fog
bool g_bulbAnimEnabled     = true;
bool g_showHUD             = true;
bool g_cinematicMode       = false;
float g_cinematicTime      = 0.0f;

std::string g_autoScreenshotFile = "";
int g_autoScreenshotFrames = 0;

float g_bulbBaseX = -4.6f;
float g_bulbBaseY = 3.95f;
float g_bulbBaseZ = 5.7f;
float g_bulbCordLength = 0.8f;
float g_bulbCurX = -4.6f;
float g_bulbCurY = 3.15f;
float g_bulbCurZ = 5.7f;
float g_bulbFlickerFactor = 1.0f;
float g_pumpkinFlicker = 1.0f;

float g_moonDir[4] = { -0.07f, 0.44f, -0.90f, 0.0f };
std::string g_tourStageTitle = "1/8: Exterior Overview & Moonlit Atmosphere";
