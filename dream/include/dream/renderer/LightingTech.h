//
// Created by Deepak Ramalingam on 2/10/23.
//

#ifndef DREAM_LIGHTINGTECH_H
#define DREAM_LIGHTINGTECH_H

#include "dream/renderer/OpenGLShader.h"

namespace Dream {
    class LightingTech {
    public:
        void setLightShaderUniforms(OpenGLShader *shader);
    };
}

#endif //DREAM_LIGHTINGTECH_H
