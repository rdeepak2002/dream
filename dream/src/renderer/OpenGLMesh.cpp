//
// Created by Deepak Ramalingam on 11/10/22.
//

#include "dream/renderer/OpenGLMesh.h"
#include "dream/util/Logger.h"

#include <glad/glad.h>
#include <utility>
#include <iostream>

namespace Dream {
    OpenGLMesh::OpenGLMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
        setVertices(std::move(vertices));
        setIndices(std::move(indices));
        finalize();
    }

    OpenGLMesh::OpenGLMesh() {
    }

    void OpenGLMesh::finalize(bool interleaved) {
        // initialize object IDs if not configured before
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        // preprocess buffer data as interleaved or separate when specified
//        std::vector<Vertex> data;
//        if (interleaved)
//        {
//            data = vertices;
//        }
//        else {
//            Logger::fatal("Not interleaved not supported")
//        }

        // configure vertex attributes (only on vertex data size() > 0)
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        // only fill the index buffer if the index array is non-empty.
        if (indices.size() > 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        }
        if (interleaved) {
            // calculate stride from number of non-empty vertex attribute arrays
            size_t stride = 3 * sizeof(float);  // positions
            stride += 2 * sizeof(float);        // uvs
            stride += 3 * sizeof(float);        // normals
            stride += 3 * sizeof(float);        // tangents
            stride += 3 * sizeof(float);        // bitangents
            stride += MAX_BONE_INFLUENCE * sizeof(int);          // for bone ids
            stride += MAX_BONE_INFLUENCE * sizeof(float);        // for weights

            // positions
            size_t offset = 0;
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
            offset += 3 * sizeof(float);

            // uvs
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
            offset += 2 * sizeof(float);

            // normals
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
            offset += 3 * sizeof(float);

            // tangents
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
            offset += 3 * sizeof(float);

            // bitangents
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
            offset += 3 * sizeof(float);

            // bone ids
            glEnableVertexAttribArray(5);
            glVertexAttribIPointer(5, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
            offset += offsetof(Vertex, boneIDs);

            // weights
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneWeights));
            offset += offsetof(Vertex, boneWeights);
        }
        else {
            Logger::fatal("Not interleaved not supported");
        }
        glBindVertexArray(0);
    }

    unsigned int OpenGLMesh::getVAO() {
        return vao;
    }

    unsigned int OpenGLMesh::getVBO() {
        return vbo;
    }

    unsigned int OpenGLMesh::getEBO() {
        return ebo;
    }
}