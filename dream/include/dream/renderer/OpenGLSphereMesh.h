//
// Created by Deepak Ramalingam on 11/10/22.
//

#ifndef DREAM_OPENGLSPHEREMESH_H
#define DREAM_OPENGLSPHEREMESH_H

#include "dream/renderer/OpenGLMesh.h"

namespace Dream {
    class OpenGLSphereMesh : public OpenGLMesh {
    public:
        OpenGLSphereMesh(unsigned int xSegments = 20, unsigned int ySegments = 20);
    };
}

#endif //DREAM_OPENGLSPHEREMESH_H
