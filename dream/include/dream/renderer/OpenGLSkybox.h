//
// Created by Deepak Ramalingam on 1/3/23.
//

#ifndef DREAM_OPENGLSKYBOX_H
#define DREAM_OPENGLSKYBOX_H

#include <vector>

namespace Dream {
    class OpenGLSkybox {
    public:
        OpenGLSkybox();
        ~OpenGLSkybox();
        unsigned int getVAO();
        unsigned int getTexture();
    private:
        unsigned int skyboxVAO, skyboxVBO, cubemapTexture;
        unsigned int loadCubemap(std::vector<std::string> faces);
    };
}

#endif //DREAM_OPENGLSKYBOX_H
