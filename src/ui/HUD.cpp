#include "HUD.h"
#include "../core/Camera.h"
#include "../audio/AudioSystem.h"
#include "../graphics/TextureManager.h"
#include "../graphics/Lighting.h"
#include <cmath>
#include <cstdio>
#include <string>

void drawString2D(float x, float y, void* font, const char* str, float r, float g, float b, float a) {    glColor4f(r, g, b, a);
    glRasterPos2f(x, y);
    while (*str) {
        glutBitmapCharacter(font, *str);
        str++;
    }
}

void drawUIPanel(float x, float y, float w, float h, float r, float g, float b, float a) {    bindTexture(TEX_NONE);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();

    glColor4f(r * 2.2f + 0.15f, g * 2.2f + 0.15f, b * 2.2f + 0.25f, a * 1.5f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();
}

void drawCinematicColorGrade() {    bindTexture(TEX_NONE);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float w = (float)g_windowWidth;
    float h = (float)g_windowHeight;

    // Subtle cool blue-grey color cast for a cohesive cinematic night grade
    glBegin(GL_QUADS);
    glColor4f(0.035f, 0.065f, 0.135f, 0.13f);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(w,    0.0f);
    glVertex2f(w,    h);
    glVertex2f(0.0f, h);
    glEnd();

    // Electric blue-white screen flash during lightning strike
    if (g_lightning.flashIntensity > 0.01f) {
        glBegin(GL_QUADS);
        glColor4f(0.60f, 0.72f, 0.98f, 0.28f * g_lightning.flashIntensity);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(w,    0.0f);
        glVertex2f(w,    h);
        glVertex2f(0.0f, h);
        glEnd();
    }
}

void drawScreenVignette() {    bindTexture(TEX_NONE);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float w = (float)g_windowWidth;
    float h = (float)g_windowHeight;
    float inset = 95.0f;

    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glVertex2f(0.0f, 0.0f); glVertex2f(w, 0.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(w, inset); glVertex2f(0.0f, inset);

    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(0.0f, h - inset); glVertex2f(w, h - inset);
    glColor4f(0.0f, 0.0f, 0.0f, 0.85f);
    glVertex2f(w, h); glVertex2f(0.0f, h);

    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glVertex2f(0.0f, 0.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(inset, 0.0f); glVertex2f(inset, h);
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glVertex2f(0.0f, h);

    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(w - inset, 0.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glVertex2f(w, 0.0f); glVertex2f(w, h);
    glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    glVertex2f(w - inset, h);
    glEnd();
}

void renderTitleScreen() {    bindTexture(TEX_NONE);
    float w = (float)g_windowWidth;
    float h = (float)g_windowHeight;

    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    glColor4f(0.015f, 0.02f, 0.04f, 1.0f);
    glVertex2f(0.0f, 0.0f); glVertex2f(w, 0.0f);
    glColor4f(0.035f, 0.015f, 0.02f, 1.0f);
    glVertex2f(w, h); glVertex2f(0.0f, h);
    glEnd();

    drawScreenVignette();

    float cardW = 780.0f;
    float cardH = 475.0f;
    float cardX = (w - cardW) * 0.5f;
    float cardY = (h - cardH) * 0.5f;

    drawUIPanel(cardX, cardY, cardW, cardH, 0.04f, 0.05f, 0.08f, 0.88f);

    drawString2D(cardX + 190.0f, cardY + 55.0f,  GLUT_BITMAP_TIMES_ROMAN_24, "HORROR HOUSE AT NIGHT", 1.0f, 0.35f, 0.15f);
    drawString2D(cardX + 170.0f, cardY + 85.0f,  GLUT_BITMAP_HELVETICA_18,   "Advanced 4-Light & Texture Mapping Demo", 0.85f, 0.85f, 0.95f);

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor4f(0.9f, 0.45f, 0.1f, 0.7f);
    glVertex2f(cardX + 40.0f, cardY + 105.0f);
    glVertex2f(cardX + cardW - 40.0f, cardY + 105.0f);
    glEnd();

    drawString2D(cardX + 50.0f, cardY + 140.0f, GLUT_BITMAP_HELVETICA_12, "PROJECT       : Computer Graphics Sessional (CSE 4-1)", 0.75f, 0.85f, 1.0f);
    drawString2D(cardX + 50.0f, cardY + 165.0f, GLUT_BITMAP_HELVETICA_18, "DEVELOPER     : MD JAHID HASAN JIM", 1.0f, 0.85f, 0.35f);
    drawString2D(cardX + 50.0f, cardY + 190.0f, GLUT_BITMAP_HELVETICA_18, "ROLL NUMBER   : 2107054", 1.0f, 0.85f, 0.35f);

    drawUIPanel(cardX + 40.0f, cardY + 215.0f, cardW - 80.0f, 165.0f, 0.02f, 0.03f, 0.05f, 0.75f);
    drawString2D(cardX + 55.0f, cardY + 238.0f, GLUT_BITMAP_HELVETICA_12, "[Light 1] POINT LIGHT       : Hanging Porch Bulb (w=1.0, Attenuation, Moving Shadow & Pool)", 1.0f, 0.85f, 0.3f);
    drawString2D(cardX + 55.0f, cardY + 260.0f, GLUT_BITMAP_HELVETICA_12, "[Light 2] DIRECTIONAL LIGHT : Moonlight Sky (w=0.0, Low Angle, Casts Planar Shadows)", 0.4f, 0.75f, 1.0f);
    drawString2D(cardX + 55.0f, cardY + 282.0f, GLUT_BITMAP_HELVETICA_12, "[Light 3] SPOT LIGHT        : Flashlight (Positional, 22 deg Soft Cone, Follows Camera)", 0.9f, 0.95f, 1.0f);
    drawString2D(cardX + 55.0f, cardY + 304.0f, GLUT_BITMAP_HELVETICA_12, "[Light 4] AREA LIGHT EMUL.  : Parlor Window Glow (Elevated Ambient Dispersion)", 1.0f, 0.6f, 0.2f);
    drawString2D(cardX + 55.0f, cardY + 326.0f, GLUT_BITMAP_HELVETICA_12, "[Atmosphere] LIVING SCENE   : Drifting Ground Mist, Bats, Wind Sway, Lightning [L]", 0.3f, 0.9f, 0.9f);
    drawString2D(cardX + 55.0f, cardY + 348.0f, GLUT_BITMAP_HELVETICA_12, "+ GL_FOG Atmosphere, Phong Materials, Carved Jack-o'-Lanterns & Cinematic Camera Tour", 0.6f, 0.9f, 0.6f);

    float pulse = 0.6f + 0.4f * std::sin(g_time * 5.0f);
    drawString2D(cardX + 230.0f, cardY + 425.0f, GLUT_BITMAP_HELVETICA_18, ">> PRESS  [ ENTER ]  OR  [ SPACE ]  TO ENTER <<", 1.0f * pulse, 0.8f * pulse, 0.2f * pulse);
}

void renderSceneHUD() {    bindTexture(TEX_NONE);
    float w = (float)g_windowWidth;
    float h = (float)g_windowHeight;

    drawCinematicColorGrade();
    drawScreenVignette();

    if (!g_showHUD) {
        // Small collapsable indicator badge in corner
        drawUIPanel(w - 270.0f, 15.0f, 250.0f, 32.0f, 0.03f, 0.04f, 0.07f, 0.75f);
        drawString2D(w - 255.0f, 36.0f, GLUT_BITMAP_HELVETICA_12, "[ Press 'H' / 'TAB' : Expand HUD ]", 0.9f, 0.85f, 0.5f);
        return;
    }

    // Crosshair (+)
    glDisable(GL_LIGHTING);
    glLineWidth(1.5f);
    glColor4f(1.0f, 1.0f, 1.0f, 0.35f);
    glBegin(GL_LINES);
    glVertex2f(w * 0.5f - 8.0f, h * 0.5f);
    glVertex2f(w * 0.5f + 8.0f, h * 0.5f);
    glVertex2f(w * 0.5f, h * 0.5f - 8.0f);
    glVertex2f(w * 0.5f, h * 0.5f + 8.0f);
    glEnd();

    // Top Banner
    drawUIPanel(20.0f, 15.0f, 480.0f, 40.0f, 0.04f, 0.05f, 0.08f, 0.82f);
    drawString2D(35.0f, 40.0f, GLUT_BITMAP_HELVETICA_18, "HORROR HOUSE AT NIGHT", 1.0f, 0.4f, 0.15f);
    
    char fpsStr[32];
    snprintf(fpsStr, sizeof(fpsStr), "FPS: %.0f", g_fps);
    drawString2D(410.0f, 40.0f, GLUT_BITMAP_HELVETICA_12, fpsStr, 0.4f, 0.9f, 0.4f);

    // Left Panel: 4-Light Live Status Indicator Card
    drawUIPanel(20.0f, 65.0f, 480.0f, 215.0f, 0.03f, 0.04f, 0.07f, 0.85f);
    drawString2D(35.0f, 88.0f, GLUT_BITMAP_HELVETICA_12, "LIGHTING & ATMOSPHERE STATUS [1, 2, 3, 4, 5, 0, T]:", 0.9f, 0.85f, 0.6f);

    if (g_light0PointOn) {
        drawString2D(35.0f, 108.0f, GLUT_BITMAP_HELVETICA_12, "[1] House Front Light (Porch) : [ ON ] Warm Amber Glow & Light Pool", 0.2f, 1.0f, 0.3f);
    } else {
        drawString2D(35.0f, 108.0f, GLUT_BITMAP_HELVETICA_12, "[1] House Front Light (Porch) : [ OFF ]", 0.7f, 0.2f, 0.2f);
    }

    if (g_light1DirectionalOn) {
        drawString2D(35.0f, 128.0f, GLUT_BITMAP_HELVETICA_12, "[2] Directional (Moonlight)   : [ ON ] Cool Silvery Blue (Planar Shadows)", 0.4f, 0.8f, 1.0f);
    } else {
        drawString2D(35.0f, 128.0f, GLUT_BITMAP_HELVETICA_12, "[2] Directional (Moonlight)   : [ OFF ]", 0.7f, 0.2f, 0.2f);
    }

    if (g_light2SpotOn) {
        drawString2D(35.0f, 148.0f, GLUT_BITMAP_HELVETICA_12, "[3] Spot Light (Flashlight)   : [ ON ] Focused 22 deg Soft Cone [F]", 1.0f, 1.0f, 0.4f);
    } else {
        drawString2D(35.0f, 148.0f, GLUT_BITMAP_HELVETICA_12, "[3] Spot Light (Flashlight)   : [ OFF ] [F]", 0.7f, 0.2f, 0.2f);
    }

    if (g_light3AreaOn) {
        drawString2D(35.0f, 168.0f, GLUT_BITMAP_HELVETICA_12, "[4] Area Light Emul (Window)  : [ ON ] Soft Ambient Dispersion", 1.0f, 0.6f, 0.2f);
    } else {
        drawString2D(35.0f, 168.0f, GLUT_BITMAP_HELVETICA_12, "[4] Area Light Emul (Window)  : [ OFF ]", 0.7f, 0.2f, 0.2f);
    }

    if (g_pumpkinLightsOn) {
        drawString2D(35.0f, 188.0f, GLUT_BITMAP_HELVETICA_12, "[5] Pumpkin Candles [K]       : [ ON ] Warm Flickering Glow & Ground Pools", 1.0f, 0.75f, 0.2f);
    } else {
        drawString2D(35.0f, 188.0f, GLUT_BITMAP_HELVETICA_12, "[5] Pumpkin Candles [K]       : [ OFF ] Extinguished", 0.7f, 0.2f, 0.2f);
    }

    if (g_texturesEnabled) {
        drawString2D(35.0f, 208.0f, GLUT_BITMAP_HELVETICA_12, "[T] Texture Mapping (GL_MOD)  : [ ON ] Wood/Roof/Ground/Stone/Bark/Rust", 0.3f, 0.95f, 0.95f);
    } else {
        drawString2D(35.0f, 208.0f, GLUT_BITMAP_HELVETICA_12, "[T] Texture Mapping           : [ OFF ] Solid Phong Materials", 0.8f, 0.6f, 0.3f);
    }

    char featStr[160];
    snprintf(featStr, sizeof(featStr), "Fog: %s [G] | Guided Tour: %s [C] | Lightning: [L]",
             g_fogEnabled ? "ON" : "OFF",
             g_cinematicMode ? "ACTIVE" : "OFF");
    drawString2D(35.0f, 230.0f, GLUT_BITMAP_HELVETICA_12, featStr, 0.8f, 0.8f, 0.9f);

    drawString2D(35.0f, 252.0f, GLUT_BITMAP_HELVETICA_12, "Floors: 1st Floor Parlor + 2nd Floor (Dotola) Attic [V/I: Cycle Views]", 1.0f, 0.85f, 0.3f);

    if (g_lightning.active) {
        drawString2D(35.0f, 272.0f, GLUT_BITMAP_HELVETICA_12, ">> DISTANT LIGHTNING STRIKE ILLUMINATING SCENE <<", 0.9f, 0.95f, 1.0f);
    }

    // Right Controls Cheat-Sheet (Collapsable via [H] / [TAB])
    drawUIPanel(w - 380.0f, 15.0f, 360.0f, 225.0f, 0.03f, 0.04f, 0.07f, 0.85f);
    drawString2D(w - 365.0f, 35.0f,  GLUT_BITMAP_HELVETICA_12, "CONTROLS & SHORTCUTS [H / TAB: Hide]", 0.9f, 0.85f, 0.6f);
    drawString2D(w - 365.0f, 53.0f,  GLUT_BITMAP_HELVETICA_12, "W, A, S, D     : Walk (Walk Up Stairs to 2nd Floor!)", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 71.0f,  GLUT_BITMAP_HELVETICA_12, "Mouse Move     : Look Around (Yaw / Pitch)", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 89.0f,  GLUT_BITMAP_HELVETICA_12, "Mouse Scroll   : Zoom In / Zoom Out (Dynamic FOV)", 0.3f, 1.0f, 0.4f);
    drawString2D(w - 365.0f, 107.0f, GLUT_BITMAP_HELVETICA_12, "C / U          : FULL GUIDED SHOWCASE TOUR", 1.0f, 0.45f, 0.2f);
    drawString2D(w - 365.0f, 125.0f, GLUT_BITMAP_HELVETICA_12, "V / I          : Cycle View (Exterior / 1st / 2nd Floor)", 1.0f, 0.85f, 0.3f);
    drawString2D(w - 365.0f, 143.0f, GLUT_BITMAP_HELVETICA_12, "Space / Ctrl   : Fly Up / Fly Down", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 161.0f, GLUT_BITMAP_HELVETICA_12, "1, 2, 3, 4, 5  : Toggle Individual Lights", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 179.0f, GLUT_BITMAP_HELVETICA_12, "0: Master Lights | K: Pumpkin Candles", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 197.0f, GLUT_BITMAP_HELVETICA_12, "T: Textures | G: Fog | L: Lightning", 0.8f, 0.85f, 0.9f);
    drawString2D(w - 365.0f, 215.0f, GLUT_BITMAP_HELVETICA_12, "H / TAB: Collapse HUD | P: Screenshot | ESC", 0.8f, 0.85f, 0.9f);

    // Comprehensive Guided Showcase Tour HUD Card (Bottom Center)
    if (g_cinematicMode) {
        drawUIPanel(w * 0.5f - 380.0f, h - 85.0f, 760.0f, 68.0f, 0.04f, 0.03f, 0.06f, 0.92f);
        
        // Header title
        drawString2D(w * 0.5f - 360.0f, h - 60.0f, GLUT_BITMAP_HELVETICA_18, "FULL PROJECT GUIDED SHOWCASE TOUR", 1.0f, 0.55f, 0.20f);
        drawString2D(w * 0.5f + 190.0f, h - 60.0f, GLUT_BITMAP_HELVETICA_12, "[Press 'C' to Exit Tour]", 0.85f, 0.85f, 0.85f);

        // Stage description & dynamic action badge
        extern std::string g_tourStageTitle;
        extern std::string g_tourActionBadge;
        drawString2D(w * 0.5f - 360.0f, h - 35.0f, GLUT_BITMAP_HELVETICA_12, g_tourStageTitle.c_str(), 0.9f, 0.9f, 0.95f);
        
        char actionStr[160];
        snprintf(actionStr, sizeof(actionStr), "Action: %s", g_tourActionBadge.c_str());
        drawString2D(w * 0.5f + 10.0f, h - 35.0f, GLUT_BITMAP_HELVETICA_12, actionStr, 0.3f, 1.0f, 0.4f);

        // Progress bar line
        float prog = g_cinematicTime / 64.0f;
        if (prog > 1.0f) prog = 1.0f;
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glColor4f(0.3f, 0.3f, 0.3f, 0.8f);
        glVertex2f(w * 0.5f - 360.0f, h - 22.0f);
        glVertex2f(w * 0.5f + 360.0f, h - 22.0f);
        glColor4f(1.0f, 0.6f, 0.1f, 1.0f);
        glVertex2f(w * 0.5f - 360.0f, h - 22.0f);
        glVertex2f(w * 0.5f - 360.0f + 720.0f * prog, h - 22.0f);
        glEnd();
    }
}
