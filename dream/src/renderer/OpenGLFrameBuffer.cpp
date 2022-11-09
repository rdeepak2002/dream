//
// Created by Deepak Ramalingam on 11/9/22.
//

#include "dream/renderer/OpenGLFrameBuffer.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <glad/glad.h>
#else
#include <glad/glad.h>
#endif

#include <iostream>

Dream::OpenGLFrameBuffer::OpenGLFrameBuffer() {
    // framebuffer configuration
    // -------------------------
    glGenFramebuffers(1, &framebuffer);
    // create a color attachment texture
    glGenTextures(1, &textureColorbuffer);
    glGenRenderbuffers(1, &rbo);
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    this->width = 0;
    this->height = 0;
}

void Dream::OpenGLFrameBuffer::resize(int fbWidth, int fbHeight) {
    this->width = fbWidth;
    this->height = fbHeight;

    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width, this->height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
}

void Dream::OpenGLFrameBuffer::bindTexture() {
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);;
}

int Dream::OpenGLFrameBuffer::getTexture() {
    return this->textureColorbuffer;
}

void Dream::OpenGLFrameBuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

int Dream::OpenGLFrameBuffer::getHeight() {
    return this->height;
}

int Dream::OpenGLFrameBuffer::getWidth() {
    return this->width;
}
