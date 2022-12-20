//
// Created by Deepak Ramalingam on 10/24/22.
//

#include "dream/renderer/OpenGLRenderer.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "dream/project/Project.h"
#include "dream/scene/Entity.h"
#include "dream/scene/component/Component.h"
#include "dream/renderer/OpenGLMesh.h"
#include "dream/util/Logger.h"
#include "dream/util/MathUtils.h"
#include "dream/window/Input.h"

namespace Dream {
    OpenGLRenderer::OpenGLRenderer() : Renderer() {
        this->printGLVersion();

        // build and compile our shader program
        shader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("shader.vert").c_str(), Project::getPath().append("assets").append("shaders").append("shader_texture.frag").c_str(), nullptr);
        texture = new OpenGLTexture(Project::getPath().append("assets").append("textures").append("missing.png"));

        // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
        shader->use();
        shader->setInt("texture_diffuse1", 0);
        shader->setVec4("diffuse_color", glm::vec4(1, 1, 1, 1));

        frameBuffer = new OpenGLFrameBuffer();
    }

    OpenGLRenderer::~OpenGLRenderer() {
        delete this->shader;
        delete this->frameBuffer;
        delete this->texture;
    }

    void OpenGLRenderer::preRender(int viewportWidth, int viewportHeight, bool fullscreen) {
        this->frameBuffer->bindFrameBuffer();
        this->resizeFrameBuffer();
        this->updateViewportSize(viewportWidth, viewportHeight, fullscreen);
    }

    void OpenGLRenderer::render(int viewportWidth, int viewportHeight, bool fullscreen) {
        Input::setPlayWindowActive(true);
        this->preRender(viewportWidth, viewportHeight, fullscreen);

        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        auto sceneCamera = Project::getScene()->getSceneCamera();
        auto mainCamera = Project::getScene()->getMainCamera();

        if ((Project::isPlaying() && mainCamera) || (!Project::isPlaying() && sceneCamera)) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // activate shader
            shader->use();
            shader->setInt("texture_diffuse1", 0);
//            shader->setVec3("diffuse_color", glm::vec3(1, 0, 0));

            glm::mat4 projection;
            if (Project::isPlaying()) {
                projection = glm::perspective(glm::radians(mainCamera.getComponent<Component::CameraComponent>().fov), (float) viewportWidth / (float) viewportHeight, mainCamera.getComponent<Component::CameraComponent>().zNear, mainCamera.getComponent<Component::CameraComponent>().zFar);
            } else {
                projection = glm::perspective(glm::radians(sceneCamera.getComponent<Component::SceneCameraComponent>().fov), (float) viewportWidth / (float) viewportHeight, sceneCamera.getComponent<Component::SceneCameraComponent>().zNear, sceneCamera.getComponent<Component::SceneCameraComponent>().zFar);
            }
            shader->setMat4("projection", projection);

            glm::mat4 view;
            if (Project::isPlaying()) {
                view = mainCamera.getComponent<Component::CameraComponent>().getViewMatrix(mainCamera);
            } else {
                view = sceneCamera.getComponent<Component::SceneCameraComponent>().getViewMatrix(sceneCamera);
            }
            shader->setMat4("view", view);

            // update all animator entities
            // TODO: IMPORTANT NOTE - THIS WILL USE THE SAME ANIMATOR FOR *ALL* MESHES, WE NEED TO SOMEHOW SEPARATE THEM
            // TODO: POSSIBLE SOLUTION 1 (BETTER - better runtime and allows for BSP in future): RENDER STARTING AT ROOT COMPONENT SINCE THIS ENSURES WE WILL USE THE CORRECT ANIMATOR FOR SUB-MESHES
            // TODO: POSSIBLE SOLUTION 2 (WORSE and more complicated): USE DATA STRUCTURE TO STORE FINAL TRANSFORM BONES PER GUID
            std::vector<glm::mat4> finalBoneMatrices;
            auto animatorEntities = Project::getScene()->getEntitiesWithComponents<Component::AnimatorComponent>();
            for(auto entityHandle : animatorEntities) {
                Entity entity = {entityHandle, Project::getScene()};
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

            // draw all entities with meshes
            auto meshEntities = Project::getScene()->getEntitiesWithComponents<Component::MeshComponent>();
            for(auto entityHandle : meshEntities) {
                Entity entity = {entityHandle, Project::getScene()};

                // load mesh of entity
                if (entity.hasComponent<Component::MeshComponent>()) {
                    if (!entity.getComponent<Component::MeshComponent>().mesh) {
                        entity.getComponent<Component::MeshComponent>().loadMesh();
                    }
                }

                // load material of entity
                if (entity.hasComponent<Component::MaterialComponent>()) {
                    if (!entity.getComponent<Component::MaterialComponent>().diffuseTexture) {
                        entity.getComponent<Component::MaterialComponent>().loadTexture();
                    }
                    shader->setVec4("diffuse_color",  entity.getComponent<Component::MaterialComponent>().diffuseColor);
                }

                if (entity.hasComponent<Component::MaterialComponent>() && !entity.getComponent<Component::MaterialComponent>().guid.empty()) {
                    if (entity.getComponent<Component::MaterialComponent>().diffuseTexture) {
                        auto* openGLTexture = dynamic_cast<OpenGLTexture*>(entity.getComponent<Component::MaterialComponent>().diffuseTexture);
                        if (openGLTexture) {
                            shader->use();
                            shader->setInt("texture_diffuse1", 0);
                            openGLTexture->bind(0);
                            shader->use();
                            shader->setInt("texture_diffuse1", 0);
                        }
                    } else {
                        Logger::warn("No texture loaded");
                    }
                } else {
                    texture->bind(0);
                }

                // get transform of entity
                glm::mat4 model = entity.getComponent<Component::TransformComponent>().getTransform(entity);
                shader->setMat4("model", model);

                if (entity.hasComponent<Component::MeshComponent>()) {
                    // set bones for animation
                    for (int i = 0; i < finalBoneMatrices.size(); i++) {
                        shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", finalBoneMatrices[i]);
                    }

                    // draw mesh of entity
                    if (entity.getComponent<Component::MeshComponent>().mesh) {
                        auto* openGLMesh = dynamic_cast<OpenGLMesh*>(entity.getComponent<Component::MeshComponent>().mesh);
                        if (openGLMesh) {
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
                            Logger::fatal("Mesh cannot be rendered in OpenGL");
                        }
                    } else if (entity.getComponent<Component::MeshComponent>().mesh && !entity.getComponent<Component::MeshComponent>().fileId.empty()) {
                        Logger::warn("No mesh loaded");
                    }
                }
            }
        } else {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        this->postRender(fullscreen);
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
        Logger::info("GL Vendor: " + std::string((const char*) glGetString(GL_VENDOR)));
        Logger::info("GL Renderer: " + std::string((const char*) glGetString(GL_RENDERER)));
        Logger::info("GL Version: " + std::string((const char*) glGetString(GL_VERSION)));
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
    }
}
