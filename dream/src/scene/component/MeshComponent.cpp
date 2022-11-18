//
// Created by Deepak Ramalingam on 11/13/22.
//

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/renderer/OpenGLMesh.h"
#include "dream/renderer/OpenGLCubeMesh.h"
#include "dream/renderer/OpenGLSphereMesh.h"
#include "dream/project/Project.h"

namespace Dream::Component {
    MeshComponent::MeshComponent(std::string guid, std::string fileID) {
        this->meshType = FROM_FILE;
        this->guid = std::move(guid);
        this->fileId = fileID;
//        if (!Project::getResourceManager()->hasData(this->guid)) {
//            std::string path = Project::getResourceManager()->getFilePathFromGUID(this->guid);
//            std::cout << "TODO: load mesh from " << path << std::endl;
//            exit(EXIT_FAILURE);
//        }
        this->mesh = (OpenGLMesh*) Project::getResourceManager()->getData(this->guid, fileID);
    }

    MeshComponent::MeshComponent(Dream::Component::MeshComponent::MeshType meshType,
                                 std::map<std::string, float> primitiveMeshData) {
        this->meshType = meshType;
        if (meshType == PRIMITIVE_CUBE) {
            this->mesh = new OpenGLCubeMesh();
        } else if (meshType == PRIMITIVE_SPHERE) {
            this->mesh = new OpenGLSphereMesh();
        } else {
            std::cout << "UNKNOWN PRIMITIVE MESH TYPE" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    Mesh *MeshComponent::getMesh() {
        if (!this->mesh) {
            std::cout << "Mesh is null" << std::endl;
        }
        return this->mesh;
    }

    void MeshComponent::serialize(YAML::Emitter &out) {
        out << YAML::Key << getComponentName();
        out << YAML::BeginMap;
        out << YAML::Key << "meshType" << YAML::Value << this->meshType;
        out << YAML::Key << "guid" << YAML::Value << this->guid;
        out << YAML::Key << "fileId" << YAML::Value << this->fileId;
        out << YAML::EndMap;
    }

    std::string MeshComponent::getComponentName() {
        return "MeshComponent";
    }
}
