//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/editor/ImGuiEditorInspectorView.h"

#include <imgui.h>
#include <imgui_internal.h>
#include "dream/scene/component/Component.h"
#include "dream/project/Project.h"

Dream::ImGuiEditorInspectorView::ImGuiEditorInspectorView() {
    selectedEntity = Entity();
}

void Dream::ImGuiEditorInspectorView::update() {
    ImGuiWindowClass inspector_window_class;
    inspector_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
    ImGui::SetNextWindowClass(&inspector_window_class);
    ImGui::Begin("Inspector");
    if (selectedEntity) {
        ImGui::Text("%s", selectedEntity.getComponent<Component::TagComponent>().tag.c_str());
        if (selectedEntity.hasComponent<Component::MaterialComponent>()) {
            auto &component = selectedEntity.getComponent<Component::MaterialComponent>();
            if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen, "Material")) {
                ImGui::Text("%s", component.texture->path.c_str());
                ImGui::TreePop();
            }
        }
        if (ImGui::Button("Remove")) {
            Project::getScene()->removeEntity(selectedEntity);
            selectedEntity = Entity();
            ImGui::End();
            return;
        }
    }
    ImGui::End();
}

void Dream::ImGuiEditorInspectorView::selectEntity(Entity& entity) {
    selectedEntity = entity;
}
