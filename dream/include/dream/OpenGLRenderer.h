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
        void render(int windowWidth, int windowHeight);
        unsigned int VBO, VAO;
    private:
        OpenGLShader *shader;
    };
}

#endif //DREAM_OPENGLRENDERER_H
