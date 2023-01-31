//
// Created by Deepak Ramalingam on 1/24/23.
//

#include "dream/renderer/OpenGLRenderer.h"
#include "dream/renderer/OpenGLShadowMapFBO.h"


namespace Dream {
    OpenGLShadowMapFBO::OpenGLShadowMapFBO(unsigned int width, unsigned int height) {
        this->fbo = 0;
        this->shadowMap = 0;

        // Create the FBO
        glGenFramebuffers(1, &fbo);

        // Create the depth buffer
        glGenTextures(1, &shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, (int) width, (int) height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        #ifndef EMSCRIPTEN
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        #endif

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

        // Disable writes to the color buffer
        #ifndef EMSCRIPTEN
        glDrawBuffer(GL_NONE);
        #endif
        glReadBuffer(GL_NONE);

//        GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//
//        if (Status != GL_FRAMEBUFFER_COMPLETE) {
//            printf("FB error, status: 0x%x\n", Status);
//            Logger::fatal("Error creating frame buffer for shadow mapping");
//        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    OpenGLShadowMapFBO::~OpenGLShadowMapFBO() {
        if (fbo != 0) {
            glDeleteFramebuffers(1, &fbo);
        }

        if (shadowMap != 0) {
            glDeleteTextures(1, &shadowMap);
        }
    }

    void OpenGLShadowMapFBO::bindForWriting() {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    }

    void OpenGLShadowMapFBO::bindForReading(int unit) {
        if (unit >= 0) {
            glActiveTexture(GL_TEXTURE0 + unit);
        }
        glBindTexture(GL_TEXTURE_2D, shadowMap);
    }

    void OpenGLShadowMapFBO::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int OpenGLShadowMapFBO::getShadowMap() {
        return shadowMap;
    }
}