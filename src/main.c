#include "raylib.h"
#include <math.h>

#define SAMPLE_RATE 44100

typedef struct {
    float phase;
    float freq;
} Osc;

Osc osc = {0};
float freq = 440.0f;

// AudioCallback already named in raylib
void audio_callback(void *buffer, unsigned int frames) {
    float *out = (float *)buffer;

    for (unsigned int i = 0; i < frames; i++) {
        float sample = sinf(osc.phase);

        osc.phase += 2.0f * PI * osc.freq / SAMPLE_RATE;

        if (osc.phase > 2.0f * PI)
            osc.phase -= 2.0f * PI;

        // stereo output
        out[i * 2 + 0] = sample; // left
        out[i * 2 + 1] = sample; // right
    }
}

int main(void) {
    InitWindow(800, 600, "Synth");
    InitAudioDevice();

    SetAudioStreamBufferSizeDefault(512);

    AudioStream stream = LoadAudioStream(SAMPLE_RATE, 32, 2);
    SetAudioStreamCallback(stream, audio_callback);
    PlayAudioStream(stream);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // C4 major
        if (IsKeyPressed(KEY_A))
            freq = 261.63f; // C4
        if (IsKeyPressed(KEY_S))
            freq = 293.66f; // D4
        if (IsKeyPressed(KEY_D))
            freq = 329.63f; // E4
        if (IsKeyPressed(KEY_F))
            freq = 349.23f; // F4
        if (IsKeyPressed(KEY_G))
            freq = 392.00f; // G4
        if (IsKeyPressed(KEY_H))
            freq = 440.00f; // A4
        if (IsKeyPressed(KEY_J))
            freq = 493.88f; // B4
        if (IsKeyPressed(KEY_K))
            freq = 523.25; // C5

        osc.freq = freq;
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("SYNTH ACTIVE", 20, 20, 30, GREEN);
        DrawText("Press ESC to exit", 20, 60, 20, RAYWHITE);

        EndDrawing();
    }

    UnloadAudioStream(stream);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}