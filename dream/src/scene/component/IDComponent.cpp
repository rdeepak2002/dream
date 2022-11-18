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

    void IDComponent::serialize(YAML::Emitter &out) {
        out << YAML::Key << getComponentName();
        out << YAML::BeginMap;
        out << YAML::Key << "id" << YAML::Value << this->id;
        out << YAML::EndMap;
    }

    std::string IDComponent::getComponentName() {
        return "IDComponent";
    }
}