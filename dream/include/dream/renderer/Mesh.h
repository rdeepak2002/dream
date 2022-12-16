//
// Created by Deepak Ramalingam on 11/13/22.
//

#ifndef DREAM_MESH_H
#define DREAM_MESH_H

#include <vector>
#include <functional>
#include <glm/glm.hpp>

#define MAX_BONES 200
#define MAX_BONE_INFLUENCE 4

namespace Dream {
    struct Vertex {
        glm::vec3 position;
        glm::vec2 uv;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec3 bitangent;
        // bones that influence this vertex
        int boneIDs[MAX_BONE_INFLUENCE];
        // effect of each bone on this vertex
        float boneWeights[MAX_BONE_INFLUENCE];
    };
    class Mesh {
    public:
        void setIndices(std::vector<unsigned int> indices);
        void setVertices(std::vector<Vertex> vertices);
        virtual std::vector<unsigned int> getIndices();
        virtual std::vector<Vertex> getVertices();
    protected:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    };
}

#endif //DREAM_MESH_H
