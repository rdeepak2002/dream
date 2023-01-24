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
    OpenGLRenderer::OpenGLRenderer() : Renderer() {
        // print GL version
        Logger::info("GL Vendor: " + std::string((const char *) glGetString(GL_VENDOR)));
        Logger::info("GL Renderer: " + std::string((const char *) glGetString(GL_RENDERER)));
        Logger::info("GL Version: " + std::string((const char *) glGetString(GL_VERSION)));

        simpleLightingShader = new OpenGLShader(Project::getPath().append("assets").append("shaders").append("simpleVertexShader.vert").c_str(),
                                                Project::getPath().append("assets").append("shaders").append("simpleLightingShader.frag").c_str(),
                                                nullptr);

        frameBuffer = new OpenGLFrameBuffer();

        openGLMesh = new OpenGLCubeMesh();
    }

    OpenGLRenderer::~OpenGLRenderer() {
        delete simpleLightingShader;
        delete frameBuffer;
        delete openGLMesh;
    }

    unsigned int OpenGLRenderer::getOutputRenderTexture() {
        return this->frameBuffer->getTexture();
    }

    void OpenGLRenderer::render(int viewportWidth, int viewportHeight, bool fullscreen) {
        Renderer::render(viewportWidth, viewportHeight, fullscreen);
        // resize viewport
        glViewport(0, 0, viewportWidth * 2, viewportHeight * 2);

        // bind and resize frame buffer
        this->frameBuffer->bindFrameBuffer();
        frameBuffer->resize(viewportWidth * 2, viewportHeight * 2);

        // start drawing
        simpleLightingShader->use();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) viewportWidth / (float) viewportHeight, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 model = glm::mat4(1.0);
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        simpleLightingShader->setMat4("projection", projection);
        simpleLightingShader->setMat4("view", view);
        simpleLightingShader->setMat4("model", model);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

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

        // clear framebuffer and return its texture
        this->frameBuffer->unbindFrameBuffer();

        // clear to color of editor
        glClearColor(0.1f, 0.105f, 0.11f, 1.0f); // set clear color to editor background
        glClear(GL_COLOR_BUFFER_BIT);

        // render the frame buffer as a quad when in full screen mode
        if (fullscreen) {
            this->frameBuffer->renderScreenQuad();
        }
    }
}
