//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/renderer/Mesh.h"

namespace Dream {
    void Mesh::setIndices(std::vector<unsigned int> indices) {
        this->indices = indices;
    }

    void Mesh::setVertices(std::vector<Vertex> vertices) {
        this->vertices = vertices;
    }

    std::vector<unsigned int> Mesh::getIndices() {
        return indices;
    }

    std::vector<Vertex> Mesh::getVertices() {
        return vertices;
    }
}
