//
// Created by Deepak Ramalingam on 11/9/22.
//

#ifndef DREAM_OPENGLFRAMEBUFFER_H
#define DREAM_OPENGLFRAMEBUFFER_H

#include "dream/renderer/OpenGLShader.h"

namespace Dream {
    class OpenGLFrameBuffer {
    public:
        OpenGLFrameBuffer();
        ~OpenGLFrameBuffer();
        void resize(int fbWidth, int fbHeight);
        void bindTexture();
        void bindFrameBuffer();
        void bindDefaultFrameBuffer();
        void renderScreenQuad();
        void clear();
        int getTexture();
        int getWidth();
        int getHeight();
    private:
        int width, height;
        unsigned int framebuffer, textureColorbuffer, rbo, screenQuadVAO;
        OpenGLShader *screenShader;
    };
}

#endif //DREAM_OPENGLFRAMEBUFFER_H
