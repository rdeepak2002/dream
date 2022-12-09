//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/project/AssetLoader.h"
#include <iostream>

namespace Dream {
    std::map<std::string, BoneInfo> AssetLoader::loadMesh(std::string guid, bool createEntities, Entity rootEntity) {
        return {};
    }

    std::map<std::string, BoneInfo> AssetLoader::loadBones(std::string guid) {
        return std::map<std::string, BoneInfo>();
    }
}
