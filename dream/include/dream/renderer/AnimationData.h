//
// Created by Deepak Ramalingam on 12/8/22.
//

#ifndef DREAM_ANIMATIONDATA_H
#define DREAM_ANIMATIONDATA_H

#pragma once

#include <glm/glm.hpp>

namespace Dream {
    struct BoneInfo
    {
        /*id is index in finalBoneMatrices*/
        int id;

        /*offset matrix transforms vertex from model space to bone space*/
        glm::mat4 offset;

    };
}

#endif //DREAM_ANIMATIONDATA_H
