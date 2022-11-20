//
// Created by Deepak Ramalingam on 11/19/22.
//

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
}

#endif //DREAM_YAMLUTILS_H
