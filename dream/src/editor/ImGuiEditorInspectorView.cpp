//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/editor/ImGuiEditorInspectorView.h"

#include <imgui.h>
#include <imgui_internal.h>
#include "dream/scene/component/Component.h"
#include "dream/project/Project.h"
#include "dream/util/SceneUtils.h"

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

    void ImGuiEditorInspectorView::renderVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth) {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();
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
        if (selectedEntity.hasComponent<Component::TagComponent>()) {
            auto &component = selectedEntity.getComponent<Component::TagComponent>();
            auto componentName = Component::TagComponent::componentName.c_str();
            if (ImGui::TreeNodeEx(componentName, ImGuiTreeNodeFlags_DefaultOpen, "%s", "TAG")) {
                std::string tag = component.tag;
                ImGui::Text("Tag");
                ImGui::SameLine();
                ImGui::Text("%s", tag.c_str());
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderHierarchyComponent() {

    }


    void ImGuiEditorInspectorView::renderTransformComponent() {
        if (selectedEntity.hasComponent<Component::TransformComponent>()) {
            auto &component = selectedEntity.getComponent<Component::TransformComponent>();
            auto componentName = Component::TransformComponent::componentName.c_str();
            if (ImGui::TreeNodeEx(componentName, ImGuiTreeNodeFlags_DefaultOpen, "%s", "TRANSFORM")) {
                renderVec3Control("Position", component.translation);
                glm::vec3 eulerRot = glm::eulerAngles(component.rotation);
                renderVec3Control("Rotation", eulerRot);
                component.rotation = glm::quat(eulerRot);
                renderVec3Control("Scale", component.scale);
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderMeshComponent() {
        if (selectedEntity.hasComponent<Component::MeshComponent>()) {
            auto &component = selectedEntity.getComponent<Component::MeshComponent>();
            auto componentName = Component::MeshComponent::componentName.c_str();
            if (ImGui::TreeNodeEx(componentName, ImGuiTreeNodeFlags_DefaultOpen, "%s", "MESH")) {
                bool canChangeMesh = component.fileId.empty();
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
                    ImGui::SameLine();
                    if (canChangeMesh && ImGui::Button("Select")) {
                        std::string oldMeshGUID = component.guid;
                        // get GUID of new mesh asset
                        // TODO: file opener to select new mesh and get its GUID
                        std::cout << "TODO: open file selector to choose new mesh" << std::endl;
                        std::string newMeshGUID = "EC05BB83-F2E3-4A2C-8B5E-2C513D14B93D";
                        // load new mesh and add sub-meshes as children
                        Project::getAssetLoader()->loadMesh(newMeshGUID, true, selectedEntity);
                        // remove reference to old meshes for all children
                        SceneUtils::removeMeshReference(selectedEntity, oldMeshGUID, true);
                    }
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