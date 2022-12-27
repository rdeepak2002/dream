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

#include "dream/renderer/Animation.h"
#include "dream/util/Logger.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    Animation::Animation(const std::string &animationPath, Entity modelEntity, int index) {
        Assimp::Importer importer;
        importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);  // fixes mixamo animations
        const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        if (scene->mNumAnimations <= index) {
            Logger::fatal("Animation " + std::to_string(index) + " does not exist for " + animationPath);
        }
        auto animation = scene->mAnimations[index];
        duration = animation->mDuration;
        ticksPerSecond = animation->mTicksPerSecond;
        animationName = animation->mName.C_Str();
        aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
        globalTransformation = globalTransformation.Inverse();
        readHierarchyData(rootNode, scene->mRootNode);
        readMissingBones(animation, modelEntity);
    }

    AnimationBone *Animation::findBone(const std::string &name) {
        auto iter = std::find_if(bones.begin(), bones.end(),
                                 [&](const AnimationBone& Bone) {
                                     return Bone.getBoneName() == name;
                                 }
        );
        if (iter == bones.end()) return nullptr;
        else return &(*iter);
    }

    float Animation::getTicksPerSecond() {
        return ticksPerSecond;
    }

    float Animation::getDuration() {
        return duration;
    }

    const AssimpNodeData &Animation::getRootNode() {
        return rootNode;
    }

    const std::map<std::string, BoneInfo> &Animation::getBoneIdMap() {
        return animationBoneInfoMap;
    }

    std::string Animation::getName() {
        return animationName;
    }

    void Animation::readMissingBones(const aiAnimation *animation, Entity &modelEntity) {
        int size = animation->mNumChannels;

        auto& boneInfoMap = modelEntity.getComponent<Component::MeshComponent>().m_BoneInfoMap;//getting m_BoneInfoMap from Model class
        int& boneCount = modelEntity.getComponent<Component::MeshComponent>().m_BoneCount; //getting the m_BoneCounter from Model class

        //reading channels(bones engaged in an animation and their keyframes)
        for (int i = 0; i < size; i++) {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
                Logger::warn("Mesh missing bone " + boneName);
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }
            bones.emplace_back(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel);
        }

        animationBoneInfoMap = boneInfoMap;
    }

    void Animation::readHierarchyData(AssimpNodeData &dest, const aiNode *src) {
        assert(src);

        dest.name = src->mName.data;
        dest.transformation = AssimpGLMHelpers::convertMatrixToGLMFormat(src->mTransformation);
        dest.childrenCount = src->mNumChildren;

        for (int i = 0; i < src->mNumChildren; i++) {
            AssimpNodeData newData;
            readHierarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }

    int Animation::numberOfAnimationsForFile(const std::string &animationPath) {
        Assimp::Importer importer;
        importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);  // fixes mixamo animations
        const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        return scene->mNumAnimations;
    }
}