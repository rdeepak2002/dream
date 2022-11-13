//
// Created by Deepak Ramalingam on 11/13/22.
//

#ifndef DREAM_MESH_H
#define DREAM_MESH_H

#include <vector>
#include <functional>
#include <glm/glm.hpp>

namespace Dream {
    class Mesh {
    public:
        void setPositions(std::vector<glm::vec3> positions);
        void setUVs(std::vector<glm::vec2> uv);
        void setNormals(std::vector<glm::vec3> normals);
        void setTangents(std::vector<glm::vec3> tangents);
        void setBitangents(std::vector<glm::vec3> bitangents);
        virtual std::vector<glm::vec3> getPositions();
        virtual std::vector<glm::vec2> getUVs();
        virtual std::vector<glm::vec3> getNormals();
        virtual std::vector<glm::vec3> getTangents();
        virtual std::vector<glm::vec3> getBitangents();
        virtual std::vector<unsigned int> getIndices();
    protected:
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;
        std::vector<unsigned int> indices;
    };
}

#endif //DREAM_MESH_H
