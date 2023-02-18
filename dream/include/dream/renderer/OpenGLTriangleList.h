//
// Created by Deepak Ramalingam on 2/18/23.
//

#ifndef DREAM_OPENGLTRIANGLELIST_H
#define DREAM_OPENGLTRIANGLELIST_H

#include <vector>
#include "glm/vec3.hpp"

namespace Dream {
    class OpenGLBaseTerrain;

    class OpenGLTriangleList {
    public:
        OpenGLTriangleList();

        void createTriangleList(int width, int depth, const OpenGLBaseTerrain* pTerrain);

        void render();

    private:

        struct Vertex {
            glm::vec3 pos;

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
