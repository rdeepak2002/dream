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
        this->guid = guid;
        this->fileId = fileID;
        this->mesh = (OpenGLMesh*) Project::getResourceManager()->getData(guid, fileID);
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
}
