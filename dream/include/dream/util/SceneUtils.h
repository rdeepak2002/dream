//
// Created by Deepak Ramalingam on 11/26/22.
//

#ifndef DREAM_SCENEUTILS_H
#define DREAM_SCENEUTILS_H

#include "dream/scene/Entity.h"

namespace Dream {
    class SceneUtils {
    public:
        /**
         * Remove references to an old mesh for an entity (and its children if recursively is true)
         * @param entity
         * @param meshGUID
         * @param recursively
         */
        static void removeMeshReference(Entity entity, std::string meshGUID, bool recursively=true);
    };
}

#endif //DREAM_SCENEUTILS_H
