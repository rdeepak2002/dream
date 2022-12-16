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
        OpenGLMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indicesIn);
        unsigned int getVAO();
        unsigned int getVBO();
        unsigned int getEBO();
        void finalize(bool interleaved = true);
    protected:
        OpenGLMesh();
        unsigned int vao = 0;
        unsigned int vbo = 0;
        unsigned int ebo = 0;
    };
}

#endif //DREAM_OPENGLMESH_H
