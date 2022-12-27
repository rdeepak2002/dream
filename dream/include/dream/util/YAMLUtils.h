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

#ifndef DREAM_YAMLUTILS_H
#define DREAM_YAMLUTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <yaml-cpp/yaml.h>

namespace YAML {
    template<>
    struct convert<glm::vec2> {
        static Node encode(const glm::vec2& rhs);
        static bool decode(const Node& node, glm::vec2& rhs);
    };

    template<>
    struct convert<glm::vec3> {
        static Node encode(const glm::vec3& rhs);
        static bool decode(const Node& node, glm::vec3& rhs);
    };

    template<>
    struct convert<glm::vec4> {
        static Node encode(const glm::vec4& rhs);
        static bool decode(const Node& node, glm::vec4& rhs);
    };

    template<>
    struct convert<glm::quat> {
        static Node encode(const glm::quat& rhs);
        static bool decode(const Node& node, glm::quat& rhs);
    };

    template<>
    struct convert<glm::mat4> {
        static Node encode(const glm::mat4& rhs);
        static bool decode(const Node& node, glm::mat4& rhs);
    };
}

#endif //DREAM_YAMLUTILS_H
