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

#include "dream/scene/component/Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "dream/util/YAMLUtils.h"

namespace Dream::Component {
    TransformComponent::TransformComponent() {
        translation = {0, 0, 0};
        rotation = {1, 0, 0, 0};
        scale = {1, 1, 1};
    }


    TransformComponent::TransformComponent(glm::vec3 translation, glm::quat rotation, glm::vec3 scale) {
        this->translation = translation;
        this->rotation = rotation;
        this->scale = scale;
    }

    glm::mat4 TransformComponent::getTransform(Entity &curEntity) {
        glm::mat4 rotMat4 = glm::toMat4(glm::quat(rotation));
        glm::mat4 model = glm::translate(glm::mat4(1.0f), translation) * rotMat4 * glm::scale(glm::mat4(1.0f), scale);
        glm::mat4 parentModel = glm::mat4(1.0);
        Entity parent = curEntity.getComponent<HierarchyComponent>().parent;
        if (parent) {
            parentModel = parent.getComponent<TransformComponent>().getTransform(parent);
        } else {
            parentModel = glm::mat4(1.0);
        }
        return parentModel * model;
    }

    void TransformComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<TransformComponent>()) {
            auto &transformComponent = entity.getComponent<TransformComponent>();
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_translation << YAML::Value << YAML::convert<glm::vec3>().encode(transformComponent.translation);
            out << YAML::Key << k_rotation << YAML::Value << YAML::convert<glm::quat>().encode(transformComponent.rotation);
            out << YAML::Key << k_scale << YAML::Value << YAML::convert<glm::vec3>().encode(transformComponent.scale);
            out << YAML::EndMap;
        }
    }

    void TransformComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            glm::vec3 translation;
            YAML::convert<glm::vec3>().decode(node[componentName][k_translation], translation);
            glm::quat rotation;
            YAML::convert<glm::quat>().decode(node[componentName][k_rotation], rotation);
            glm::vec3 scale;
            YAML::convert<glm::vec3>().decode(node[componentName][k_scale], scale);
            entity.addComponent<TransformComponent>(translation, rotation, scale);
        }
    }
}
