//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/scene/component/Component.h"

namespace Dream::Component {
    TagComponent::TagComponent(std::string tag) {
        this->tag = std::move(tag);
    }

    void TagComponent::serialize(YAML::Emitter& out) {
        out << YAML::Key << getComponentName();
        out << YAML::BeginMap;
        out << YAML::Key << "tag" << YAML::Value << this->tag;
        out << YAML::EndMap;
    }

    std::string TagComponent::getComponentName() {
        return "TagComponent";
    }
}
