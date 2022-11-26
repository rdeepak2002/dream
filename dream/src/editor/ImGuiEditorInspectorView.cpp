//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/editor/ImGuiEditorInspectorView.h"

#include <imgui.h>
#include <imgui_internal.h>
#include "dream/scene/component/Component.h"
#include "dream/project/Project.h"

namespace Dream {
    ImGuiEditorInspectorView::ImGuiEditorInspectorView() {
        selectedEntity = Entity();
    }

    void ImGuiEditorInspectorView::update() {
        ImGuiWindowClass inspector_window_class;
        inspector_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&inspector_window_class);
        ImGui::Begin("Inspector");
        if (selectedEntity) {
            ImGui::Text("%s", selectedEntity.getComponent<Component::TagComponent>().tag.c_str());
            renderMaterialComponent();
            if (ImGui::Button("Remove")) {
                Project::getScene()->removeEntity(selectedEntity);
                selectedEntity = Entity();
                ImGui::End();
                return;
            }
        }
        ImGui::End();
    }

    void ImGuiEditorInspectorView::selectEntity(Entity &entity) {
        selectedEntity = entity;
    }

    std::string ImGuiEditorInspectorView::shorten(std::string str, int maxLength) {
        if (str.length() > maxLength) {
            return "..." + str.substr(str.length() - maxLength, str.length());
        }
        return str;
    }

    void ImGuiEditorInspectorView::renderMaterialComponent() {
        if (selectedEntity.hasComponent<Component::MaterialComponent>()) {
            auto &component = selectedEntity.getComponent<Component::MaterialComponent>();
            auto componentName = Component::MaterialComponent::componentName.c_str();
            if (ImGui::TreeNodeEx(componentName, ImGuiTreeNodeFlags_DefaultOpen, "%s", "MATERIAL")) {
                std::string diffuseTexturePath = shorten(component.diffuseTexture->path.substr());
                ImGui::Text("Diffuse Texture");
                ImGui::SameLine();
                ImGui::Text("%s", diffuseTexturePath.c_str());
                ImGui::TreePop();
            }
        }
    }
}