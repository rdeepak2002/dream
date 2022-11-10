//
// Created by Deepak Ramalingam on 11/10/22.
//

#include "dream/renderer/OpenGLCubeMesh.h"

#include <glad/glad.h>
#include <iostream>

namespace Dream {
    OpenGLCubeMesh::OpenGLCubeMesh() {
        Positions = std::vector<glm::vec3> {
                glm::vec3(-0.5f, -0.5f, -0.5f),
                glm::vec3( 0.5f,  0.5f, -0.5f),
                glm::vec3( 0.5f, -0.5f, -0.5f),
                glm::vec3( 0.5f,  0.5f, -0.5f),
                glm::vec3(-0.5f, -0.5f, -0.5f),
                glm::vec3(-0.5f,  0.5f, -0.5f),

                glm::vec3(-0.5f, -0.5f,  0.5f),
                glm::vec3( 0.5f, -0.5f,  0.5f),
                glm::vec3( 0.5f,  0.5f,  0.5f),
                glm::vec3( 0.5f,  0.5f,  0.5f),
                glm::vec3(-0.5f,  0.5f,  0.5f),
                glm::vec3(-0.5f, -0.5f,  0.5f),

                glm::vec3(-0.5f,  0.5f,  0.5f),
                glm::vec3(-0.5f,  0.5f, -0.5f),
                glm::vec3(-0.5f, -0.5f, -0.5f),
                glm::vec3(-0.5f, -0.5f, -0.5f),
                glm::vec3(-0.5f, -0.5f,  0.5f),
                glm::vec3(-0.5f,  0.5f,  0.5f),

                glm::vec3( 0.5f,  0.5f,  0.5f),
                glm::vec3( 0.5f, -0.5f, -0.5f),
                glm::vec3( 0.5f,  0.5f, -0.5f),
                glm::vec3( 0.5f, -0.5f, -0.5f),
                glm::vec3( 0.5f,  0.5f,  0.5f),
                glm::vec3( 0.5f, -0.5f,  0.5f),

                glm::vec3(-0.5f, -0.5f, -0.5f),
                glm::vec3( 0.5f, -0.5f, -0.5f),
                glm::vec3( 0.5f, -0.5f,  0.5f),
                glm::vec3( 0.5f, -0.5f,  0.5f),
                glm::vec3(-0.5f, -0.5f,  0.5f),
                glm::vec3(-0.5f, -0.5f, -0.5f),

                glm::vec3(-0.5f,  0.5f, -0.5f),
                glm::vec3( 0.5f,  0.5f,  0.5f),
                glm::vec3( 0.5f,  0.5f, -0.5f),
                glm::vec3( 0.5f,  0.5f,  0.5f),
                glm::vec3(-0.5f,  0.5f, -0.5f),
                glm::vec3(-0.5f,  0.5f,  0.5f),
        };

        UV = std::vector<glm::vec2> {
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(0.0f, 0.0f),
                glm::vec2(0.0f, 1.0f),

                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(0.0f, 0.0f),

                glm::vec2(1.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 0.0f),

                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.0f, 0.0f),

                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.0f, 0.0f),
                glm::vec2(0.0f, 1.0f),

                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(0.0f, 0.0f),
        };
        Normals = std::vector<glm::vec3>{
                glm::vec3( 0.0f,  0.0f, -1.0f),
                glm::vec3( 0.0f,  0.0f, -1.0f),
                glm::vec3( 0.0f,  0.0f, -1.0f),
                glm::vec3( 0.0f,  0.0f, -1.0f),
                glm::vec3( 0.0f,  0.0f, -1.0f),
                glm::vec3( 0.0f,  0.0f, -1.0f),

                glm::vec3( 0.0f,  0.0f,  1.0f),
                glm::vec3( 0.0f,  0.0f,  1.0f),
                glm::vec3( 0.0f,  0.0f,  1.0f),
                glm::vec3( 0.0f,  0.0f,  1.0f),
                glm::vec3( 0.0f,  0.0f,  1.0f),
                glm::vec3( 0.0f,  0.0f,  1.0f),

                glm::vec3(-1.0f,  0.0f,  0.0f),
                glm::vec3(-1.0f,  0.0f,  0.0f),
                glm::vec3(-1.0f,  0.0f,  0.0f),
                glm::vec3(-1.0f,  0.0f,  0.0f),
                glm::vec3(-1.0f,  0.0f,  0.0f),
                glm::vec3(-1.0f,  0.0f,  0.0f),

                glm::vec3( 1.0f,  0.0f,  0.0f),
                glm::vec3( 1.0f,  0.0f,  0.0f),
                glm::vec3( 1.0f,  0.0f,  0.0f),
                glm::vec3( 1.0f,  0.0f,  0.0f),
                glm::vec3( 1.0f,  0.0f,  0.0f),
                glm::vec3( 1.0f,  0.0f,  0.0f),

                glm::vec3( 0.0f, -1.0f,  0.0f),
                glm::vec3( 0.0f, -1.0f,  0.0f),
                glm::vec3( 0.0f, -1.0f,  0.0f),
                glm::vec3( 0.0f, -1.0f,  0.0f),
                glm::vec3( 0.0f, -1.0f,  0.0f),
                glm::vec3( 0.0f, -1.0f,  0.0f),

                glm::vec3( 0.0f,  1.0f,  0.0f),
                glm::vec3( 0.0f,  1.0f,  0.0f),
                glm::vec3( 0.0f,  1.0f,  0.0f),
                glm::vec3( 0.0f,  1.0f,  0.0f),
                glm::vec3( 0.0f,  1.0f,  0.0f),
                glm::vec3( 0.0f,  1.0f,  0.0f),
        };

        this->Finalize();
    }

    void OpenGLCubeMesh::Finalize(bool interleaved) {
        // initialize object IDs if not configured before
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        // preprocess buffer data as interleaved or seperate when specified
        std::vector<float> data;
        if (interleaved)
        {
            for (int i = 0; i < Positions.size(); ++i)
            {
                data.push_back(Positions[i].x);
                data.push_back(Positions[i].y);
                data.push_back(Positions[i].z);
                if (UV.size() > 0)
                {
                    data.push_back(UV[i].x);
                    data.push_back(UV[i].y);
                }
                if (Normals.size() > 0)
                {
                    data.push_back(Normals[i].x);
                    data.push_back(Normals[i].y);
                    data.push_back(Normals[i].z);
                }
//            if (Tangents.size() > 0)
//            {
//                data.push_back(Tangents[i].x);
//                data.push_back(Tangents[i].y);
//                data.push_back(Tangents[i].z);
//            }
//            if (Bitangents.size() > 0)
//            {
//                data.push_back(Bitangents[i].x);
//                data.push_back(Bitangents[i].y);
//                data.push_back(Bitangents[i].z);
//            }
            }
        }
        else
        {
            // if any of the float arrays are empty, data won't be filled by them.
            for (int i = 0; i < Positions.size(); ++i)
            {
                data.push_back(Positions[i].x);
                data.push_back(Positions[i].y);
                data.push_back(Positions[i].z);
            }
            for (int i = 0; i < UV.size(); ++i)
            {
                data.push_back(UV[i].x);
                data.push_back(UV[i].y);
            }
            for (int i = 0; i < Normals.size(); ++i)
            {
                data.push_back(Normals[i].x);
                data.push_back(Normals[i].y);
                data.push_back(Normals[i].z);
            }
//        for (int i = 0; i < Tangents.size(); ++i)
//        {
//            data.push_back(Tangents[i].x);
//            data.push_back(Tangents[i].y);
//            data.push_back(Tangents[i].z);
//        }
//        for (int i = 0; i < Bitangents.size(); ++i)
//        {
//            data.push_back(Bitangents[i].x);
//            data.push_back(Bitangents[i].y);
//            data.push_back(Bitangents[i].z);
//        }
        }

        // configure vertex attributes (only on vertex data size() > 0)
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        // only fill the index buffer if the index array is non-empty.
        if (Indices.size() > 0)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);
        }
        if (interleaved)
        {
            // calculate stride from number of non-empty vertex attribute arrays
            size_t stride                      = 3 * sizeof(float);
            if (UV.size() > 0)         stride += 2 * sizeof(float);
            if (Normals.size() > 0)    stride += 3 * sizeof(float);
//        if (Tangents.size() > 0)   stride += 3 * sizeof(float);
//        if (Bitangents.size() > 0) stride += 3 * sizeof(float);

            size_t offset = 0;
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
            offset += 3 * sizeof(float);
            if (UV.size() > 0)
            {
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
                offset += 2 * sizeof(float);
            }
            if (Normals.size() > 0)
            {
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
                offset += 3 * sizeof(float);
            }
//        if (Tangents.size() > 0)
//        {
//            glEnableVertexAttribArray(3);
//            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
//            offset += 3 * sizeof(float);
//        }
//        if (Bitangents.size() > 0)
//        {
//            glEnableVertexAttribArray(4);
//            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
//            offset += 3 * sizeof(float);
//        }
        }
        else
        {
            size_t offset = 0;
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
            offset += Positions.size() * sizeof(float);
            if (UV.size() > 0)
            {
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
                offset += UV.size() * sizeof(float);
            }
            if (Normals.size() > 0)
            {
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
                offset += Normals.size() * sizeof(float);
            }
//        if (Tangents.size() > 0)
//        {
//            glEnableVertexAttribArray(3);
//            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
//            offset += Tangents.size() * sizeof(float);
//        }
//        if (Bitangents.size() > 0)
//        {
//            glEnableVertexAttribArray(4);
//            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offset);
//            offset += Bitangents.size() * sizeof(float);
//        }
        }
        glBindVertexArray(0);
    }
}
