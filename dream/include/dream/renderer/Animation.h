//
// Created by Deepak Ramalingam on 12/8/22.
//

#ifndef DREAM_ANIMATION_H
#define DREAM_ANIMATION_H

#pragma once
#include "dream/renderer/AnimationBone.h"
#include "dream/renderer/AnimationData.h"
#include "dream/scene/Entity.h"
#include "dream/renderer/AssimpNodeData.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <functional>
#include <vector>
#include <map>

namespace Dream {
    class Animation {
    public:
        explicit Animation(const std::string& animationPath, Entity modelEntity, int index);
        AnimationBone* findBone(const std::string& name);
        float getTicksPerSecond();
        float getDuration();
        const AssimpNodeData& getRootNode();
        const std::map<std::string,BoneInfo>& getBoneIdMap();
        std::string getName();
        static int numberOfAnimationsForFile(const std::string& animationPath);
    private:
        void readMissingBones(const aiAnimation* animation, Entity& modelEntity);
        void readHierarchyData(AssimpNodeData& dest, const aiNode* src);
        float duration;
        int ticksPerSecond;
        std::string animationName;
        std::vector<AnimationBone> bones;
        AssimpNodeData rootNode;
        std::map<std::string, BoneInfo> animationBoneInfoMap;
    };
}

#endif //DREAM_ANIMATION_H
