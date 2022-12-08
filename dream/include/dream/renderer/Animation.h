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
//#include "dream/scene/component/Component.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Dream {
    class Animation
    {
    public:
        Animation() = default;

        Animation(const std::string& animationPath, Entity modelEntity)
        {
            Assimp::Importer importer;
            importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);  // fixes mixamo animations
            const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
            assert(scene && scene->mRootNode);
            auto animation = scene->mAnimations[0];
            m_Duration = animation->mDuration;
            m_TicksPerSecond = animation->mTicksPerSecond;
            aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
            globalTransformation = globalTransformation.Inverse();
            ReadHeirarchyData(m_RootNode, scene->mRootNode);
            ReadMissingBones(animation, modelEntity);
        }

        ~Animation()
        {
        }

        Bone* FindBone(const std::string& name)
        {
            auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
                                     [&](const Bone& Bone)
                                     {
                                         return Bone.GetBoneName() == name;
                                     }
            );
            if (iter == m_Bones.end()) return nullptr;
            else return &(*iter);
        }


        inline float GetTicksPerSecond() { return m_TicksPerSecond; }
        inline float GetDuration() { return m_Duration;}
        inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
        inline const std::map<std::string,BoneInfo>& GetBoneIDMap()
        {
            return m_BoneInfoMap;
        }

    private:
        void ReadMissingBones(const aiAnimation* animation, Entity& modelEntity)
        {
            int size = animation->mNumChannels;

            auto& boneInfoMap = modelEntity.getComponent<Component::MeshComponent>().m_BoneInfoMap;//getting m_BoneInfoMap from Model class
            int& boneCount = modelEntity.getComponent<Component::MeshComponent>().m_BoneCount; //getting the m_BoneCounter from Model class

            //reading channels(bones engaged in an animation and their keyframes)
            for (int i = 0; i < size; i++)
            {
                auto channel = animation->mChannels[i];
                std::string boneName = channel->mNodeName.data;

                if (boneInfoMap.find(boneName) == boneInfoMap.end())
                {
                    std::cout << "Missing bone: " << boneName << std::endl;
                    boneInfoMap[boneName].id = boneCount;
                    boneCount++;
                }
                m_Bones.push_back(Bone(channel->mNodeName.data,
                                       boneInfoMap[channel->mNodeName.data].id, channel));
            }

            m_BoneInfoMap = boneInfoMap;
        }

        void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
        {
            assert(src);

            dest.name = src->mName.data;
            dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
            dest.childrenCount = src->mNumChildren;

            for (int i = 0; i < src->mNumChildren; i++)
            {
                AssimpNodeData newData;
                ReadHeirarchyData(newData, src->mChildren[i]);
                dest.children.push_back(newData);
            }
        }
        float m_Duration;
        int m_TicksPerSecond;
        std::vector<Bone> m_Bones;
        AssimpNodeData m_RootNode;
        std::map<std::string, BoneInfo> m_BoneInfoMap;
    };
}

#endif //DREAM_ANIMATION_H
