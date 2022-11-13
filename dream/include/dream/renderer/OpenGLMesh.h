//
// Created by Deepak Ramalingam on 11/10/22.
//

#ifndef DREAM_OPENGLMESH_H
#define DREAM_OPENGLMESH_H

#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include "dream/renderer/Mesh.h"

namespace Dream {
    class OpenGLMesh : public Mesh {
    public:
        OpenGLMesh();
        OpenGLMesh(std::string path);
        unsigned int getVAO();
        unsigned int getVBO();
        unsigned int getEBO();
        void finalize(bool interleaved = true);
    protected:
        unsigned int m_VAO = 0;
        unsigned int m_VBO;
        unsigned int m_EBO;
    };
}

#endif //DREAM_OPENGLMESH_H
