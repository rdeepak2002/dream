//
// Created by Deepak Ramalingam on 2/10/23.
//

#ifndef DREAM_SKINNINGTECH_H
#define DREAM_SKINNINGTECH_H

#include "OpenGLShader.h"
#include "dream/scene/Entity.h"

namespace Dream {
    class SkinningTech {
    public:
        void setJointUniforms(Entity entity, OpenGLShader *shader);
    };
}

#endif //DREAM_SKINNINGTECH_H
