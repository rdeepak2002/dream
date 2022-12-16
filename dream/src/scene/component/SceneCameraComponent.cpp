//
// Created by Deepak Ramalingam on 12/10/22.
//

#include "dream/scene/component/Component.h"
#include "dream/util/Logger.h"
#include "dream/util/MathUtils.h"
#include "dream/window/Input.h"
#include "dream/window/KeyCodes.h"
#include "dream/util/YAMLUtils.h"

namespace Dream::Component {
    void SceneCameraComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<SceneCameraComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_fov << YAML::Value << entity.getComponent<SceneCameraComponent>().fov;
            out << YAML::Key << k_zNear << YAML::Value << entity.getComponent<SceneCameraComponent>().zNear;
            out << YAML::Key << k_zFar << YAML::Value << entity.getComponent<SceneCameraComponent>().zFar;
            out << YAML::Key << k_front << YAML::Value << YAML::convert<glm::vec3>().encode(entity.getComponent<SceneCameraComponent>().front);
            out << YAML::Key << k_up << YAML::Value << YAML::convert<glm::vec3>().encode(entity.getComponent<SceneCameraComponent>().up);
            out << YAML::Key << k_right << YAML::Value << YAML::convert<glm::vec3>().encode(entity.getComponent<SceneCameraComponent>().right);
            out << YAML::Key << k_worldUp << YAML::Value << YAML::convert<glm::vec3>().encode(entity.getComponent<SceneCameraComponent>().worldUp);
            out << YAML::Key << k_yaw << YAML::Value << entity.getComponent<SceneCameraComponent>().yaw;
            out << YAML::Key << k_pitch << YAML::Value << entity.getComponent<SceneCameraComponent>().pitch;
            out << YAML::EndMap;
        }
    }

    void SceneCameraComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            if (!node[componentName][k_fov]) {
                Logger::fatal("No fov variable for scene camera component");
            }

            // TODO: check other variables are defined too, not just fov

            auto fov = node[componentName][k_fov].as<float>();
            auto zNear = node[componentName][k_zNear].as<float>();
            auto zFar = node[componentName][k_zFar].as<float>();
            glm::vec3 front;
            YAML::convert<glm::vec3>().decode(node[componentName][k_front], front);
            glm::vec3 up;
            YAML::convert<glm::vec3>().decode(node[componentName][k_up], up);
            glm::vec3 right;
            YAML::convert<glm::vec3>().decode(node[componentName][k_right], right);
            glm::vec3 worldUp;
            YAML::convert<glm::vec3>().decode(node[componentName][k_worldUp], worldUp);
            auto yaw = node[componentName][k_yaw].as<float>();
            auto pitch = node[componentName][k_pitch].as<float>();

            entity.addComponent<SceneCameraComponent>(fov);
            entity.getComponent<SceneCameraComponent>().zNear = zNear;
            entity.getComponent<SceneCameraComponent>().zFar = zFar;
            entity.getComponent<SceneCameraComponent>().front = front;
            entity.getComponent<SceneCameraComponent>().up = up;
            entity.getComponent<SceneCameraComponent>().right = right;
            entity.getComponent<SceneCameraComponent>().worldUp = worldUp;
            entity.getComponent<SceneCameraComponent>().yaw = yaw;
            entity.getComponent<SceneCameraComponent>().pitch = pitch;
        }
    }

    SceneCameraComponent::SceneCameraComponent(float fov) {
        this->fov = fov;
        worldUp = {0, -1.f, 0};
        yaw = -90.f;
        pitch = 0.0f;
        updateCameraVectors();
    }

    glm::mat4 SceneCameraComponent::getViewMatrix(Entity sceneCamera) {
        glm::vec3 &position = sceneCamera.getComponent<TransformComponent>().translation;
        auto view = glm::lookAt(position, position + front, up);
        return view;
    }

    void SceneCameraComponent::processInput(Entity sceneCamera, float dt) {
        auto mouseMovement = Input::getMouseMovement();
        auto mouseScroll = Input::getMouseScroll();
        glm::vec3 &position = sceneCamera.getComponent<TransformComponent>().translation;
        if (Input::getButtonDown(Key::LeftMouse)) {
            float padding = 0.01;
            position -= right * mouseMovement.x * padding;
            position += up * mouseMovement.y * padding;
        }
        {
            float padding = 0.1;
            position += front * mouseScroll.y * padding;
        }
        if (Input::getButtonDown(Key::RightMouse)) {
            float padding = 0.4;
            yaw -= mouseMovement.x * padding;
            pitch += mouseMovement.y * padding;
        }
        if (pitch > 89.9) {
            pitch = 89.9;
        }
        if (pitch < -89.9) {
            pitch = -89.9;
        }
        sceneCamera.getComponent<TransformComponent>().rotation = glm::quat(glm::vec3(yaw, pitch, 0));
        updateCameraVectors();
    }

    void SceneCameraComponent::updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }

    void SceneCameraComponent::lookAt(Entity sceneCamera, glm::vec3 lookAtPos) {
        glm::vec3 &position = sceneCamera.getComponent<TransformComponent>().translation;
        glm::quat q = glm::conjugate(glm::toQuat(
                glm::lookAt(position,
                            glm::vec3(lookAtPos.x, lookAtPos.y, lookAtPos.z),
                            glm::vec3(0, 1, 0)
                )
        ));
        glm::vec3 eulerAngles = glm::eulerAngles(q);
        yaw = glm::degrees(eulerAngles.y) + 90;
        pitch = glm::degrees(eulerAngles.x);
    }
}