//
// Created by Deepak Ramalingam on 2/10/23.
//

#include "dream/renderer/LightingTech.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"
#include "dream/renderer/OpenGLTexture.h"

namespace Dream {
    LightingTech::LightingTech() {
        whiteTexture = new OpenGLTexture(Project::getPath().append("assets").append("textures").append("white.png"));
        blackTexture = new OpenGLTexture(Project::getPath().append("assets").append("textures").append("black.png"));
    }

    LightingTech::~LightingTech() {
        delete this->whiteTexture;
        delete this->blackTexture;
    }

    void LightingTech::setTextureAndColorUniforms(Entity entity, std::vector<OpenGLShadowMapFBO *> shadowMapFbos, DirectionalLightShadowTech* directionalLightShadowTech, OpenGLShader *shader) {
        setLightShaderUniforms(shader);

        if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::FINAL) {
            // final rendering (combine all variables to compute final color)
            // set shininess
            if (entity.hasComponent<Component::MaterialComponent>()) {
                float shininess = entity.getComponent<Component::MaterialComponent>().shininess;
                if (shininess <= 0) {
                    shader->setFloat("shininess", 2.0f);
                } else {
                    shader->setFloat("shininess", shininess);
                }
            } else {
                shader->setFloat("shininess", 20.0f);
            }

            // load diffuse color + texture of entity
            {
                if (entity.hasComponent<Component::MaterialComponent>()) {
                    shader->setVec4("diffuse_color", entity.getComponent<Component::MaterialComponent>().diffuseColor);
                } else {
                    shader->setVec4("diffuse_color", glm::vec4(1.0, 1.0, 1.0, 1.0));
                }

                if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().diffuseTextureGuids.empty()) {
                    // diffuse texture
                    // TODO: iterate through vector and do not just get first diffuse texture, instead blend them
                    entity.getComponent<Component::MaterialComponent>().loadTextures();
                    auto diffuseTexture = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().diffuseTextureGuids.at(0));
                    if (auto openGLDiffuseTexture = std::dynamic_pointer_cast<OpenGLTexture>(diffuseTexture)) {
                        shader->setInt("texture_diffuse1", 0);
                        openGLDiffuseTexture->bind(0);
                    } else {
                        Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                    }
                } else {
                    // default diffuse texture
                    shader->setInt("texture_diffuse1", 0);
                    whiteTexture->bind(0);
                }
            }

            // load specular color + texture of entity
            {
                if (entity.hasComponent<Component::MaterialComponent>()) {
                    shader->setVec4("specular_color", entity.getComponent<Component::MaterialComponent>().specularColor);
                } else {
                    shader->setVec4("specular_color", glm::vec4(1.0, 1.0, 1.0, 1.0));
                }

                if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().specularTextureGuid.empty()) {
                    entity.getComponent<Component::MaterialComponent>().loadTextures();
                    auto specularTexture = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().specularTextureGuid);
                    if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(specularTexture)) {
                        shader->setInt("texture_specular", 1);
                        openGLTexture->bind(1);
                    } else {
                        Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                    }
                } else {
                    // default specular texture
                    shader->setInt("texture_specular", 1);
                    blackTexture->bind(1);
                }
            }

            // load normal texture of entity
            {
                if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().normalTextureGuid.empty()) {
                    entity.getComponent<Component::MaterialComponent>().loadTextures();
                    auto normalTexture = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().normalTextureGuid);
                    if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(normalTexture)) {
                        shader->setInt("texture_normal", 2);
                        openGLTexture->bind(2);
                    } else {
                        Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                    }
                } else {
                    // default diffuse texture
                    shader->setInt("texture_normal", 2);
                    blackTexture->bind(2);
                }
            }

            // load height texture of entity
            {
                if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().heightTextureGuid.empty()) {
                    entity.getComponent<Component::MaterialComponent>().loadTextures();
                    auto heightTexture = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().heightTextureGuid);
                    if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(heightTexture)) {
                        shader->setInt("texture_height", 3);
                        openGLTexture->bind(3);
                    } else {
                        Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                    }
                } else {
                    // default diffuse texture
                    shader->setInt("texture_height", 3);
                    blackTexture->bind(3);
                }
            }

            // load ambient color + texture of entity
            {
                if (entity.hasComponent<Component::MaterialComponent>()) {
                    shader->setVec4("ambient_color", entity.getComponent<Component::MaterialComponent>().ambientColor);
                } else {
                    shader->setVec4("ambient_color", glm::vec4(1.0, 1.0, 1.0, 1.0));
                }

                if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().ambientTextureGuid.empty()) {
                    entity.getComponent<Component::MaterialComponent>().loadTextures();
                    auto ambientTexture = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().ambientTextureGuid);
                    if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(ambientTexture)) {
                        shader->setInt("texture_ambient", 4);
                        openGLTexture->bind(4);
                    } else {
                        Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                    }
                } else {
                    // default ambient texture
                    shader->setInt("texture_ambient", 4);
                    whiteTexture->bind(4);
                }
            }

            // pass in shadow maps
            int shadowMapTexturesStart = 5;
            for (int i = 0; i < directionalLightShadowTech->getNumCascades(); ++i) {
                int textureIndex = shadowMapTexturesStart + i;
                shader->setInt("shadowMaps[" + std::to_string(i) + "]", textureIndex);
                shadowMapFbos.at(i)->bindForReading(textureIndex);
            }
        } else if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::DIFFUSE) {
            // debug diffuse
            shader->setVec4("color", {1, 1, 1, 1});
            if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().diffuseTextureGuids.empty()) {
                entity.getComponent<Component::MaterialComponent>().loadTextures();
                auto tex = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().diffuseTextureGuids.at(0));
                if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(tex)) {
                    shader->setInt("tex", 0);
                    openGLTexture->bind(0);
                } else {
                    Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                }
            } else {
                shader->setInt("tex", 0);
                blackTexture->bind(0);
            }
        } else if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::SPECULAR) {
            // debug specular
            shader->setVec4("color", {1, 1, 1, 1});
            if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().specularTextureGuid.empty()) {
                entity.getComponent<Component::MaterialComponent>().loadTextures();
                auto tex = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().specularTextureGuid);
                if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(tex)) {
                    shader->setInt("tex", 0);
                    openGLTexture->bind(0);
                } else {
                    Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                }
            } else {
                shader->setInt("tex", 0);
                blackTexture->bind(0);
            }
        } else if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::NORMAL) {
            // debug normal
            shader->setVec4("color", {1, 1, 1, 1});
            if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().normalTextureGuid.empty()) {
                entity.getComponent<Component::MaterialComponent>().loadTextures();
                auto tex = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().normalTextureGuid);
                if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(tex)) {
                    shader->setInt("tex", 0);
                    openGLTexture->bind(0);
                } else {
                    Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                }
            } else {
                shader->setInt("texture", 0);
                blackTexture->bind(0);
            }
        } else {
            Logger::fatal("Unknown rendering type");
        }
    }

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