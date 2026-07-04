#include "raylib.h"
#include <math.h>

#define SAMPLE_RATE 44100
#define MAX_VOICES 4

typedef struct {
    float phase;
    float freq;
} Osc;

typedef struct {
    float phase;
    float freq;
    float env;
    int gate; // 1 = key held, 0 = released
} Voice;

Voice voices[MAX_VOICES] = {0};

float osc(float phase) {
    float s1 = (sinf(phase) > 0) ? 1.0f : -1.0f;
    float s2 = (sinf(phase * 1.003f) > 0) ? 1.0f : -1.0f;
    return 0.5f * (s1 + s2);
}

void trigger_voice(float freq) {
    for (int i = 0; i < MAX_VOICES; i++) {
        if (!voices[i].gate) {
            voices[i].freq = freq;
            voices[i].gate = 1;
            voices[i].env = 0.0f;
            return;
        }
    }

    // if all busy, steal voice 0
    voices[0].freq = freq;
    voices[0].env = 0.0f;
}

void release_voice(float freq) {
    for (int i = 0; i < MAX_VOICES; i++) {
        if (voices[i].gate && voices[i].freq == freq) {
            voices[i].gate = 0; // start release phase
            return;
        }
    }
}

// AudioCallback already named in raylib
void audio_callback(void *buffer, unsigned int frames) {
    float *out = buffer;

    for (unsigned int i = 0; i < frames; i++) {
        float sample = 0.0f;

        for (int v = 0; v < MAX_VOICES; v++) {
            Voice *voice = &voices[v];

            if (voice->gate || voice->env > 0.001f) {
                voice->phase += 2.0f * PI * voice->freq / 44100.0f;
                if (voice->phase > 2.0f * PI)
                    voice->phase -= 2.0f * PI;

                if (voice->gate)
                    voice->env += 0.002f; // attack
                else
                    voice->env -= 0.003f; // release

                if (voice->env > 1.0f)
                    voice->env = 1.0f;
                if (voice->env < 0.0f) {
                    voice->env = 0.0f;
                    voice->gate = 0;
                }

                float s = sinf(voice->phase) * voice->env;
                sample += s;
            }
        }

        sample *= 0.25f; // prevent clipping (4 voices)

        out[i * 2 + 0] = sample;
        out[i * 2 + 1] = sample;
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

    int gate;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_A))
            trigger_voice(261.63f);
        if (IsKeyPressed(KEY_S))
            trigger_voice(293.66f);
        if (IsKeyPressed(KEY_D))
            trigger_voice(329.63f);
        if (IsKeyPressed(KEY_F))
            trigger_voice(349.23f);
        if (IsKeyPressed(KEY_G))
            trigger_voice(392.00f);
        if (IsKeyPressed(KEY_H))
            trigger_voice(440.0f);
        if (IsKeyPressed(KEY_J))
            trigger_voice(493.88f);
        if (IsKeyPressed(KEY_K))
            trigger_voice(523.25f);

        if (IsKeyReleased(KEY_A))
            release_voice(261.63f);
        if (IsKeyReleased(KEY_S))
            release_voice(293.66f);
        if (IsKeyReleased(KEY_D))
            release_voice(329.63f);
        if (IsKeyReleased(KEY_F))
            release_voice(349.23f);
        if (IsKeyReleased(KEY_G))
            release_voice(392.00f);
        if (IsKeyReleased(KEY_H))
            release_voice(440.0f);
        if (IsKeyReleased(KEY_J))
            release_voice(493.88f);
        if (IsKeyReleased(KEY_K))
            release_voice(523.25f);

        DrawText("SYNTH v1 - HOLD KEYS", 20, 20, 30, GREEN);

        EndDrawing();
    }

    UnloadAudioStream(stream);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}