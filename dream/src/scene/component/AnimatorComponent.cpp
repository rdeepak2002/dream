//
// Created by Deepak Ramalingam on 12/3/22.
//

#include "dream/scene/component/Component.h"

namespace Dream::Component {
    AnimatorComponent::AnimatorComponent() {

    }

    AnimatorComponent::AnimatorComponent(std::vector<std::string> animations) {
        this->animations = animations;
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
                entity.addComponent<AnimatorComponent>(animations);
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
}