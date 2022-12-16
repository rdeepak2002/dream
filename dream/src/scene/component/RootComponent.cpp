//
// Created by Deepak Ramalingam on 11/18/22.
//

#include <utility>

#include "dream/scene/component/Component.h"

namespace Dream::Component {
    RootComponent::RootComponent() {
        this->name = "root";
    }

    RootComponent::RootComponent(std::string name) {
        this->name = std::move(name);
    }

    void RootComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<RootComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_name << YAML::Value << entity.getComponent<RootComponent>().name;
            out << YAML::EndMap;
        }
    }

    void RootComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            auto name = node[componentName][k_name].as<std::string>();
            entity.addComponent<RootComponent>(name);
        }
    }
}