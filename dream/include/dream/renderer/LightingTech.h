//
// Created by Deepak Ramalingam on 2/10/23.
//

#ifndef DREAM_LIGHTINGTECH_H
#define DREAM_LIGHTINGTECH_H

#include "dream/renderer/OpenGLShader.h"
#include "dream/renderer/DirectionalLightShadowTech.h"
#include "dream/scene/Entity.h"
#include "OpenGLTexture.h"
#include "OpenGLShadowMapFBO.h"

namespace Dream {
    class LightingTech {
    public:
        LightingTech();
        ~LightingTech();
        void setTextureAndColorUniforms(Entity entity, std::vector<OpenGLShadowMapFBO *> shadowMapFbos, DirectionalLightShadowTech* directionalLightShadowTech, OpenGLShader *shader);
    private:
        void setLightShaderUniforms(OpenGLShader *shader);
        OpenGLTexture *whiteTexture;
        OpenGLTexture *blackTexture;
    };
}

#endif //DREAM_LIGHTINGTECH_H
