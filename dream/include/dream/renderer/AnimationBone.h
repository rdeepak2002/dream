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
        void Update(float animationTime);
        glm::mat4 GetLocalTransform();
        std::string GetBoneName() const;
        int GetBoneID();
        int GetPositionIndex(float animationTime);
        int GetRotationIndex(float animationTime);
        int GetScaleIndex(float animationTime);
    private:
        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
        glm::mat4 InterpolatePosition(float animationTime);
        glm::mat4 InterpolateRotation(float animationTime);
        glm::mat4 InterpolateScaling(float animationTime);
        std::vector<KeyPosition> m_Positions;
        std::vector<KeyRotation> m_Rotations;
        std::vector<KeyScale> m_Scales;
        int m_NumPositions;
        int m_NumRotations;
        int m_NumScalings;
        glm::mat4 m_LocalTransform;
        std::string m_Name;
        int m_ID;
    };
}

#endif //DREAM_ANIMATIONBONE_H
