#ifndef SELENE_AUDIO_SYSTEM_H_
#define SELENE_AUDIO_SYSTEM_H_

#include "selene.h"
#include "lua_helper.h"
#include "modules/audio/audio.h"
#include "plugins/selSDL/src/selSDL.h"

typedef struct {
    int format;
    int channels;
    int sampleRate;
    int chunkSize;
} AudioSpec;

typedef struct AudioSystem AudioSystem;
struct AudioSystem {
    SDL_AudioDeviceID* devId;
    Data* auxData;
    AudioSpec spec;
};

typedef struct Music Music;
typedef struct Sound Sound;

#endif /* SELENE_AUDIO_SYSTEM_H_ */