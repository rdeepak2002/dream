//
// Created by Deepak Ramalingam on 11/10/22.
//

#include "dream/renderer/OpenGLSphereMesh.h"
#include "glm/ext/scalar_constants.hpp"

#include <glm/glm.hpp>

namespace Dream {
    OpenGLSphereMesh::OpenGLSphereMesh() {
        unsigned int xSegments = 20;
        unsigned int ySegments = 20;

        for (unsigned int y = 0; y <= ySegments; ++y)
        {
            for (unsigned int x = 0; x <= xSegments; ++x)
            {
                float xSegment = (float)x / (float)xSegments;
                float ySegment = (float)y / (float)ySegments;
                float xPos = std::cos(xSegment * 2 * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>()); // TAU is 2PI
                float yPos = std::cos(ySegment * glm::pi<float>());
                float zPos = std::sin(xSegment * 2 * glm::pi<float>()) * std::sin(ySegment * glm::pi<float>());

                Positions.push_back(glm::vec3(xPos, yPos, zPos));
                UV.push_back(glm::vec2(xSegment, ySegment));
                Normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        for (int y = 0; y < ySegments; ++y)
        {
            for (int x = 0; x < xSegments; ++x)
            {
                Indices.push_back((y + 1) * (xSegments + 1) + x);
                Indices.push_back(y       * (xSegments + 1) + x);
                Indices.push_back(y       * (xSegments + 1) + x + 1);

                Indices.push_back((y + 1) * (xSegments + 1) + x);
                Indices.push_back(y       * (xSegments + 1) + x + 1);
                Indices.push_back((y + 1) * (xSegments + 1) + x + 1);
            }
        }

        this->finalize();
    }
}