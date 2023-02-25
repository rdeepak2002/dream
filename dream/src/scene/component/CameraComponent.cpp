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
#include "dream/util/Logger.h"
#include "dream/util/MathUtils.h"
#include "dream/window/Input.h"
#include "dream/window/KeyCodes.h"
#include "dream/util/YAMLUtils.h"

namespace Dream::Component {
    void CameraComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<CameraComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_fov << YAML::Value << entity.getComponent<CameraComponent>().fov;
            out << YAML::Key << k_zNear << YAML::Value << entity.getComponent<CameraComponent>().zNear;
            out << YAML::Key << k_zFar << YAML::Value << entity.getComponent<CameraComponent>().zFar;
            out << YAML::Key << k_yaw << YAML::Value << entity.getComponent<CameraComponent>().yaw;
            out << YAML::Key << k_pitch << YAML::Value << entity.getComponent<CameraComponent>().pitch;
            out << YAML::EndMap;
        }
    }

    void CameraComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            if (!node[componentName][k_fov]) {
                Logger::fatal("No fov variable for camera component");
            }

            if (!node[componentName][k_zNear]) {
                Logger::fatal("No near variable for camera component");
            }

            if (!node[componentName][k_zFar]) {
                Logger::fatal("No far variable for camera component");
            }

            if (!node[componentName][k_yaw]) {
                Logger::fatal("No yaw variable for camera component");
            }

            if (!node[componentName][k_pitch]) {
                Logger::fatal("No pitch variable for camera component");
            }

            auto fov = node[componentName][k_fov].as<float>();
            auto zNear = node[componentName][k_zNear].as<float>();
            auto zFar = node[componentName][k_zFar].as<float>();
            auto yaw = node[componentName][k_yaw].as<float>();
            auto pitch = node[componentName][k_pitch].as<float>();

            entity.addComponent<CameraComponent>(fov);
            entity.getComponent<CameraComponent>().fov = fov;
            entity.getComponent<CameraComponent>().zNear = zNear;
            entity.getComponent<CameraComponent>().zFar = zFar;
            entity.getComponent<CameraComponent>().yaw = yaw;
            entity.getComponent<CameraComponent>().pitch = pitch;
        }
    }

    CameraComponent::CameraComponent(float fov) {
        this->fov = fov;
        this->up = {0, 1, 0};
        this->worldUp = {0, 1, 0};
        this->front = {1, 0, 0};
        updateCameraVectors();
    }

    void CameraComponent::updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(yaw) * cos(pitch);
        newFront.y = sin(pitch);
        newFront.z = sin(yaw) * cos(pitch);
//        front = glm::normalize(newFront);
        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
    }

    void CameraComponent::updateRendererCamera(Dream::Camera &camera, Entity &sceneCameraEntity) {
        auto eulerAngles = glm::eulerAngles(sceneCameraEntity.getComponent<TransformComponent>().rotation);
        auto transformComponent = sceneCameraEntity.getComponent<TransformComponent>();
//        camera.yaw = eulerAngles.x;
//        camera.pitch = eulerAngles.y;
        camera.fov = fov;
        camera.position = transformComponent.translation;
        camera.zFar = zFar;
        camera.zNear = zNear;
//        camera.updateCameraVectors();
        camera.front = front;
    }

    void CameraComponent::lookAt(Entity sceneCamera, glm::vec3 lookAtPos) {
        glm::vec3 &position = sceneCamera.getComponent<TransformComponent>().translation;
//        glm::quat q = glm::conjugate(glm::toQuat(
//                glm::lookAt(position,
//                            glm::vec3(lookAtPos.x, lookAtPos.y, lookAtPos.z),
//                            glm::vec3(0, 1, 0)
//                )
//        ));
//        glm::quat q = glm::conjugate(glm::toQuat(
//                glm::lookAt(glm::vec3(lookAtPos.x, lookAtPos.y, lookAtPos.z),
//                            position,
//                            glm::vec3(0, 1, 0)
//                )
//        ));
//        glm::vec3 eulerAngles = glm::eulerAngles(q);
//        yaw = eulerAngles.x;
//        pitch = eulerAngles.y;
        front = glm::vec3(lookAtPos.x, lookAtPos.y, lookAtPos.z) - position;
        pitch = asin(-front.y);
        yaw = atan2(front.x, front.z);
        sceneCamera.getComponent<TransformComponent>().rotation = glm::quat(glm::vec3(yaw, pitch, 0.0f));
    }
}