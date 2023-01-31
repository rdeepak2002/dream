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

#define SHADOW_MAP_TEXTURE_UNIT 0

namespace Dream {
    OpenGLRenderer::OpenGLRenderer() : Renderer() {
        // print GL version
        Logger::info("GL Vendor: " + std::string((const char *) glGetString(GL_VENDOR)));
        Logger::info("GL Renderer: " + std::string((const char *) glGetString(GL_RENDERER)));
        Logger::info("GL Version: " + std::string((const char *) glGetString(GL_VERSION)));

        // shader for rendering final lighting
        simpleLightingShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("simpleVertexShader.vert").c_str(),
                                                Project::getPath().append("assets").append("shaders").append("simpleLightingShader.frag").c_str(),
                                                nullptr);

        // shader for writing to depth buffer for shadow calculation
        simpleDepthShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("simpleDepthShader.vert").c_str(),
                                                Project::getPath().append("assets").append("shaders").append("simpleDepthShader.frag").c_str(),
                                                nullptr);

        // output render texture to allow for post-processing and embedding of scene in editor (draw final scene to a texture basically)
        outputRenderTextureFbo = new OpenGLFrameBuffer();

        // frame buffer to write shadow depth (create a depth map from perspective of directional light)
        shadowMapFbo = new OpenGLShadowMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT);

        // cube mesh primitive
        cubeMesh = new OpenGLCubeMesh();
    }

    OpenGLRenderer::~OpenGLRenderer() {
        delete simpleLightingShader;
        delete outputRenderTextureFbo;
        delete shadowMapFbo;
        delete cubeMesh;
    }

    unsigned int OpenGLRenderer::getOutputRenderTexture() {
        return this->outputRenderTextureFbo->getTexture();
//        return this->shadowMapFbo->getShadowMap();
    }

    void OpenGLRenderer::render(int viewportWidth, int viewportHeight, bool fullscreen) {
        Renderer::render(viewportWidth, viewportHeight, fullscreen);

        // renderer camera
        std::optional<Camera> camera = getMainCamera(viewportWidth, viewportHeight);

        // OpenGL options (enable blending and depth testing)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        // draw scene to directional light shadow map
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        shadowMapFbo->bindForWriting();
        glClear(GL_DEPTH_BUFFER_BIT);
        drawScene(camera, simpleDepthShader, RENDER_FLAG_SHADOW);
        shadowMapFbo->unbind();

        // reset viewport
        glViewport(0, 0, viewportWidth * 2, viewportHeight * 2);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw final, lighted scene to output render texture
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        outputRenderTextureFbo->bindFrameBuffer();
        if (outputRenderTextureFbo->getWidth() != viewportWidth * 2 || outputRenderTextureFbo->getHeight() != viewportHeight * 2) {
            outputRenderTextureFbo->resize(viewportWidth * 2, viewportHeight * 2);
        }
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawScene(camera, simpleLightingShader, RENDER_FLAG_FINAL);
        outputRenderTextureFbo->unbindFrameBuffer();
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.105f, 0.11f, 1.0f); // set clear color to editor background
        glClear(GL_COLOR_BUFFER_BIT);

        // render the frame buffer as a quad over the entire screen when in full screen mode
        if (fullscreen) {
            this->outputRenderTextureFbo->renderScreenQuad();
        }
    }

    void OpenGLRenderer::drawScene(std::optional<Camera> maybeCamera, OpenGLShader *shader, int flags) {
        // if no camera is set (main or scene) then clear screen and show nothing
        if (!maybeCamera) {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            return;
        }

        // unwrap optional
        Camera camera = *maybeCamera;

        // use the shader
        shader->use();

        glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

        // set variables for directional light for shadow computations
        if ((flags & RENDER_FLAG_SHADOW) || (flags & RENDER_FLAG_FINAL)) {
            float near_plane = 1.0f, far_plane = 20.0f;
            glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
            glm::mat4 lightSpaceMatrix = lightProjection * lightView;
            shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        }

        // set view and projection matrices for final rendering from camera perspective
        if (flags & RENDER_FLAG_FINAL) {
            shader->setMat4("projection", camera.getProjectionMatrix());
            shader->setMat4("view", camera.getViewMatrix());
            shader->setVec3("lightPos", lightPos);
        }

        // bind textures for final render
        if (flags & RENDER_FLAG_FINAL) {
            // bind shadow map
            shader->setInt("shadowMap", SHADOW_MAP_TEXTURE_UNIT);
            shadowMapFbo->bindForReading(SHADOW_MAP_TEXTURE_UNIT);
        }

        // draw root entity and its children
        drawEntity(Project::getScene()->getRootEntity(), shader);
    }

    void OpenGLRenderer::drawEntity(Dream::Entity entity, OpenGLShader* shader) {
        // draw children
        Entity child = entity.getComponent<Component::HierarchyComponent>().first;
        while (child) {
            drawEntity(child, shader);
            child = child.getComponent<Component::HierarchyComponent>().next;
        }

        if (entity.hasComponent<Component::MeshComponent>()) {
            // set model matrix
            auto modelMatrix = entity.getComponent<Component::TransformComponent>().getTransform(entity);
            shader->setMat4("model", modelMatrix);
            // draw mesh of entity
            auto &meshComponent = entity.getComponent<Component::MeshComponent>();
            if (meshComponent.meshType == Component::MeshComponent::PRIMITIVE_CUBE) {
                drawMesh(cubeMesh);
            }
        }
    }

    void OpenGLRenderer::drawMesh(OpenGLMesh* openGLMesh) {
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

    std::optional<Camera> OpenGLRenderer::getMainCamera(int viewportWidth, int viewportHeight) {
        // renderer camera
        std::optional<Camera> camera;

        // update renderer camera using scene camera entity's position and camera attributes like yaw, pitch, and fov
        auto sceneCameraEntity = Project::getScene()->getSceneCamera();
        if (sceneCameraEntity) {
            camera = {(float) viewportWidth * 2.0f, (float) viewportHeight * 2.0f};
            sceneCameraEntity.getComponent<Component::SceneCameraComponent>().updateRendererCamera(*camera, sceneCameraEntity);
        }

        return camera;
    }
}
