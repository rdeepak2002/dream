//
// Created by Deepak Ramalingam on 11/10/22.
//

#include "dream/renderer/OpenGLMesh.h"

#include <glad/glad.h>
#include <utility>
#include <iostream>

namespace Dream {
    OpenGLMesh::OpenGLMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, bool hasSkeleton) {
//        setPositions(std::move(positions));
//        setUVs(std::move(uv));
//        setNormals(std::move(normals));

        setVertices(std::move(vertices));
        setIndices(std::move(indices));
//        setTangents(std::move(tangents));
//        setBitangents(std::move(bitangents));
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            boneIds[i] = -1;
            weights[i] = 0;
        }
        this->hasSkeleton = hasSkeleton;
        finalize();
    }

    OpenGLMesh::OpenGLMesh() {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            boneIds[i] = -1;
            weights[i] = 0;
        }
        this->hasSkeleton = false;
    }

    void OpenGLMesh::finalize(bool interleaved) {
        // initialize object IDs if not configured before
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        // preprocess buffer data as interleaved or separate when specified
        std::vector<Vertex> data;
        if (interleaved)
        {
            data = vertices;
//            for (int i = 0; i < positions.size(); ++i)
//            {
//                Vertex vertex = {};
//                if (!positions.empty()) {
//                    vertex.Position =  positions[i];
//                } else {
//                    std::cout << "No positions provided for mesh" << std::endl;
//                    exit(EXIT_FAILURE);
//                }
//                if (!uv.empty()) {
//                    vertex.TexCoords = uv[i];
//                } else {
//                    std::cout << "No uv provided for mesh" << std::endl;
//                    exit(EXIT_FAILURE);
//                }
//                if (!normals.empty()) {
//                    vertex.Normal = normals[i];
//                } else {
//                    std::cout << "No normal provided for mesh" << std::endl;
//                    exit(EXIT_FAILURE);
//                }
//                if (!tangents.empty()) {
//                    vertex.Tangent = tangents[i];
//                } else {
//                    vertex.Tangent = {0, 0, 0};
//                }
//                if (!bitangents.empty()) {
//                    vertex.Bitangent = bitangents[i];
//                } else {
//                    vertex.Bitangent = {0, 0, 0};
//                }
//                for (int j = 0; j < MAX_BONE_INFLUENCE; ++j) {
//                    vertex.m_BoneIDs[j] = boneIds[j];
//                    vertex.m_Weights[j] = weights[j];
//                }
//                data.push_back(vertex);
//            }
        }
        else {
            std::cout << "Not interleaved not supported" << std::endl;
            exit(EXIT_FAILURE);
        }

        // configure vertex attributes (only on vertex data size() > 0)
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Vertex), &data[0], GL_STATIC_DRAW);
        // only fill the index buffer if the index array is non-empty.
        if (indices.size() > 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        }
        if (interleaved) {
            // calculate stride from number of non-empty vertex attribute arrays
            size_t stride = 3 * sizeof(float);  // positions
            stride += 2 * sizeof(float);        // uvs
            stride += 3 * sizeof(float);        // normals
            stride += 3 * sizeof(float);        // tangents
            stride += 3 * sizeof(float);        // bitangents
            stride += 4 * sizeof(int);          // for bone ids
            stride += 4 * sizeof(float);        // for weights

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
            glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
            offset += offsetof(Vertex, m_BoneIDs);

            // weights
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
            offset += offsetof(Vertex, m_Weights);
        }
        else {
            std::cout << "Not interleaved not supported" << std::endl;
            exit(EXIT_FAILURE);
        }
        glBindVertexArray(0);
    }

    unsigned int OpenGLMesh::getVAO() {
        return m_VAO;
    }

    unsigned int OpenGLMesh::getVBO() {
        return m_VBO;
    }

    unsigned int OpenGLMesh::getEBO() {
        return m_EBO;
    }
}