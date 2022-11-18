//
// Created by Deepak Ramalingam on 11/18/22.
//

#include "dream/scene/component/Component.h"

namespace Dream::Component {
    void RootComponent::serialize(YAML::Emitter &out) {
        out << YAML::Key << getComponentName();
        out << YAML::BeginMap;
        out << YAML::Key << "name" << YAML::Value << this->name;
        out << YAML::EndMap;
    }

    std::string RootComponent::getComponentName() {
        return "RootComponent";
    }
}