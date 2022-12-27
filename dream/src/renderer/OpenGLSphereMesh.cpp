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
                float xSegment = (float) x / (float) xSegments;
                float ySegment = (float) y / (float) ySegments;
                float xPos =
                        std::cos(xSegment * 2 * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>()); // TAU is 2PI
                float yPos = std::cos(ySegment * glm::pi<float>());
                float zPos = std::sin(xSegment * 2 * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());

                positions.emplace_back(xPos, yPos, zPos);
                uv.emplace_back(xSegment, ySegment);
                normals.emplace_back(xPos, yPos, zPos);
            }
        }

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