//
// Created by Deepak Ramalingam on 12/3/22.
//

#include "dream/scene/component/Component.h"
#include "dream/renderer/Animation.h"
#include "dream/project/Project.h"

namespace Dream::Component {
    AnimatorComponent::AnimatorComponent() {
        m_CurrentTime = 0.0;
        m_CurrentAnimation = nullptr;
        m_FinalBoneMatrices.reserve(MAX_BONES);
        for (int i = 0; i < MAX_BONES; i++) {
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        }
    }

    AnimatorComponent::AnimatorComponent(Entity modelEntity, std::vector<std::string> animations) {
        this->animations = std::move(animations);
        m_CurrentTime = 0.0;
        m_CurrentAnimation = nullptr;
        m_FinalBoneMatrices.reserve(MAX_BONES);
        for (int i = 0; i < MAX_BONES; i++) {
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        }
    }

    void AnimatorComponent::UpdateAnimation(float dt)
    {
        m_DeltaTime = dt;
        if (m_CurrentAnimation)
        {
            m_CurrentTime += ((Animation*)m_CurrentAnimation)->GetTicksPerSecond() * dt;
            m_CurrentTime = fmod(m_CurrentTime, ((Animation*)m_CurrentAnimation)->GetDuration());
            CalculateBoneTransform(&((Animation*)m_CurrentAnimation)->GetRootNode(), glm::mat4(1.0f));
        }
    }

    void AnimatorComponent::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
    {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone* Bone = ((Animation*)m_CurrentAnimation)->FindBone(nodeName);

        if (Bone)
        {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = ((Animation*)m_CurrentAnimation)->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

    std::vector<glm::mat4> AnimatorComponent::computeFinalBoneMatrices(Entity armatureEntity, std::vector<Entity> bones) {
        std::vector<glm::mat4> finalBoneMatrices;
        for (int i = 0; i < MAX_BONES; ++i) {
            finalBoneMatrices.emplace_back(1.0);
        }

        for (auto boneEntity : bones) {
            auto boneID = boneEntity.getComponent<BoneComponent>().boneID;
            if (boneID < finalBoneMatrices.size()) {
                auto trans = glm::mat4(1.0);
//                trans = glm::scale(trans, glm::vec3(2, 3, 2));
                finalBoneMatrices.at(boneID) = trans;
            }
        }

        return finalBoneMatrices;
    }

    void AnimatorComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            if (node[componentName][AnimatorComponent::k_animations]) {
                auto animations = node[componentName][AnimatorComponent::k_animations].as<std::vector<std::string>>();
                entity.addComponent<AnimatorComponent>(entity, animations);
            } else {
                entity.addComponent<AnimatorComponent>();
            }
        }
    }

    void AnimatorComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<AnimatorComponent>()) {
            out << YAML::Key << AnimatorComponent::componentName;
            out << YAML::BeginMap;
            out << YAML::Key << AnimatorComponent::k_animations << YAML::Value << entity.getComponent<AnimatorComponent>().animations;
            out << YAML::EndMap;
        }
    }

    void AnimatorComponent::loadAnimations(Entity modelEntity) {
        if (!this->animations.empty()) {
            for (const auto& guid : this->animations) {
                // TODO: store animations in a list
                auto animationFilePath = Project::getResourceManager()->getFilePathFromGUID(guid);
                m_CurrentAnimation = new Animation(animationFilePath, modelEntity);
            }
        } else {
            m_CurrentAnimation = nullptr;
            // TODO: we don't need this else technically, just set everything to glm::mat4(1.0) [t-pose]
            std::cout << "Error: No animation found" << std::endl;
            exit(EXIT_FAILURE);
        }
        this->needsToLoadAnimations = false;
    }
}