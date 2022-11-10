//
// Created by Deepak Ramalingam on 11/10/22.
//

#ifndef DREAM_OPENGLMESH_H
#define DREAM_OPENGLMESH_H

#include <vector>
#include <functional>
#include <glm/glm.hpp>

namespace Dream {
    class OpenGLMesh {
    public:
        unsigned int m_VAO = 0;
        unsigned int m_VBO;
        unsigned int m_EBO;
    public:
        std::vector<glm::vec3> Positions;
        std::vector<glm::vec2> UV;
        std::vector<glm::vec3> Normals;
        std::vector<glm::vec3> Tangents;
        std::vector<glm::vec3> Bitangents;
        std::vector<unsigned int> Indices;
    public:
        OpenGLMesh();

        void setPositions(std::vector<glm::vec3> positions);
        void setUVs(std::vector<glm::vec2> uv);
        void setNormals(std::vector<glm::vec3> normals);
        void setTangents(std::vector<glm::vec3> tangents, std::vector<glm::vec3> bitangents);

        // commits all buffers and attributes to the GPU driver
        void finalize(bool interleaved = true);
    };
}

#endif //DREAM_OPENGLMESH_H
