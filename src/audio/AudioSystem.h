#pragma once
#include "../core/Config.h"

#ifdef _WIN32
extern bool g_audioEnabled;
extern std::vector<unsigned char> g_ambientWav;
extern std::vector<unsigned char> g_thunderWav;
extern float g_thunderAudioTimer;

void createWavHeader(unsigned char* header, int sampleRate, int numSamples);
void initProceduralAudio();
void playAmbientAudio();
void playThunderAudio();
void toggleAudio();
#endif
