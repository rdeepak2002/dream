//
// Created by Deepak Ramalingam on 11/10/22.
//

#include "dream/renderer/OpenGLMesh.h"

#include <iostream>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

namespace Dream {
    OpenGLMesh::OpenGLMesh() {

    }

    OpenGLMesh::OpenGLMesh(std::string path) {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        std::cout << "Root node name: " << scene->mRootNode->mName.C_Str() << std::endl;

        auto node = scene->mRootNode;

        // process each child at current node
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            std::cout << "Root node child name: " << node->mChildren[i]->mName.C_Str() << std::endl;

            // TODO: this will be called in the second depth of recursive tree normally
            auto childNode = node->mChildren[i];
            std::cout << childNode->mNumMeshes << std::endl;

            // TODO: bad assumption there is one mesh at child
            auto mesh = scene->mMeshes[childNode->mMeshes[0]];

            std::cout << "adding mesh: " << mesh->mName.C_Str() << std::endl;

            // TODO: move to process mesh function
            // data to fill
//            vector<Vertex> vertices;
//            std::vector<unsigned int> indices;

            std::cout << "num vertices: " << mesh->mNumVertices << std::endl;

            // walk through each of the mesh's vertices
            for(unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
                // positions
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                this->positions.push_back(vector);
                // normals
                if (mesh->HasNormals())
                {
                    vector.x = mesh->mNormals[i].x;
                    vector.y = mesh->mNormals[i].y;
                    vector.z = mesh->mNormals[i].z;
                    this->normals.push_back(vector);
                }
                // texture coordinates
                if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
                {
                    glm::vec2 vec;
                    // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                    // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    this->uv.push_back(vec);
                    // tangent
                    vector.x = mesh->mTangents[i].x;
                    vector.y = mesh->mTangents[i].y;
                    vector.z = mesh->mTangents[i].z;
                    // TODO: add tangents
//                    vertex.Tangent = vector;
                    // bitangent
                    vector.x = mesh->mBitangents[i].x;
                    vector.y = mesh->mBitangents[i].y;
                    vector.z = mesh->mBitangents[i].z;
                    // TODO: add bitangents
//                    vertex.Bitangent = vector;
                } else {
                    this->uv.emplace_back(0.0f, 0.0f);
                }
            }
            // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
            for(unsigned int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                // retrieve all indices of the face and store them in the indices vector
                for(unsigned int j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }
            // TODO: process materials and textures
        }

        // process each mesh at the current node
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene.
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            std::cout << "Root node child mesh name: " << mesh->mName.C_Str() << std::endl;
        }

        this->finalize();
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
}