//
// Created by Deepak Ramalingam on 11/13/22.
//

#ifndef DREAM_IMGUIEDITORSCENEVIEW_H
#define DREAM_IMGUIEDITORSCENEVIEW_H

#include "dream/scene/Entity.h"

namespace Dream {
    class ImGuiEditorSceneView {
    public:
        void update();
        void renderSceneViewEntity(Entity &entity);
    };
}

#endif //DREAM_IMGUIEDITORSCENEVIEW_H
