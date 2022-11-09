//
// Created by Deepak Ramalingam on 10/24/22.
//

#ifndef DREAM_OPENGLRENDERER_H
#define DREAM_OPENGLRENDERER_H

#include "dream/renderer/Renderer.h"
#include "dream/renderer/OpenGLShader.h"

namespace Dream {
    class OpenGLRenderer : public Renderer {
    public:
        OpenGLRenderer();
        ~OpenGLRenderer();
        unsigned int render(int viewportWidth, int viewportHeight, bool fullscreen) override;
    private:
        void resizeFrameBuffer();
        void printGLVersion();
        OpenGLShader *shader;
        OpenGLShader *screenShader;
        unsigned int VBO, VAO, framebuffer, quadVAO, textureColorbuffer, rbo;
    };
}

#endif //DREAM_OPENGLRENDERER_H
