/**********************************************************************************
 *  Dream is a software for developing real-time 3D experiences.
 *  Copyright (C) 2023 Deepak Ramalignam
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **********************************************************************************/

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/renderer/OpenGLTexture.h"
#include "dream/project/Project.h"
#include "dream/util/Logger.h"
#include "dream/util/YAMLUtils.h"

namespace Dream::Component {
    void MaterialComponent::loadTextures() {
        // TODO: why do we not have to load the other textures??
        if (!this->diffuseTextureGuids.empty()) {
            if (!Project::getResourceManager()->hasTextureData(this->diffuseTextureGuids.at(0))) {
                std::string path = Project::getResourceManager()->getFilePathFromGUID(this->diffuseTextureGuids.at(0));
                Project::getResourceManager()->storeTextureData(new OpenGLTexture(path), this->diffuseTextureGuids.at(0));
            }
        }
    }

    void MaterialComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<MaterialComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_isEmbedded << YAML::Value << entity.getComponent<MaterialComponent>().isEmbedded;
            out << YAML::Key << k_shininess << YAML::Value << entity.getComponent<MaterialComponent>().shininess;
            out << YAML::Key << k_diffuseTextureGuids << YAML::Value
                << entity.getComponent<MaterialComponent>().diffuseTextureGuids;
            out << YAML::Key << k_diffuseColor << YAML::Value
                << YAML::convert<glm::vec4>().encode(entity.getComponent<MaterialComponent>().diffuseColor);
            out << YAML::Key << k_specularTextureGuid << YAML::Value
                << entity.getComponent<MaterialComponent>().specularTextureGuid;
            out << YAML::Key << k_specularColor << YAML::Value
                << YAML::convert<glm::vec4>().encode(entity.getComponent<MaterialComponent>().specularColor);
            out << YAML::Key << k_heightTextureGuid << YAML::Value
                << entity.getComponent<MaterialComponent>().heightTextureGuid;
            out << YAML::Key << k_normalTextureGuid << YAML::Value
                << entity.getComponent<MaterialComponent>().normalTextureGuid;
            out << YAML::Key << k_ambientTextureGuid << YAML::Value
                << entity.getComponent<MaterialComponent>().ambientTextureGuid;
            out << YAML::Key << k_ambientColor << YAML::Value
                << YAML::convert<glm::vec4>().encode(entity.getComponent<MaterialComponent>().ambientColor);
            out << YAML::EndMap;
        }
    }

    void MaterialComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            // whether or not textures are embedded in the 3D model
            auto isEmbedded = false;
            if (node[componentName][k_isEmbedded]) {
                isEmbedded = node[componentName][k_isEmbedded].as<bool>();
            }
            // shininess
            auto shininess = 20.0f;
            if (node[componentName][k_shininess]) {
                shininess = node[componentName][k_shininess].as<float>();
            }
            // diffuse color and texture
            std::vector<std::string> diffuseTextureGuids;
            if (node[componentName][k_diffuseTextureGuids]) {
                diffuseTextureGuids = node[componentName][k_diffuseTextureGuids].as<std::vector<std::string>>();
            }
            glm::vec4 diffuseColor = {1, 1, 1, 1};
            if (node[componentName][k_diffuseColor]) {
                YAML::convert<glm::vec4>().decode(node[componentName][k_diffuseColor], diffuseColor);
            }
            // specular color and texture
            std::string specularTextureGuid;
            if (node[componentName][k_specularTextureGuid]) {
                specularTextureGuid = node[componentName][k_specularTextureGuid].as<std::string>();
            }
            glm::vec4 specularColor = {1, 1, 1, 1};
            if (node[componentName][k_specularColor]) {
                YAML::convert<glm::vec4>().decode(node[componentName][k_specularColor], specularColor);
            }
            // height texture
            std::string heightTextureGuid;
            if (node[componentName][k_heightTextureGuid]) {
                heightTextureGuid = node[componentName][k_heightTextureGuid].as<std::string>();
            }
            // normal texture
            std::string normalTextureGuid;
            if (node[componentName][k_normalTextureGuid]) {
                normalTextureGuid = node[componentName][k_normalTextureGuid].as<std::string>();
            }
            // ambient color and texture
            std::string ambientTextureGuid;
            if (node[componentName][k_ambientTextureGuid]) {
                ambientTextureGuid = node[componentName][k_ambientTextureGuid].as<std::string>();
            }
            glm::vec4 ambientColor = {1, 1, 1, 1};
            if (node[componentName][k_ambientColor]) {
                YAML::convert<glm::vec4>().decode(node[componentName][k_ambientColor], ambientColor);
            }
            entity.addComponent<MaterialComponent>();
            entity.getComponent<MaterialComponent>().isEmbedded = isEmbedded;
            entity.getComponent<MaterialComponent>().shininess = shininess;
            entity.getComponent<MaterialComponent>().diffuseTextureGuids = std::move(diffuseTextureGuids);
            entity.getComponent<MaterialComponent>().diffuseColor = diffuseColor;
            entity.getComponent<MaterialComponent>().specularTextureGuid = specularTextureGuid;
            entity.getComponent<MaterialComponent>().specularColor = specularColor;
            entity.getComponent<MaterialComponent>().heightTextureGuid = heightTextureGuid;
            entity.getComponent<MaterialComponent>().normalTextureGuid = normalTextureGuid;
            entity.getComponent<MaterialComponent>().ambientTextureGuid = ambientTextureGuid;
            entity.getComponent<MaterialComponent>().ambientColor = ambientColor;
        }
    }
}
