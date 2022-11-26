//
// Created by Deepak Ramalingam on 11/13/22.
//

#ifndef DREAM_IMGUIEDITORINSPECTORVIEW_H
#define DREAM_IMGUIEDITORINSPECTORVIEW_H

#include "dream/scene/Entity.h"

namespace Dream {
    class ImGuiEditorInspectorView {
    public:
        ImGuiEditorInspectorView();
        void update();
        void selectEntity(Entity& entity);
    private:
        Entity selectedEntity;
        void renderMaterialComponent();
        std::string shorten(std::string str, int maxLength = 14);
    };
}

#endif //DREAM_IMGUIEDITORINSPECTORVIEW_H
