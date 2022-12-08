//
// Created by Deepak Ramalingam on 12/8/22.
//

#ifndef DREAM_ASSIMPNODEDATA_H
#define DREAM_ASSIMPNODEDATA_H

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

namespace Dream {
    struct AssimpNodeData
    {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };
}

#endif //DREAM_ASSIMPNODEDATA_H
