//
// Created by Deepak Ramalingam on 12/3/22.
//

#include "dream/scene/component/Component.h"

namespace Dream::Component {
    AnimatorComponent::AnimatorComponent() {

    }

    void AnimatorComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
//            auto id = node[componentName][k_id].as<std::string>();
            entity.addComponent<AnimatorComponent>();
        }
    }

    void AnimatorComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<AnimatorComponent>()) {
            out << YAML::Key << AnimatorComponent::componentName;
            out << YAML::BeginMap;
//            out << YAML::Key << IDComponent::k_id << YAML::Value << entity.getComponent<IDComponent>().id;
            out << YAML::EndMap;
        }
    }
}