//
// Created by Deepak Ramalingam on 12/8/22.
//

#ifndef DREAM_ANIMATIONDATA_H
#define DREAM_ANIMATIONDATA_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Dream {
    struct BoneInfo {
        int id;
        glm::mat4 offset;
    };

    struct KeyPosition {
        glm::vec3 position;
        float timeStamp;
    };

    struct KeyRotation {
        glm::quat orientation;
        float timeStamp;
    };

    struct KeyScale {
        glm::vec3 scale;
        float timeStamp;
    };
}

#endif //DREAM_ANIMATIONDATA_H
