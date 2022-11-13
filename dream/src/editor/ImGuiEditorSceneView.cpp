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
        std::string tagStr = entity.getComponent<Component::TagComponent>().tag;
        auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_SpanFullWidth;
        if (entity.numChildren() == 0) {
            treeNodeFlags = treeNodeFlags | ImGuiTreeNodeFlags_Leaf;
        }
        if (ImGui::TreeNodeEx(tagStr.c_str(), treeNodeFlags, "%s", tagStr.c_str())) {
            if (ImGui::IsItemClicked()) {
                std::cout << "TODO: show entity info in inspector" << std::endl;
            }
            Entity child = entity.getComponent<Component::HierarchyComponent>().first;
            while (child) {
                this->renderSceneViewEntity(child);
                child = child.getComponent<Component::HierarchyComponent>().next;
            }
            ImGui::TreePop();
        }
    }
}
