//
// Created by Deepak Ramalingam on 11/13/22.
//

#ifndef DREAM_IMGUIEDITORINSPECTORVIEW_H
#define DREAM_IMGUIEDITORINSPECTORVIEW_H

#include "dream/scene/Entity.h"
#include "glm/vec3.hpp"
#include "glm/ext/quaternion_float.hpp"
#include <imgui.h>
#include <imgui-filebrowser/imfilebrowser.h>

namespace Dream {
    class ImGuiEditorInspectorView {
    public:
        ImGuiEditorInspectorView();
        ~ImGuiEditorInspectorView();
        void update();
        void selectEntity(Entity& entity);
    private:
        Entity selectedEntity;
        std::string shorten(std::string str, int maxLength = 14);
        void renderVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
        void renderRootComponent();
        void renderIDComponent();
        void renderTagComponent();
        void renderHierarchyComponent();
        void renderTransformComponent();
        void renderMeshComponent();
        void renderMaterialComponent();
        // TODO: move mesh component renderer to new class
        // mesh component renderer variables
        ImGui::FileBrowser* meshSelectorBrowser;
        std::string oldMeshGUID;
    };
}

#endif //DREAM_IMGUIEDITORINSPECTORVIEW_H
