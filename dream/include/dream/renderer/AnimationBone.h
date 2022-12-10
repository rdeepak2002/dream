//
// Created by Deepak Ramalingam on 12/8/22.
//

#ifndef DREAM_ANIMATIONBONE_H
#define DREAM_ANIMATIONBONE_H

#pragma once

#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include "dream/util/AssimpGLMHelpers.h"
#include "AnimationData.h"

namespace Dream {
    class AnimationBone {
    public:
        AnimationBone(const std::string& name, int ID, const aiNodeAnim* channel);
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
