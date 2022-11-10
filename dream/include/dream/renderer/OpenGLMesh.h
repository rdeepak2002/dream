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
//        std::vector<glm::vec3> Tangents;
//        std::vector<glm::vec3> Bitangents;

//        TOPOLOGY Topology = TRIANGLES;
        std::vector<unsigned int> Indices;
    public:
        OpenGLMesh();
//        OpenGLMesh(std::vector<glm::vec3> positions, std::vector<unsigned int> indices);
//        OpenGLMesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uv, std::vector<unsigned int> indices);
//        OpenGLMesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uv, std::vector<glm::vec3> normals, std::vector<unsigned int> indices);
//        OpenGLMesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uv, std::vector<glm::vec3> normals, std::vector<glm::vec3> tangents, std::vector<glm::vec3> bitangents, std::vector<unsigned int> indices);

        void SetPositions(std::vector<glm::vec3> positions);
        void SetUVs(std::vector<glm::vec2> uv);
        void SetNormals(std::vector<glm::vec3> normals);
//        void SetTangents(std::vector<glm::vec3> tangents, std::vector<glm::vec3> bitangents);

        // commits all buffers and attributes to the GPU driver
        virtual void Finalize(bool interleaved = true);
    };
}

#endif //DREAM_OPENGLMESH_H
