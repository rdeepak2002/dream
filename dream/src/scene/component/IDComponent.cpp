//
// Created by Deepak Ramalingam on 11/18/22.
//

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/util/IDUtils.h"

namespace Dream::Component {
    IDComponent::IDComponent() {
        this->id = IDUtils::newGUID();
    }

    IDComponent::IDComponent(std::string id) {
        this->id = std::move(id);
    }

    std::string IDComponent::getID() {
        return this->id;
    }

    void IDComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<IDComponent>()) {
            out << YAML::Key << IDComponent::componentName;
            out << YAML::BeginMap;
            out << YAML::Key << IDComponent::k_id << YAML::Value << entity.getComponent<IDComponent>().id;
            out << YAML::EndMap;
        }
    }

    void IDComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            auto id = node[componentName][k_id].as<std::string>();
            entity.addComponent<IDComponent>(id);
        }
    }
}