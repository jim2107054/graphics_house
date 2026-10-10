#include "Screenshot.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

void saveScreenshot(const char* filename) {    int w = g_windowWidth;
    int h = g_windowHeight;
    int rowPadded = (w * 3 + 3) & (~3);
    int dataSize = rowPadded * h;

    std::vector<unsigned char> pixels(dataSize, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadPixels(0, 0, w, h, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels.data());

    unsigned char fileHeader[14] = {
        'B', 'M',
        0, 0, 0, 0,
        0, 0, 0, 0,
        54, 0, 0, 0
    };
    int fileSize = 54 + dataSize;
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);

    unsigned char infoHeader[40] = {
        40, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        1, 0,
        24, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    infoHeader[4]  = (unsigned char)(w);
    infoHeader[5]  = (unsigned char)(w >> 8);
    infoHeader[6]  = (unsigned char)(w >> 16);
    infoHeader[7]  = (unsigned char)(w >> 24);
    infoHeader[8]  = (unsigned char)(h);
    infoHeader[9]  = (unsigned char)(h >> 8);
    infoHeader[10] = (unsigned char)(h >> 16);
    infoHeader[11] = (unsigned char)(h >> 24);
    infoHeader[20] = (unsigned char)(dataSize);
    infoHeader[21] = (unsigned char)(dataSize >> 8);
    infoHeader[22] = (unsigned char)(dataSize >> 16);
    infoHeader[23] = (unsigned char)(dataSize >> 24);

    FILE* fp = fopen(filename, "wb");
    if (fp) {
        fwrite(fileHeader, 1, 14, fp);
        fwrite(infoHeader, 1, 40, fp);
        fwrite(pixels.data(), 1, dataSize, fp);
        fclose(fp);
        std::cout << "[SCREENSHOT] Saved capture to " << filename << std::endl;
    } else {
        std::cerr << "[SCREENSHOT ERROR] Could not open file " << filename << " for writing." << std::endl;
    }
}
