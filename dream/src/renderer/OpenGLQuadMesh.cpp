//
// Created by Deepak Ramalingam on 2/27/23.
//

#include "dream/renderer/OpenGLQuadMesh.h"

namespace Dream {
    OpenGLQuadMesh::OpenGLQuadMesh() {
        std::vector<glm::vec3> positions ={
            { -1.0f,  1.0f, 0.0f },
            { -1.0f, -1.0f, 0.0f },
            {  1.0f,  1.0f, 0.0f },
            {  1.0f, -1.0f, 0.0f },
        };

        std::vector<glm::vec2> uv = {
            { 0.0f, 1.0f },
            { 0.0f, 0.0f },
            { 1.0f, 1.0f },
            { 1.0f, 0.0f },
        };

        std::vector<glm::vec3> normals ={
                { 0.0f, 0.0f, 1.0f },
                { 0.0f, 0.0f, 1.0f },
                { 0.0f, 0.0f, 1.0f },
                { 0.0f, 0.0f, 1.0f },
        };

        std::vector<glm::vec3> tangents = {
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f),
        };

        for (int i = 0; i < positions.size(); ++i) {
            Vertex vertex = {
                    .position = positions[i],
                    .uv = uv[i],
                    .normal = normals[i],
                    .tangent = tangents[i],
                    .bitangent = {0, 0, 0}
            };
            for (int j = 0; j < MAX_BONE_INFLUENCE; ++j) {
                vertex.boneWeights[j] = 0;
                vertex.boneIDs[j] = -1;
            }
            vertices.push_back(vertex);
        }
        this->finalize();
    }
}