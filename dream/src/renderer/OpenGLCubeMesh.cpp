//
// Created by Deepak Ramalingam on 11/10/22.
//

#include "dream/renderer/OpenGLCubeMesh.h"

namespace Dream {
    OpenGLCubeMesh::OpenGLCubeMesh() {
        std::vector<glm::vec3> positions = std::vector<glm::vec3> {
                glm::vec3(-0.5f, -0.5f, -0.5f),
                glm::vec3( 0.5f,  0.5f, -0.5f),
                glm::vec3( 0.5f, -0.5f, -0.5f),
                glm::vec3( 0.5f,  0.5f, -0.5f),
                glm::vec3(-0.5f, -0.5f, -0.5f),
                glm::vec3(-0.5f,  0.5f, -0.5f),

                glm::vec3(-0.5f, -0.5f,  0.5f),
                glm::vec3( 0.5f, -0.5f,  0.5f),
                glm::vec3( 0.5f,  0.5f,  0.5f),
                glm::vec3( 0.5f,  0.5f,  0.5f),
                glm::vec3(-0.5f,  0.5f,  0.5f),
                glm::vec3(-0.5f, -0.5f,  0.5f),

                glm::vec3(-0.5f,  0.5f,  0.5f),
                glm::vec3(-0.5f,  0.5f, -0.5f),
                glm::vec3(-0.5f, -0.5f, -0.5f),
                glm::vec3(-0.5f, -0.5f, -0.5f),
                glm::vec3(-0.5f, -0.5f,  0.5f),
                glm::vec3(-0.5f,  0.5f,  0.5f),

                glm::vec3( 0.5f,  0.5f,  0.5f),
                glm::vec3( 0.5f, -0.5f, -0.5f),
                glm::vec3( 0.5f,  0.5f, -0.5f),
                glm::vec3( 0.5f, -0.5f, -0.5f),
                glm::vec3( 0.5f,  0.5f,  0.5f),
                glm::vec3( 0.5f, -0.5f,  0.5f),

                glm::vec3(-0.5f, -0.5f, -0.5f),
                glm::vec3( 0.5f, -0.5f, -0.5f),
                glm::vec3( 0.5f, -0.5f,  0.5f),
                glm::vec3( 0.5f, -0.5f,  0.5f),
                glm::vec3(-0.5f, -0.5f,  0.5f),
                glm::vec3(-0.5f, -0.5f, -0.5f),

                glm::vec3(-0.5f,  0.5f, -0.5f),
                glm::vec3( 0.5f,  0.5f,  0.5f),
                glm::vec3( 0.5f,  0.5f, -0.5f),
                glm::vec3( 0.5f,  0.5f,  0.5f),
                glm::vec3(-0.5f,  0.5f, -0.5f),
                glm::vec3(-0.5f,  0.5f,  0.5f),
        };

        std::vector<glm::vec2> uv = std::vector<glm::vec2> {
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(0.0f, 0.0f),
                glm::vec2(0.0f, 1.0f),

                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(0.0f, 0.0f),

                glm::vec2(1.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 0.0f),

                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.0f, 0.0f),

                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.0f, 0.0f),
                glm::vec2(0.0f, 1.0f),

                glm::vec2(0.0f, 1.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(1.0f, 1.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.0f, 1.0f),
                glm::vec2(0.0f, 0.0f),
        };

        std::vector<glm::vec3> normals = std::vector<glm::vec3> {
                glm::vec3( 0.0f,  0.0f, -1.0f),
                glm::vec3( 0.0f,  0.0f, -1.0f),
                glm::vec3( 0.0f,  0.0f, -1.0f),
                glm::vec3( 0.0f,  0.0f, -1.0f),
                glm::vec3( 0.0f,  0.0f, -1.0f),
                glm::vec3( 0.0f,  0.0f, -1.0f),

                glm::vec3( 0.0f,  0.0f,  1.0f),
                glm::vec3( 0.0f,  0.0f,  1.0f),
                glm::vec3( 0.0f,  0.0f,  1.0f),
                glm::vec3( 0.0f,  0.0f,  1.0f),
                glm::vec3( 0.0f,  0.0f,  1.0f),
                glm::vec3( 0.0f,  0.0f,  1.0f),

                glm::vec3(-1.0f,  0.0f,  0.0f),
                glm::vec3(-1.0f,  0.0f,  0.0f),
                glm::vec3(-1.0f,  0.0f,  0.0f),
                glm::vec3(-1.0f,  0.0f,  0.0f),
                glm::vec3(-1.0f,  0.0f,  0.0f),
                glm::vec3(-1.0f,  0.0f,  0.0f),

                glm::vec3( 1.0f,  0.0f,  0.0f),
                glm::vec3( 1.0f,  0.0f,  0.0f),
                glm::vec3( 1.0f,  0.0f,  0.0f),
                glm::vec3( 1.0f,  0.0f,  0.0f),
                glm::vec3( 1.0f,  0.0f,  0.0f),
                glm::vec3( 1.0f,  0.0f,  0.0f),

                glm::vec3( 0.0f, -1.0f,  0.0f),
                glm::vec3( 0.0f, -1.0f,  0.0f),
                glm::vec3( 0.0f, -1.0f,  0.0f),
                glm::vec3( 0.0f, -1.0f,  0.0f),
                glm::vec3( 0.0f, -1.0f,  0.0f),
                glm::vec3( 0.0f, -1.0f,  0.0f),

                glm::vec3( 0.0f,  1.0f,  0.0f),
                glm::vec3( 0.0f,  1.0f,  0.0f),
                glm::vec3( 0.0f,  1.0f,  0.0f),
                glm::vec3( 0.0f,  1.0f,  0.0f),
                glm::vec3( 0.0f,  1.0f,  0.0f),
                glm::vec3( 0.0f,  1.0f,  0.0f),
        };

        for (int i = 0; i < positions.size(); ++i) {
            Vertex vertex = {
                    .position = positions[i],
                    .uv = uv[i],
                    .normal = normals[i],
                    .tangent = {0, 0, 0},
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
