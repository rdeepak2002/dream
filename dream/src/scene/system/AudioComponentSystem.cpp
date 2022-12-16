//
// Created by Deepak Ramalingam on 12/12/22.
//

#include "dream/scene/system/AudioComponentSystem.h"
#include "dream/util/Logger.h"

namespace Dream {
    AudioComponentSystem::AudioComponentSystem() {
        device = alcOpenDevice(nullptr);
        if (!device) {
            Logger::fatal("Unable to open audio device");
        }
    }

    AudioComponentSystem::~AudioComponentSystem() {
//        alDeleteSources(1, &source);
//        alDeleteBuffers(1, &buffer);
//        device = alcGetContextsDevice(context);
//        alcMakeContextCurrent(NULL);
//        alcDestroyContext(context);
        alcCloseDevice(device);
    }

    void AudioComponentSystem::update(float dt) {

    }

    void AudioComponentSystem::init() {

    }
}
