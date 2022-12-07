//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/project/OpenGLAssetLoader.h"

#include <iostream>
#include <filesystem>
#include <utility>
#include "dream/renderer/OpenGLMesh.h"
#include "dream/renderer/OpenGLTexture.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"
#include "dream/util/IDUtils.h"

namespace Dream {
    Entity OpenGLAssetLoader::loadMesh(std::string guid, bool createEntities, Entity rootEntity) {
        std::string path = Project::getResourceManager()->getFilePathFromGUID(guid);
        if (!std::filesystem::exists(path)) {
            std::cout << "Mesh file does not exist" << std::endl;
            exit(EXIT_FAILURE);
        }
        // use assimp to get scene of model
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return {};
        }
        // process root node
        auto node = scene->mRootNode;
        meshID = 0;
        boneCount = 0;
        Entity dreamEntityRootNode = processNode(path, guid, node, scene, createEntities, rootEntity);
        if (dreamEntityRootNode) {
            dreamEntityRootNode.addComponent<Component::MeshComponent>(guid);
        }
        return dreamEntityRootNode;
    }

    Entity OpenGLAssetLoader::processNode(std::string path, std::string guid, aiNode *node, const aiScene *scene, bool createEntities, Entity rootEntity) {
        Entity dreamNode;
        if (createEntities) {
            if (rootEntity) {
                dreamNode = rootEntity;
            } else {
                dreamNode = Project::getScene()->createEntity(node->mName.C_Str());
            }
        }
        // process meshes for this node
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            Entity child = processMesh(path, guid, mesh, scene, createEntities);
            if (createEntities) {
                dreamNode.addChild(child);
            }
        }
        // process child nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            Entity child = processNode(path, guid, node->mChildren[i], scene, createEntities, {});
            if (createEntities) {
                dreamNode.addChild(child);
            }
        }
        return dreamNode;
    }

    void OpenGLAssetLoader::setVertexBoneData(Vertex& vertex, int boneID, float weight) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            if (vertex.m_BoneIDs[i] < 0) {
                vertex.m_Weights[i] = weight;
                vertex.m_BoneIDs[i] = boneID;
                break;
            }
        }
    }

    Entity OpenGLAssetLoader::processMesh(std::string path, std::string guid, aiMesh *mesh, const aiScene *scene, bool createEntities) {
        boneCount = 0;
        meshID++;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex = {};

            // positions
            glm::vec3 position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
            // normals
            glm::vec3 normal = {0, 0, 0};
            if (mesh->HasNormals()) {
                normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
            }
            // texture coordinates
            glm::vec2 uv = {0, 0};
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                uv = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
            }

            // tangent and bitangent
            glm::vec3 tangent = {0, 0, 0};
            glm::vec3 bitangent = {0, 0, 0};
            if (mesh->HasTangentsAndBitangents()) {
                tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
                bitangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
            }

            vertex = {
                    .Position = position,
                    .TexCoords = uv,
                    .Normal = normal,
                    .Tangent = tangent,
                    .Bitangent = bitangent
            };

            for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
                vertex.m_BoneIDs[j] = -1;
                vertex.m_Weights[j] = 0.0f;
            }

            vertices.push_back(vertex);
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
        bool textureEmbeddedInModel = false;
        const aiTexture* assimpTexture = nullptr;
        auto textureType = aiTextureType_DIFFUSE;
        std::string texturePath = "";
        std::string textureFileGUID;
        for(unsigned int i = 0; i < material->GetTextureCount(textureType); i++) {
            aiString str;
            material->GetTexture(textureType, i, &str);
            if(auto texture = scene->GetEmbeddedTexture(str.C_Str())) {
                // texture embedded in model
                textureEmbeddedInModel = true;
                assimpTexture = texture;
                texturePath = str.C_Str();
                textureFileGUID = IDUtils::newFileID(str.C_Str());
                Project::getResourceManager()->setFilePathFromGUID(textureFileGUID, texturePath);
            } else {
                // regular texture file
                textureEmbeddedInModel = false;
                texturePath = std::filesystem::path(path).parent_path().append(str.C_Str());
                textureFileGUID = IDUtils::getGUIDForFile(texturePath);
            }
        }

        // create entity for this mesh
        Entity entity;
        if (createEntities) {
            entity = Project::getScene()->createEntity(mesh->mName.C_Str());
        }

        // keep track of whether the mesh has bones we need to keep track of
        bool hasSkeleton = false;

        // process bones
        if (createEntities) {
            // create map of bones for this mesh (look at child entities and see which ones have bones)
            std::map<std::string, Entity> boneEntitiesForMesh;     // bone name : entity that represents the bone
            Entity currentEntity = entity.getComponent<Component::HierarchyComponent>().first;
            while (currentEntity) {
                if (currentEntity.hasComponent<Component::BoneComponent>()) {
                    auto boneComponent = currentEntity.getComponent<Component::BoneComponent>();
                    boneEntitiesForMesh.insert(std::make_pair(boneComponent.boneName, currentEntity));
                }
                currentEntity = currentEntity.getComponent<Component::HierarchyComponent>().next;
            }

            // traverse bones for this mesh found by assimp
            for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
                hasSkeleton = true;
                std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
                int boneID = boneCount;
                glm::mat4 offset = convertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);;
                Entity boneEntity;

                // check if bone entity is already a child
                if (boneEntitiesForMesh.count(boneName) > 0) {
                    // found existing
                    boneEntity = boneEntitiesForMesh[boneName];
                } else {
                    // not found, so create new one
                    boneEntity = Project::getScene()->createEntity(boneName);
                    boneEntity.addComponent<Component::BoneComponent>(boneName, boneID, offset);
                    entity.addChild(boneEntity);
                    boneCount++;
                }

                // get the vertices associated with this bone and the weight this bone has on them
                std::vector<int> boneVertices;
                std::vector<float> boneWeightsForVertices;
                auto assimpWeights = mesh->mBones[boneIndex]->mWeights;
                for (int weightIndex = 0; weightIndex < mesh->mBones[boneIndex]->mNumWeights; ++weightIndex) {
                    int vertexId = (int) (assimpWeights[weightIndex].mVertexId);
                    float weight = assimpWeights[weightIndex].mWeight;
                    int numVertices = (int) (vertices.size());
                    boneVertices.push_back(vertexId);
                    boneWeightsForVertices.push_back(weight);
                    assert(vertexId <= numVertices);
                    // specify which bones modify which vertex and the weight of effect
                    setVertexBoneData(vertices[vertexId], boneID, weight);
                }
                boneEntity.getComponent<Component::BoneComponent>().vertices = boneVertices;
                boneEntity.getComponent<Component::BoneComponent>().weights = boneWeightsForVertices;
            }
        } else {
            for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
                int boneID = boneCount;
                boneCount++;
                // get the vertices associated with this bone and the weight this bone has on them
                std::vector<int> boneVertices;
                std::vector<float> boneWeightsForVertices;
                auto assimpWeights = mesh->mBones[boneIndex]->mWeights;

                for (int weightIndex = 0; weightIndex < mesh->mBones[boneIndex]->mNumWeights; ++weightIndex) {
                    int vertexId = (int) (assimpWeights[weightIndex].mVertexId);
                    float weight = assimpWeights[weightIndex].mWeight;
                    int numVertices = (int) (vertices.size());
                    boneVertices.push_back(vertexId);
                    boneWeightsForVertices.push_back(weight);
                    assert(vertexId <= numVertices);
                    // specify which bones modify which vertex and the weight of effect
                    setVertexBoneData(vertices[vertexId], boneID, weight);
                }
            }
        }

        // add mesh component
        std::string meshFileGUID = std::move(guid);
        std::string subMeshFileID = IDUtils::newFileID(std::string(std::to_string(meshID) + "0"));
        if (!Project::getResourceManager()->hasData(meshFileGUID)) {
            auto* dreamMesh = new OpenGLMesh(vertices, indices, hasSkeleton);
            Project::getResourceManager()->storeData(meshFileGUID, subMeshFileID, dreamMesh);
        }
        if (createEntities) {
            entity.addComponent<Component::MeshComponent>(meshFileGUID, subMeshFileID);
        }
        // add material component
        if (!texturePath.empty()) {
            if (textureEmbeddedInModel && assimpTexture) {
                // add texture embedded into model file
                auto buffer = reinterpret_cast<unsigned char*>(assimpTexture->pcData);
                int len = assimpTexture->mHeight == 0 ? static_cast<int>(assimpTexture->mWidth) : static_cast<int>(assimpTexture->mWidth * assimpTexture->mHeight);
                auto* dreamTexture = new OpenGLTexture(buffer, len);
                Project::getResourceManager()->storeData(textureFileGUID, dreamTexture);
                if (createEntities) {
                    entity.addComponent<Component::MaterialComponent>(textureFileGUID, true);
                }
            } else if (!textureEmbeddedInModel) {
                // add texture stored in an external image file
                if (!Project::getResourceManager()->hasData(textureFileGUID)) {
                    auto* dreamTexture = new OpenGLTexture(texturePath);
                    Project::getResourceManager()->storeData(textureFileGUID, dreamTexture);
                }
                if (createEntities) {
                    entity.addComponent<Component::MaterialComponent>(textureFileGUID, false);
                }
            } else {
                std::cout << "Error: Invalid state" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return entity;
    }

    glm::mat4 OpenGLAssetLoader::convertMatrixToGLMFormat(const aiMatrix4x4 &from) {
        glm::mat4 to;
        //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
        to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
        to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
        to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
        return to;
    }
}
