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

namespace Dream {
    OpenGLRenderer::OpenGLRenderer() : Renderer() {
        this->printGLVersion();

        // build and compile our shader program
        shader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("shader.vert").c_str(),
                                  Project::getPath().append("assets").append("shaders").append("shader.frag").c_str(),
                                  nullptr);

        texture = new OpenGLTexture(Project::getPath().append("assets").append("textures").append("missing.png"));

        // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
        // -------------------------------------------------------------------------------------------
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
        // bind framebuffer to draw screen contents to a texture
        this->frameBuffer->bindFrameBuffer();
        this->resizeFrameBuffer();
        // update gl viewport size
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

        // draw all entities with meshes
        auto meshEntities = Project::getScene()->getEntitiesWithComponents<Component::MeshComponent>();
        for(auto entityHandle : meshEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            if (entity.hasComponent<Component::MaterialComponent>()) {
                auto* openGLTexture = dynamic_cast<OpenGLTexture*>(entity.getComponent<Component::MaterialComponent>().texture);
                if (openGLTexture) {
                    shader->use();
                    shader->setInt("texture_diffuse1", 0);
                    openGLTexture->bind(0);
                    shader->use();
                    shader->setInt("texture_diffuse1", 0);
                }
            } else {
                // TODO: this is unknown material - use purple
                texture->bind(0);
            }

            // get transform of entity
            glm::mat4 model = entity.getComponent<Component::TransformComponent>().getTransform(entity);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // draw mesh of entity
            auto mesh = entity.getComponent<Component::MeshComponent>().mesh;
            auto* openGLMesh = dynamic_cast<OpenGLMesh*>(mesh);
            if (openGLMesh) {
                if (!openGLMesh->getIndices().empty()) {
                    // case where vertices are indexed
                    auto numIndices = openGLMesh->getIndices().size();
                    glBindVertexArray(openGLMesh->getVAO());
                    glDrawElements(GL_TRIANGLES, (int) numIndices, GL_UNSIGNED_INT, nullptr);
                    glBindVertexArray(0);
                } else if (!openGLMesh->getPositions().empty()) {
                    // case where vertices are not indexed
                    glBindVertexArray(openGLMesh->getVAO());
                    glDrawArrays(GL_TRIANGLES, 0, (int) openGLMesh->getPositions().size());
                    glBindVertexArray(0);
                } else {
                    std::cout << "Unable to render mesh" << std::endl;
                    exit(EXIT_FAILURE);
                }
            } else {
                std::cout << "Mesh cannot be rendered in OpenGL" << std::endl;
                exit(EXIT_FAILURE);
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
        printf("GL Vendor:   %s\n", glGetString(GL_VENDOR));
        printf("GL Renderer: %s\n", glGetString(GL_RENDERER));
        printf("GL Version:  %s\n", glGetString(GL_VERSION));
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
