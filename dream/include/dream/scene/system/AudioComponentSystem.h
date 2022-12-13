//
// Created by Deepak Ramalingam on 12/12/22.
//

#ifndef DREAM_AUDIOCOMPONENTSYSTEM_H
#define DREAM_AUDIOCOMPONENTSYSTEM_H

#include <AL/al.h>
#include <AL/alc.h>

namespace Dream {
    class AudioComponentSystem {
    public:
        AudioComponentSystem();
        ~AudioComponentSystem();
        void update(float dt);
    private:
        ALCdevice *device;
    };
}

#endif //DREAM_AUDIOCOMPONENTSYSTEM_H
