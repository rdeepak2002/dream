//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/project/OpenGLAssetImporter.h"

#include <iostream>
#include <filesystem>
#include "dream/renderer/OpenGLMesh.h"
#include "dream/renderer/OpenGLTexture.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    Entity OpenGLAssetImporter::importMesh(std::string path) {
        if (!std::filesystem::exists(path)) {
            std::cout << "File does not exist" << std::endl;
            exit(EXIT_FAILURE);
        }
        // use assimp to get scene of model
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
//        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return Entity();
        }
        // process root node
        auto node = scene->mRootNode;
        Entity dreamEntityRootNode = processNode(path, node, scene);
        return dreamEntityRootNode;
    }

    Entity OpenGLAssetImporter::processNode(std::string path, aiNode *node, const aiScene *scene) {
        Entity dreamNode = Project::getScene().createEntity(node->mName.C_Str());
        // process meshes for this node
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            Entity child = processMesh(path, mesh, scene);
            dreamNode.addChild(child);
        }
        // process child nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            Entity child = processNode(path, node->mChildren[i], scene);
            dreamNode.addChild(child);
        }
        return dreamNode;
    }

    Entity OpenGLAssetImporter::processMesh(std::string path, aiMesh *mesh, const aiScene *scene) {
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
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // TODO: optimize texture loading since right now we are loading same textures multiple times
        auto textureType = aiTextureType_DIFFUSE;
        std::string texturePath = "";
        for(unsigned int i = 0; i < material->GetTextureCount(textureType); i++) {
            aiString str;
            material->GetTexture(textureType, i, &str);
            texturePath = std::filesystem::path(path).parent_path().append(str.C_Str());
            std::cout << "TODO: optimize texture loading memory usage (same textures are repeatedly loaded)" << std::endl;
        }

        auto* dreamMesh = new OpenGLMesh(positions, uv, normals, indices);
        Entity entity = Project::getScene().createEntity(mesh->mName.C_Str());
        entity.addComponent<Component::MeshComponent>(dreamMesh);
        if (!texturePath.empty()) {
            auto* dreamTexture = new OpenGLTexture(texturePath);
            entity.addComponent<Component::MaterialComponent>(dreamTexture);
        }
        return entity;
    }
}
