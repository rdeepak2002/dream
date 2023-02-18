//
// Created by Deepak Ramalingam on 2/18/23.
//

#ifndef DREAM_OPENGLTRIANGLELIST_H
#define DREAM_OPENGLTRIANGLELIST_H

#include "dream/renderer/OpenGLRenderer.h"
#include <vector>

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
        void initIndices(std::vector<uint>& Indices);

        int m_width = 0;
        int m_depth = 0;
        GLuint m_vao;
        GLuint m_vb;
        GLuint m_ib;
    };
}

#endif //DREAM_OPENGLTRIANGLELIST_H
