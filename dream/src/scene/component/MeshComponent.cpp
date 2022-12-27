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

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/renderer/OpenGLMesh.h"
#include "dream/renderer/OpenGLCubeMesh.h"
#include "dream/renderer/OpenGLSphereMesh.h"
#include "dream/project/Project.h"
#include "dream/util/Logger.h"
#include "dream/util/SceneUtils.h"

namespace Dream::Component {
    MeshComponent::MeshComponent(std::string guid, std::string fileID) {
        this->meshType = FROM_FILE;
        this->guid = std::move(guid);
        this->fileId = std::move(fileID);
    }


    MeshComponent::MeshComponent(std::string guid) {
        this->meshType = FROM_FILE;
        this->guid = std::move(guid);
    }

    MeshComponent::MeshComponent(std::string guid, std::map<std::string, BoneInfo> boneMap) {
        this->meshType = FROM_FILE;
        this->guid = std::move(guid);
        this->m_BoneCount = (int) boneMap.size();
        this->m_BoneInfoMap = std::move(boneMap);
    }

    MeshComponent::MeshComponent(Dream::Component::MeshComponent::MeshType meshType,
                                 std::map<std::string, float> primitiveMeshData) {
        this->meshType = meshType;
    }

    void MeshComponent::loadMesh() {
        if (meshType == MeshType::FROM_FILE) {
            if (!this->fileId.empty()) {
                if (!Project::getResourceManager()->hasData(this->guid, this->fileId)) {
                    std::string path = Project::getResourceManager()->getFilePathFromGUID(this->guid);
                    Project::getAssetLoader()->loadMesh(this->guid);
                }
                this->mesh = (OpenGLMesh *) Project::getResourceManager()->getData(this->guid, this->fileId);
            } else {
                if (!this->guid.empty()) {
                    if (needsToLoadBones) {
                        // TODO: THIS CREATES NEW POINTERS THAT ARE UNUSED, PASS OPTION TO LOAD MESH TO NOT CREATE NEW OBJECTS
                        auto boneMap = Project::getAssetLoader()->loadMesh(this->guid);
                        this->m_BoneCount = (int) boneMap.size();
                        this->m_BoneInfoMap = std::move(boneMap);
                        needsToLoadBones = false;
                    }
                }
            }
        } else {
            if (meshType == PRIMITIVE_CUBE) {
                this->mesh = new OpenGLCubeMesh();
            } else if (meshType == PRIMITIVE_SPHERE) {
                this->mesh = new OpenGLSphereMesh();
            } else {
                Logger::fatal("Unknown primitive mesh type");
            }
        }
    }

    void MeshComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<MeshComponent>()) {
            auto &meshComponent = entity.getComponent<MeshComponent>();
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_meshType << YAML::Value << meshComponent.meshType;
            out << YAML::Key << k_guid << YAML::Value << meshComponent.guid;
            out << YAML::Key << k_fileId << YAML::Value << meshComponent.fileId;
            out << YAML::EndMap;
        }
    }

    void MeshComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            auto meshType = static_cast<MeshType>(node[componentName][k_meshType].as<int>());
            auto guid = node[componentName][k_guid].as<std::string>();
            auto fileId = node[componentName][k_fileId].as<std::string>();
            bool isPrimitiveMesh = guid.empty() && fileId.empty();
            bool isSubMeshFromFile = !guid.empty() && !fileId.empty();
            bool isMeshParentFromFile = !guid.empty() && fileId.empty();
            if (isPrimitiveMesh) {
                // loading primitive mesh
                std::map<std::string, float> primitiveMeshData;
                entity.addComponent<MeshComponent>(meshType, primitiveMeshData);
            } else if (isSubMeshFromFile) {
                // loading mesh from file
                entity.addComponent<MeshComponent>(guid, fileId);
            } else if (isMeshParentFromFile) {
                // reference that this entity is the parent of a mesh
                entity.addComponent<MeshComponent>(guid);
            } else {
                Logger::fatal("Invalid mesh scene data");
            }
        }
    }

    void MeshComponent::changeMeshType(MeshComponent::MeshType newMeshType, Entity entity) {
        // remove child meshes
        auto oldMeshType = meshType;
        auto oldGUID = guid;

        // reset skeleton
        this->m_BoneInfoMap.clear();
        this->needsToLoadBones = true;

        // change mesh
        if (newMeshType == PRIMITIVE_CUBE) {
            this->mesh = new OpenGLCubeMesh();
            this->guid = "";
            this->fileId = "";
        } else if (newMeshType == PRIMITIVE_SPHERE) {
            this->mesh = new OpenGLSphereMesh();
            this->guid = "";
            this->fileId = "";
        } else if (newMeshType == FROM_FILE) {
            // TODO: handle freeing of memory (but note other meshes share this memory)
            this->mesh = nullptr;
            this->guid = "";
            this->fileId = "";
        } else {
            Logger::fatal("Unknown new mesh type");
        }

        this->meshType = newMeshType;

        if (oldMeshType == FROM_FILE) {
            SceneUtils::removeMeshReference(entity, oldGUID, true);
        }
    }
}
