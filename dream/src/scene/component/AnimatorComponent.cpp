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