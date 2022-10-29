//
// Created by Deepak Ramalingam on 10/24/22.
//

#ifndef DREAM_OPENGLRENDERER_H
#define DREAM_OPENGLRENDERER_H

#include "dream/OpenGLShader.h"

namespace Dream {
    class OpenGLRenderer {
    public:
        OpenGLRenderer();
        ~OpenGLRenderer();
        unsigned int render(int viewportWidth, int viewportHeight, bool fullscreen);
        void initFrameBuffer(int viewportWidth, int viewportHeight);
        void resizeFrameBuffer();
    private:
        OpenGLShader *shader;
        OpenGLShader *screenShader;
        unsigned int VBO, VAO, framebuffer, quadVAO, textureColorbuffer, rbo;
    };
}

#endif //DREAM_OPENGLRENDERER_H
