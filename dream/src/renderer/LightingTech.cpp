//
// Created by Deepak Ramalingam on 2/10/23.
//

#include "dream/renderer/LightingTech.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    // TODO: move to lighting tech?
    void LightingTech::setLightShaderUniforms(OpenGLShader* shader) {
        std::vector<Entity> directionalLights;
        std::vector<Entity> pointLights;
        std::vector<Entity> spotLights;
        for (auto lightEntityHandle : Project::getScene()->getEntitiesWithComponents<Component::LightComponent>()) {
            Entity lightEntity = {lightEntityHandle, Project::getScene()};
            if (lightEntity.getComponent<Component::LightComponent>().type == Component::LightComponent::DIRECTIONAL) {
                directionalLights.push_back(lightEntity);
            } else if (lightEntity.getComponent<Component::LightComponent>().type == Component::LightComponent::POINT) {
                pointLights.push_back(lightEntity);
            } else if (lightEntity.getComponent<Component::LightComponent>().type == Component::LightComponent::SPOTLIGHT) {
                spotLights.push_back(lightEntity);
            } else {
                Logger::fatal("Unknown light type");
            }
        }

        shader->setVec3("ambientColor", glm::vec3(0.15, 0.15, 0.15));

        // define current number of point lights
        shader->setInt("numberOfDirLights", (int) directionalLights.size());
        shader->setInt("numberOfPointLights", (int) pointLights.size());
        shader->setInt("numberOfSpotLights", (int) spotLights.size());

        for (int i = 0; i < directionalLights.size(); i++) {
            auto &lightEntity = directionalLights.at(i);
            const auto &lightComponent = lightEntity.getComponent<Component::LightComponent>();
            std::string prefix = "dirLights[" + std::to_string(i) + "]";

            shader->setVec3(prefix + ".direction", lightEntity.getComponent<Component::TransformComponent>().getFront());
            shader->setVec3(prefix + ".ambient", lightComponent.color);
            shader->setVec3(prefix + ".diffuse", lightComponent.color);
            shader->setVec3(prefix + ".specular", lightComponent.color);
        }

        for (int i = 0; i < pointLights.size(); i++) {
            auto &lightEntity = pointLights.at(i);
            const auto &lightComponent = lightEntity.getComponent<Component::LightComponent>();
            std::string prefix = "pointLights[" + std::to_string(i) + "]";
            // TODO: get global translation
            auto lightPos = lightEntity.getComponent<Component::TransformComponent>().translation;
            shader->setVec3(prefix + ".position", lightPos);
            shader->setVec3(prefix + ".ambient", lightComponent.color);
            shader->setVec3(prefix + ".diffuse", lightComponent.color);
            shader->setVec3(prefix + ".specular", lightComponent.color);
            shader->setFloat(prefix + ".constant", lightComponent.constant);
            shader->setFloat(prefix + ".linear", lightComponent.linear);
            shader->setFloat(prefix + ".quadratic", lightComponent.quadratic);
        }

        for (int i = 0; i < spotLights.size(); i++) {
            auto &lightEntity = spotLights.at(i);
            const auto &lightComponent = lightEntity.getComponent<Component::LightComponent>();
            std::string prefix = "spotLights[" + std::to_string(i) + "]";
            // TODO: get global translation
            shader->setVec3(prefix + ".position", lightEntity.getComponent<Component::TransformComponent>().translation);
            shader->setVec3(prefix + ".direction", lightEntity.getComponent<Component::TransformComponent>().getFront());
            shader->setVec3(prefix + ".ambient", lightComponent.color);
            shader->setVec3(prefix + ".diffuse", lightComponent.color);
            shader->setVec3(prefix + ".specular", lightComponent.color);
            shader->setFloat(prefix + ".constant", lightComponent.constant);
            shader->setFloat(prefix + ".linear", lightComponent.linear);
            shader->setFloat(prefix + ".quadratic", lightComponent.quadratic);
            shader->setFloat(prefix + ".cutOff", glm::cos(glm::radians(lightComponent.cutOff)));
            shader->setFloat(prefix + ".outerCutOff", glm::cos(glm::radians(lightComponent.outerCutOff)));
        }
    }
}