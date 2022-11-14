//
// Created by Deepak Ramalingam on 11/13/22.
//

#ifndef DREAM_ASSETIMPORTER_H
#define DREAM_ASSETIMPORTER_H

#include "dream/renderer/Mesh.h"
#include "dream/scene/Entity.h"

namespace Dream {
    class AssetImporter {
    public:
        virtual Entity importMesh(std::string path);
    };
}

#endif //DREAM_ASSETIMPORTER_H
