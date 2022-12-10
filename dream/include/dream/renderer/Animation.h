//
// Created by Deepak Ramalingam on 12/8/22.
//

#ifndef DREAM_ANIMATION_H
#define DREAM_ANIMATION_H

#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <dream/renderer/Bone.h>
#include <functional>
#include <dream/renderer/AnimationData.h>
#include "dream/scene/Entity.h"
#include "dream/scene/component/Component.h"
#include "AssimpNodeData.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Dream {
    class Animation {
    public:
        Animation() = default;

        Animation(const std::string& animationPath, Entity modelEntity, int index) {
            Assimp::Importer importer;
            importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);  // fixes mixamo animations
            const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
            assert(scene && scene->mRootNode);
            if (scene->mNumAnimations <= index) {
                std::cout << "Error: animation " << index << " does not exist for " << animationPath << std::endl;
            }
            auto animation = scene->mAnimations[index];
            duration = animation->mDuration;
            ticksPerSecond = animation->mTicksPerSecond;
            animationName = animation->mName.C_Str();
            aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
            globalTransformation = globalTransformation.Inverse();
            ReadHeirarchyData(rootNode, scene->mRootNode);
            ReadMissingBones(animation, modelEntity);
        }

        ~Animation() {
        }

        Bone* findBone(const std::string& name) {
            auto iter = std::find_if(bones.begin(), bones.end(),
                                     [&](const Bone& Bone)
                                     {
                                         return Bone.GetBoneName() == name;
                                     }
            );
            if (iter == bones.end()) return nullptr;
            else return &(*iter);
        }

        inline float getTicksPerSecond() { return ticksPerSecond; }
        inline float getDuration() { return duration;}
        inline const AssimpNodeData& getRootNode() { return rootNode; }
        inline const std::map<std::string,BoneInfo>& getBoneIdMap() {
            return animationBoneInfoMap;
        }
        inline std::string getName() {
            return animationName;
        }

    private:
        void ReadMissingBones(const aiAnimation* animation, Entity& modelEntity) {
            int size = animation->mNumChannels;

            auto& boneInfoMap = modelEntity.getComponent<Component::MeshComponent>().m_BoneInfoMap;//getting m_BoneInfoMap from Model class
            int& boneCount = modelEntity.getComponent<Component::MeshComponent>().m_BoneCount; //getting the m_BoneCounter from Model class

            //reading channels(bones engaged in an animation and their keyframes)
            for (int i = 0; i < size; i++) {
                auto channel = animation->mChannels[i];
                std::string boneName = channel->mNodeName.data;

                if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
                    std::cout << "Missing bone: " << boneName << std::endl;
                    boneInfoMap[boneName].id = boneCount;
                    boneCount++;
                }
                bones.emplace_back(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel);
            }

            animationBoneInfoMap = boneInfoMap;
        }

        void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src) {
            assert(src);

            dest.name = src->mName.data;
            dest.transformation = AssimpGLMHelpers::convertMatrixToGLMFormat(src->mTransformation);
            dest.childrenCount = src->mNumChildren;

            for (int i = 0; i < src->mNumChildren; i++) {
                AssimpNodeData newData;
                ReadHeirarchyData(newData, src->mChildren[i]);
                dest.children.push_back(newData);
            }
        }
        float duration;
        int ticksPerSecond;
        std::string animationName;
        std::vector<Bone> bones;
        AssimpNodeData rootNode;
        std::map<std::string, BoneInfo> animationBoneInfoMap;
    };
}

#endif //DREAM_ANIMATION_H
