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

namespace Dream {
    OpenGLRenderer::OpenGLRenderer() : Renderer() {
        this->printGLVersion();

        // build and compile our shader program
        shader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("shader.vert").c_str(), Project::getPath().append("assets").append("shaders").append("shader.frag").c_str(), nullptr);
        texture = new OpenGLTexture(Project::getPath().append("assets").append("textures").append("missing.png"));

        // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
        shader->use();
        shader->setInt("texture_diffuse1", 0);

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
        this->preRender(viewportWidth, viewportHeight, fullscreen);

        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // activate shader
        shader->use();
        shader->setInt("texture_diffuse1", 0);

        // create transformations
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), (float) viewportWidth / (float) viewportHeight, 0.1f, 100.0f);
        // retrieve the matrix uniform locations
        int modelLoc = glGetUniformLocation(shader->ID, "model");
        int viewLoc  = glGetUniformLocation(shader->ID, "view");
        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        shader->setMat4("projection", projection);

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
                std::cout << "Error: no mesh component for entity with animator so bones cannot be loaded" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (entity.getComponent<Component::AnimatorComponent>().needsToLoadAnimations) {
                entity.getComponent<Component::AnimatorComponent>().loadAnimations(entity);
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
                    std::cout << "WARNING: no texture loaded" << std::endl;
                }
            } else {
                texture->bind(0);
            }

            // get transform of entity
            glm::mat4 model = entity.getComponent<Component::TransformComponent>().getTransform(entity);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

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
                            std::cout << "Unable to render mesh" << std::endl;
                            exit(EXIT_FAILURE);
                        }
                    } else {
                        std::cout << "Mesh cannot be rendered in OpenGL" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                } else if (entity.getComponent<Component::MeshComponent>().mesh && !entity.getComponent<Component::MeshComponent>().fileId.empty()) {
                    std::cout << "WARNING: no mesh loaded" << std::endl;
                }
            }
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
        Logger::debug("GL Vendor:\t\t" + std::string((const char*) glGetString(GL_VENDOR)));
        Logger::debug("GL Renderer:\t" + std::string((const char*) glGetString(GL_RENDERER)));
        Logger::debug("GL Version:\t\t" + std::string((const char*) glGetString(GL_VERSION)));
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
