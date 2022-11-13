//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/renderer/Mesh.h"

namespace Dream {
    void Mesh::setPositions(std::vector<glm::vec3> positions) {
        this->positions = positions;
    }

    void Mesh::setUVs(std::vector<glm::vec2> uv) {
        this->uv = uv;
    }

    void Mesh::setNormals(std::vector<glm::vec3> normals) {
        this->normals = normals;
    }

    void Mesh::setTangents(std::vector<glm::vec3> tangents) {
        this->tangents = tangents;
    }

    void Mesh::setBitangents(std::vector<glm::vec3> bitangents) {
        this->bitangents = bitangents;
    }

    std::vector<glm::vec3> Mesh::getPositions() {
        return positions;
    }

    std::vector<glm::vec2> Mesh::getUVs() {
        return uv;
    }

    std::vector<glm::vec3> Mesh::getNormals() {
        return normals;
    }

    std::vector<glm::vec3> Mesh::getTangents() {
        return tangents;
    }

    std::vector<glm::vec3> Mesh::getBitangents() {
        return bitangents;
    }

    std::vector<unsigned int> Mesh::getIndices() {
        return indices;
    }
}
