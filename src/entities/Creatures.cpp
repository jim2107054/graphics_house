#include "Creatures.h"
#include "Terrain.h"
#include "../core/Camera.h"
#include "../graphics/Material.h"
#include "../graphics/TextureManager.h"
#include "../graphics/Primitives.h"
#include <cmath>
#include <algorithm>
#include <iostream>

// Owl Proximity Escape State Variables
bool g_owlEscaped = false;
float g_owlTakeoffTime = 0.0f;

void resetOwl() {
    g_owlEscaped = false;
    g_owlTakeoffTime = 0.0f;
}

// ============================================================================
// DYNAMIC REALISTIC CREATURES (Prowling Black Cat & Great Horned Owl)
// Crafted with authentic anatomical fidelity, organic materials, and lifelike
// dynamic kinematics.
// ============================================================================

// ----------------------------------------------------------------------------
// MATERIALS FOR REALISTIC ANIMALS
// ----------------------------------------------------------------------------
static const Material MAT_BLACK_CAT_FUR = {
    { 0.04f, 0.04f, 0.05f, 1.0f },
    { 0.10f, 0.10f, 0.12f, 1.0f },
    { 0.35f, 0.35f, 0.38f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    72.0f // Sleek silky velvet fur sheen
};

static const Material MAT_CAT_EYE_GLOW = {
    { 0.15f, 0.80f, 0.25f, 1.0f },
    { 0.40f, 1.00f, 0.45f, 1.0f },
    { 0.85f, 1.00f, 0.85f, 1.0f },
    { 0.65f, 1.00f, 0.60f, 1.0f }, // Nocturnal tapetum lucidum luminescence
    128.0f
};

static const Material MAT_CAT_NOSE_PINK = {
    { 0.25f, 0.16f, 0.16f, 1.0f },
    { 0.55f, 0.34f, 0.34f, 1.0f },
    { 0.18f, 0.12f, 0.12f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    16.0f
};

static const Material MAT_OWL_FEATHER_MANTLE = {
    { 0.12f, 0.09f, 0.06f, 1.0f },
    { 0.32f, 0.23f, 0.16f, 1.0f },
    { 0.10f, 0.08f, 0.06f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    12.0f
};

static const Material MAT_OWL_CHEST_BARRED = {
    { 0.24f, 0.20f, 0.15f, 1.0f },
    { 0.65f, 0.56f, 0.42f, 1.0f },
    { 0.10f, 0.08f, 0.06f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    10.0f
};

static const Material MAT_OWL_FACIAL_DISC = {
    { 0.20f, 0.17f, 0.13f, 1.0f },
    { 0.52f, 0.45f, 0.35f, 1.0f },
    { 0.08f, 0.07f, 0.06f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    8.0f
};

static const Material MAT_OWL_DISC_RIM = {
    { 0.05f, 0.04f, 0.03f, 1.0f },
    { 0.14f, 0.10f, 0.07f, 1.0f },
    { 0.06f, 0.05f, 0.04f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    10.0f
};

static const Material MAT_OWL_EYE_AMBER = {
    { 0.95f, 0.65f, 0.08f, 1.0f },
    { 1.00f, 0.80f, 0.14f, 1.0f },
    { 1.00f, 0.98f, 0.70f, 1.0f },
    { 0.80f, 0.55f, 0.08f, 1.0f }, // Glowing nocturnal raptor glare
    120.0f
};

static const Material MAT_OWL_BEAK_SLATE = {
    { 0.06f, 0.06f, 0.07f, 1.0f },
    { 0.18f, 0.18f, 0.19f, 1.0f },
    { 0.30f, 0.30f, 0.32f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    36.0f
};

static const Material MAT_OWL_TALON_HORN = {
    { 0.04f, 0.04f, 0.04f, 1.0f },
    { 0.10f, 0.10f, 0.10f, 1.0f },
    { 0.35f, 0.35f, 0.35f, 1.0f },
    { 0.00f, 0.00f, 0.00f, 1.0f },
    48.0f
};

// ============================================================================
// 1. PROWLING BLACK CAT (Articulated Quadruped Kinematics & Sinuous Tail)
// ============================================================================

// Helper: Cat Digitigrade Leg with knee/hock bending and rounded paw
static void drawCatLeg(bool isFront, float swingAngle, float kneeAngle) {
    glPushMatrix();
    applyMaterial(MAT_BLACK_CAT_FUR);
    bindTexture(TEX_NONE);

    if (isFront) {
        // --- Front Leg ---
        // Shoulder blade / Scapula
        glRotatef(swingAngle, 1.0f, 0.0f, 0.0f);
        drawBeveledBox(0.038f, 0.11f, 0.045f, 0.008f);

        // Upper arm to elbow
        glTranslatef(0.0f, -0.09f, 0.0f);
        glRotatef(kneeAngle * 0.5f, 1.0f, 0.0f, 0.0f);
        drawCylinder(0.018f, 0.015f, 0.095f, 8);

        // Forearm to carpus / paw
        glTranslatef(0.0f, -0.085f, 0.005f);
        glRotatef(-kneeAngle * 0.4f, 1.0f, 0.0f, 0.0f);
        drawCylinder(0.015f, 0.013f, 0.085f, 8);

        // Front Paw (Rounded with subtle toe pad pads)
        glTranslatef(0.0f, -0.015f, 0.015f);
        drawSphere(0.022f, 8, 6);
    } else {
        // --- Rear Leg (Muscular feline thigh & high hock joint) ---
        glRotatef(swingAngle, 1.0f, 0.0f, 0.0f);
        // Thigh
        drawBeveledBox(0.046f, 0.12f, 0.055f, 0.010f);

        // Hock / Shank (Angled backward)
        glTranslatef(0.0f, -0.10f, -0.015f);
        glRotatef(25.0f + kneeAngle * 0.6f, 1.0f, 0.0f, 0.0f);
        drawCylinder(0.020f, 0.015f, 0.095f, 8);

        // Lower metatarsus to paw
        glTranslatef(0.0f, -0.085f, 0.010f);
        glRotatef(-35.0f - kneeAngle * 0.4f, 1.0f, 0.0f, 0.0f);
        drawCylinder(0.015f, 0.013f, 0.085f, 8);

        // Rear Paw
        glTranslatef(0.0f, -0.015f, 0.015f);
        drawSphere(0.022f, 8, 6);
    }
    glPopMatrix();
}

// Helper: Cat Head, Ears, Whiskers & Glowing Emerald Slit Eyes
static void drawCatHead(float lookYaw, float lookPitch) {
    glPushMatrix();
    glTranslatef(0.0f, 0.08f, 0.18f); // Neck crest
    glRotatef(lookYaw, 0.0f, 1.0f, 0.0f);
    glRotatef(lookPitch, 1.0f, 0.0f, 0.0f);

    applyMaterial(MAT_BLACK_CAT_FUR);
    bindTexture(TEX_NONE);

    // Cranium / Skull
    glPushMatrix();
    glScalef(1.05f, 0.95f, 1.0f);
    drawSphere(0.065f, 12, 10);
    glPopMatrix();

    // Tapered Muzzle / Whiskers Pads
    glPushMatrix();
    glTranslatef(0.0f, -0.018f, 0.055f);
    drawSphere(0.038f, 10, 8);

    // Pinkish/Black Nose Leather
    applyMaterial(MAT_CAT_NOSE_PINK);
    glTranslatef(0.0f, 0.015f, 0.032f);
    drawBox(0.015f, 0.010f, 0.012f);
    glPopMatrix();

    // Two Erect Pointed Triangular Ears with Pink Inner Hollow
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * 0.042f, 0.055f, 0.010f);
        glRotatef(side * -18.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(-8.0f, 1.0f, 0.0f, 0.0f);

        // Outer Ear Shell
        applyMaterial(MAT_BLACK_CAT_FUR);
        drawPrismRoof(0.045f, 0.065f, 0.035f);

        // Inner Ear Pinkish Lining
        applyMaterial(MAT_CAT_NOSE_PINK);
        glTranslatef(0.0f, 0.005f, 0.012f);
        drawPrismRoof(0.030f, 0.048f, 0.015f);
        glPopMatrix();
    }

    // Two Glowing Emerald Almond Eyes with Slit Pupils
    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * 0.028f, 0.015f, 0.055f);
        glRotatef(side * -12.0f, 0.0f, 1.0f, 0.0f);

        // Glowing Emerald Iris
        applyMaterial(MAT_CAT_EYE_GLOW);
        drawSphere(0.015f, 8, 6);

        // Vertical Black Slit Pupil
        applyMaterial(MAT_BLACK_CAT_FUR);
        glTranslatef(0.0f, 0.0f, 0.012f);
        drawBox(0.004f, 0.018f, 0.004f);
        glPopMatrix();
    }

    // Delicate Whiskers (Fine white lines sprouting from muzzle)
    glDisable(GL_LIGHTING);
    glColor3f(0.85f, 0.88f, 0.92f);
    glLineWidth(1.2f);
    glBegin(GL_LINES);
    for (int side = -1; side <= 1; side += 2) {
        float wx = side * 0.025f;
        float wy = -0.016f;
        float wz = 0.075f;
        // Upper whisker
        glVertex3f(wx, wy, wz);
        glVertex3f(wx + side * 0.09f, wy + 0.015f, wz - 0.01f);
        // Middle whisker
        glVertex3f(wx, wy, wz);
        glVertex3f(wx + side * 0.10f, wy - 0.005f, wz - 0.01f);
        // Lower whisker
        glVertex3f(wx, wy, wz);
        glVertex3f(wx + side * 0.085f, wy - 0.022f, wz - 0.01f);
    }
    glEnd();
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

// Master Black Cat Routine
void drawProwlingBlackCat() {
    // ------------------------------------------------------------------------
    // FENCE PATROL PATHWAY (Flanking the path near x ≈ -6.2f, z ≈ 15.2f)
    // The cat prowls back and forth atop the upper wooden fence rail!
    // ------------------------------------------------------------------------
    float railStartX = -7.40f, railStartZ = 15.55f;
    float railEndX   = -5.90f, railEndZ   = 15.15f;
    float railY = getTerrainHeight(-6.65f, 15.35f) + 1.25f; // Fence top rail height

    float pathDx = railEndX - railStartX;
    float pathDz = railEndZ - railStartZ;
    float pathLen = std::sqrt(pathDx * pathDx + pathDz * pathDz);
    float pathAngle = std::atan2(pathDz, pathDx) * (180.0f / 3.14159265f);

    // Continuous 20-second patrol state machine
    float cyclePeriod = 20.0f;
    float t = std::fmod(g_time, cyclePeriod);

    float posX = railStartX, posZ = railStartZ;
    float facingYaw = -pathAngle;
    float strideSpeed = 0.0f;
    float backArch = 0.0f;
    float lookYaw = 0.0f;
    float lookPitch = 0.0f;

    if (t < 7.0f) {
        // State 0: Prowl forward along fence rail (Start → End)
        float u = t / 7.0f;
        posX = railStartX + u * pathDx;
        posZ = railStartZ + u * pathDz;
        facingYaw = -pathAngle + 90.0f;
        strideSpeed = 1.0f;
        lookYaw = std::sin(g_time * 2.0f) * 8.0f; // Sinuous head bob
    } else if (t < 9.5f) {
        // State 1: Reached end post: Pauses, crouches alert, turns head towards path/player!
        posX = railEndX;
        posZ = railEndZ;
        facingYaw = -pathAngle + 90.0f;
        strideSpeed = 0.0f;
        backArch = 0.04f * std::sin((t - 7.0f) * 1.8f);
        lookYaw = -45.0f; // Look directly towards the pathway and player!
        lookPitch = -8.0f;
    } else if (t < 10.5f) {
        // State 2: Smooth 180° turn on the post
        posX = railEndX;
        posZ = railEndZ;
        float turnFrac = (t - 9.5f) / 1.0f;
        facingYaw = (-pathAngle + 90.0f) + turnFrac * 180.0f;
        strideSpeed = 0.2f;
    } else if (t < 17.5f) {
        // State 3: Prowl backward along fence rail (End → Start)
        float u = (t - 10.5f) / 7.0f;
        posX = railEndX - u * pathDx;
        posZ = railEndZ - u * pathDz;
        facingYaw = -pathAngle - 90.0f;
        strideSpeed = 1.0f;
        lookYaw = std::sin(g_time * 2.0f) * 8.0f;
    } else if (t < 19.2f) {
        // State 4: Reached start post: Sits down, arches back in classic Halloween feline arch!
        posX = railStartX;
        posZ = railStartZ;
        facingYaw = -pathAngle - 90.0f;
        strideSpeed = 0.0f;
        backArch = 0.07f * (1.0f - std::cos((t - 17.5f) * 3.7f));
        lookYaw = 35.0f; // Stare towards the spooky house and moon
        lookPitch = 12.0f;
    } else {
        // State 5: Pivot 180° to restart cycle
        posX = railStartX;
        posZ = railStartZ;
        float turnFrac = (t - 19.2f) / 0.8f;
        facingYaw = (-pathAngle - 90.0f) + turnFrac * 180.0f;
        strideSpeed = 0.2f;
    }

    glPushMatrix();
    glTranslatef(posX, railY, posZ);
    glRotatef(facingYaw, 0.0f, 1.0f, 0.0f);

    // Quadruped Walk Kinematics (Alternating 4-beat diagonal lateral sequence)
    float walkPhase = g_time * 7.5f * strideSpeed;
    float swingFL = std::sin(walkPhase) * 22.0f * strideSpeed;
    float swingRR = std::sin(walkPhase + 1.57f) * 22.0f * strideSpeed;
    float swingFR = std::sin(walkPhase + 3.14f) * 22.0f * strideSpeed;
    float swingRL = std::sin(walkPhase + 4.71f) * 22.0f * strideSpeed;

    float kneeFL = std::max(0.0f, std::sin(walkPhase)) * 32.0f * strideSpeed;
    float kneeRR = std::max(0.0f, std::sin(walkPhase + 1.57f)) * 32.0f * strideSpeed;
    float kneeFR = std::max(0.0f, std::sin(walkPhase + 3.14f)) * 32.0f * strideSpeed;
    float kneeRL = std::max(0.0f, std::sin(walkPhase + 4.71f)) * 32.0f * strideSpeed;

    // Body bob and breathing spine flex
    float spineBob = std::abs(std::sin(walkPhase * 2.0f)) * 0.015f * strideSpeed + backArch;
    glTranslatef(0.0f, 0.16f + spineBob, 0.0f);

    // --- SLEEK FELINE TORSO ---
    applyMaterial(MAT_BLACK_CAT_FUR);
    bindTexture(TEX_NONE);

    // 1. Ribcage / Chest Barrel
    glPushMatrix();
    glTranslatef(0.0f, 0.01f, 0.05f);
    glScalef(0.95f, 1.05f, 1.25f);
    drawSphere(0.075f, 10, 8);
    glPopMatrix();

    // 2. Tapered Waist / Flank (Arching with movement)
    glPushMatrix();
    glTranslatef(0.0f, 0.02f + backArch * 0.5f, -0.06f);
    drawBeveledBox(0.11f, 0.11f, 0.14f, 0.02f);
    glPopMatrix();

    // 3. Muscular Pelvic Rump
    glPushMatrix();
    glTranslatef(0.0f, 0.025f, -0.15f);
    glScalef(1.02f, 1.08f, 1.05f);
    drawSphere(0.068f, 10, 8);
    glPopMatrix();

    // --- HEAD & EYES ---
    drawCatHead(lookYaw, lookPitch);

    // --- 4 ARTICULATED DIGITIGRADE LEGS ---
    // Front-Left Leg
    glPushMatrix();
    glTranslatef(-0.048f, 0.0f, 0.09f);
    drawCatLeg(true, swingFL, kneeFL);
    glPopMatrix();

    // Front-Right Leg
    glPushMatrix();
    glTranslatef(0.048f, 0.0f, 0.09f);
    drawCatLeg(true, swingFR, kneeFR);
    glPopMatrix();

    // Rear-Left Leg
    glPushMatrix();
    glTranslatef(-0.048f, 0.01f, -0.15f);
    drawCatLeg(false, swingRL, kneeRL);
    glPopMatrix();

    // Rear-Right Leg
    glPushMatrix();
    glTranslatef(0.048f, 0.01f, -0.15f);
    drawCatLeg(false, swingRR, kneeRR);
    glPopMatrix();

    // --- SINUOUS MULTI-SEGMENT FLEXIBLE TAIL ---
    glPushMatrix();
    glTranslatef(0.0f, 0.045f, -0.21f); // Base of tail at sacrum
    int tailSegs = 6;
    float segLen = 0.055f;
    for (int s = 0; s < tailSegs; ++s) {
        // Wave-propagation equation for graceful harmonic S-curve
        float wave = std::sin(g_time * 3.4f - (float)s * 0.65f);
        float curlUp = (s > 2) ? (float)(s - 2) * 8.5f : 0.0f; // Inquisitive upward tip curl

        glRotatef(wave * 12.0f, 0.0f, 1.0f, 0.0f); // Lateral sway
        glRotatef(-8.0f + curlUp, 1.0f, 0.0f, 0.0f); // Vertical curvature
        drawCylinder(0.016f - s * 0.0018f, 0.014f - s * 0.0018f, segLen, 8);
        glTranslatef(0.0f, 0.0f, -segLen);
    }
    // Rounded tail tip
    drawSphere(0.010f, 6, 6);
    glPopMatrix();

    glPopMatrix(); // End Black Cat
}

// ============================================================================
// 2. GREAT HORNED OWL (Perched Raptorial Anatomy, Snap Head-Turns & Talons)
// ============================================================================

// Helper: Owl Raptor Talons with fluffy down-feather pantaloons clutching the timber post
static void drawOwlTalons(bool tucked) {
    applyMaterial(MAT_OWL_TALON_HORN);
    bindTexture(TEX_NONE);

    for (int foot = -1; foot <= 1; foot += 2) {
        float fs = (float)foot;
        glPushMatrix();
        glTranslatef(fs * 0.052f, tucked ? -0.02f : 0.0f, tucked ? -0.08f : 0.0f);

        if (tucked) {
            // In flight: legs tucked back under belly plumage
            glRotatef(-65.0f, 1.0f, 0.0f, 0.0f);
            glRotatef(fs * -12.0f, 0.0f, 0.0f, 1.0f);
        }

        // Fluffy down-feather "pantaloons" (thick plumage covering thigh & tarsus)
        applyMaterial(MAT_OWL_CHEST_BARRED);
        glPushMatrix();
        glScalef(1.15f, 1.35f, 1.15f);
        drawSphere(0.028f, 8, 6);
        glPopMatrix();

        // Ankle / Tarsus
        applyMaterial(MAT_OWL_TALON_HORN);
        drawCylinder(0.012f, 0.010f, 0.038f, 6);
        glTranslatef(0.0f, 0.008f, 0.0f);

        if (!tucked) {
            // 2 Forward Curved Toes wrapping tightly over front of post
            for (int toe = -1; toe <= 1; toe += 2) {
                float ts = (float)toe;
                glPushMatrix();
                glTranslatef(ts * 0.012f, 0.0f, 0.016f);
                glRotatef(44.0f, 1.0f, 0.0f, 0.0f);
                drawBox(0.009f, 0.009f, 0.038f);
                // Sharp black hooked raptor claw
                glTranslatef(0.0f, -0.010f, 0.022f);
                glRotatef(48.0f, 1.0f, 0.0f, 0.0f);
                drawBox(0.006f, 0.006f, 0.020f);
                glPopMatrix();
            }

            // 2 Rear Curved Claws wrapping backward
            for (int btoe = -1; btoe <= 1; btoe += 2) {
                float bts = (float)btoe;
                glPushMatrix();
                glTranslatef(bts * 0.011f, 0.0f, -0.016f);
                glRotatef(-44.0f, 1.0f, 0.0f, 0.0f);
                drawBox(0.009f, 0.009f, 0.034f);
                glPopMatrix();
            }
        } else {
            // Tucked curled claws in flight
            glPushMatrix();
            glTranslatef(0.0f, -0.015f, 0.010f);
            drawBox(0.016f, 0.012f, 0.020f);
            glPopMatrix();
        }

        glPopMatrix();
    }
}

// Helper: Owl Head with Parabolic Facial Disc, Wispy Plumicorns, Hooked Beak & Deep-Set Amber Eyes
static void drawOwlHead(float snapYaw, float cockRoll) {
    glPushMatrix();
    glTranslatef(0.0f, 0.27f, 0.02f); // Above shoulders
    glRotatef(snapYaw, 0.0f, 1.0f, 0.0f); // The signature owl head-swivel
    glRotatef(cockRoll, 0.0f, 0.0f, 1.0f); // Inquisitive head tilt

    // Broad Rounded Raptor Skull
    applyMaterial(MAT_OWL_FEATHER_MANTLE);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glScalef(1.06f, 0.94f, 1.02f);
    drawSphere(0.088f, 12, 10);
    glPopMatrix();

    // Dark Feather Collar Ruff framing back of head
    applyMaterial(MAT_OWL_DISC_RIM);
    glPushMatrix();
    glTranslatef(0.0f, -0.04f, -0.02f);
    glScalef(1.15f, 0.60f, 1.15f);
    drawSphere(0.080f, 10, 8);
    glPopMatrix();

    // Twin Concave Parabolic Facial Discs with Dark Outer Rim
    for (int eyeSide = -1; eyeSide <= 1; eyeSide += 2) {
        float es = (float)eyeSide;

        // Outer Dark Facial Ruff Rim (framing the parabolic dish)
        applyMaterial(MAT_OWL_DISC_RIM);
        glPushMatrix();
        glTranslatef(es * 0.038f, -0.005f, 0.065f);
        glRotatef(es * -10.0f, 0.0f, 1.0f, 0.0f);
        glScalef(1.15f, 1.25f, 0.20f);
        drawSphere(0.046f, 10, 8);
        glPopMatrix();

        // Inner Buff Acoustic Basin (recessed dish)
        applyMaterial(MAT_OWL_FACIAL_DISC);
        glPushMatrix();
        glTranslatef(es * 0.038f, -0.005f, 0.068f);
        glRotatef(es * -10.0f, 0.0f, 1.0f, 0.0f);
        glScalef(1.00f, 1.10f, 0.18f);
        drawSphere(0.042f, 10, 8);
        glPopMatrix();

        // Fierce Overhanging Feathered Brow Ridge (Predatory raptor scowl)
        applyMaterial(MAT_OWL_FEATHER_MANTLE);
        glPushMatrix();
        glTranslatef(es * 0.034f, 0.025f, 0.076f);
        glRotatef(es * -18.0f, 0.0f, 0.0f, 1.0f); // Angled down towards beak
        glRotatef(20.0f, 1.0f, 0.0f, 0.0f);       // Projecting forward over eye
        drawBox(0.030f, 0.012f, 0.022f);
        glPopMatrix();

        // Large Nocturnal Amber Eye (Recessed deep inside the basin)
        glPushMatrix();
        glTranslatef(es * 0.035f, 0.000f, 0.076f);

        // Luminous Amber-Gold Iris
        applyMaterial(MAT_OWL_EYE_AMBER);
        drawSphere(0.020f, 10, 8);

        // Deep Black Concentric Pupil
        applyMaterial(MAT_BLACK_CAT_FUR);
        glTranslatef(0.0f, 0.0f, 0.015f);
        drawSphere(0.010f, 8, 6);
        glPopMatrix();
    }

    // Downcurved Hooked Slate Raptor Beak nestled between discs
    applyMaterial(MAT_OWL_BEAK_SLATE);
    glPushMatrix();
    glTranslatef(0.0f, -0.018f, 0.082f);
    glRotatef(34.0f, 1.0f, 0.0f, 0.0f);
    drawPrismRoof(0.020f, 0.042f, 0.032f);
    // Sharp Hook Tip
    glTranslatef(0.0f, -0.024f, 0.014f);
    glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
    drawBox(0.010f, 0.022f, 0.012f);
    glPopMatrix();

    // Nasal Bristle Feathers covering base of beak
    applyMaterial(MAT_OWL_FACIAL_DISC);
    glPushMatrix();
    glTranslatef(0.0f, -0.002f, 0.088f);
    drawBox(0.018f, 0.018f, 0.016f);
    glPopMatrix();

    // WISPY, FEATHERY EAR TUFTS ("PLUMICORNS") - NO MORE BLOCKY CHIMNEYS!
    // Modeled as clusters of 3 curved, tapered, overlapping feather plumes
    applyMaterial(MAT_OWL_FEATHER_MANTLE);
    for (int horn = -1; horn <= 1; horn += 2) {
        float hs = (float)horn;

        glPushMatrix();
        glTranslatef(hs * 0.046f, 0.065f, 0.030f);
        glRotatef(hs * -26.0f, 0.0f, 0.0f, 1.0f); // Tilted outwards
        glRotatef(-22.0f, 1.0f, 0.0f, 0.0f);        // Swept backward

        // Plume 1: Tall leading feather plume
        glPushMatrix();
        drawPrismRoof(0.022f, 0.085f, 0.020f);
        glTranslatef(0.0f, 0.070f, -0.008f);
        glRotatef(hs * -8.0f, 0.0f, 0.0f, 1.0f);
        drawBox(0.012f, 0.035f, 0.010f); // Delicate tapered tip
        glPopMatrix();

        // Plume 2: Overlapping rear feather quill
        glPushMatrix();
        glTranslatef(hs * -0.008f, 0.010f, -0.015f);
        glRotatef(-8.0f, 1.0f, 0.0f, 0.0f);
        drawPrismRoof(0.018f, 0.070f, 0.016f);
        glTranslatef(0.0f, 0.055f, -0.006f);
        drawBox(0.010f, 0.025f, 0.008f);
        glPopMatrix();

        // Plume 3: Outer splayed feather quill
        glPushMatrix();
        glTranslatef(hs * 0.010f, 0.005f, -0.008f);
        glRotatef(hs * -15.0f, 0.0f, 0.0f, 1.0f);
        drawPrismRoof(0.016f, 0.055f, 0.014f);
        glPopMatrix();

        glPopMatrix();
    }

    glPopMatrix(); // End Owl Head
}

// Helper: Layered Folded Wings conforming naturally to the raptor's body
// (Completely eliminates the blocky rectangular boxes!)
static void drawFoldedRaptorWings(float ruffleAngle) {
    applyMaterial(MAT_OWL_FEATHER_MANTLE);
    bindTexture(TEX_NONE);

    for (int wing = -1; wing <= 1; wing += 2) {
        float ws = (float)wing;
        glPushMatrix();
        glTranslatef(ws * 0.105f, 0.18f, -0.01f);
        glRotatef(ws * -12.0f + ws * ruffleAngle, 0.0f, 0.0f, 1.0f);
        glRotatef(-16.0f, 1.0f, 0.0f, 0.0f); // Swept down-back along flank

        // 1. Curved Shoulder Cape / Scapular Coverts (wraps body contours)
        glPushMatrix();
        glScalef(0.65f, 1.25f, 0.85f);
        drawSphere(0.085f, 10, 8);
        glPopMatrix();

        // 2. Secondary Flight Feather Blanket (hugging flank)
        glPushMatrix();
        glTranslatef(0.0f, -0.06f, -0.015f);
        glRotatef(ws * 4.0f, 0.0f, 0.0f, 1.0f);
        glScalef(0.55f, 1.40f, 0.70f);
        drawSphere(0.075f, 8, 6);
        glPopMatrix();

        // 3. Layered Fan of 4 Overlapping Primary Flight Feathers
        // Tapering to an elegant pointed apex crossed above the tail!
        for (int p = 0; p < 4; ++p) {
            float pf = (float)p;
            glPushMatrix();
            glTranslatef(ws * (0.010f - pf * 0.005f), -0.10f - pf * 0.025f, -0.020f - pf * 0.015f);
            glRotatef(ws * (6.0f - pf * 2.0f), 0.0f, 0.0f, 1.0f);
            glRotatef(-12.0f - pf * 3.0f, 1.0f, 0.0f, 0.0f); // Tapering backward

            // Feather quill blade
            applyMaterial(MAT_OWL_FEATHER_MANTLE);
            drawBeveledBox(0.018f, 0.11f - pf * 0.010f, 0.040f - pf * 0.005f, 0.006f);

            // Subtle darker transverse feather bar on each quill
            applyMaterial(MAT_OWL_DISC_RIM);
            glTranslatef(0.0f, -0.02f, 0.002f);
            drawBox(0.019f, 0.020f, 0.038f - pf * 0.005f);
            glPopMatrix();
        }

        // 4. Pointed Wingtip Primary Apex (crossing over back)
        applyMaterial(MAT_OWL_FEATHER_MANTLE);
        glPushMatrix();
        glTranslatef(ws * -0.008f, -0.22f, -0.065f);
        glRotatef(ws * 14.0f, 0.0f, 0.0f, 1.0f); // Crossing inward toward midline
        glRotatef(-28.0f, 1.0f, 0.0f, 0.0f);
        drawBox(0.014f, 0.090f, 0.024f);
        glPopMatrix();

        glPopMatrix();
    }
}

// Helper: Massive Broad Raptor Wings Spread in Flight (1.35m wingspan with slotted primaries)
static void drawFlyingRaptorWings(float flapAngle) {
    applyMaterial(MAT_OWL_FEATHER_MANTLE);
    bindTexture(TEX_NONE);

    for (int wing = -1; wing <= 1; wing += 2) {
        float ws = (float)wing;
        glPushMatrix();
        glTranslatef(ws * 0.10f, 0.16f, 0.00f);
        glRotatef(ws * flapAngle, 0.0f, 0.0f, 1.0f); // Primary downstroke/upstroke

        // --- Inner Wing (Arm / Secondary Flight Feathers) ---
        glPushMatrix();
        glTranslatef(ws * 0.18f, 0.0f, -0.04f);
        glRotatef(ws * -8.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(6.0f, 1.0f, 0.0f, 0.0f); // Aerodynamic camber
        drawBeveledBox(0.24f, 0.025f, 0.22f, 0.010f);
        glPopMatrix();

        // --- Outer Wing (Forearm & Hand with Aeroelastic Flex) ---
        float tipFlex = -flapAngle * 0.35f; // Wingtip trails stroke
        glPushMatrix();
        glTranslatef(ws * 0.36f, 0.0f, -0.04f);
        glRotatef(ws * tipFlex, 0.0f, 0.0f, 1.0f);
        drawBeveledBox(0.22f, 0.020f, 0.18f, 0.008f);

        // --- 5 Slotted Primary Flight Feathers ("Wingtip Fingers") ---
        for (int f = 0; f < 5; ++f) {
            float ff = (float)f;
            glPushMatrix();
            glTranslatef(ws * (0.12f + ff * 0.035f), 0.005f, -0.06f - ff * 0.025f);
            glRotatef(ws * (15.0f + ff * 8.0f), 0.0f, 1.0f, 0.0f); // Splayed out like fingers
            glRotatef(ws * (flapAngle * 0.20f), 0.0f, 0.0f, 1.0f); // Primary feather curl
            drawBeveledBox(0.025f, 0.008f, 0.14f + ff * 0.015f, 0.004f);
            glPopMatrix();
        }
        glPopMatrix();

        glPopMatrix();
    }
}

// Master Perched Owl Routine with Proximity Escape System
void drawPerchedOwl() {
    // ------------------------------------------------------------------------
    // PERCH LOCATION: Dedicated rustic timber post flanking the entrance path (x = 3.45f, z = 18.40f)
    // ------------------------------------------------------------------------
    float perchX =  3.45f;
    float perchZ = 18.40f;
    float gy = getTerrainHeight(perchX, perchZ);
    float postH = 1.95f;
    float perchY = gy + postH;

    // --- 1. WEATHERED TIMBER PERCH POST (Always renders on lawn) ---
    glPushMatrix();
    glTranslatef(perchX, gy, perchZ);
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glPushMatrix();
    glTranslatef(0.0f, postH * 0.5f, 0.0f);
    drawBox(0.18f, postH, 0.18f);
    glPopMatrix();

    // Aged iron collar banding near the top of the timber post
    applyMaterial(MAT_BLACK_IRON);
    bindTexture(TEX_NONE);
    glPushMatrix();
    glTranslatef(0.0f, postH * 0.88f, 0.0f);
    drawBox(0.20f, 0.04f, 0.20f);
    glPopMatrix();

    // Weathered timber post cap with subtle beveled top
    applyMaterial(MAT_DARK_WOOD);
    bindTexture(TEX_WALL);
    glPushMatrix();
    glTranslatef(0.0f, postH + 0.01f, 0.0f);
    drawBeveledBox(0.19f, 0.025f, 0.19f, 0.006f);
    glPopMatrix();
    glPopMatrix();

    // --- 2. PROXIMITY DETECTION TO PLAYER CAMERA ---
    float camDx = g_cam.x - perchX;
    float camDz = g_cam.z - perchZ;
    float camDist = std::sqrt(camDx * camDx + camDz * camDz);

    // If player approaches within 3.2 meters, startle the owl into escape flight!
    if (camDist < 3.2f && !g_owlEscaped) {
        g_owlEscaped = true;
        g_owlTakeoffTime = g_time;
        std::cout << "[CREATURE] Great Horned Owl startled! Taking flight into the night sky..." << std::endl;
    }

    // --- 3. STATE MACHINE RENDERING ---
    if (!g_owlEscaped) {
        // ====================================================================
        // STATE A: PERCHED PEACEFULLY ON TIMBER POST
        // ====================================================================
        float period = 16.0f;
        float timeMod = std::fmod(g_time, period);

        float targetYaw = 0.0f;
        float targetRoll = 0.0f;

        if (timeMod < 3.5f) {
            // Phase 1: Staring forward down the visitor pathway
            targetYaw = -25.0f;
            targetRoll = 3.0f;
        } else if (timeMod < 4.0f) {
            // Fast snap turn to left (Cemetery & Wrecked Car across the lawn)
            float s = (timeMod - 3.5f) / 0.5f;
            targetYaw = -25.0f + s * (-65.0f);
            targetRoll = 3.0f + s * (-9.0f);
        } else if (timeMod < 7.5f) {
            // Phase 2: Scanning the graveyard knolls and rusted car wreck
            targetYaw = -90.0f;
            targetRoll = -6.0f;
        } else if (timeMod < 8.2f) {
            // Deep snap turn all the way over shoulder (140 degrees backward!)
            float s = (timeMod - 7.5f) / 0.7f;
            targetYaw = -90.0f + s * (-70.0f);
            targetRoll = -6.0f + s * (16.0f);
        } else if (timeMod < 11.5f) {
            // Phase 3: Watching the dark woods behind the post
            targetYaw = -160.0f;
            targetRoll = 10.0f;
        } else if (timeMod < 12.2f) {
            // Snap turn across to look at the manor house & glowing windows
            float s = (timeMod - 11.5f) / 0.7f;
            targetYaw = -160.0f + s * (205.0f);
            targetRoll = 10.0f + s * (-18.0f);
        } else if (timeMod < 15.2f) {
            // Phase 4: Staring at the front porch, stairs and candelabra
            targetYaw = 45.0f;
            targetRoll = -8.0f;
        } else {
            // Phase 5: Snap back to pathway
            float s = (timeMod - 15.2f) / 0.8f;
            targetYaw = 45.0f + s * (-70.0f);
            targetRoll = -8.0f + s * (11.0f);
        }

        // Respiratory Chest Rise and Fall
        float breath = std::sin(g_time * 2.4f) * 0.012f;

        // Periodic Wing Ruffle / Shrug every 8 seconds
        float wingRuffle = 0.0f;
        float ruffleCycle = std::fmod(g_time, 8.0f);
        if (ruffleCycle < 0.6f) {
            wingRuffle = std::sin(ruffleCycle * (3.14159f / 0.6f)) * 9.0f;
        }

        glPushMatrix();
        glTranslatef(perchX, perchY + 0.02f, perchZ);
        glRotatef(-15.0f, 0.0f, 1.0f, 0.0f); // Body posture angled towards the lawn path

        // Talons clutching timber
        drawOwlTalons(false);

        // Teardrop Raptor Torso
        glTranslatef(0.0f, 0.04f, 0.0f);
        applyMaterial(MAT_OWL_FEATHER_MANTLE);
        bindTexture(TEX_NONE);
        glPushMatrix();
        glTranslatef(0.0f, 0.16f + breath, 0.0f);
        glRotatef(-14.0f, 1.0f, 0.0f, 0.0f);
        glScalef(1.05f, 1.45f, 1.0f);
        drawSphere(0.125f, 12, 10);
        glPopMatrix();

        // Barred Buff Chest Plumage (Front Bib)
        applyMaterial(MAT_OWL_CHEST_BARRED);
        glPushMatrix();
        glTranslatef(0.0f, 0.17f + breath * 1.5f, 0.055f);
        glRotatef(-16.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.92f, 1.25f, 0.55f);
        drawSphere(0.105f, 10, 8);
        glPopMatrix();

        // Organic Folded Wings with Primary Flight Quills (No blocky boxes!)
        drawFoldedRaptorWings(wingRuffle);

        // Wedge-Shaped Tail Feathers
        glPushMatrix();
        glTranslatef(0.0f, 0.06f, -0.10f);
        glRotatef(-28.0f, 1.0f, 0.0f, 0.0f);
        drawBeveledBox(0.09f, 0.18f, 0.025f, 0.008f);
        glPopMatrix();

        // Articulated Head with Parabolic Facial Disc, Wispy Ear Tufts & Glowing Eyes
        drawOwlHead(targetYaw, targetRoll);

        glPopMatrix();

    } else {
        // ====================================================================
        // STATE B: STARTLED ESCAPE FLIGHT INTO THE NIGHT SKY
        // ====================================================================
        float ft = g_time - g_owlTakeoffTime;

        if (ft < 0.28f) {
            // Phase 1: Startled crouch & wings burst open on the post!
            float crouch = ft / 0.28f;
            glPushMatrix();
            glTranslatef(perchX, perchY + 0.02f - crouch * 0.04f, perchZ);
            glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);

            drawOwlTalons(false);

            glTranslatef(0.0f, 0.04f, 0.0f);
            applyMaterial(MAT_OWL_FEATHER_MANTLE);
            bindTexture(TEX_NONE);
            glPushMatrix();
            glTranslatef(0.0f, 0.14f, 0.0f);
            glRotatef(-22.0f, 1.0f, 0.0f, 0.0f);
            glScalef(1.05f, 1.45f, 1.0f);
            drawSphere(0.125f, 12, 10);
            glPopMatrix();

            // Wings unfurling
            float spreadFlap = crouch * -25.0f;
            drawFlyingRaptorWings(spreadFlap);

            // Head looking alertly at player
            drawOwlHead(-camDx * 8.0f, 0.0f);
            glPopMatrix();

        } else if (ft < 14.0f) {
            // Phase 2: Escaping flight climbing steeply into the night sky!
            float t = ft - 0.28f;

            // Escape trajectory climbing towards the dark forest / distant sky
            float escX = perchX + 4.2f * t + 1.2f * std::sin(t * 1.4f);
            float escY = perchY + 1.5f * t + 0.32f * t * t; // Smooth parabolic altitude climb
            float escZ = perchZ - 6.5f * t; // Flying away into the distance

            float vx = 4.2f + 1.68f * std::cos(t * 1.4f);
            float vy = 1.5f + 0.64f * t;
            float vz = -6.5f;

            float speedH = std::sqrt(vx * vx + vz * vz);
            float flightYaw = std::atan2(vx, -vz) * (180.0f / 3.14159265f);
            float flightPitch = -std::atan2(vy, speedH) * (180.0f / 3.14159265f);
            float flightRoll = -16.0f + 8.0f * std::sin(t * 1.8f);
            float flapAngle = std::sin(t * 8.0f) * 44.0f; // Broad, deep wingbeats

            glPushMatrix();
            glTranslatef(escX, escY, escZ);
            glRotatef(flightYaw,   0.0f, 1.0f, 0.0f);
            glRotatef(flightPitch, 1.0f, 0.0f, 0.0f);
            glRotatef(flightRoll,  0.0f, 0.0f, 1.0f);

            // Tucked talons in flight
            drawOwlTalons(true);

            // Streamlined Torso in flight
            applyMaterial(MAT_OWL_FEATHER_MANTLE);
            bindTexture(TEX_NONE);
            glPushMatrix();
            glRotatef(12.0f, 1.0f, 0.0f, 0.0f);
            glScalef(0.95f, 0.85f, 1.45f);
            drawSphere(0.125f, 12, 10);
            glPopMatrix();

            // Broad fanned tail rectrices for flight stability
            glPushMatrix();
            glTranslatef(0.0f, 0.02f, -0.16f);
            glRotatef(18.0f, 1.0f, 0.0f, 0.0f);
            drawBeveledBox(0.18f, 0.015f, 0.22f, 0.006f);
            glPopMatrix();

            // Massive Spreading Raptor Wings flapping in flight
            drawFlyingRaptorWings(flapAngle);

            // Head looking forward in flight direction
            drawOwlHead(0.0f, 0.0f);

            glPopMatrix();
        }
        // Beyond 14 seconds, the owl has flown completely out of the world bounds, post is empty!
    }
}

// ----------------------------------------------------------------------------
// MASTER DISPATCH ROUTINE
// ----------------------------------------------------------------------------
void drawAllCreatures() {
    drawProwlingBlackCat();
    drawPerchedOwl();
}
