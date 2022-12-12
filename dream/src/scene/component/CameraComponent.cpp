//
// Created by Deepak Ramalingam on 12/10/22.
//

#include "dream/scene/component/Component.h"
#include "dream/util/Logger.h"
#include "dream/util/YAMLUtils.h"

namespace Dream::Component {
    void CameraComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<CameraComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_fov << YAML::Value << entity.getComponent<CameraComponent>().fov;
            out << YAML::Key << k_zNear << YAML::Value << entity.getComponent<CameraComponent>().zNear;
            out << YAML::Key << k_zFar << YAML::Value << entity.getComponent<CameraComponent>().zFar;
            out << YAML::Key << k_lookAt << YAML::Value << YAML::convert<glm::vec3>().encode(entity.getComponent<CameraComponent>().lookAt);
            out << YAML::Key << k_front << YAML::Value << YAML::convert<glm::vec3>().encode(entity.getComponent<CameraComponent>().front);
            out << YAML::Key << k_up << YAML::Value << YAML::convert<glm::vec3>().encode(entity.getComponent<CameraComponent>().up);
            out << YAML::Key << k_right << YAML::Value << YAML::convert<glm::vec3>().encode(entity.getComponent<CameraComponent>().right);
            out << YAML::EndMap;
        }
    }

    void CameraComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            if (!node[componentName][k_fov]) {
                Logger::fatal("No fov variable for camera component");
            }

            // TODO: check other variables are defined too (zNear, zFar, etc.)

            auto fov = node[componentName][k_fov].as<float>();
            auto zNear = node[componentName][k_zNear].as<float>();
            auto zFar = node[componentName][k_zFar].as<float>();
            glm::vec3 lookAt;
            YAML::convert<glm::vec3>().decode(node[componentName][k_lookAt], lookAt);
            glm::vec3 front;
            YAML::convert<glm::vec3>().decode(node[componentName][k_front], front);
            glm::vec3 up;
            YAML::convert<glm::vec3>().decode(node[componentName][k_up], up);
            glm::vec3 right;
            YAML::convert<glm::vec3>().decode(node[componentName][k_right], right);

            entity.addComponent<CameraComponent>(fov);
            entity.getComponent<CameraComponent>().zNear = zNear;
            entity.getComponent<CameraComponent>().zFar = zFar;
            entity.getComponent<CameraComponent>().lookAt = lookAt;
            entity.getComponent<CameraComponent>().front = front;
            entity.getComponent<CameraComponent>().up = up;
            entity.getComponent<CameraComponent>().right = right;
        }
    }

    glm::mat4 CameraComponent::getViewMatrix(Entity camera) {
        glm::vec3 &position = camera.getComponent<TransformComponent>().translation;
        if (glm::l2Norm(lookAt - position) <= 0.0f) {
            lookAt = position + glm::vec3(1, 0, 0);
        }
        return glm::lookAt(position, lookAt, up);
    }

    CameraComponent::CameraComponent(float fov) {
        this->fov = fov;
        this->up = {0, -1, 0};
        this->front = {-1, 0, 0};
        this->right = {0, 0, 1};
    }

    void CameraComponent::updateCameraVectors(Entity camera) {
        glm::vec3 position = camera.getComponent<TransformComponent>().translation;
        glm::vec3 newFront;
        if (glm::l2Norm(lookAt - position) <= 0.0f) {
            newFront = {0, -1, 0};
        } else {
            // TODO: this is untested, verify it
            newFront = glm::normalize(lookAt - position);
        }
        front = glm::normalize(newFront);
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
}