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

#ifndef DREAM_ANIMATIONBONE_H
#define DREAM_ANIMATIONBONE_H

#pragma once

#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "dream/util/AssimpGLMHelpers.h"
#include "dream/renderer/AnimationData.h"

namespace Dream {
    class AnimationBone {
    public:
        AnimationBone(const std::string &name, int ID, const aiNodeAnim *channel);

        void update(float animationTime);

        glm::mat4 getLocalTransform();

        [[nodiscard]] std::string getBoneName() const;

        int getBoneID();

        int getPositionIndex(float animationTime);

        int getRotationIndex(float animationTime);

        int getScaleIndex(float animationTime);

    private:
        float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

        glm::mat4 interpolatePosition(float animationTime);

        glm::mat4 interpolateRotation(float animationTime);

        glm::mat4 interpolateScaling(float animationTime);

        std::vector<KeyPosition> positions;
        std::vector<KeyRotation> rotations;
        std::vector<KeyScale> scales;
        int numPositions;
        int numRotations;
        int numScalings;
        glm::mat4 localTransform;
        std::string name;
        int id;
    };
}

#endif //DREAM_ANIMATIONBONE_H
