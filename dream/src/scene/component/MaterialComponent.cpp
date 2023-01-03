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
        if (shouldLoadTextures) {
            if (!this->diffuseTextureGuid.empty()) {
                if (!Project::getResourceManager()->hasTextureData(this->diffuseTextureGuid)) {
                    std::string path = Project::getResourceManager()->getFilePathFromGUID(this->diffuseTextureGuid);
                    Project::getResourceManager()->storeTextureData(new OpenGLTexture(path), this->diffuseTextureGuid);
                }
            }
            shouldLoadTextures = false;
        }
    }

    void MaterialComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<MaterialComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_isEmbedded << YAML::Value << entity.getComponent<MaterialComponent>().isEmbedded;
            out << YAML::Key << k_diffuseTextureGuid << YAML::Value
                << entity.getComponent<MaterialComponent>().diffuseTextureGuid;
            out << YAML::Key << k_diffuseColor << YAML::Value
                << YAML::convert<glm::vec4>().encode(entity.getComponent<MaterialComponent>().diffuseColor);
            out << YAML::EndMap;
        }
    }

    void MaterialComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            auto isEmbedded = false;
            if (node[componentName][k_isEmbedded]) {
                isEmbedded = node[componentName][k_isEmbedded].as<bool>();
            }
            std::string diffuseTextureGuid = "";
            if (node[componentName][k_diffuseTextureGuid]) {
                diffuseTextureGuid = node[componentName][k_diffuseTextureGuid].as<std::string>();
            }
            glm::vec4 diffuseColor = {1, 1, 1, 1};
            if (node[componentName][k_diffuseColor]) {
                YAML::convert<glm::vec4>().decode(node[componentName][k_diffuseColor], diffuseColor);
            }
            entity.addComponent<MaterialComponent>();
            entity.getComponent<MaterialComponent>().isEmbedded = isEmbedded;
            entity.getComponent<MaterialComponent>().diffuseTextureGuid = diffuseTextureGuid;
            entity.getComponent<MaterialComponent>().diffuseColor = diffuseColor;
        }
    }
}
