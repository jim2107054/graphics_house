#pragma once
#include "../core/Config.h"

enum TextureID {
    TEX_NONE = 0,
    TEX_WALL,
    TEX_ROOF,
    TEX_GROUND,
    TEX_STONE,
    TEX_BARK,
    TEX_RUST,
    TEX_MOON,
    TEX_COUNT
};

extern GLuint g_textureHandles[TEX_COUNT];

void bindTexture(TextureID id);
void generateProceduralTexture(TextureID id, int width, int height, std::vector<unsigned char>& data);
void loadSceneTexture(TextureID id, const std::vector<std::string>& fileCandidates);
void initAllTextures();
