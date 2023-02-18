//
// Created by Deepak Ramalingam on 2/18/23.
//

#include "dream/renderer/OpenGLTriangleList.h"
#include "dream/renderer/OpenGLBaseTerrain.h"
#include "dream/renderer/OpenGLRenderer.h"

namespace Dream {

    OpenGLTriangleList::OpenGLTriangleList() {

    }

    void OpenGLTriangleList::createTriangleList(int width, int depth, const OpenGLBaseTerrain *pTerrain) {
        m_width = width;
        m_depth = depth;

        createGLState();

        populateBuffers(pTerrain);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void OpenGLTriangleList::render() {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, (m_depth - 1) * (m_width - 1) * 6, GL_UNSIGNED_INT, NULL);
        glBindVertexArray(0);
    }

    void OpenGLTriangleList::createGLState() {
        glGenVertexArrays(1, &m_vao);

        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vb);

        glBindBuffer(GL_ARRAY_BUFFER, m_vb);

        glGenBuffers(1, &m_ib);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);

        int POS_LOC = 0;

        size_t NumFloats = 0;

        glEnableVertexAttribArray(POS_LOC);
        glVertexAttribPointer(POS_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(NumFloats * sizeof(float)));
        NumFloats += 3;
    }

    void OpenGLTriangleList::populateBuffers(const OpenGLBaseTerrain *pTerrain) {
        std::vector<Vertex> Vertices;
        Vertices.resize(m_width * m_depth);

        initVertices(pTerrain, Vertices);

        std::vector<unsigned int> Indices;
        int NumQuads = (m_width - 1) * (m_depth - 1);
        Indices.resize(NumQuads * 6);
        initIndices(Indices);

        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
    }

    void OpenGLTriangleList::initVertices(const OpenGLBaseTerrain *pTerrain, std::vector<Vertex> &Vertices) {
        int Index = 0;

        for (int z = 0; z < m_depth; z++) {
            for (int x = 0; x < m_width; x++) {
                assert(Index < Vertices.size());
                Vertices[Index].initVertex(pTerrain, x, z);
                Index++;
            }
        }

        assert(Index == Vertices.size());
    }

    void OpenGLTriangleList::initIndices(std::vector<uint> &Indices) {
        int Index = 0;

        for (int z = 0; z < m_depth - 1; z++) {
            for (int x = 0; x < m_width - 1; x++) {
                unsigned int IndexBottomLeft = z * m_width + x;
                unsigned int IndexTopLeft = (z + 1) * m_width + x;
                unsigned int IndexTopRight = (z + 1) * m_width + x + 1;
                unsigned int IndexBottomRight = z * m_width + x + 1;

                // Add top left triangle
                assert(Index < Indices.size());
                Indices[Index++] = IndexBottomLeft;
                assert(Index < Indices.size());
                Indices[Index++] = IndexTopLeft;
                assert(Index < Indices.size());
                Indices[Index++] = IndexTopRight;

                // Add bottom right triangle
                assert(Index < Indices.size());
                Indices[Index++] = IndexBottomLeft;
                assert(Index < Indices.size());
                Indices[Index++] = IndexTopRight;
                assert(Index < Indices.size());
                Indices[Index++] = IndexBottomRight;
            }
        }

        assert(Index == Indices.size());
    }

    void OpenGLTriangleList::Vertex::initVertex(const OpenGLBaseTerrain *pTerrain, int x, int z) {
        float y = pTerrain->getHeight(x, z);

        float WorldScale = pTerrain->getWorldScale();

        pos = { x * WorldScale, y, z * WorldScale };
    }
}