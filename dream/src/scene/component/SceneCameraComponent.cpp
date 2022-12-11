//
// Created by Deepak Ramalingam on 12/10/22.
//

#include "dream/scene/component/Component.h"
#include "dream/util/Logger.h"

namespace Dream::Component {
    void SceneCameraComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<SceneCameraComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_fov << YAML::Value << entity.getComponent<SceneCameraComponent>().fov;
            out << YAML::EndMap;
        }
    }

    void SceneCameraComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            if (!node[componentName][k_fov]) {
                Logger::fatal("No fov variable for scene camera component");
            }
            auto fov = node[componentName][k_fov].as<float>();
            entity.addComponent<SceneCameraComponent>(fov);
        }
    }

    SceneCameraComponent::SceneCameraComponent(float fov) {
        this->fov = fov;
    }
}