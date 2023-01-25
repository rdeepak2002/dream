//
// Created by Deepak Ramalingam on 1/24/23.
//

#ifndef DREAM_OPENGLSHADOWMAPFBO_H
#define DREAM_OPENGLSHADOWMAPFBO_H

namespace Dream {
    class OpenGLShadowMapFBO {
    public:
        OpenGLShadowMapFBO(unsigned int width, unsigned int height);
        ~OpenGLShadowMapFBO();
        void bindForWriting();
        void bindForReading(int unit);
        void unbind();
        unsigned int getShadowMap();
    private:
        unsigned int fbo = 0;
        unsigned int shadowMap = 0;
    };
}

#endif //DREAM_OPENGLSHADOWMAPFBO_H
