//
// Created by Deepak Ramalingam on 2/1/23.
//

#ifndef DREAM_OPENGLSHADOWMAPFBO_H
#define DREAM_OPENGLSHADOWMAPFBO_H

namespace Dream {
    class OpenGLShadowMapFBO {
    public:
        OpenGLShadowMapFBO(int width, int height);
        ~OpenGLShadowMapFBO();
        void bind();
        void unbind();
        void bindForReading(int unit);
        unsigned int getTexture();
        int getWidth();
        int getHeight();
    private:
        unsigned int depthMapFBO, depthMap;
        int width, height;
    };
}

#endif //DREAM_OPENGLSHADOWMAPFBO_H
