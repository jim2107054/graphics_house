#include "AudioSystem.h"

#ifdef _WIN32
bool g_audioEnabled = false;
std::vector<unsigned char> g_ambientWav;
std::vector<unsigned char> g_thunderWav;
float g_thunderAudioTimer = 0.0f;

void createWavHeader(unsigned char* header, int sampleRate, int numSamples) {
    (void)header; (void)sampleRate; (void)numSamples;
}

void initProceduralAudio() {
    // Sound removed as requested
}

void playAmbientAudio() {
    // Sound removed as requested
}

void playThunderAudio() {
    // Sound removed as requested
}

void toggleAudio() {
    // Sound removed as requested
}
#endif

