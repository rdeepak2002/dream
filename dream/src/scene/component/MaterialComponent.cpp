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
//        if (!Project::getResourceManager()->hasData(this->guid)) {
//            std::string path = Project::getResourceManager()->getFilePathFromGUID(this->guid);
//            std::cout << "loading texture " << path << std::endl;
//            Project::getResourceManager()->storeData(this->guid, new OpenGLTexture(path));
//        }
//        this->texture = (OpenGLTexture*) Project::getResourceManager()->getData(this->guid);
    }

    void MaterialComponent::loadTexture() {
        if (!Project::getResourceManager()->hasData(this->guid)) {
            std::string path = Project::getResourceManager()->getFilePathFromGUID(this->guid);
            Project::getResourceManager()->storeData(this->guid, new OpenGLTexture(path));
        }
        this->texture = (OpenGLTexture*) Project::getResourceManager()->getData(this->guid);
    }

    Texture *MaterialComponent::getTexture() {
        if (!this->texture) {
            std::cout << "Texture is null" << std::endl;
        }
        return this->texture;
    }

    void MaterialComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<MaterialComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_guid << YAML::Value << entity.getComponent<MaterialComponent>().guid;
            out << YAML::EndMap;
        }
    }

    void MaterialComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            auto guid = node[componentName][k_guid].as<std::string>();
            entity.addComponent<MaterialComponent>(guid);
        }
    }
}
