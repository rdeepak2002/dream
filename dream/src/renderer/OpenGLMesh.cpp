//
// Created by Deepak Ramalingam on 11/10/22.
//

#include "dream/renderer/OpenGLMesh.h"

#include <glad/glad.h>
#include <utility>

namespace Dream {
    OpenGLMesh::OpenGLMesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uv, std::vector<glm::vec3> normals, std::vector<unsigned int> indices) {
        setPositions(std::move(positions));
        setUVs(std::move(uv));
        setNormals(std::move(normals));
        setIndices(std::move(indices));
        finalize();
    }

    void OpenGLMesh::finalize(bool interleaved) {
        // initialize object IDs if not configured before
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        // preprocess buffer data as interleaved or seperate when specified
        std::vector<float> data;
        if (interleaved)
        {
            for (int i = 0; i < positions.size(); ++i)
            {
                data.push_back(positions[i].x);
                data.push_back(positions[i].y);
                data.push_back(positions[i].z);
                if (!uv.empty()) {
                    data.push_back(uv[i].x);
                    data.push_back(uv[i].y);
                }
                if (!normals.empty()) {
                    data.push_back(normals[i].x);
                    data.push_back(normals[i].y);
                    data.push_back(normals[i].z);
                }
                if (!tangents.empty()) {
                    data.push_back(tangents[i].x);
                    data.push_back(tangents[i].y);
                    data.push_back(tangents[i].z);
                }
                if (!bitangents.empty()) {
                    data.push_back(bitangents[i].x);
                    data.push_back(bitangents[i].y);
                    data.push_back(bitangents[i].z);
                }
            }
        }
        else {
            for (auto & position : positions) {
                data.push_back(position.x);
                data.push_back(position.y);
                data.push_back(position.z);
            }

            for (auto & i : uv) {
                data.push_back(i.x);
                data.push_back(i.y);
            }

            for (auto & normal : normals) {
                data.push_back(normal.x);
                data.push_back(normal.y);
                data.push_back(normal.z);
            }

            for (auto & tangent : tangents) {
                data.push_back(tangent.x);
                data.push_back(tangent.y);
                data.push_back(tangent.z);
            }

            for (auto & bitangent : bitangents) {
                data.push_back(bitangent.x);
                data.push_back(bitangent.y);
                data.push_back(bitangent.z);
            }
        }

        // configure vertex attributes (only on vertex data size() > 0)
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        // only fill the index buffer if the index array is non-empty.
        if (indices.size() > 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        }
        if (interleaved) {
            // calculate stride from number of non-empty vertex attribute arrays
            size_t stride = 3 * sizeof(float);

            if (!uv.empty()) {
                stride += 2 * sizeof(float);
            }

            if (!normals.empty()) {
                stride += 3 * sizeof(float);
            }

            if (!tangents.empty()) {
                stride += 3 * sizeof(float);
            }

            if (!bitangents.empty()) {
                stride += 3 * sizeof(float);
            }

            size_t offset = 0;
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
            offset += 3 * sizeof(float);
            if (!uv.empty()) {
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
                offset += 2 * sizeof(float);
            }
            if (!normals.empty()) {
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
                offset += 3 * sizeof(float);
            }
            if (!tangents.empty()) {
                glEnableVertexAttribArray(3);
                glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
                offset += 3 * sizeof(float);
            }
            if (!bitangents.empty()) {
                glEnableVertexAttribArray(4);
                glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
                offset += 3 * sizeof(float);
            }
        }
        else {
            size_t offset = 0;
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
            offset += positions.size() * sizeof(float);
            if (!uv.empty()) {
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
                offset += uv.size() * sizeof(float);
            }
            if (!normals.empty()) {
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
                offset += normals.size() * sizeof(float);
            }
            if (!tangents.empty()) {
                glEnableVertexAttribArray(3);
                glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
                offset += tangents.size() * sizeof(float);
            }
            if (!bitangents.empty()) {
                glEnableVertexAttribArray(4);
                glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
                offset += bitangents.size() * sizeof(float);
            }
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

    OpenGLMesh::OpenGLMesh() {

    }
}