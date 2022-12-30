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

#ifndef DREAM_MATHUTILS_H
#define DREAM_MATHUTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Dream {
    class MathUtils {
    public:
        static void decomposeMatrix(const glm::mat4 &m, glm::vec3 &pos, glm::quat &rot, glm::vec3 &scale);
        static glm::vec3 eulerAngles(glm::quat rot);
        static glm::vec3 normalizeVec3(glm::vec3 vec);
        static glm::vec2 normalizeVec2(glm::vec2 vec);
        static float angle(glm::vec2 vec1, glm::vec2 vec2);
        static glm::quat safeQuatLookAt(glm::vec3 const& lookFrom,glm::vec3 const& lookTo,glm::vec3 const& up, glm::vec3 const& alternativeUp);
        static glm::vec3 crossProductVec3(glm::vec3 a, glm::vec3 b);
    };
}

#endif //DREAM_MATHUTILS_H
