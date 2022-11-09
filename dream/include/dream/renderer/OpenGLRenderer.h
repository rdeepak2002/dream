//
// Created by Deepak Ramalingam on 10/24/22.
//

#ifndef DREAM_OPENGLRENDERER_H
#define DREAM_OPENGLRENDERER_H

#include "dream/renderer/Renderer.h"
#include "dream/renderer/OpenGLShader.h"
#include "dream/renderer/OpenGLFrameBuffer.h"

namespace Dream {
    class OpenGLRenderer : public Renderer {
    public:
        OpenGLRenderer();
        ~OpenGLRenderer();
        unsigned int render(int viewportWidth, int viewportHeight, bool fullscreen) override;
    private:
        OpenGLShader *shader;
        OpenGLShader *screenShader;
        OpenGLFrameBuffer* frameBuffer;
        unsigned int VBO, VAO;
        unsigned int screenQuadVAO;
        void resizeFrameBuffer();
        void printGLVersion();
        std::pair<int, int> getViewportDimensions();
    };
}

#endif //DREAM_OPENGLRENDERER_H
