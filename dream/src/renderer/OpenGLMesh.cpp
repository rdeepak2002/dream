//
// Created by Deepak Ramalingam on 11/10/22.
//

#include "dream/renderer/OpenGLMesh.h"

#include <iostream>
#include <glad/glad.h>

namespace Dream {
    OpenGLMesh::OpenGLMesh()
    {

    }

//    OpenGLMesh::OpenGLMesh(std::vector<glm::vec3> positions, std::vector<unsigned int> indices)
//    {
//        Positions = positions;
//        Indices = indices;
//    }
//
//    OpenGLMesh::OpenGLMesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uv, std::vector<unsigned int> indices)
//    {
//        Positions = positions;
//        UV = uv;
//        Indices = indices;
//    }
//    // --------------------------------------------------------------------------------------------
//    OpenGLMesh::OpenGLMesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uv, std::vector<glm::vec3> normals, std::vector<unsigned int> indices)
//    {
//        Positions = positions;
//        UV = uv;
//        Normals = normals;
//        Indices = indices;
//    }
//
//    OpenGLMesh::OpenGLMesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> uv, std::vector<glm::vec3> normals, std::vector<glm::vec3> tangents, std::vector<glm::vec3> bitangents, std::vector<unsigned int> indices)
//    {
//        Positions = positions;
//        UV = uv;
//        Normals = normals;
//        Tangents = tangents;
//        Bitangents = bitangents;
//        Indices = indices;
//    }

    void OpenGLMesh::SetPositions(std::vector<glm::vec3> positions)
    {
        Positions = positions;
    }

    void OpenGLMesh::SetUVs(std::vector<glm::vec2> uv)
    {
        UV = uv;
    }

    void OpenGLMesh::SetNormals(std::vector<glm::vec3> normals)
    {
        Normals = normals;
    }

//    void OpenGLMesh::SetTangents(std::vector<glm::vec3> tangents, std::vector<glm::vec3> bitangents)
//    {
//        Tangents = tangents;
//        Bitangents = bitangents;
//    }

    void OpenGLMesh::Finalize(bool interleaved)
    {
//        // initialize object IDs if not configured before
//        if (!m_VAO)
//        {
//            glGenVertexArrays(1, &m_VAO);
//            glGenBuffers(1, &m_VBO);
//            glGenBuffers(1, &m_EBO);
//        }
//
//        // preprocess buffer data as interleaved or seperate when specified
//        std::vector<float> data;
//        if (interleaved)
//        {
//            for (int i = 0; i < Positions.size(); ++i)
//            {
//                data.push_back(Positions[i].x);
//                data.push_back(Positions[i].y);
//                data.push_back(Positions[i].z);
//                if (UV.size() > 0)
//                {
//                    data.push_back(UV[i].x);
//                    data.push_back(UV[i].y);
//                }
//                if (Normals.size() > 0)
//                {
//                    data.push_back(Normals[i].x);
//                    data.push_back(Normals[i].y);
//                    data.push_back(Normals[i].z);
//                }
//                if (Tangents.size() > 0)
//                {
//                    data.push_back(Tangents[i].x);
//                    data.push_back(Tangents[i].y);
//                    data.push_back(Tangents[i].z);
//                }
//                if (Bitangents.size() > 0)
//                {
//                    data.push_back(Bitangents[i].x);
//                    data.push_back(Bitangents[i].y);
//                    data.push_back(Bitangents[i].z);
//                }
//            }
//        }
//        else
//        {
//            // if any of the float arrays are empty, data won't be filled by them.
//            for (int i = 0; i < Positions.size(); ++i)
//            {
//                data.push_back(Positions[i].x);
//                data.push_back(Positions[i].y);
//                data.push_back(Positions[i].z);
//            }
//            for (int i = 0; i < UV.size(); ++i)
//            {
//                data.push_back(UV[i].x);
//                data.push_back(UV[i].y);
//            }
//            for (int i = 0; i < Normals.size(); ++i)
//            {
//                data.push_back(Normals[i].x);
//                data.push_back(Normals[i].y);
//                data.push_back(Normals[i].z);
//            }
//            for (int i = 0; i < Tangents.size(); ++i)
//            {
//                data.push_back(Tangents[i].x);
//                data.push_back(Tangents[i].y);
//                data.push_back(Tangents[i].z);
//            }
//            for (int i = 0; i < Bitangents.size(); ++i)
//            {
//                data.push_back(Bitangents[i].x);
//                data.push_back(Bitangents[i].y);
//                data.push_back(Bitangents[i].z);
//            }
//        }
//
//        // configure vertex attributes (only on vertex data size() > 0)
//        glBindVertexArray(m_VAO);
//        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
//        // only fill the index buffer if the index array is non-empty.
//        if (Indices.size() > 0)
//        {
//            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
//            glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);
//        }
//        if (interleaved)
//        {
//            // calculate stride from number of non-empty vertex attribute arrays
//            size_t stride                      = 3 * sizeof(float);
//            if (UV.size() > 0)         stride += 2 * sizeof(float);
//            if (Normals.size() > 0)    stride += 3 * sizeof(float);
//            if (Tangents.size() > 0)   stride += 3 * sizeof(float);
//            if (Bitangents.size() > 0) stride += 3 * sizeof(float);
//
//            size_t offset = 0;
//            glEnableVertexAttribArray(0);
//            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
//            offset += 3 * sizeof(float);
//            if (UV.size() > 0)
//            {
//                glEnableVertexAttribArray(1);
//                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
//                offset += 2 * sizeof(float);
//            }
//            if (Normals.size() > 0)
//            {
//                glEnableVertexAttribArray(2);
//                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
//                offset += 3 * sizeof(float);
//            }
//            if (Tangents.size() > 0)
//            {
//                glEnableVertexAttribArray(3);
//                glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
//                offset += 3 * sizeof(float);
//            }
//            if (Bitangents.size() > 0)
//            {
//                glEnableVertexAttribArray(4);
//                glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
//                offset += 3 * sizeof(float);
//            }
//        }
//        else
//        {
//            size_t offset = 0;
//            glEnableVertexAttribArray(0);
//            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
//            offset += Positions.size() * sizeof(float);
//            if (UV.size() > 0)
//            {
//                glEnableVertexAttribArray(1);
//                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
//                offset += UV.size() * sizeof(float);
//            }
//            if (Normals.size() > 0)
//            {
//                glEnableVertexAttribArray(2);
//                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
//                offset += Normals.size() * sizeof(float);
//            }
//            if (Tangents.size() > 0)
//            {
//                glEnableVertexAttribArray(3);
//                glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
//                offset += Tangents.size() * sizeof(float);
//            }
//            if (Bitangents.size() > 0)
//            {
//                glEnableVertexAttribArray(4);
//                glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
//                offset += Bitangents.size() * sizeof(float);
//            }
//        }
//        glBindVertexArray(0);
    }
}