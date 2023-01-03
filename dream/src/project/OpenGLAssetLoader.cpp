/**********************************************************************************
 *  Dream is a software for developing real-time 3D experiences.
 *  Copyright (C) 2023 Deepak Ramalignam
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **********************************************************************************/

#include "dream/project/OpenGLAssetLoader.h"

#include <iostream>
#include <filesystem>
#include <utility>
#include "dream/renderer/OpenGLMesh.h"
#include "dream/renderer/OpenGLTexture.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"
#include "dream/util/IDUtils.h"
#include "dream/util/AssimpGLMHelpers.h"
#include "dream/util/Logger.h"

namespace Dream {
    std::map<std::string, BoneInfo>
    OpenGLAssetLoader::loadMesh(std::string guid, bool createEntities, Entity rootEntity) {
        std::string path = Project::getResourceManager()->getFilePathFromGUID(guid);
        if (!std::filesystem::exists(path)) {
            Logger::fatal("Mesh file at " + path + " does not exist");
        }
        // use assimp to get scene of model
        Assimp::Importer importer;
        importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);  // fixes mixamo animations
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                       aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            Logger::fatal("Assimp importing error " + std::string(importer.GetErrorString()));
            return {};
        }
        auto node = scene->mRootNode;
        meshID = 0;
        boneCount = 0;
        m_BoneInfoMap.clear();
        nodeEntities.clear();
        // process root node
        Entity dreamEntityRootNode = processNode(path, guid, node, scene, createEntities, rootEntity, true);
        std::map<std::string, BoneInfo> boneInfoMapCpy(m_BoneInfoMap);
        // go through bone info map and see if an entity exists for a bone, then create a bone component for that entity
        if (createEntities) {
            // add bone component to nodes that are associated to bones
            for (auto nodeEntity: nodeEntities) {
                auto entityTag = nodeEntity.getComponent<Component::TagComponent>().tag;
                if (m_BoneInfoMap.count(entityTag) > 0) {
                    BoneInfo boneInfo = m_BoneInfoMap[entityTag];
                    nodeEntity.addComponent<Component::BoneComponent>(boneInfo.id);
                }
            }
        }
        m_BoneInfoMap.clear();
        nodeEntities.clear();
        if (dreamEntityRootNode) {
            if (boneInfoMapCpy.empty()) {
                // static model
                dreamEntityRootNode.addComponent<Component::MeshComponent>(guid);
            } else {
                // rigged model
                dreamEntityRootNode.addComponent<Component::MeshComponent>(guid, boneInfoMapCpy);
                dreamEntityRootNode.addComponent<Component::AnimatorComponent>();
            }
        }
        return boneInfoMapCpy;
    }

    Entity OpenGLAssetLoader::processNode(std::string path, std::string guid, aiNode *node, const aiScene *scene,
                                          bool createEntities, Entity rootEntity, bool createMeshObjects) {
        Entity dreamNode;
        if (createEntities) {
            if (rootEntity) {
                dreamNode = rootEntity;
            } else {
                dreamNode = Project::getScene()->createEntity(node->mName.C_Str());
            }
            nodeEntities.push_back(dreamNode);
        }
        // process meshes for this node
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            Entity child = processMesh(path, guid, mesh, scene, createEntities, createMeshObjects);
            if (createEntities) {
                dreamNode.addChild(child);
            }
        }
        // process child nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            Entity child = processNode(path, guid, node->mChildren[i], scene, createEntities, {}, createMeshObjects);
            if (createEntities) {
                dreamNode.addChild(child);
            }
        }
        return dreamNode;
    }

    void OpenGLAssetLoader::setVertexBoneData(Vertex &vertex, int boneID, float weight) {
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            if (vertex.boneIDs[i] < 0) {
                vertex.boneWeights[i] = weight;
                vertex.boneIDs[i] = boneID;
                break;
            }
        }
    }

    Entity OpenGLAssetLoader::processMesh(std::string path, std::string guid, aiMesh *mesh, const aiScene *scene,
                                          bool createEntities, bool createMeshObjects) {
        meshID++;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
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
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
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
                    .position = position,
                    .uv = uv,
                    .normal = normal,
                    .tangent = tangent,
                    .bitangent = bitangent
            };

            for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
                vertex.boneIDs[j] = -1;
                vertex.boneWeights[j] = 0.0f;
            }

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // process materials
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        bool textureEmbeddedInModel = false;
        const aiTexture *assimpTexture = nullptr;
        auto textureType = aiTextureType_DIFFUSE;
        std::string texturePath = "";
        std::string textureFileGUID;
        aiColor4D aiDiffuseColor;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiffuseColor);
        glm::vec4 diffuseColor = {aiDiffuseColor.r, aiDiffuseColor.g, aiDiffuseColor.b, aiDiffuseColor.a};
        for (unsigned int i = 0; i < material->GetTextureCount(textureType); i++) {
            aiString str;
            material->GetTexture(textureType, i, &str);
            if (auto texture = scene->GetEmbeddedTexture(str.C_Str())) {
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

        // extract bone weights for vertices
        extractBoneWeightForVertices(vertices, mesh);

        // add mesh component
        std::string meshFileGUID = std::move(guid);
        std::string subMeshFileID = IDUtils::newFileID(std::string(std::to_string(meshID) + "0"));
        if (!Project::getResourceManager()->hasData(meshFileGUID)) {
            if (createMeshObjects) {
                auto *dreamMesh = new OpenGLMesh(vertices, indices);
                Project::getResourceManager()->storeData(meshFileGUID, subMeshFileID, dreamMesh);
            }
        }
        if (createEntities) {
            entity.addComponent<Component::MeshComponent>(meshFileGUID, subMeshFileID);
        }
        // add material component
        if (!texturePath.empty()) {
            if (textureEmbeddedInModel && assimpTexture) {
                // add texture embedded into model file
                auto buffer = reinterpret_cast<unsigned char *>(assimpTexture->pcData);
                int len = assimpTexture->mHeight == 0 ? static_cast<int>(assimpTexture->mWidth) : static_cast<int>(
                        assimpTexture->mWidth * assimpTexture->mHeight);
                if (createMeshObjects) {
                    auto *dreamTexture = new OpenGLTexture(buffer, len);
                    Project::getResourceManager()->storeTextureData(dreamTexture, textureFileGUID);
                    if (createEntities) {
                        entity.addComponent<Component::MaterialComponent>(textureFileGUID, true);
                    }
                }
            } else if (!textureEmbeddedInModel) {
                // add texture stored in an external image file
                if (!Project::getResourceManager()->hasData(textureFileGUID)) {
                    if (createMeshObjects) {
                        auto *dreamTexture = new OpenGLTexture(texturePath);
                        Project::getResourceManager()->storeTextureData(dreamTexture, textureFileGUID);
                    }
                }
                if (createEntities) {
                    entity.addComponent<Component::MaterialComponent>(textureFileGUID, false);
                }
            } else {
                Logger::fatal("Invalid state while processing mesh");
            }
        }
        if (createEntities) {
            if (!entity.hasComponent<Component::MaterialComponent>()) {
                entity.addComponent<Component::MaterialComponent>("", false);
            }
            entity.getComponent<Component::MaterialComponent>().diffuseColor = diffuseColor;
        }
        return entity;
    }

    void OpenGLAssetLoader::extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh) {
        auto &boneInfoMap = m_BoneInfoMap;
        int &boneCount1 = this->boneCount;

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCount1;
                newBoneInfo.offset = AssimpGLMHelpers::convertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount1;
                boneCount1++;
            } else {
                boneID = boneInfoMap[boneName].id;
            }
            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                setVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }

    std::map<std::string, BoneInfo> OpenGLAssetLoader::loadBones(std::string guid) {
        std::string path = Project::getResourceManager()->getFilePathFromGUID(guid);
        if (!std::filesystem::exists(path)) {
            Logger::fatal("Mesh file does not exist");
        }
        // use assimp to get scene of model
        Assimp::Importer importer;
        importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);  // fixes mixamo animations
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                       aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            Logger::fatal("Assimp scene parsing error " + std::string(importer.GetErrorString()));
            return {};
        }
        auto node = scene->mRootNode;
        meshID = 0;
        boneCount = 0;
        m_BoneInfoMap.clear();
        nodeEntities.clear();
        // process root node
        processNode(path, guid, node, scene, false, Entity(), false);
        std::map<std::string, BoneInfo> boneInfoMapCpy(m_BoneInfoMap);
        m_BoneInfoMap.clear();
        return boneInfoMapCpy;
    }
}
