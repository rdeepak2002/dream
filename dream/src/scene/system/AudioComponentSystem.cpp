/**********************************************************************************
 *  Dream is a software for developing real-time 3D experiences.
 *  Copyright (C) 2023 Deepak Ramalignam
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **********************************************************************************/

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
