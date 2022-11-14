//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/project/OpenGLAssetImporter.h"

#include <iostream>
#include <glad/glad.h>
#include "dream/renderer/OpenGLMesh.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    Entity OpenGLAssetImporter::importMesh(std::string path) {
        // use assimp to get scene of model
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            exit(EXIT_FAILURE);
        }
        // process root node
        auto node = scene->mRootNode;
        Entity dreamEntityRootNode = processNode(node, scene);
        return dreamEntityRootNode;
    }

    Entity OpenGLAssetImporter::processNode(aiNode *node, const aiScene *scene) {
        Entity dreamNode = Project::getInstance().getScene().createEntity(node->mName.C_Str());
        // process meshes for this node
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            Entity child = processMesh(mesh, scene);
            dreamNode.addChild(child);
        }
        // process child nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            Entity child = processNode(node->mChildren[i], scene);
            dreamNode.addChild(child);
        }
        return dreamNode;
    }

    Entity OpenGLAssetImporter::processMesh(aiMesh *mesh, const aiScene *scene) {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uv;
        std::vector<unsigned int> indices;

        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            glm::vec3 vector;
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            positions.push_back(vector);
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                normals.push_back(vector);
            }
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                uv.push_back(vec);
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
                uv.emplace_back(0.0f, 0.0f);
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

        auto* dreamMesh = new OpenGLMesh(positions, uv, normals, indices);
        Entity entity = Project::getInstance().getScene().createEntity(mesh->mName.C_Str());
        entity.addComponent<Component::MeshComponent>(dreamMesh);
        return entity;
    }
}
