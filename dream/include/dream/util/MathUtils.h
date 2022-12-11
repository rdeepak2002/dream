//
// Created by Deepak Ramalingam on 12/10/22.
//

#ifndef DREAM_MATHUTILS_H
#define DREAM_MATHUTILS_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Dream {
    class MathUtils {
    public:
        static void decomposeMatrix(const glm::mat4& m, glm::vec3& pos, glm::quat& rot, glm::vec3& scale);
    };
}

#endif //DREAM_MATHUTILS_H
