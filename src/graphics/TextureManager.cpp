#include "TextureManager.h"
#include <iostream>

#include "stb_image.h"

GLuint g_textureHandles[TEX_COUNT] = { 0 };

void bindTexture(TextureID id) {    if (!g_texturesEnabled || id == TEX_NONE || g_textureHandles[id] == 0) {
        glDisable(GL_TEXTURE_2D);
    } else {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, g_textureHandles[id]);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
}

// Generate high-resolution procedural textures in memory as robust fallbacks
void generateProceduralTexture(TextureID id, int width, int height, std::vector<unsigned char>& data) {    data.resize(width * height * 4);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 4;
            unsigned char r = 200, g = 200, b = 200, a = 255;

            switch (id) {
                case TEX_WALL: {
                    // Weathered dark charcoal-brown horizontal wood planks with grain & seams (Reference Image)
                    int plankHeight = height / 8;
                    int plankIdx = y / plankHeight;
                    int lineInPlank = y % plankHeight;
                    float grain = 0.88f + 0.12f * std::sin(x * 0.25f + std::sin(y * 0.05f) * 4.0f);
                    float woodR = 46 + (plankIdx * 3) % 8;
                    float woodG = 42 + (plankIdx * 2) % 6;
                    float woodB = 38 + (plankIdx * 2) % 6;

                    if (lineInPlank < 2 || lineInPlank > plankHeight - 3) {
                        // Dark plank seam/crack
                        woodR *= 0.40f; woodG *= 0.40f; woodB *= 0.40f;
                    }
                    r = (unsigned char)(woodR * grain);
                    g = (unsigned char)(woodG * grain);
                    b = (unsigned char)(woodB * grain);
                    break;
                }
                case TEX_ROOF: {
                    // Deep slate near-black shingles pattern (Reference Image)
                    int shingleH = height / 12;
                    int shingleW = width / 8;
                    int row = y / shingleH;
                    int col = (x + (row % 2) * (shingleW / 2)) / shingleW;
                    int inX = (x + (row % 2) * (shingleW / 2)) % shingleW;
                    int inY = y % shingleH;

                    float tileShade = 0.85f + 0.15f * std::sin((float)(row * 17 + col * 23));
                    float base = 30.0f * tileShade;
                    if (inX < 2 || inY < 2) base *= 0.45f; // Shingle borders

                    r = (unsigned char)(base * 0.90f);
                    g = (unsigned char)(base * 0.95f);
                    b = (unsigned char)(base * 1.05f);
                    break;
                }
                case TEX_GROUND: {
                    // Dark damp midnight soil / earth with subtle dark noise (zero bright green)
                    float n1 = std::sin(x * 0.15f) * std::cos(y * 0.15f);
                    float n2 = std::sin(x * 0.4f + y * 0.3f);
                    float noise = 0.8f + 0.2f * (n1 + n2 * 0.5f);
                    r = (unsigned char)(22.0f * noise);
                    g = (unsigned char)(26.0f * noise);
                    b = (unsigned char)(22.0f * noise);
                    break;
                }
                case TEX_STONE: {
                    // Weathered cobblestone / rock flagstones
                    int cellW = width / 6;
                    int cellH = height / 6;
                    int cx = x % cellW;
                    int cy = y % cellH;
                    float distCenter = std::sqrt((float)((cx - cellW/2)*(cx - cellW/2) + (cy - cellH/2)*(cy - cellH/2)));
                    float stoneBase = 120.0f + 25.0f * std::sin(x * 0.1f + y * 0.1f);
                    if (cx < 3 || cy < 3 || distCenter > cellW * 0.48f) {
                        stoneBase *= 0.45f; // Mortar groove
                    }
                    r = (unsigned char)(stoneBase * 0.92f);
                    g = (unsigned char)(stoneBase * 0.95f);
                    b = (unsigned char)(stoneBase * 1.05f);
                    break;
                }
                case TEX_BARK: {
                    // Deep vertical tree bark ridges and grooves
                    float ridge = std::sin(x * 0.35f + std::sin(y * 0.08f) * 6.0f);
                    float base = 75.0f + 35.0f * ridge;
                    r = (unsigned char)(base * 0.95f);
                    g = (unsigned char)(base * 0.80f);
                    b = (unsigned char)(base * 0.65f);
                    break;
                }
                case TEX_RUST: {
                    // Peeling vintage paint, deep iron oxidation rust, and lower mud splatter
                    float p1 = std::sin(x * 0.12f + y * 0.08f);
                    float p2 = std::cos(x * 0.25f - y * 0.20f);
                    float p3 = std::sin(x * 0.45f + std::sin(y * 0.35f) * 2.0f);
                    float rustPatch = p1 * p2 + p3 * 0.22f;

                    // Vertical mud / dirt gradient near lower vehicle base (y < height * 0.38)
                    float dirtFactor = 0.0f;
                    if (y < height * 0.38f) {
                        float v = 1.0f - (float)y / (height * 0.38f);
                        dirtFactor = v * (0.75f + 0.25f * std::sin(x * 0.32f));
                    }

                    if (dirtFactor > 0.38f) {
                        // Dark damp earth grime & mud splatters near wheels and rocker panels
                        r = (unsigned char)(42.0f + 14.0f * p1);
                        g = (unsigned char)(32.0f + 10.0f * p2);
                        b = (unsigned char)(22.0f +  8.0f * p1);
                    } else if (rustPatch > 0.08f) {
                        // Oxidized orange-brown iron rust pitting and blistered metal
                        float t = (rustPatch - 0.08f) * 2.2f;
                        if (t > 1.0f) t = 1.0f;
                        r = (unsigned char)(145.0f + 35.0f * t);
                        g = (unsigned char)( 62.0f + 20.0f * t);
                        b = (unsigned char)( 28.0f + 12.0f * t);
                    } else {
                        // Weathered peeling paint (classic 1960s faded slate/teal with chipped borders)
                        float chip = (rustPatch > 0.02f) ? 0.70f : 1.0f;
                        r = (unsigned char)((72.0f + 20.0f * p1) * chip);
                        g = (unsigned char)((86.0f + 22.0f * p2) * chip);
                        b = (unsigned char)((96.0f + 18.0f * p1) * chip);
                    }
                    break;
                }
                case TEX_MOON: {
                    // Luminous cratered lunar surface
                    float crater1 = std::sin(x * 0.08f) * std::cos(y * 0.08f);
                    float crater2 = std::sin(x * 0.22f + y * 0.18f);
                    float shade = 0.82f + 0.18f * (crater1 * 0.6f + crater2 * 0.4f);
                    r = (unsigned char)(235.0f * shade);
                    g = (unsigned char)(242.0f * shade);
                    b = (unsigned char)(255.0f * shade);
                    break;
                }
                default:
                    break;
            }

            data[idx + 0] = r;
            data[idx + 1] = g;
            data[idx + 2] = b;
            data[idx + 3] = a;
        }
    }
}

// Load texture from disk or generate fallback
void loadSceneTexture(TextureID id, const std::vector<std::string>& fileCandidates) {    int width = 0, height = 0, channels = 0;
    unsigned char* imgData = nullptr;
    std::string loadedPath = "";

    stbi_set_flip_vertically_on_load(true);

    for (const auto& path : fileCandidates) {
        imgData = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (imgData) {
            loadedPath = path;
            break;
        }
    }

    glGenTextures(1, &g_textureHandles[id]);
    glBindTexture(GL_TEXTURE_2D, g_textureHandles[id]);

    // Set GL_REPEAT wrapping so textures tile seamlessly across geometry
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (imgData) {
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
        stbi_image_free(imgData);
        std::cout << "[TEXTURE LOADED] " << loadedPath << " -> ID " << id << " (" << width << "x" << height << ")" << std::endl;
    } else {
        // Synthesize fallback procedural texture
        width = 256; height = 256;
        std::vector<unsigned char> procData;
        generateProceduralTexture(id, width, height, procData);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, procData.data());
        std::cout << "[TEXTURE SYNTHESIZED] Procedural pattern generated for ID " << id << " (256x256)" << std::endl;
    }
}

void initAllTextures() {    loadSceneTexture(TEX_WALL,   { "textures/wall.bmp",   "textures/wall.png",   "textures/wall.jpg",   "textures/wood.bmp",   "textures/wood.png",   "textures/wood.jpg" });
    loadSceneTexture(TEX_ROOF,   { "textures/roof.bmp",   "textures/roof.png",   "textures/roof.jpg",   "textures/shingle.bmp","textures/shingle.png","textures/shingle.jpg" });
    loadSceneTexture(TEX_GROUND, { "textures/ground.bmp", "textures/ground.png", "textures/ground.jpg", "textures/mud.bmp",   "textures/mud.png",   "textures/mud.jpg" });
    loadSceneTexture(TEX_STONE,  { "textures/stone.bmp",  "textures/stone.png",  "textures/stone.jpg",  "textures/cobble.bmp", "textures/cobble.png", "textures/cobble.jpg" });
    loadSceneTexture(TEX_BARK,   { "textures/bark.bmp",   "textures/bark.jpg",   "textures/tree.bmp",   "textures/tree.png",   "textures/tree.jpg" });
    loadSceneTexture(TEX_RUST,   { "textures/rust.bmp",   "textures/rust.jpg",   "textures/metal.bmp",  "textures/metal.png",  "textures/metal.jpg" });
    loadSceneTexture(TEX_MOON,   { "textures/moon.bmp",   "textures/moon.png",   "textures/moon.jpg" });
}
