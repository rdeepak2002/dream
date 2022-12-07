//
// Created by Deepak Ramalingam on 11/10/22.
//

#include "dream/renderer/OpenGLSphereMesh.h"
#include "glm/ext/scalar_constants.hpp"

#include <glm/glm.hpp>

namespace Dream {
    OpenGLSphereMesh::OpenGLSphereMesh(unsigned int xSegments, unsigned int ySegments) {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;

        for (unsigned int y = 0; y <= ySegments; ++y) {
            for (unsigned int x = 0; x <= xSegments; ++x) {
                float xSegment = (float)x / (float)xSegments;
                float ySegment = (float)y / (float)ySegments;
                float xPos = std::cos(xSegment * 2 * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>()); // TAU is 2PI
                float yPos = std::cos(ySegment * glm::pi<float>());
                float zPos = std::sin(xSegment * 2 * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());

                positions.emplace_back(xPos, yPos, zPos);
                uv.emplace_back(xSegment, ySegment);
                normals.emplace_back(xPos, yPos, zPos);
            }
        }

        for (int i = 0; i < positions.size(); ++i) {
            Vertex vertex = {
                    .Position = positions[i],
                    .TexCoords = uv[i],
                    .Normal = normals[i],
                    .Tangent = {0, 0, 0},
                    .Bitangent = {0, 0, 0}
            };
            for (int j = 0; j < 4; ++j) {
                vertex.m_Weights[j] = 0;
                vertex.m_BoneIDs[j] = -1;
            }
            vertices.push_back(vertex);
        }

        for (int y = 0; y < ySegments; ++y) {
            for (int x = 0; x < xSegments; ++x) {
                indices.push_back((y + 1) * (xSegments + 1) + x);
                indices.push_back(y * (xSegments + 1) + x);
                indices.push_back(y * (xSegments + 1) + x + 1);

                indices.push_back((y + 1) * (xSegments + 1) + x);
                indices.push_back(y * (xSegments + 1) + x + 1);
                indices.push_back((y + 1) * (xSegments + 1) + x + 1);
            }
        }

        this->finalize();
    }
}