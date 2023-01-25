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

#include "dream/renderer/OpenGLFrameBuffer.h"

#include <iostream>
#include <glad/glad.h>
#include "dream/project/Project.h"

Dream::OpenGLFrameBuffer::OpenGLFrameBuffer() {
    screenShader = new OpenGLShader(
            Project::getPath().append("assets").append("shaders").append("screen_shader.vert").c_str(),
            Project::getPath().append("assets").append("shaders").append("screen_shader.frag").c_str(),
            nullptr);
    // screen quad VAO
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
    };
    unsigned int screenQuadVBO;
    glGenVertexArrays(1, &screenQuadVAO);
    glGenBuffers(1, &screenQuadVBO);
    glBindVertexArray(screenQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));

    // framebuffer configuration
    // -------------------------
    glGenFramebuffers(1, &framebuffer);
    // create a color attachment texture
    glGenTextures(1, &textureColorbuffer);
    glGenRenderbuffers(1, &rbo);
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::fatal("Framebuffer is not complete");
    }
    this->width = 0;
    this->height = 0;
}

void Dream::OpenGLFrameBuffer::resize(int fbWidth, int fbHeight) {
    this->width = fbWidth;
    this->height = fbHeight;

    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

//#ifdef EMSCRIPTEN
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//#else
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//#endif

#ifdef EMSCRIPTEN
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
#endif

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width,
                          this->height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                              rbo); // now actually attach it
}

void Dream::OpenGLFrameBuffer::bindTexture() {
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);;
}

int Dream::OpenGLFrameBuffer::getTexture() {
    return this->textureColorbuffer;
}

void Dream::OpenGLFrameBuffer::bindFrameBuffer() {
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

int Dream::OpenGLFrameBuffer::getHeight() {
    return this->height;
}

int Dream::OpenGLFrameBuffer::getWidth() {
    return this->width;
}

void Dream::OpenGLFrameBuffer::renderScreenQuad() {
    this->screenShader->use();
    glBindVertexArray(screenQuadVAO);
    this->bindTexture();
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

Dream::OpenGLFrameBuffer::~OpenGLFrameBuffer() {
    delete this->screenShader;
}

void Dream::OpenGLFrameBuffer::unbindFrameBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
}
