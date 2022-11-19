//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/scene/component/Component.h"

namespace Dream::Component {
    TagComponent::TagComponent(std::string tag) {
        this->tag = std::move(tag);
    }

    void TagComponent::serialize(YAML::Emitter& out, Entity &entity) {
        if (entity.hasComponent<TagComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_tag << YAML::Value << entity.getComponent<TagComponent>().tag;
            out << YAML::EndMap;
        }
    }

    void TagComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            auto tag = node[componentName][k_tag].as<std::string>();
            entity.addComponent<TagComponent>(tag);
        }
    }
}
