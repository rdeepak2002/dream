//
// Created by Deepak Ramalingam on 12/10/22.
//

#include "dream/scene/component/Component.h"
#include "dream/util/Logger.h"

namespace Dream::Component {
    void CameraComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<CameraComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_fov << YAML::Value << entity.getComponent<CameraComponent>().fov;
            out << YAML::EndMap;
        }
    }

    void CameraComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            if (!node[componentName][k_fov]) {
                Logger::fatal("No fov variable for camera component");
            }
            auto fov = node[componentName][k_fov].as<float>();
            entity.addComponent<CameraComponent>(fov);
        }
    }

    CameraComponent::CameraComponent(float fov) {
        this->fov = fov;
    }
}