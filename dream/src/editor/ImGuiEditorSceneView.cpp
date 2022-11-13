//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/editor/ImGuiEditorSceneView.h"

#include <imgui.h>
#include <imgui_internal.h>
#include "dream/Project.h"
#include "dream/scene/Component.h"

namespace Dream {
    void ImGuiEditorSceneView::update() {
        ImGuiWindowClass scene_window_class;
        scene_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&scene_window_class);
        ImGui::Begin("Scene");
        // render scene hierarchy
        Entity rootEntity = Project::getInstance().getScene().getRootEntity();
        renderSceneViewEntity(rootEntity);
        ImGui::End();
    }

    void ImGuiEditorSceneView::renderSceneViewEntity(Entity &entity) {
        if (ImGui::TreeNode(entity.getComponent<Component::TagComponent>().tag.c_str())) {
            Entity child = entity.getComponent<Component::HierarchyComponent>().first;
            while (child) {
                this->renderSceneViewEntity(child);
                child = child.getComponent<Component::HierarchyComponent>().next;
            }
            ImGui::TreePop();
        }
    }
}
