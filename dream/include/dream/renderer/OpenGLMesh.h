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
        OpenGLMesh();
        OpenGLMesh(std::string path);
        OpenGLMesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uv, std::vector<glm::vec3> normals);
        void setPositions(std::vector<glm::vec3> positions);
        void setUVs(std::vector<glm::vec2> uv);
        void setNormals(std::vector<glm::vec3> normals);
        void setTangents(std::vector<glm::vec3> tangents);
        void setBitangents(std::vector<glm::vec3> bitangents);
        std::vector<glm::vec3> getPositions();
        std::vector<glm::vec2> getUVs();
        std::vector<glm::vec3> getNormals();
        std::vector<glm::vec3> getTangents();
        std::vector<glm::vec3> getBitangents();
        unsigned int getVAO();
        unsigned int getVBO();
        unsigned int getEBO();
        std::vector<unsigned int> getIndices();
        void finalize(bool interleaved = true);
    private:

    protected:
        unsigned int m_VAO = 0;
        unsigned int m_VBO;
        unsigned int m_EBO;
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;
        std::vector<unsigned int> indices;
    };
}

#endif //DREAM_OPENGLMESH_H
