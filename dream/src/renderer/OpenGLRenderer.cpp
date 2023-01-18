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
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    void renderQuad()
    {
        if (quadVAO == 0)
        {
            float quadVertices[] = {
                    // positions        // texture Coords
                    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                    1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

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

        debugDepthQuad = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("debug_quad.vert").c_str(),
                                             Project::getPath().append("assets").append("shaders").append(
                                                     "debug_quad_depth.frag").c_str(), nullptr);

        skybox = new OpenGLSkybox();

        whiteTexture = new OpenGLTexture(Project::getPath().append("assets").append("textures").append("white.png"));

        blackTexture = new OpenGLTexture(Project::getPath().append("assets").append("textures").append("black.png"));

        frameBuffer = new OpenGLFrameBuffer();

        glGenFramebuffers(1, &depthMapFBO);
        // create depth texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, (int) SHADOW_WIDTH, (int) SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        // TODO: this causes issue where shadow is repeated in scene for web build...
        #ifndef EMSCRIPTEN
        glDrawBuffer(GL_NONE);
        #endif
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        debugDepthQuad->use();
        debugDepthQuad->setInt("depthMap", 0);

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
        delete this->whiteTexture;
        delete this->blackTexture;
    }

    void OpenGLRenderer::preRender(int viewportWidth, int viewportHeight, bool fullscreen) {
        this->frameBuffer->bindFrameBuffer();
        this->resizeFrameBuffer();
        this->updateViewportSize(viewportWidth, viewportHeight, fullscreen);
    }

    void OpenGLRenderer::renderScene(Dream::OpenGLShader *shader) {
        shader->use();
        renderSceneHelper(Project::getScene()->getRootEntity(), shader);
//        // floor
//        glm::mat4 model = glm::mat4(1.0f);
//        shader->setMat4("model", model);
//        glBindVertexArray(planeVAO);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        // cubes
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
//        model = glm::scale(model, glm::vec3(0.5f));
//        shader.setMat4("model", model);
//        renderCube();
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
//        model = glm::scale(model, glm::vec3(0.5f));
//        shader->setMat4("model", model);
//        renderCube();
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
//        model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
//        model = glm::scale(model, glm::vec3(0.25));
//        shader->setMat4("model", model);
//        renderCube();
    }

    void OpenGLRenderer::renderSceneHelper(Dream::Entity entity, Dream::OpenGLShader *shader) {
        if (entity.hasComponent<Component::MeshComponent>()) {
            // load mesh of entity
            // TODO: only load when necessary (add a flag internally - and reset it when fields are modified)
            entity.getComponent<Component::MeshComponent>().loadMesh();

            // get transform of entity
            glm::mat4 model = entity.getComponent<Component::TransformComponent>().getTransform(entity);
            if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::FINAL) {
                shader->setMat4("model", model);
            }

            // draw mesh of entity
            if (!entity.getComponent<Component::MeshComponent>().fileId.empty() || entity.getComponent<Component::MeshComponent>().meshType != Component::MeshComponent::FROM_FILE) {
                auto guid = entity.getComponent<Component::MeshComponent>().guid;
                auto fileId = entity.getComponent<Component::MeshComponent>().fileId;

                // TODO: make handling of primitives cleaner - maybe store their mesh data somewhere else like in constructor of this class
                if (entity.getComponent<Component::MeshComponent>().meshType != Component::MeshComponent::FROM_FILE) {
                    if (entity.getComponent<Component::MeshComponent>().meshType == Component::MeshComponent::PRIMITIVE_CUBE) {
                        guid = "cube";
                        fileId = "";
                    } else if (entity.getComponent<Component::MeshComponent>().meshType == Component::MeshComponent::PRIMITIVE_SPHERE) {
                        guid = "sphere";
                        fileId = "";
                    } else {
                        Logger::fatal("Unknown primitive type to find GUID for");
                    }
                }

                auto mesh = Project::getResourceManager()->getMeshData(guid, fileId);
                if (auto openGLMesh = std::dynamic_pointer_cast<OpenGLMesh>(mesh)) {
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
                } else {
                    Logger::fatal("Unable to dynamic cast Mesh to type OpenGLMesh for entity " + entity.getComponent<Component::IDComponent>().id);
                }
            }
        }

        // repeat process for children
        Entity child = entity.getComponent<Component::HierarchyComponent>().first;
        while (child) {
            renderSceneHelper(child, shader);
            child = child.getComponent<Component::HierarchyComponent>().next;
        }
    }

    void OpenGLRenderer::render(int viewportWidth, int viewportHeight, bool fullscreen) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
//        glEnable(GL_CULL_FACE);

        Input::setPlayWindowActive(true);
//        this->preRender(viewportWidth, viewportHeight, fullscreen);


        auto sceneCamera = Project::getScene()->getSceneCamera();
        auto mainCamera = Project::getScene()->getMainCamera();

        if ((Project::isPlaying() && mainCamera) || (!Project::isPlaying() && sceneCamera)) {
            // 1. render depth of scene to texture (from light's perspective)
            // --------------------------------------------------------------
            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;
            float near_plane = 1.0f, far_plane = 7.5f;
//            lightProjection = glm::perspective(glm::radians(mainCamera.getComponent<Component::CameraComponent>().fov),
//                                               (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT,
//                                               mainCamera.getComponent<Component::CameraComponent>().zNear,
//                                               mainCamera.getComponent<Component::CameraComponent>().zFar);
//            lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
            lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
//            glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
            glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
            lightPos.x *= -1;
            lightPos.y *= -1;
            lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;
            // render scene from light's point of view
            simpleDepthShader->use();
            simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            renderScene(simpleDepthShader);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // reset viewport
            glViewport(0, 0, viewportWidth * 2, viewportHeight * 2);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // render Depth map to quad for visual debugging
            // ---------------------------------------------
//            debugDepthQuad->use();
//            debugDepthQuad->setFloat("near_plane", near_plane);
//            debugDepthQuad->setFloat("far_plane", far_plane);
//            glActiveTexture(GL_TEXTURE0);
//            glBindTexture(GL_TEXTURE_2D, depthMap);
//            renderQuad();

            // 2. render scene as normal using the generated depth/shadow map
            // --------------------------------------------------------------
//            shader.use();
//            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//            glm::mat4 view = camera.GetViewMatrix();
//            shader.setMat4("projection", projection);
//            shader.setMat4("view", view);
//            // set light uniforms
//            shader.setVec3("viewPos", camera.Position);
//            shader.setVec3("lightPos", lightPos);
//            shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
//            glActiveTexture(GL_TEXTURE0);
//            glBindTexture(GL_TEXTURE_2D, woodTexture);
//            glActiveTexture(GL_TEXTURE1);
//            glBindTexture(GL_TEXTURE_2D, depthMap);
//            renderScene(shader);

//            this->preRender(viewportWidth, viewportHeight, fullscreen);

            // calculate projection and view using current camera
            glm::mat4 projection;
            if (Project::isPlaying()) {
                projection = glm::perspective(glm::radians(mainCamera.getComponent<Component::CameraComponent>().fov), (float)viewportWidth / (float)viewportHeight, 0.1f, 100.0f);
//                projection = glm::perspective(glm::radians(mainCamera.getComponent<Component::CameraComponent>().fov),
//                                              (float) viewportWidth / (float) viewportHeight,
//                                              mainCamera.getComponent<Component::CameraComponent>().zNear,
//                                              mainCamera.getComponent<Component::CameraComponent>().zFar);
            } else {
                projection = glm::perspective(glm::radians(sceneCamera.getComponent<Component::SceneCameraComponent>().fov), (float)viewportWidth / (float)viewportHeight, 0.1f, 100.0f);
//                projection = glm::perspective(
//                        glm::radians(sceneCamera.getComponent<Component::SceneCameraComponent>().fov),
//                        (float) viewportWidth / (float) viewportHeight,
//                        sceneCamera.getComponent<Component::SceneCameraComponent>().zNear,
//                        sceneCamera.getComponent<Component::SceneCameraComponent>().zFar);
            }
//            projection = lightProjection;

            glm::mat4 view;
            if (Project::isPlaying()) {
                view = mainCamera.getComponent<Component::CameraComponent>().getViewMatrix(mainCamera);
            } else {
                view = sceneCamera.getComponent<Component::SceneCameraComponent>().getViewMatrix(sceneCamera);
            }
//            view = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, -1.0, 0.0));

            {
                // bind frame buffer for drawing to output render texture
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                this->frameBuffer->bindFrameBuffer();
                this->resizeFrameBuffer();
                this->updateViewportSize(viewportWidth, viewportHeight, fullscreen);
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }

            // draw meshes
            {
                if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::FINAL) {
                    lightingShader->use();
                    lightingShader->setMat4("projection", projection);
                    lightingShader->setMat4("view", view);
                    glm::vec3 viewPos = {1, 1, 1};
                    if (mainCamera) {
                        viewPos = mainCamera.getComponent<Component::TransformComponent>().translation;
                    } else if (sceneCamera) {
                        viewPos = sceneCamera.getComponent<Component::TransformComponent>().translation;
                    } else {
                        Logger::fatal("Unable to set view pos due to missing camera");
                    }
                    lightingShader->setVec3("viewPos", viewPos);
                    applyLighting();

                    lightingShader->setVec3("lightPos", lightPos);
                    lightingShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
                } else {
                    singleTextureShader->use();
                    singleTextureShader->setMat4("projection", projection);
                    singleTextureShader->setMat4("view", view);
                }
                renderEntityAndChildren(Project::getScene()->getRootEntity());
            }

            // draw physics debug
            if (Project::getConfig().physicsConfig.physicsDebugger && (!Project::isPlaying() || (Project::isPlaying() && Project::getConfig().physicsConfig.physicsDebuggerWhilePlaying))) {
                if (Project::getConfig().physicsConfig.depthTest) {
                    glEnable(GL_DEPTH_TEST);
                } else {
                    glDisable(GL_DEPTH_TEST);
                }
                physicsDebugShader->use();
                physicsDebugShader->setMat4("projection", projection);
                physicsDebugShader->setMat4("view", view);
                if (Project::getScene()->getPhysicsComponentSystem()) {
                    Project::getScene()->getPhysicsComponentSystem()->debugDrawWorld();
                }
                if (!Project::getConfig().physicsConfig.depthTest) {
                    glEnable(GL_DEPTH_TEST);
                }
            }

            // draw skybox
            {
                // draw skybox as last
                glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
                skyboxShader->use();
                view = glm::mat4(glm::mat3(view));
                skyboxShader->setMat4("view", view);
                skyboxShader->setMat4("projection", projection);
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

        this->postRender(fullscreen);
    }

    void OpenGLRenderer::renderEntityAndChildren(Entity entity) {
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
                        lightingShader->setFloat("shininess", 2.0f);
                    } else {
                        lightingShader->setFloat("shininess", shininess);
                    }
                } else {
                    lightingShader->setFloat("shininess", 20.0f);
                }

                // load diffuse color + texture of entity
                {
                    if (entity.hasComponent<Component::MaterialComponent>()) {
                        lightingShader->setVec4("diffuse_color", entity.getComponent<Component::MaterialComponent>().diffuseColor);
                    } else {
                        lightingShader->setVec4("diffuse_color", glm::vec4(1.0, 1.0, 1.0, 1.0));
                    }

                    if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().diffuseTextureGuids.empty()) {
                        // diffuse texture
                        // TODO: iterate through vector and do not just get first diffuse texture, instead blend them
                        entity.getComponent<Component::MaterialComponent>().loadTextures();
                        auto diffuseTexture = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().diffuseTextureGuids.at(0));
                        if (auto openGLDiffuseTexture = std::dynamic_pointer_cast<OpenGLTexture>(diffuseTexture)) {
                            lightingShader->setInt("texture_diffuse1", 0);
                            openGLDiffuseTexture->bind(0);
                        } else {
                            Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                        }
                    } else {
                        // default diffuse texture
                        lightingShader->setInt("texture_diffuse1", 0);
                        whiteTexture->bind(0);
                    }
                }

                // load specular color + texture of entity
                {
                    if (entity.hasComponent<Component::MaterialComponent>()) {
                        lightingShader->setVec4("specular_color", entity.getComponent<Component::MaterialComponent>().specularColor);
                    } else {
                        lightingShader->setVec4("specular_color", glm::vec4(1.0, 1.0, 1.0, 1.0));
                    }

                    if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().specularTextureGuid.empty()) {
                        // specular texture
                        entity.getComponent<Component::MaterialComponent>().loadTextures();
                        auto specularTexture = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().specularTextureGuid);
                        if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(specularTexture)) {
                            lightingShader->setInt("texture_specular", 1);
                            openGLTexture->bind(1);
                        } else {
                            Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                        }
                    } else {
                        // default specular texture
                        lightingShader->setInt("texture_specular", 1);
                        blackTexture->bind(1);
                    }
                }

                // load normal texture of entity
                {
                    if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().normalTextureGuid.empty()) {
                        entity.getComponent<Component::MaterialComponent>().loadTextures();
                        auto normalTexture = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().normalTextureGuid);
                        if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(normalTexture)) {
                            lightingShader->setInt("texture_normal", 2);
                            openGLTexture->bind(2);
                        } else {
                            Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                        }
                    } else {
                        // default diffuse texture
                        lightingShader->setInt("texture_normal", 2);
                        whiteTexture->bind(2);
                    }
                }

                // load height texture of entity
                {
                    if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().heightTextureGuid.empty()) {
                        entity.getComponent<Component::MaterialComponent>().loadTextures();
                        auto heightTexture = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().heightTextureGuid);
                        if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(heightTexture)) {
                            lightingShader->setInt("texture_height", 3);
                            openGLTexture->bind(3);
                        } else {
                            Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                        }
                    } else {
                        // default diffuse texture
                        lightingShader->setInt("texture_height", 3);
                        blackTexture->bind(3);
                    }
                }

                // load ambient color + texture of entity
                {
                    if (entity.hasComponent<Component::MaterialComponent>()) {
                        lightingShader->setVec4("ambient_color", entity.getComponent<Component::MaterialComponent>().ambientColor);
                    } else {
                        lightingShader->setVec4("ambient_color", glm::vec4(1.0, 1.0, 1.0, 1.0));
                    }

                    if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().ambientTextureGuid.empty()) {
                        // specular texture
                        entity.getComponent<Component::MaterialComponent>().loadTextures();
                        auto ambientTexture = Project::getResourceManager()->getTextureData(entity.getComponent<Component::MaterialComponent>().ambientTextureGuid);
                        if (auto openGLTexture = std::dynamic_pointer_cast<OpenGLTexture>(ambientTexture)) {
                            lightingShader->setInt("texture_ambient", 4);
                            openGLTexture->bind(4);
                        } else {
                            Logger::fatal("Unable to dynamic cast Texture to type OpenGLTexture");
                        }
                    } else {
                        // default specular texture
                        lightingShader->setInt("texture_ambient", 4);
                        whiteTexture->bind(4);
                    }
                }

                // load shadow map
                lightingShader->setInt("shadowMap", 5);
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, depthMap);
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
                lightingShader->setMat4("model", model);
            } else {
                singleTextureShader->setMat4("model", model);
            }

            // set bones for animation
            for (int i = 0; i < finalBoneMatrices.size(); i++) {
                if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::FINAL) {
                    lightingShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", finalBoneMatrices[i]);
                } else {
                    singleTextureShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", finalBoneMatrices[i]);
                }
            }

            // draw mesh of entity
            if (!entity.getComponent<Component::MeshComponent>().fileId.empty() || entity.getComponent<Component::MeshComponent>().meshType != Component::MeshComponent::FROM_FILE) {
                auto guid = entity.getComponent<Component::MeshComponent>().guid;
                auto fileId = entity.getComponent<Component::MeshComponent>().fileId;

                // TODO: make handling of primitives cleaner - maybe store their mesh data somewhere else like in constructor of this class
                if (entity.getComponent<Component::MeshComponent>().meshType != Component::MeshComponent::FROM_FILE) {
                    if (entity.getComponent<Component::MeshComponent>().meshType == Component::MeshComponent::PRIMITIVE_CUBE) {
                        guid = "cube";
                        fileId = "";
                    } else if (entity.getComponent<Component::MeshComponent>().meshType == Component::MeshComponent::PRIMITIVE_SPHERE) {
                        guid = "sphere";
                        fileId = "";
                    } else {
                        Logger::fatal("Unknown primitive type to find GUID for");
                    }
                }

                auto mesh = Project::getResourceManager()->getMeshData(guid, fileId);
                if (auto openGLMesh = std::dynamic_pointer_cast<OpenGLMesh>(mesh)) {
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
                } else {
                    Logger::fatal("Unable to dynamic cast Mesh to type OpenGLMesh for entity " + entity.getComponent<Component::IDComponent>().id);
                }
            }
        }

        // part 3
        // repeat process for children
        Entity child = entity.getComponent<Component::HierarchyComponent>().first;
        while (child) {
            renderEntityAndChildren(child);
            child = child.getComponent<Component::HierarchyComponent>().next;
        }
    }

    void OpenGLRenderer::postRender(bool fullscreen) {
        // bind the default screen frame buffer
        this->frameBuffer->bindDefaultFrameBuffer();

        // clear framebuffer and return its texture
        this->frameBuffer->clear();

        if (fullscreen) {
            this->frameBuffer->renderScreenQuad();
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

    void OpenGLRenderer::updateViewportSize(int viewportWidth, int viewportHeight, bool fullscreen) {
#ifdef EMSCRIPTEN
        if (fullscreen) {
            auto dpiScale = 2;
            glViewport(0, 0, viewportWidth * dpiScale, viewportHeight * dpiScale);
        }
#endif
    }

    unsigned int OpenGLRenderer::getOutputRenderTexture() {
        return this->frameBuffer->getTexture();
//        return this->depthMap;
    }

    void OpenGLRenderer::applyLighting() {
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

        // define current number of point lights
        lightingShader->setInt("numberOfDirLights", (int) directionalLights.size());
        lightingShader->setInt("numberOfPointLights", (int) pointLights.size());
        lightingShader->setInt("numberOfSpotLights", (int) spotLights.size());

        for (int i = 0; i < directionalLights.size(); i++) {
            auto &lightEntity = directionalLights.at(i);
            const auto &lightComponent = lightEntity.getComponent<Component::LightComponent>();
            std::string prefix = "dirLights[" + std::to_string(i) + "]";

            lightingShader->setVec3(prefix + ".direction", lightEntity.getComponent<Component::TransformComponent>().getFront());
            lightingShader->setVec3(prefix + ".ambient", lightComponent.color);
            lightingShader->setVec3(prefix + ".diffuse", lightComponent.color);
            lightingShader->setVec3(prefix + ".specular", lightComponent.color);
        }

        for (int i = 0; i < pointLights.size(); i++) {
            auto &lightEntity = pointLights.at(i);
            const auto &lightComponent = lightEntity.getComponent<Component::LightComponent>();
            std::string prefix = "pointLights[" + std::to_string(i) + "]";
            // TODO: get global translation
            auto lightPos = lightEntity.getComponent<Component::TransformComponent>().translation;
            lightPos.x *= -1;
            lightPos.y *= -1;
            lightingShader->setVec3(prefix + ".position", lightPos);
            lightingShader->setVec3(prefix + ".ambient", lightComponent.color);
            lightingShader->setVec3(prefix + ".diffuse", lightComponent.color);
            lightingShader->setVec3(prefix + ".specular", lightComponent.color);
            lightingShader->setFloat(prefix + ".constant", lightComponent.constant);
            lightingShader->setFloat(prefix + ".linear", lightComponent.linear);
            lightingShader->setFloat(prefix + ".quadratic", lightComponent.quadratic);
        }

        for (int i = 0; i < spotLights.size(); i++) {
            auto &lightEntity = spotLights.at(i);
            const auto &lightComponent = lightEntity.getComponent<Component::LightComponent>();
            std::string prefix = "spotLights[" + std::to_string(i) + "]";
            // TODO: get global translation
            lightingShader->setVec3(prefix + ".position", lightEntity.getComponent<Component::TransformComponent>().translation);
            lightingShader->setVec3(prefix + ".direction", lightEntity.getComponent<Component::TransformComponent>().getFront());
            lightingShader->setVec3(prefix + ".ambient", lightComponent.color);
            lightingShader->setVec3(prefix + ".diffuse", lightComponent.color);
            lightingShader->setVec3(prefix + ".specular", lightComponent.color);
            lightingShader->setFloat(prefix + ".constant", lightComponent.constant);
            lightingShader->setFloat(prefix + ".linear", lightComponent.linear);
            lightingShader->setFloat(prefix + ".quadratic", lightComponent.quadratic);
            lightingShader->setFloat(prefix + ".cutOff", glm::cos(glm::radians(lightComponent.cutOff)));
            lightingShader->setFloat(prefix + ".outerCutOff", glm::cos(glm::radians(lightComponent.outerCutOff)));
        }
    }
}
