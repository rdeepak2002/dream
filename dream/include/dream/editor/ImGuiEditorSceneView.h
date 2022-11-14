//
// Created by Deepak Ramalingam on 11/13/22.
//

#ifndef DREAM_IMGUIEDITORSCENEVIEW_H
#define DREAM_IMGUIEDITORSCENEVIEW_H

#include "dream/scene/Entity.h"
#include "dream/editor/ImGuiEditorInspectorView.h"

namespace Dream {
    class ImGuiEditorSceneView {
    public:
        ImGuiEditorSceneView();
        void update();
        void renderSceneViewEntity(Entity &entity);
        void setInspectorView(ImGuiEditorInspectorView* inspectorView);
    private:
        ImGuiEditorInspectorView* inspectorView;
    };
}

#endif //DREAM_IMGUIEDITORSCENEVIEW_H
