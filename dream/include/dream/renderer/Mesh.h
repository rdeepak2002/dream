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
        // position
        glm::vec3 Position;
        // texCoords
        glm::vec2 TexCoords;
        // normal
        glm::vec3 Normal;
        // tangent
        glm::vec3 Tangent;
        // bitangent
        glm::vec3 Bitangent;
        //bone indexes which will influence this vertex
        int m_BoneIDs[MAX_BONE_INFLUENCE];
        //weights from each bone
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
