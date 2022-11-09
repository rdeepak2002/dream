//
// Created by Deepak Ramalingam on 11/9/22.
//

#ifndef DREAM_OPENGLFRAMEBUFFER_H
#define DREAM_OPENGLFRAMEBUFFER_H

namespace Dream {
    class OpenGLFrameBuffer {
    public:
        OpenGLFrameBuffer();
        void resize(int fbWidth, int fbHeight);
        void bindTexture();
        void bind();
        int getTexture();
        int getWidth();
        int getHeight();
    private:
        int width, height;
        unsigned int framebuffer, textureColorbuffer, rbo;
    };
}

#endif //DREAM_OPENGLFRAMEBUFFER_H
