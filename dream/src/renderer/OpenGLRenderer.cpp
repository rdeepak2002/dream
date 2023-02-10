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

#include "dream/renderer/OpenGLRenderer.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/glad.h>
#include "dream/project/Project.h"
#include "dream/scene/Entity.h"
#include "dream/scene/component/Component.h"
#include "dream/renderer/OpenGLMesh.h"
#include "dream/util/Logger.h"
#include "dream/window/Input.h"
#include "dream/renderer/OpenGLCubeMesh.h"
#include "dream/util/MathUtils.h"

namespace Dream {
    OpenGLRenderer::OpenGLRenderer() : Renderer() {
        this->printGLVersion();

        // build and compile our shader programs
        lightingShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("shader.vert").c_str(),
                                               Project::getPath().append("assets").append("shaders").append(
                                                       "lighting_shader.frag").c_str(), nullptr);

        singleTextureShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("shader.vert").c_str(),
                                               Project::getPath().append("assets").append("shaders").append(
                                          "shader_single_texture.frag").c_str(), nullptr);

        physicsDebugShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("physics.vert").c_str(),
                                  Project::getPath().append("assets").append("shaders").append(
                                          "physics.frag").c_str(), nullptr);

        skyboxShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("skybox.vert").c_str(),
                                              Project::getPath().append("assets").append("shaders").append(
                                                      "skybox.frag").c_str(), nullptr);

        simpleDepthShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("shadow_mapping_depth.vert").c_str(),
                                        Project::getPath().append("assets").append("shaders").append(
                                                "shadow_mapping_depth.frag").c_str(), nullptr);

        skybox = new OpenGLSkybox();

        whiteTexture = new OpenGLTexture(Project::getPath().append("assets").append("textures").append("white.png"));

        blackTexture = new OpenGLTexture(Project::getPath().append("assets").append("textures").append("black.png"));

        frameBuffer = new OpenGLFrameBuffer();

        for (int i = 0; i < NUM_CASCADES; ++i) {
            auto shadowMapFbo = new OpenGLShadowMapFBO((int) SHADOW_WIDTH, (int) SHADOW_HEIGHT);
            shadowMapFbos.push_back(shadowMapFbo);
        }

        // load primitive shapes
        if (!Project::getResourceManager()->hasMeshData("sphere")) {
            Project::getResourceManager()->storeMeshData(new OpenGLSphereMesh(), "sphere");
        }

        if (!Project::getResourceManager()->hasMeshData("cube")) {
            Project::getResourceManager()->storeMeshData(new OpenGLCubeMesh(), "cube");
        }
    }

    OpenGLRenderer::~OpenGLRenderer() {
        delete this->lightingShader;
        delete this->singleTextureShader;
        delete this->physicsDebugShader;
        delete this->simpleDepthShader;
        delete this->frameBuffer;
        for (int i = 0; i < NUM_CASCADES; ++i) {
            delete shadowMapFbos.at(i);
        }
        delete this->whiteTexture;
        delete this->blackTexture;
    }

    void OpenGLRenderer::render(int viewportWidth, int viewportHeight, bool fullscreen) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        Input::setPlayWindowActive(true);

        // renderer camera
        std::optional<Camera> maybeCamera;

        // TODO: get main camera instead when game is playing
        // update renderer camera using scene camera entity's position and camera attributes like yaw, pitch, and fov
        auto sceneCameraEntity = Project::getScene()->getSceneCamera();
        if (sceneCameraEntity) {
            maybeCamera = {(float) viewportWidth * 2.0f, (float) viewportHeight * 2.0f};
            sceneCameraEntity.getComponent<Component::SceneCameraComponent>().updateRendererCamera(*maybeCamera, sceneCameraEntity);
        }

        if (maybeCamera) {
            auto camera = *maybeCamera;

            // light spaces matrices for shadow cascades
            auto lightSpaceMatrices = getLightSpaceMatrices(camera, getDirectionalLightDirection());

            // render scene from light's point of view
            {
                glEnable(GL_DEPTH_CLAMP);
                for (int i = 0; i < NUM_CASCADES; ++i) {
                    glm::mat4 lightSpaceMatrix = lightSpaceMatrices.at(i);
                    simpleDepthShader->use();
                    // TODO: iterate through cascades and set each one for each shadow map fbo object
                    simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
                    glViewport(0, 0, (int) SHADOW_WIDTH, (int) SHADOW_HEIGHT);
                    shadowMapFbos.at(i)->bind();
                    glClear(GL_DEPTH_BUFFER_BIT);
                    drawEntities(Project::getScene()->getRootEntity(), simpleDepthShader);
                    shadowMapFbos.at(i)->unbind();
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                }
                glDisable(GL_DEPTH_CLAMP);
            }

            {
                // bind frame buffer for drawing to output render texture
                glViewport(0, 0, viewportWidth * 2, viewportHeight * 2);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                this->frameBuffer->bindFrameBuffer();
                this->resizeFrameBuffer();
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }

            {
                // draw meshes
                if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::FINAL) {
                    lightingShader->use();
                    lightingShader->setMat4("projection", camera.getProjectionMatrix());
                    lightingShader->setMat4("view", camera.getViewMatrix());
                    lightingShader->setFloat("farPlane", camera.zFar);
                    // TODO: use global position for camera
                    glm::vec3 viewPos = camera.position;
                    lightingShader->setVec3("viewPos", viewPos);
                    applyLighting(lightingShader);
                    lightingShader->setVec3("lightDir", getDirectionalLightDirection());
                    for (int i = 0; i < NUM_CASCADES; ++i) {
                        lightingShader->setMat4("lightSpaceMatrices[" + std::to_string(i) + "]", lightSpaceMatrices.at(i));
                    }

                    auto shadowCascadeLevels = getShadowCascadeLevels(camera);
                    for (int i = 0; i < shadowCascadeLevels.size(); ++i) {
                        lightingShader->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels.at(i));
                    }
                    drawEntities(Project::getScene()->getRootEntity(), lightingShader);
                } else {
                    singleTextureShader->use();
                    singleTextureShader->setMat4("projection", camera.getProjectionMatrix());
                    singleTextureShader->setMat4("view", camera.getViewMatrix());
                    drawEntities(Project::getScene()->getRootEntity(), singleTextureShader);
                }
            }

            {
                // draw physics debug
                if (Project::getConfig().physicsConfig.physicsDebugger && (!Project::isPlaying() || (Project::isPlaying() && Project::getConfig().physicsConfig.physicsDebuggerWhilePlaying))) {
                    if (Project::getConfig().physicsConfig.depthTest) {
                        glEnable(GL_DEPTH_TEST);
                    } else {
                        glDisable(GL_DEPTH_TEST);
                    }
                    physicsDebugShader->use();
                    physicsDebugShader->setMat4("projection", camera.getProjectionMatrix());
                    physicsDebugShader->setMat4("view", camera.getViewMatrix());
                    if (Project::getScene()->getPhysicsComponentSystem()) {
                        Project::getScene()->getPhysicsComponentSystem()->debugDrawWorld();
                    }
                    if (!Project::getConfig().physicsConfig.depthTest) {
                        glEnable(GL_DEPTH_TEST);
                    }
                }
            }

            // draw skybox
            {
                // draw skybox as last
                glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
                skyboxShader->use();
                auto skyboxView = glm::mat4(glm::mat3(camera.getViewMatrix()));
                skyboxShader->setMat4("view", skyboxView);
                skyboxShader->setMat4("projection", camera.getProjectionMatrix());
                skyboxShader->setInt("skybox", 0);
                // skybox cube
                glBindVertexArray(skybox->getVAO());
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getTexture());
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glBindVertexArray(0);
                glDepthFunc(GL_LESS); // set depth function back to default
            }
        } else {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        // bind the default screen frame buffer
        this->frameBuffer->bindDefaultFrameBuffer();

        // clear framebuffer and return its texture
        this->frameBuffer->clear();

        if (fullscreen) {
            this->frameBuffer->renderScreenQuad();
        }
    }

    void OpenGLRenderer::drawEntities(Entity entity, OpenGLShader* shader) {
        // part 1
        // initialize bones for animated meshes
        std::vector<glm::mat4> finalBoneMatrices;
        if (entity.hasComponent<Component::AnimatorComponent>()) {
            if (entity.hasComponent<Component::MeshComponent>()) {
                entity.getComponent<Component::MeshComponent>().loadMesh();
            } else {
                Logger::fatal("No mesh component for entity with animator so bones cannot be loaded");
            }
            if (entity.getComponent<Component::AnimatorComponent>().needsToLoadAnimations) {
                entity.getComponent<Component::AnimatorComponent>().loadStateMachine(entity);
            }
            finalBoneMatrices = entity.getComponent<Component::AnimatorComponent>().m_FinalBoneMatrices;
        }

        // part 2
        // draw mesh of entity
        if (entity.hasComponent<Component::MeshComponent>()) {
            // load mesh of entity
            // TODO: only load when necessary (add a flag internally - and reset it when fields are modified)
            entity.getComponent<Component::MeshComponent>().loadMesh();

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
                        // specular texture
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
                        whiteTexture->bind(2);
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
                        // specular texture
                        entity.getComponent<Component::MaterialComponent>().loadTextures();
                        auto ambientTexture = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().ambientTextureGuid);
                        if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(ambientTexture)) {
                            shader->setInt("texture_ambient", 4);
                            openGLTexture->bind(4);
                        } else {
                            Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                        }
                    } else {
                        // default specular texture
                        shader->setInt("texture_ambient", 4);
                        whiteTexture->bind(4);
                    }
                }

                // load shadow map
                int shadowMapTexturesStart = 5;
                for (int i = 0; i < NUM_CASCADES; ++i) {
                    int textureIndex = shadowMapTexturesStart + i;
                    shader->setInt("shadowMaps[" + std::to_string(i) + "]", textureIndex);
                    shadowMapFbos.at(i)->bindForReading(textureIndex);
                }
            } else if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::DIFFUSE) {
                // debug diffuse
                singleTextureShader->setVec4("color", {1, 1, 1, 1});
                if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().diffuseTextureGuids.empty()) {
                    entity.getComponent<Component::MaterialComponent>().loadTextures();
                    auto tex = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().diffuseTextureGuids.at(0));
                    if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(tex)) {
                        singleTextureShader->setInt("tex", 0);
                        openGLTexture->bind(0);
                    } else {
                        Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                    }
                } else {
                    singleTextureShader->setInt("tex", 0);
                    blackTexture->bind(0);
                }
            } else if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::SPECULAR) {
                // debug specular
                singleTextureShader->setVec4("color", {1, 1, 1, 1});
                if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().specularTextureGuid.empty()) {
                    entity.getComponent<Component::MaterialComponent>().loadTextures();
                    auto tex = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().specularTextureGuid);
                    if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(tex)) {
                        singleTextureShader->setInt("tex", 0);
                        openGLTexture->bind(0);
                    } else {
                        Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                    }
                } else {
                    singleTextureShader->setInt("tex", 0);
                    blackTexture->bind(0);
                }
            } else if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::NORMAL) {
                // debug normal
                singleTextureShader->setVec4("color", {1, 1, 1, 1});
                if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().normalTextureGuid.empty()) {
                    entity.getComponent<Component::MaterialComponent>().loadTextures();
                    auto tex = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().normalTextureGuid);
                    if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(tex)) {
                        singleTextureShader->setInt("tex", 0);
                        openGLTexture->bind(0);
                    } else {
                        Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                    }
                } else {
                    singleTextureShader->setInt("texture", 0);
                    blackTexture->bind(0);
                }
            } else {
                Logger::fatal("Unknown rendering type");
            }

            // get transform of entity
            glm::mat4 model = entity.getComponent<Component::TransformComponent>().getTransform(entity);
            if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::FINAL) {
                shader->setMat4("model", model);
            } else {
                singleTextureShader->setMat4("model", model);
            }

            // set bones for animation
            for (int i = 0; i < finalBoneMatrices.size(); i++) {
                if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::FINAL) {
                    shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", finalBoneMatrices[i]);
                } else {
                    singleTextureShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", finalBoneMatrices[i]);
                }
            }

            // draw mesh of entity
            if (!entity.getComponent<Component::MeshComponent>().fileId.empty() || entity.getComponent<Component::MeshComponent>().meshType != Component::MeshComponent::FROM_FILE) {
                auto guid = entity.getComponent<Component::MeshComponent>().getMeshGuid();
                auto fileId = entity.getComponent<Component::MeshComponent>().getMeshFileID();
                auto mesh = Project::getResourceManager()->getMeshData(guid, fileId);
                if (auto openGLMesh = std::dynamic_pointer_cast<OpenGLMesh>(mesh)) {
                    drawMesh(openGLMesh);
                } else {
                    Logger::fatal("Unable to dynamic cast Mesh to type OpenGLMesh for entity " + entity.getComponent<Component::IDComponent>().id);
                }
            }
        }

        // part 3
        // repeat process for children
        Entity child = entity.getComponent<Component::HierarchyComponent>().first;
        while (child) {
            drawEntities(child, shader);
            child = child.getComponent<Component::HierarchyComponent>().next;
        }
    }

    void OpenGLRenderer::drawMesh(std::shared_ptr<OpenGLMesh> openGLMesh) {
        if (!openGLMesh->getIndices().empty()) {
            // case where vertices are indexed
            auto numIndices = openGLMesh->getIndices().size();
            glBindVertexArray(openGLMesh->getVAO());
            glDrawElements(GL_TRIANGLES, (int) numIndices, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        } else if (!openGLMesh->getVertices().empty()) {
            // case where vertices are not indexed
            glBindVertexArray(openGLMesh->getVAO());
            glDrawArrays(GL_TRIANGLES, 0, (int) openGLMesh->getVertices().size());
            glBindVertexArray(0);
        } else {
            Logger::fatal("Unable to render mesh");
        }
    }

    void OpenGLRenderer::resizeFrameBuffer() {
        // resize framebuffer whenever there is a new viewport size
        GLint viewportWidth = this->getViewportDimensions().first;
        GLint viewportHeight = this->getViewportDimensions().second;
        if (frameBuffer->getWidth() != viewportWidth || frameBuffer->getHeight() != viewportHeight) {
            frameBuffer->resize(viewportWidth, viewportHeight);
        }
    }

    void OpenGLRenderer::printGLVersion() {
        Logger::info("GL Vendor: " + std::string((const char *) glGetString(GL_VENDOR)));
        Logger::info("GL Renderer: " + std::string((const char *) glGetString(GL_RENDERER)));
        Logger::info("GL Version: " + std::string((const char *) glGetString(GL_VERSION)));
    }

    std::pair<int, int> OpenGLRenderer::getViewportDimensions() {
        GLint dims[4] = {0};
        glGetIntegerv(GL_VIEWPORT, dims);
        return std::make_pair(dims[2], dims[3]);
    }

    unsigned int OpenGLRenderer::getOutputRenderTexture() {
        return this->frameBuffer->getTexture();
    }

    void OpenGLRenderer::applyLighting(OpenGLShader* shader) {
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

    std::vector<glm::vec4> OpenGLRenderer::getFrustumCornersWorldSpace(const glm::mat4& projview) {
        const auto inv = glm::inverse(projview);

        std::vector<glm::vec4> frustumCorners;
        for (unsigned int x = 0; x < 2; ++x) {
            for (unsigned int y = 0; y < 2; ++y) {
                for (unsigned int z = 0; z < 2; ++z) {
                    const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                    frustumCorners.push_back(pt / pt.w);
                }
            }
        }

        return frustumCorners;
    }

    std::vector<glm::vec4> OpenGLRenderer::getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view) {
        return getFrustumCornersWorldSpace(proj * view);
    }

    glm::mat4 OpenGLRenderer::getLightSpaceMatrix(Camera camera, glm::vec3 lightDir, const float nearPlane, const float farPlane) {
        const auto proj = glm::perspective(glm::radians(camera.fov), camera.getAspect(), nearPlane,farPlane);
        const auto corners = getFrustumCornersWorldSpace(proj, camera.getViewMatrix());

        glm::vec3 center = glm::vec3(0, 0, 0);
        for (const auto& v : corners) {
            center += glm::vec3(v);
        }
        center /= corners.size();

        const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();
        for (const auto& v : corners) {
            const auto trf = lightView * v;
            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        // tune this parameter according to the scene
        constexpr float zMult = 10.0f;
        if (minZ < 0) {
            minZ *= zMult;
        }
        else {
            minZ /= zMult;
        }

        if (maxZ < 0) {
            maxZ /= zMult;
        }
        else {
            maxZ *= zMult;
        }

        const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, -maxZ, -minZ);

        return lightProjection * lightView;
    }

    std::vector<glm::mat4> OpenGLRenderer::getLightSpaceMatrices(Camera camera, glm::vec3 lightDir) {
        auto shadowCascadeLevels = getShadowCascadeLevels(camera);
        std::vector<glm::mat4> ret;
        for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i) {
            if (i == 0) {
                ret.push_back(getLightSpaceMatrix(camera, lightDir, camera.zNear, shadowCascadeLevels[i]));
            }
            else if (i < shadowCascadeLevels.size()) {
                ret.push_back(getLightSpaceMatrix(camera, lightDir, shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
            }
            else {
                ret.push_back(getLightSpaceMatrix(camera, lightDir, shadowCascadeLevels[i - 1], camera.zNear));
            }
        }
        return ret;
    }

    glm::vec3 OpenGLRenderer::getDirectionalLightDirection() {
        glm::vec3 lightDir(1, 0, 0);
        auto lightEntities = Project::getScene()->getEntitiesWithComponents<Component::LightComponent>();
        for (const auto &lightEntity : lightEntities) {
            Entity entity = {lightEntity, Project::getScene()};
            if (entity.getComponent<Component::LightComponent>().type == Component::LightComponent::LightType::DIRECTIONAL) {
                lightDir = glm::normalize(entity.getComponent<Component::TransformComponent>().getFront());
                lightDir *= -1;
            }
        }
        return lightDir;
    }

    std::vector<float> OpenGLRenderer::getShadowCascadeLevels(Camera camera) {
        float cameraFarPlane = camera.zFar;
        std::vector<float> shadowCascadeLevels{ 8.0f, 10.0f * 2, 36.0f * 2, 500.0f * 2 };
//        std::vector<float> shadowCascadeLevels{ cameraFarPlane / 26.0f, cameraFarPlane / 15.0f, cameraFarPlane / 7.0f, cameraFarPlane - 0.001f };
//        std::vector<float> shadowCascadeLevels{ cameraFarPlane / 26.0f, cameraFarPlane / 15.0f, cameraFarPlane / 7.0f, cameraFarPlane / 2.0f };
        assert(NUM_CASCADES == shadowCascadeLevels.size());
        return shadowCascadeLevels;
    }
}
