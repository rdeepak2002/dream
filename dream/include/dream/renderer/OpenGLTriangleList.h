//
// Created by Deepak Ramalingam on 2/18/23.
//

#ifndef DREAM_OPENGLTRIANGLELIST_H
#define DREAM_OPENGLTRIANGLELIST_H

#include <vector>
#include "glm/vec3.hpp"
#include "Mesh.h"

namespace Dream {
    class OpenGLBaseTerrain;

    class OpenGLTriangleList {
    public:
        OpenGLTriangleList();

        void createTriangleList(int width, int depth, const OpenGLBaseTerrain* pTerrain);

        void render();

    private:
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

            void initVertex(const OpenGLBaseTerrain* pTerrain, int x, int z);
        };

        void createGLState();

        void populateBuffers(const OpenGLBaseTerrain* pTerrain);
        void initVertices(const OpenGLBaseTerrain* pTerrain, std::vector<Vertex>& Vertices);
        void initIndices(std::vector<unsigned int>& Indices);

        int m_width = 0;
        int m_depth = 0;
        unsigned int m_vao, m_vb, m_ib;
    };
}

#endif //DREAM_OPENGLTRIANGLELIST_H
