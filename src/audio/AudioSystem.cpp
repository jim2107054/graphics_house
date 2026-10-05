#include "AudioSystem.h"

#ifdef _WIN32
bool g_audioEnabled = true;
std::vector<unsigned char> g_ambientWav;
std::vector<unsigned char> g_thunderWav;
float g_thunderAudioTimer = 0.0f;

void createWavHeader(unsigned char* header, int sampleRate, int numSamples) {    int dataSize = numSamples * 2;
    int fileSize = 36 + dataSize;
    
    // "RIFF"
    header[0] = 'R'; header[1] = 'I'; header[2] = 'F'; header[3] = 'F';
    header[4] = (unsigned char)(fileSize & 0xFF);
    header[5] = (unsigned char)((fileSize >> 8) & 0xFF);
    header[6] = (unsigned char)((fileSize >> 16) & 0xFF);
    header[7] = (unsigned char)((fileSize >> 24) & 0xFF);
    
    // "WAVE"
    header[8] = 'W'; header[9] = 'A'; header[10] = 'V'; header[11] = 'E';
    
    // "fmt "
    header[12] = 'f'; header[13] = 'm'; header[14] = 't'; header[15] = ' ';
    header[16] = 16; header[17] = 0; header[18] = 0; header[19] = 0; // Subchunk1Size (16 for PCM)
    header[20] = 1;  header[21] = 0; // AudioFormat (1 = PCM)
    header[22] = 1;  header[23] = 0; // NumChannels (1 = Mono)
    
    header[24] = (unsigned char)(sampleRate & 0xFF);
    header[25] = (unsigned char)((sampleRate >> 8) & 0xFF);
    header[26] = (unsigned char)((sampleRate >> 16) & 0xFF);
    header[27] = (unsigned char)((sampleRate >> 24) & 0xFF);
    
    int byteRate = sampleRate * 2;
    header[28] = (unsigned char)(byteRate & 0xFF);
    header[29] = (unsigned char)((byteRate >> 8) & 0xFF);
    header[30] = (unsigned char)((byteRate >> 16) & 0xFF);
    header[31] = (unsigned char)((byteRate >> 24) & 0xFF);
    
    header[32] = 2; header[33] = 0;  // BlockAlign (2 bytes)
    header[34] = 16; header[35] = 0; // BitsPerSample (16 bits)
    
    // "data"
    header[36] = 'd'; header[37] = 'a'; header[38] = 't'; header[39] = 'a';
    header[40] = (unsigned char)(dataSize & 0xFF);
    header[41] = (unsigned char)((dataSize >> 8) & 0xFF);
    header[42] = (unsigned char)((dataSize >> 16) & 0xFF);
    header[43] = (unsigned char)((dataSize >> 24) & 0xFF);
}

void initProceduralAudio() {    int sampleRate = 22050;
    
    // 1. Ambient Night Loop (6.0 seconds continuous seamless cycle)
    float ambDuration = 6.0f;
    int ambSamples = (int)(sampleRate * ambDuration);
    g_ambientWav.resize(44 + ambSamples * 2);
    createWavHeader(g_ambientWav.data(), sampleRate, ambSamples);
    
    short* ambData = (short*)(g_ambientWav.data() + 44);
    for (int i = 0; i < ambSamples; ++i) {
        float t = (float)i / sampleRate;
        
        // Low howling night wind (sub-harmonics + noise breath)
        float wind1 = std::sin(2.0f * (float)M_PI * 48.0f * t + 0.4f * std::sin(2.0f * (float)M_PI * 0.35f * t));
        float wind2 = std::sin(2.0f * (float)M_PI * 72.0f * t);
        float noise = ((float)(rand() % 2000) / 1000.0f - 1.0f) * 0.28f;
        float windVol = 0.20f + 0.10f * std::sin(2.0f * (float)M_PI * (t / ambDuration));
        float wind = (wind1 * 0.5f + wind2 * 0.3f + noise * 0.2f) * windVol;
        
        // Faint distant night crickets (intermittent chirp bursts around 4200 Hz)
        float chirpCadence = std::fmod(t, 0.48f);
        float cricket = 0.0f;
        if (chirpCadence < 0.055f) {
            cricket = std::sin(2.0f * (float)M_PI * 4200.0f * t) * 0.045f;
        }
        
        float sampleVal = (wind + cricket) * 13000.0f;
        if (sampleVal > 32767.0f) sampleVal = 32767.0f;
        if (sampleVal < -32768.0f) sampleVal = -32768.0f;
        ambData[i] = (short)sampleVal;
    }
    
    // 2. Thunder Sound Effect (3.0 seconds)
    float thDuration = 3.0f;
    int thSamples = (int)(sampleRate * thDuration);
    g_thunderWav.resize(44 + thSamples * 2);
    createWavHeader(g_thunderWav.data(), sampleRate, thSamples);
    
    short* thData = (short*)(g_thunderWav.data() + 44);
    for (int i = 0; i < thSamples; ++i) {
        float t = (float)i / sampleRate;
        float sampleVal = 0.0f;
        
        if (t < 0.03f) {
            sampleVal = 0.0f;
        } else if (t < 0.22f) {
            float tBoom = t - 0.03f;
            float sub = std::sin(2.0f * (float)M_PI * 45.0f * tBoom) * std::exp(-tBoom * 16.0f);
            float crack = ((float)(rand() % 2000) / 1000.0f - 1.0f) * std::exp(-tBoom * 20.0f);
            sampleVal = (sub * 0.70f + crack * 0.55f);
        } else {
            float tRoll = t - 0.22f;
            float rollEnv = std::exp(-tRoll * 1.15f);
            float r1 = std::sin(2.0f * (float)M_PI * 36.0f * tRoll);
            float r2 = std::sin(2.0f * (float)M_PI * 52.0f * tRoll + std::sin(tRoll * 5.0f));
            float rNoise = ((float)(rand() % 2000) / 1000.0f - 1.0f) * 0.22f;
            sampleVal = (r1 * 0.5f + r2 * 0.35f + rNoise * 0.15f) * rollEnv * 0.85f;
        }
        
        float finalSample = sampleVal * 23000.0f;
        if (finalSample > 32767.0f) finalSample = 32767.0f;
        if (finalSample < -32768.0f) finalSample = -32768.0f;
        thData[i] = (short)finalSample;
    }
}

void playAmbientAudio() {    if (!g_audioEnabled || g_ambientWav.empty()) return;
    PlaySoundA((LPCSTR)g_ambientWav.data(), NULL, SND_MEMORY | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
}

void playThunderAudio() {    if (!g_audioEnabled || g_thunderWav.empty()) return;
    PlaySoundA((LPCSTR)g_thunderWav.data(), NULL, SND_MEMORY | SND_ASYNC | SND_NODEFAULT);
    g_thunderAudioTimer = 3.1f; // Resume ambient loop when thunder finishes
}

void toggleAudio() {    g_audioEnabled = !g_audioEnabled;
    if (g_audioEnabled) {
        playAmbientAudio();
        std::cout << "[AUDIO] Ambient Night Audio: ON" << std::endl;
    } else {
        PlaySoundA(NULL, NULL, 0);
        std::cout << "[AUDIO] Ambient Night Audio: MUTED" << std::endl;
    }
}
#endif
