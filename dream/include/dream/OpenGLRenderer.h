//
// Created by Deepak Ramalingam on 10/24/22.
//

#ifndef DREAM_OPENGLRENDERER_H
#define DREAM_OPENGLRENDERER_H

namespace Dream {
    class OpenGLRenderer {
    public:
        OpenGLRenderer();
        void render(int windowWidth, int windowHeight);
        unsigned int VBO, VAO, shaderProgram;
    };
}

#endif //DREAM_OPENGLRENDERER_H
