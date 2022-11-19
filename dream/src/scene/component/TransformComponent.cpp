//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/scene/component/Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Dream::Component {
    TransformComponent::TransformComponent() {
        translation = {0, 0, 0};
        rotation = {0, 0, 0, 1.0};
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
        }
        return model * parentModel;
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
