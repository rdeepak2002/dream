//
// Created by Deepak Ramalingam on 11/13/22.
//

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/renderer/OpenGLTexture.h"
#include "dream/project/Project.h"

namespace Dream::Component {
    MaterialComponent::MaterialComponent(std::string guid) {
        this->guid = std::move(guid);
        this->texture = (OpenGLTexture*) Project::getResourceManager()->getData(this->guid);
    }

    Texture *MaterialComponent::getTexture() {
        if (!this->texture) {
            std::cout << "Texture is null" << std::endl;
        }
        return this->texture;
    }

    void MaterialComponent::serialize(YAML::Emitter &out) {
        out << YAML::Key << getComponentName();
        out << YAML::BeginMap;
        out << YAML::Key << "guid" << YAML::Value << this->guid;
        out << YAML::EndMap;
    }

    std::string MaterialComponent::getComponentName() {
        return "MaterialComponent";
    }
}
