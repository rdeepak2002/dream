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

#include "dream/util/MathUtils.h"
#include "dream/util/Logger.h"
#include <glm/gtx/vector_angle.hpp>

namespace Dream {
    void MathUtils::decomposeMatrix(const glm::mat4 &m, glm::vec3 &pos, glm::quat &rot, glm::vec3 &scale) {
        pos = m[3];
        for (int i = 0; i < 3; i++) {
            scale[i] = glm::length(glm::vec3(m[i]));
        }
        const glm::mat3 rotMtx(glm::vec3(m[0]) / scale[0], glm::vec3(m[1]) / scale[1], glm::vec3(m[2]) / scale[2]);
        rot = glm::quat_cast(rotMtx);
    }

    glm::vec3 MathUtils::eulerAngles(glm::quat rot) {
        return glm::eulerAngles(rot);
    }

    glm::vec3 MathUtils::normalizeVec3(glm::vec3 vec) {
        if (glm::distance({0, 0, 0}, vec) == 0) {
            Logger::error("Attempting to normalize zero vector");
            return {0, 0, 0};
        }
        return glm::normalize(vec);
    }

    glm::vec2 MathUtils::normalizeVec2(glm::vec2 vec) {
        if (glm::distance({0, 0}, vec) == 0) {
            Logger::error("Attempting to normalize zero vector");
            return {0, 0};
        }
        return glm::normalize(vec);
    }

    float MathUtils::angle(glm::vec2 vec1, glm::vec2 vec2) {
//        return atan2(vec1.x*vec2.y-vec1.y*vec2.x,vec1.x*vec2.x+vec1.y*vec2.y);
        auto v1n = normalize(vec1);
        auto v2n = normalize(vec2);
        return glm::angle(v1n, v2n);
    }

    glm::quat MathUtils::safeQuatLookAt(
            glm::vec3 const& lookFrom,
            glm::vec3 const& lookTo,
            glm::vec3 const& up,
            glm::vec3 const& alternativeUp) {
        glm::vec3 direction = lookTo - lookFrom;
        float directionLength = glm::length(direction);

        // Check if the direction is valid; Also deals with NaN
        if (!(directionLength > 0.0001)) {
            Logger::warn("Invalid direction for safe look at");
            return glm::quat(1, 0, 0, 0); // Just return identity
        }

        // Normalize direction
        direction /= directionLength;

        // Is the normal up (nearly) parallel to direction?
        if(glm::abs(glm::dot(direction, up)) > .9999f) {
            // Use alternative up
            return glm::quatLookAt(direction, alternativeUp);
        }
        else {
            return glm::quatLookAt(direction, up);
        }
    }

    glm::vec3 MathUtils::crossProductVec3(glm::vec3 a, glm::vec3 b) {
        return glm::normalize(glm::cross(a, b));
    }

    float MathUtils::magnitudeVec3(glm::vec3 v) {
        return glm::l2Norm(v);
    }
}