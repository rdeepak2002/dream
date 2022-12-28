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

#include "dream/renderer/AnimationBone.h"

namespace Dream {
    AnimationBone::AnimationBone(const std::string &name, int ID, const aiNodeAnim *channel) : name(name), id(ID),
                                                                                               localTransform(1.0f) {
        numPositions = channel->mNumPositionKeys;

        for (int positionIndex = 0; positionIndex < numPositions; ++positionIndex) {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timeStamp = channel->mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            data.position = AssimpGLMHelpers::getGLMVec(aiPosition);
            data.timeStamp = timeStamp;
            positions.push_back(data);
        }

        numRotations = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < numRotations; ++rotationIndex) {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
            KeyRotation data;
            data.orientation = AssimpGLMHelpers::getGLMQuat(aiOrientation);
            data.timeStamp = timeStamp;
            rotations.push_back(data);
        }

        numScalings = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < numScalings; ++keyIndex) {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timeStamp = channel->mScalingKeys[keyIndex].mTime;
            KeyScale data;
            data.scale = AssimpGLMHelpers::getGLMVec(scale);
            data.timeStamp = timeStamp;
            scales.push_back(data);
        }
    }

    void AnimationBone::update(float animationTime) {
        glm::mat4 translation = interpolatePosition(animationTime);
        glm::mat4 rotation = interpolateRotation(animationTime);
        glm::mat4 scale = interpolateScaling(animationTime);
        localTransform = translation * rotation * scale;
    }

    glm::mat4 AnimationBone::getLocalTransform() {
        return localTransform;
    }

    std::string AnimationBone::getBoneName() const {
        return name;
    }

    int AnimationBone::getBoneID() {
        return id;
    }

    int AnimationBone::getPositionIndex(float animationTime) {
        for (int index = 0; index < numPositions - 1; ++index) {
            if (animationTime < positions[index + 1].timeStamp)
                return index;
        }
        assert(0);
    }

    int AnimationBone::getRotationIndex(float animationTime) {
        for (int index = 0; index < numRotations - 1; ++index) {
            if (animationTime < rotations[index + 1].timeStamp) {
                return index;
            }
        }
        assert(0);
    }

    int AnimationBone::getScaleIndex(float animationTime) {
        for (int index = 0; index < numScalings - 1; ++index) {
            if (animationTime < scales[index + 1].timeStamp) {
                return index;
            }
        }
        assert(0);
    }

    float AnimationBone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp;
        scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }

    glm::mat4 AnimationBone::interpolatePosition(float animationTime) {
        if (1 == numPositions)
            return glm::translate(glm::mat4(1.0f), positions[0].position);

        int p0Index = getPositionIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = getScaleFactor(positions[p0Index].timeStamp,
                                           positions[p1Index].timeStamp, animationTime);
        glm::vec3 finalPosition = glm::mix(positions[p0Index].position, positions[p1Index].position, scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    glm::mat4 AnimationBone::interpolateRotation(float animationTime) {
        if (1 == numRotations) {
            auto rotation = glm::normalize(rotations[0].orientation);
            return glm::toMat4(rotation);
        }

        int p0Index = getRotationIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = getScaleFactor(rotations[p0Index].timeStamp,
                                           rotations[p1Index].timeStamp, animationTime);
        glm::quat finalRotation = glm::slerp(rotations[p0Index].orientation, rotations[p1Index].orientation,
                                             scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return glm::toMat4(finalRotation);
    }

    glm::mat4 AnimationBone::interpolateScaling(float animationTime) {
        if (1 == numScalings) {
            return glm::scale(glm::mat4(1.0f), scales[0].scale);
        }

        int p0Index = getScaleIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = getScaleFactor(scales[p0Index].timeStamp, scales[p1Index].timeStamp, animationTime);
        glm::vec3 finalScale = glm::mix(scales[p0Index].scale, scales[p1Index].scale, scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }
}