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
        std::string shorten(std::string str, int maxLength = 14);
        void renderRootComponent();
        void renderIDComponent();
        void renderTagComponent();
        void renderHierarchyComponent();
        void renderTransformComponent();
        void renderMeshComponent();
        void renderMaterialComponent();
    };
}

#endif //DREAM_IMGUIEDITORINSPECTORVIEW_H
