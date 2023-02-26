//
// Created by Deepak Ramalingam on 2/1/23.
//

#include "dream/renderer/OpenGLShadowMapFBO.h"
#include "dream/renderer/OpenGLRenderer.h"

namespace Dream {
    OpenGLShadowMapFBO::OpenGLShadowMapFBO(int width, int height) {
        this->width = width;
        this->height = height;
        glGenFramebuffers(1, &depthMapFBO);
        // create depth texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // TODO: webgl does not support GL_CLAMP_TO_BORDER...
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//        #ifndef EMSCRIPTEN
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
//        #endif
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        #ifndef EMSCRIPTEN
        glDrawBuffer(GL_NONE);
        #endif
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    OpenGLShadowMapFBO::~OpenGLShadowMapFBO() {

    }

    void OpenGLShadowMapFBO::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    }

    void OpenGLShadowMapFBO::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLShadowMapFBO::bindForReading(int unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, depthMap);
    }

    unsigned int OpenGLShadowMapFBO::getTexture() {
        return depthMap;
    }

    int OpenGLShadowMapFBO::getWidth() {
        return width;
    }

    int OpenGLShadowMapFBO::getHeight() {
        return height;
    }
}