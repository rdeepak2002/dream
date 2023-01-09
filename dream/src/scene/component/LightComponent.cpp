//
// Created by Deepak Ramalingam on 1/4/23.
//

#include "dream/scene/component/Component.h"
#include "dream/util/YAMLUtils.h"

namespace Dream::Component {
    void LightComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<LightComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_type << YAML::Value << entity.getComponent<LightComponent>().type;
            out << YAML::Key << k_cutOff << YAML::Value << entity.getComponent<LightComponent>().cutOff;
            out << YAML::Key << k_outerCutOff << YAML::Value << entity.getComponent<LightComponent>().outerCutOff;
            out << YAML::Key << k_constant << YAML::Value << entity.getComponent<LightComponent>().constant;
            out << YAML::Key << k_linear << YAML::Value << entity.getComponent<LightComponent>().linear;
            out << YAML::Key << k_quadratic << YAML::Value << entity.getComponent<LightComponent>().quadratic;
            out << YAML::Key << k_color << YAML::Value
                << YAML::convert<glm::vec3>().encode(entity.getComponent<LightComponent>().color);
            out << YAML::EndMap;
        }
    }

    void LightComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            // type
            LightType type = LightType::DIRECTIONAL;
            if (node[componentName][k_type]) {
                type = static_cast<LightType>(node[componentName][k_type].as<int>());
            }
            // cut off
            float cutOff = 0.0;
            if (node[componentName][k_cutOff]) {
                cutOff = node[componentName][k_cutOff].as<float>();
            }
            // outer cut off
            float outerCutOff = 0.0;
            if (node[componentName][k_outerCutOff]) {
                outerCutOff = node[componentName][k_outerCutOff].as<float>();
            }
            // constant
            float constant = 0.0;
            if (node[componentName][k_constant]) {
                constant = node[componentName][k_constant].as<float>();
            }
            // linear
            float linear = 0.0;
            if (node[componentName][k_linear]) {
                linear = node[componentName][k_linear].as<float>();
            }
            // linear
            float quadratic = 0.0;
            if (node[componentName][k_quadratic]) {
                quadratic = node[componentName][k_quadratic].as<float>();
            }
            glm::vec3 color = {1, 1, 1};
            if (node[componentName][k_color]) {
                YAML::convert<glm::vec3>().decode(node[componentName][k_color], color);
            }
            entity.addComponent<LightComponent>();
            entity.getComponent<LightComponent>().type = type;
            entity.getComponent<LightComponent>().cutOff = cutOff;
            entity.getComponent<LightComponent>().outerCutOff = outerCutOff;
            entity.getComponent<LightComponent>().constant = constant;
            entity.getComponent<LightComponent>().linear = linear;
            entity.getComponent<LightComponent>().quadratic = quadratic;
            entity.getComponent<LightComponent>().color = color;
        }
    }
}