#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// Cross-platform openAL audio headers
#include <AL/al.h>
#include <AL/alc.h>

static ALCdevice* sound_hardware_device = NULL;
static ALCcontext* audio_context = NULL;
static ALuint sound_effect_source = 0;
static ALuint music_source = 0;
static ALuint music_buffer = 0;

#define MAX_PARTICLES 250

typedef struct {
    float x, y;
    float vx, vy;
    float r, g, b, alpha;
    float size;
    int active;
} Particle;

static Particle particle_pool[MAX_PARTICLES];

// Pyforge.init_audio_hardware()
PyObject* method_init_audio_hardware(PyObject* self, PyObject* args) {
    sound_hardware_device = alcOpenDevice(NULL);
    if (!sound_hardware_device) {
        PyErr_SetString(PyExc_RuntimeError, "Could not open audio device controller.");
        return NULL;
    }
    audio_context = alcCreateContext(sound_hardware_device, NULL);
    if (!audio_context) {
        alcCloseDevice(sound_hardware_device);
        PyErr_SetString(PyExc_RuntimeError, "Could not create audio hardware context.");
        return NULL;
    }
    alcMakeContextCurrent(audio_context);
    
    // Allocate two separate audio hardware channels (one for SFX, one for Music)
    alGenSources(1, &sound_effect_source);
    alGenSources(1, &music_source);
    
    // Reset our particle pool data slots completely on launch
    for(int i = 0; i < MAX_PARTICLES; i++) {
        particle_pool[i].active = 0;
    }
    Py_RETURN_NONE;
}

static int load_wav_file(const char* filename, ALuint buffer) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return 0;

    char chunk_id[4];
    // FIXED: Check return values of fread to satisfy GCC warning requirements!
    if (fread(chunk_id, 4, 1, fp) != 1) { fclose(fp); return 0; }
    fseek(fp, 22, SEEK_SET);
    
    short channels;
    if (fread(&channels, 2, 1, fp) != 1) { fclose(fp); return 0; }

    int sample_rate;
    if (fread(&sample_rate, 4, 1, fp) != 1) { fclose(fp); return 0; }

    fseek(fp, 34, SEEK_SET);
    short bits_per_sample;
    if (fread(&bits_per_sample, 2, 1, fp) != 1) { fclose(fp); return 0; }

    fseek(fp, 40, SEEK_SET);
    int data_size;
    if (fread(&data_size, 4, 1, fp) != 1) { fclose(fp); return 0; }

    unsigned char* data = (unsigned char*)malloc(data_size);
    if (!data) { fclose(fp); return 0; }
    
    if (fread(data, data_size, 1, fp) != 1) { free(data); fclose(fp); return 0; }
    fclose(fp);

    ALenum format = AL_FORMAT_MONO16;
    if (channels == 1 && bits_per_sample == 8) format = AL_FORMAT_MONO8;
    else if (channels == 1 && bits_per_sample == 16) format = AL_FORMAT_MONO16;
    else if (channels == 2 && bits_per_sample == 8) format = AL_FORMAT_STEREO8;
    else if (channels == 2 && bits_per_sample == 16) format = AL_FORMAT_STEREO16;

    alBufferData(buffer, format, data, data_size, sample_rate);
    free(data);
    return 1;
}


// Pyforge.play_sound_file(filepath) - For short sound effect files like jumps/clicks
PyObject* method_play_sound_file(PyObject* self, PyObject* args) {
    const char* filepath;
    if (!PyArg_ParseTuple(args, "s", &filepath)) return NULL;
    if (!audio_context) Py_RETURN_NONE;

    ALuint buffer;
    alGenBuffers(1, &buffer);
    if (load_wav_file(filepath, buffer)) {
        alSourcei(sound_effect_source, AL_BUFFER, buffer);
        alSourcePlay(sound_effect_source);
    } else {
        printf("⚠️ Pyforge Error: Could not read audio file: %s\n", filepath);
    }
    Py_RETURN_NONE;
}

// Pyforge.play_music_file(filepath, loop_toggle) - For background ambient sound tracks [2]
PyObject* method_play_music_file(PyObject* self, PyObject* args) {
    const char* filepath;
    int loop_toggle;
    if (!PyArg_ParseTuple(args, "si", &filepath, &loop_toggle)) return NULL;
    if (!audio_context) Py_RETURN_NONE;

    // Delete old music buffer allocation if it is currently occupied
    if (music_buffer) {
        alSourceStop(music_source);
        alDeleteBuffers(1, &music_buffer);
    }

    alGenBuffers(1, &music_buffer);
    if (load_wav_file(filepath, music_buffer)) {
        alSourcei(music_source, AL_BUFFER, music_buffer);
        alSourcei(music_source, AL_LOOPING, loop_toggle ? AL_TRUE : AL_FALSE); // Loop background music infinitely [2]
        alSourcePlay(music_source);
        printf("🎵 Streaming background audio track: %s\n", filepath);
    } else {
        printf("⚠️ Pyforge Error: Could not read music file: %s\n", filepath);
    }
    Py_RETURN_NONE;
}

// Pyforge.spawn_burst_effect(x, y, r, g, b)
PyObject* method_spawn_burst_effect(PyObject* self, PyObject* args) {
    double x, y;
    float r, g, b;
    if (!PyArg_ParseTuple(args, "ddfff", &x, &y, &r, &g, &b)) return NULL;

    int burst_count = 15;
    for (int i = 0; i < MAX_PARTICLES && burst_count > 0; i++) {
        if (!particle_pool[i].active) {
            particle_pool[i].active = 1;
            particle_pool[i].x = (float)x;
            particle_pool[i].y = (float)y;
            
            float angle = (float)(rand() % 360) * (M_PI / 180.0f);
            float speed = (float)(rand() % 50 + 20) * 0.1f;
            particle_pool[i].vx = cosf(angle) * speed;
            particle_pool[i].vy = sinf(angle) * speed;
            
            particle_pool[i].r = r;
            particle_pool[i].g = g;
            particle_pool[i].b = b;
            particle_pool[i].alpha = 1.0f;
            particle_pool[i].size = (float)(rand() % 4 + 2);
            burst_count--;
        }
    }
    Py_RETURN_NONE;
}

// Pyforge.update_and_render_particles()
PyObject* method_update_and_render_particles(PyObject* self, PyObject* args) {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particle_pool[i].active) {
            particle_pool[i].x += particle_pool[i].vx;
            particle_pool[i].y += particle_pool[i].vy;
            particle_pool[i].alpha -= 0.025f;
            
            if (particle_pool[i].alpha <= 0.0f) {
                particle_pool[i].active = 0;
                continue;
            }

            glColor4f(particle_pool[i].r, particle_pool[i].g, particle_pool[i].b, particle_pool[i].alpha);
            float hs = particle_pool[i].size / 2.0f;
            
            glVertex2d(particle_pool[i].x - hs, particle_pool[i].y - hs);
            glVertex2d(particle_pool[i].x + hs, particle_pool[i].y - hs);
            glVertex2d(particle_pool[i].x + hs, particle_pool[i].y + hs);
            glVertex2d(particle_pool[i].x - hs, particle_pool[i].y + hs);
        }
    }
    glEnd();
    Py_RETURN_NONE;
}
