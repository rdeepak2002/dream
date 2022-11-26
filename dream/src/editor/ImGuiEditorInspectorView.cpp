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
            renderTagComponent();
//            renderRootComponent();
//            renderIDComponent();
//            renderHierarchyComponent();
            renderTransformComponent();
            renderMeshComponent();
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

    void ImGuiEditorInspectorView::renderRootComponent() {
        if (selectedEntity.hasComponent<Component::RootComponent>()) {
            auto &component = selectedEntity.getComponent<Component::RootComponent>();
            auto componentName = Component::RootComponent::componentName.c_str();
            if (ImGui::TreeNodeEx(componentName, ImGuiTreeNodeFlags_DefaultOpen, "%s", "ROOT")) {
                std::string name = component.name;
                ImGui::Text("Name");
                ImGui::SameLine();
                ImGui::Text("%s", name.c_str());
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderIDComponent() {
        if (selectedEntity.hasComponent<Component::IDComponent>()) {
            auto &component = selectedEntity.getComponent<Component::IDComponent>();
            auto componentName = Component::IDComponent::componentName.c_str();
            if (ImGui::TreeNodeEx(componentName, ImGuiTreeNodeFlags_DefaultOpen, "%s", "ID")) {
                std::string id = component.getID();
                ImGui::Text("ID");
                ImGui::SameLine();
                ImGui::Text("%s", id.c_str());
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderTagComponent() {
        ImGui::Text("%s", selectedEntity.getComponent<Component::TagComponent>().tag.c_str());
    }

    void ImGuiEditorInspectorView::renderHierarchyComponent() {

    }


    void ImGuiEditorInspectorView::renderTransformComponent() {
        if (selectedEntity.hasComponent<Component::TransformComponent>()) {
            auto &component = selectedEntity.getComponent<Component::TransformComponent>();
            auto componentName = Component::TransformComponent::componentName.c_str();
            if (ImGui::TreeNodeEx(componentName, ImGuiTreeNodeFlags_DefaultOpen, "%s", "TRANSFORM")) {
                // TODO: editable vector 3 fields
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderMeshComponent() {
        if (selectedEntity.hasComponent<Component::MeshComponent>()) {
            auto &component = selectedEntity.getComponent<Component::MeshComponent>();
            auto componentName = Component::MeshComponent::componentName.c_str();
            if (ImGui::TreeNodeEx(componentName, ImGuiTreeNodeFlags_DefaultOpen, "%s", "MESH")) {
//                bool canChangeMesh = component.fileId.empty();
                if (component.meshType == Component::MeshComponent::MeshType::FROM_FILE) {
                    ImGui::Text("Type");
                    ImGui::SameLine();
                    ImGui::Text("%s", "From file");
                } else if (component.meshType == Component::MeshComponent::MeshType::PRIMITIVE_SPHERE) {
                    ImGui::Text("Type");
                    ImGui::SameLine();
                    ImGui::Text("%s", "Sphere");
                } else if (component.meshType == Component::MeshComponent::MeshType::PRIMITIVE_CUBE) {
                    ImGui::Text("Type");
                    ImGui::SameLine();
                    ImGui::Text("%s", "Cube");
                }

                if (component.meshType == Component::MeshComponent::MeshType::FROM_FILE) {
                    std::string meshPath = shorten(Project::getResourceManager()->getFilePathFromGUID(component.guid));
                    ImGui::Text("Path");
                    ImGui::SameLine();
                    ImGui::Text("%s", meshPath.c_str());
                }

                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderMaterialComponent() {
        if (selectedEntity.hasComponent<Component::MaterialComponent>()) {
            auto &component = selectedEntity.getComponent<Component::MaterialComponent>();
            auto componentName = Component::MaterialComponent::componentName.c_str();
            if (ImGui::TreeNodeEx(componentName, ImGuiTreeNodeFlags_DefaultOpen, "%s", "MATERIAL")) {
                std::string diffuseTexturePath = shorten(Project::getResourceManager()->getFilePathFromGUID(component.guid));
                ImGui::Text("Diffuse Texture");
                ImGui::SameLine();
                ImGui::Text("%s", diffuseTexturePath.c_str());
                ImGui::TreePop();
            }
        }
    }
}