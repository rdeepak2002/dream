//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/editor/ImGuiEditorInspectorView.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include "dream/scene/component/Component.h"
#include "dream/project/Project.h"
#include "dream/util/SceneUtils.h"
#include "dream/util/IDUtils.h"

namespace Dream {
    ImGuiEditorInspectorView::ImGuiEditorInspectorView() {
        selectedEntity = Entity();
        meshSelectorBrowser = nullptr;
    }

    ImGuiEditorInspectorView::~ImGuiEditorInspectorView() {
        delete meshSelectorBrowser;
    }

    void ImGuiEditorInspectorView::setTextEditor(ImGuiTextEditor *imGuiTextEditor) {
        this->imGuiTextEditor = imGuiTextEditor;
    }

    void ImGuiEditorInspectorView::setAnimatorGraphEditor(ImGuiEditorAnimatorGraph* animatorGraphEditor) {
        this->animatorGraphEditor = animatorGraphEditor;
    }

    void ImGuiEditorInspectorView::update() {
        ImGuiWindowClass inspector_window_class;
        inspector_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&inspector_window_class);
        ImGui::Begin("Inspector");
        if (selectedEntity) {
            renderTagComponent();
            renderTransformComponent();
            renderMeshComponent();
            renderMaterialComponent();
            renderLuaScriptComponent();
            renderAnimatorComponent();
            renderAddComponent();
            renderRemoveComponent();
        }
        ImGui::End();
    }

    void ImGuiEditorInspectorView::addComponent(std::string componentID) {
        if (selectedEntity) {
            if (componentID == Component::MeshComponent::componentName) {
                std::string guid;
                selectedEntity.addComponent<Component::MeshComponent>(guid);
            } else if (componentID == Component::MaterialComponent::componentName) {
                selectedEntity.addComponent<Component::MaterialComponent>("", false);
            } else if (componentID == Component::LuaScriptComponent::componentName) {
                selectedEntity.addComponent<Component::LuaScriptComponent>("");
            } else if (componentID == Component::AnimatorComponent::componentName) {
                selectedEntity.addComponent<Component::AnimatorComponent>();
            }
        }
    }

    void ImGuiEditorInspectorView::renderRemoveComponent() {
        if (ImGui::Button("Remove Entity", ImVec2(ImGui::GetWindowContentRegionWidth(), 0.0f))) {
            Project::getScene()->removeEntity(selectedEntity);
            selectedEntity = Entity();
        }
    }

    void ImGuiEditorInspectorView::renderAddComponent() {
        std::map<std::string, std::string> components;

        if (!selectedEntity.hasComponent<Component::MeshComponent>()) {
            components.insert(std::make_pair("Mesh", Component::MeshComponent::componentName));
        }

        if (!selectedEntity.hasComponent<Component::MaterialComponent>()) {
            components.insert(std::make_pair("Material", Component::MaterialComponent::componentName));
        }

        if (!selectedEntity.hasComponent<Component::LuaScriptComponent>()) {
            components.insert(std::make_pair("Lua Script", Component::LuaScriptComponent::componentName));
        }

        if (!selectedEntity.hasComponent<Component::AnimatorComponent>()) {
            components.insert(std::make_pair("Animator", Component::AnimatorComponent::componentName));
        }

        if (!selectedEntity.hasComponent<Component::RootComponent>() && !components.empty()) {
            if (ImGui::BeginCombo("##Add Component", "Add Component")) {
                for (auto it = components.begin(); it != components.end(); it++) {
                    if (ImGui::Selectable(it->first.c_str())) {
                        addComponent(it->second);
                    }
                }
                ImGui::EndCombo();
            }
        }
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

    void ImGuiEditorInspectorView::renderTagComponent() {
        if (selectedEntity.hasComponent<Component::TagComponent>()) {
            auto &component = selectedEntity.getComponent<Component::TagComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Tag", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::SameLine();
            ImGui::Text("Tag");

            if (treeNodeOpen) {
                ImGui::InputTextWithHint("##Tag", "Tag", &component.tag);
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderTransformComponent() {
        if (selectedEntity.hasComponent<Component::TransformComponent>()) {
            auto &component = selectedEntity.getComponent<Component::TransformComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::SameLine();
            ImGui::Text("Transform");

            if (treeNodeOpen) {
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
            if (meshSelectorBrowser) {
                meshSelectorBrowser->Display();
                if (meshSelectorBrowser->HasSelected()) {
                    std::filesystem::path selectedFilePath = meshSelectorBrowser->GetSelected();
                    // get GUID of new mesh asset
                    std::string newMeshGUID = IDUtils::getGUIDForFile(selectedFilePath);
                    if (newMeshGUID != oldMeshGUID) {
                        // load new mesh and add sub-meshes as children
                        Project::getAssetLoader()->loadMesh(newMeshGUID, true, selectedEntity);
                        // remove reference to old meshes for all children
                        SceneUtils::removeMeshReference(selectedEntity, oldMeshGUID, true);
                        meshSelectorBrowser->ClearSelected();
                    }
                }
            }

            auto &component = selectedEntity.getComponent<Component::MeshComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Mesh", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::SameLine();
            ImGui::Text("Mesh");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 5);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
            if (ImGui::Button("X", ImVec2(0.f, 0.f))) {
                SceneUtils::removeMeshReference(selectedEntity, selectedEntity.getComponent<Component::MeshComponent>().guid, true);
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            if (treeNodeOpen) {
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
                    if (canChangeMesh) {
                        ImGui::SameLine();
                        if (ImGui::Button("Select")) {
                            delete meshSelectorBrowser;
                            meshSelectorBrowser = new ImGui::FileBrowser();
                            meshSelectorBrowser->SetTitle("select mesh");
                            meshSelectorBrowser->SetPwd(Project::getPath());
                            meshSelectorBrowser->Open();
                            oldMeshGUID = component.guid;
                        }
                    }
                }

                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderMaterialComponent() {
        if (selectedEntity.hasComponent<Component::MaterialComponent>()) {
            auto &component = selectedEntity.getComponent<Component::MaterialComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Material", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::SameLine();
            ImGui::Text("Material");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 5);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
            if (ImGui::Button("X", ImVec2(0.f, 0.f))) {
                selectedEntity.removeComponent<Component::MaterialComponent>();
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            if (treeNodeOpen) {
                std::string diffuseTexturePath = shorten(Project::getResourceManager()->getFilePathFromGUID(component.guid));
                ImGui::Text("Diffuse Texture");
                ImGui::SameLine();
                if (component.guid.empty()) {
                    if (ImGui::Button("Select")) {
                        std::cout << "TODO: allow selection of diffuse texture" << std::endl;
                    }
                } else {
                    ImGui::Text("%s", diffuseTexturePath.c_str());
                }
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderLuaScriptComponent() {
        if (selectedEntity.hasComponent<Component::LuaScriptComponent>()) {
            auto &component = selectedEntity.getComponent<Component::LuaScriptComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Lua Script", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::SameLine();
            ImGui::Text("Lua Script");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 5);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
            if (ImGui::Button("X", ImVec2(0.f, 0.f))) {
                selectedEntity.removeComponent<Component::LuaScriptComponent>();
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            if (treeNodeOpen) {
                std::string scriptPath = Project::getResourceManager()->getFilePathFromGUID(component.guid);
                std::string shortScriptPath = shorten(scriptPath);
                ImGui::Text("Script Path");
                ImGui::SameLine();
                ImGui::Text("%s", shortScriptPath.c_str());
                if (ImGui::Button("Edit Script")) {
                    this->imGuiTextEditor->open(scriptPath);
                }
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderAnimatorComponent() {
        if (selectedEntity.hasComponent<Component::AnimatorComponent>()) {
            auto &component = selectedEntity.getComponent<Component::AnimatorComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Animator", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::SameLine();
            ImGui::Text("Animator");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 5);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
            if (ImGui::Button("X", ImVec2(0.f, 0.f))) {
                selectedEntity.removeComponent<Component::AnimatorComponent>();
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            if (treeNodeOpen) {
                if (ImGui::Button("Edit")) {
                    this->animatorGraphEditor->open("dummy_guid");
                }
                ImGui::TreePop();
            }
        }
    }
}