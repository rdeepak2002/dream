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
        glm::vec3 Position;
        glm::vec2 TexCoords;
        glm::vec3 Normal;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
        // bones that influence this vertex
        int m_BoneIDs[MAX_BONE_INFLUENCE];
        // effect of each bone on this vertex
        float m_Weights[MAX_BONE_INFLUENCE];
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
