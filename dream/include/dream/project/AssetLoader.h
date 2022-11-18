//
// Created by Deepak Ramalingam on 11/13/22.
//

#ifndef DREAM_ASSETLOADER_H
#define DREAM_ASSETLOADER_H

#include "dream/renderer/Mesh.h"
#include "dream/scene/Entity.h"

namespace Dream {
    class AssetLoader {
    public:
        virtual Entity loadMesh(std::string guid);
    };
}

#endif //DREAM_ASSETLOADER_H
