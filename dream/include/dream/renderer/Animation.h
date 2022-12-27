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
