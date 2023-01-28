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

        // output render texture to allow for post-processing and embedding of scene in editor
        outputFrameBuffer = new OpenGLFrameBuffer();

        // frame buffer to write shadow depth
        shadowMapFbo = new OpenGLShadowMapFBO(SHADOW_WIDTH, SHADOW_HEIGHT);

        // cube primitive mesh
        cubeMesh = new OpenGLCubeMesh();
    }

    OpenGLRenderer::~OpenGLRenderer() {
        delete simpleLightingShader;
        delete outputFrameBuffer;
        delete cubeMesh;
    }

    unsigned int OpenGLRenderer::getOutputRenderTexture() {
        return this->outputFrameBuffer->getTexture();
//        return this->shadowMapFbo->getShadowMap();
    }

    void OpenGLRenderer::render(int viewportWidth, int viewportHeight, bool fullscreen) {

        // TODO: maybe depth test not being done properly?

        // TODO: refer to this commit https://github.com/rdeepak2002/dream/blob/e36d00a602cf93e5b8f3f3b75725284b7a117110/dream/src/renderer/OpenGLRenderer.cpp
        // TODO: how to fix -> refer to dream engine and where we clear, set depth buffer, set color, etc. (remove this code from buffer classes)
        // TODO: how to fix -> refer to dream engine and where we clear, set depth buffer, set color, etc. (remove this code from buffer classes)
        // TODO: how to fix -> refer to dream engine and where we clear, set depth buffer, set color, etc. (remove this code from buffer classes)
        // TODO: how to fix -> refer to drema engine and where we clear, set depth buffer, set color, etc. (remove this code from buffer classes)
        // TODO: how to fix -> refer to dream engine and where we clear, set depth buffer, set color, etc. (remove this code from buffer classes)
        // TODO: how to fix -> refer to dream engine and where we clear, set depth buffer, set color, etc. (remove this code from buffer classes)
        // TODO: how to fix -> refer to dream engine and where we clear, set depth buffer, set color, etc. (remove this code from buffer classes)
        // TODO: how to fix -> refer to dream engine and where we clear, set depth buffer, set color, etc. (remove this code from buffer classes)

        Renderer::render(viewportWidth, viewportHeight, fullscreen);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        // draw scene to shadow map
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        shadowMapFbo->bindForWriting();
        drawScene(viewportWidth, viewportHeight, simpleDepthShader, RENDER_FLAG_SHADOW);
        shadowMapFbo->unbind();

        // draw final, lighted scene
        glViewport(0, 0, viewportWidth * 2, viewportHeight * 2);
        outputFrameBuffer->bindFrameBuffer();
        outputFrameBuffer->resize(viewportWidth * 2, viewportHeight * 2);
        drawScene(viewportWidth, viewportHeight, simpleLightingShader, RENDER_FLAG_FINAL);
        outputFrameBuffer->unbindFrameBuffer();

        // clear frame buffer to color of editor
        glClearColor(0.1f, 0.105f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render the frame buffer as a quad over the entire screen when in full screen mode
        if (fullscreen) {
            this->outputFrameBuffer->renderScreenQuad();
        }
    }

    // TODO: pass flags (add diffuse texture, add normal maps to texture, add specular texture, add shadow map, etc.)
    void OpenGLRenderer::drawScene(int viewportWidth, int viewportHeight, OpenGLShader *shader, int flags) {
        // use the shader
        shader->use();

        // light position for shadow
        glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

        // bind textures for final render
        if (flags & RENDER_FLAG_FINAL) {
            shader->setInt("shadowMap", 0);
            shadowMapFbo->bindForReading(0);
            shader->setVec3("lightPos", lightPos);
        }

        // clear frame buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // projection * view matrix of directional light that casts shadows
        if ((flags & RENDER_FLAG_SHADOW) || (flags & RENDER_FLAG_FINAL)) {
            float near_plane = 1.0f, far_plane = 7.5f;
            glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
            glm::mat4 lightSpaceMatrix = lightProjection * lightView;
            shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        }

        // setting view and projection matrices for final rendering
        if (flags & RENDER_FLAG_FINAL) {
            // set model, view, and projection matrices
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) viewportWidth / (float) viewportHeight, 0.1f, 100.0f);
            glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 8.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            shader->setMat4("projection", projection);
            shader->setMat4("view", view);
        }

        // draw floor mesh
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2, 0.0));
        model = glm::scale(model, glm::vec3(100, 1, 100));
        shader->setMat4("model", model);
        drawMesh(cubeMesh);

        // draw cube meshes
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        shader->setMat4("model", model);
        drawMesh(cubeMesh);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
        model = glm::scale(model, glm::vec3(0.5f));
        shader->setMat4("model", model);
        drawMesh(cubeMesh);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
        model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        model = glm::scale(model, glm::vec3(0.25));
        shader->setMat4("model", model);
        drawMesh(cubeMesh);
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
}
