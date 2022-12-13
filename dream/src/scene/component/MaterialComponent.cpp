//
// Created by Deepak Ramalingam on 11/13/22.
//

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/renderer/OpenGLTexture.h"
#include "dream/project/Project.h"
#include "dream/util/Logger.h"
#include "dream/util/YAMLUtils.h"

namespace Dream::Component {
    MaterialComponent::MaterialComponent(std::string guid, bool isEmbedded) {
        this->guid = std::move(guid);
        this->isEmbedded = isEmbedded;
    }

    void MaterialComponent::loadTexture() {
        if (!this->guid.empty()) {
            if (!Project::getResourceManager()->hasData(this->guid)) {
                std::string path = Project::getResourceManager()->getFilePathFromGUID(this->guid);
                Project::getResourceManager()->storeData(this->guid, new OpenGLTexture(path));
            }
            this->diffuseTexture = (OpenGLTexture*) Project::getResourceManager()->getData(this->guid);
        }
    }

    Texture *MaterialComponent::getTexture() {
        if (!this->diffuseTexture) {
            Logger::fatal("Texture is null");
        }
        return this->diffuseTexture;
    }

    void MaterialComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<MaterialComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_guid << YAML::Value << entity.getComponent<MaterialComponent>().guid;
            out << YAML::Key << k_isEmbedded << YAML::Value << entity.getComponent<MaterialComponent>().isEmbedded;
            out << YAML::Key << k_diffuseColor << YAML::Value << YAML::convert<glm::vec4>().encode(entity.getComponent<MaterialComponent>().diffuseColor);
            out << YAML::EndMap;
        }
    }

    void MaterialComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            std::string guid = "";
            if (node[componentName][k_guid]) {
                guid = node[componentName][k_guid].as<std::string>();
            }
            auto isEmbedded = false;
            if (node[componentName][k_isEmbedded]) {
                isEmbedded = node[componentName][k_isEmbedded].as<bool>();
            }
            glm::vec4 diffuseColor = {1, 1, 1, 1};
            if (node[componentName][k_diffuseColor]) {
                YAML::convert<glm::vec4>().decode(node[componentName][k_diffuseColor], diffuseColor);
            }
            entity.addComponent<MaterialComponent>(guid, isEmbedded);
            entity.getComponent<MaterialComponent>().diffuseColor = diffuseColor;
        }
    }
}
