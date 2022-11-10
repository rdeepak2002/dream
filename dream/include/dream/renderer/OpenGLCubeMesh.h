//
// Created by Deepak Ramalingam on 11/10/22.
//

#ifndef DREAM_OPENGLCUBEMESH_H
#define DREAM_OPENGLCUBEMESH_H

#include "dream/renderer/OpenGLMesh.h"

namespace Dream {
    class OpenGLCubeMesh : public OpenGLMesh {
    public:
        OpenGLCubeMesh();
        void Finalize(bool interleaved = true) override;
    };
}

#endif //DREAM_OPENGLCUBEMESH_H
