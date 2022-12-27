/**********************************************************************************
 *  Dream is a software for developing real-time 3D experiences.
 *  Copyright (C) 2023 Deepak Ramalignam
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **********************************************************************************/

#include "dream/editor/ImGuiEditorInspectorView.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "dream/scene/component/Component.h"
#include "dream/project/Project.h"
#include "dream/util/SceneUtils.h"
#include "dream/util/IDUtils.h"
#include "dream/util/Logger.h"
#include "dream/renderer/OpenGLTexture.h"
#include "dream/util/StringUtils.h"

namespace Dream {
    ImGuiEditorInspectorView::ImGuiEditorInspectorView() {
        selectedEntity = Entity();
        meshSelectorBrowser = nullptr;
        luaScriptSelectorBrowser = nullptr;

        // TODO: use specific renderer (not OpenGL)
        Texture *selectIconTexture = new OpenGLTexture(
                std::filesystem::current_path().append("resources").append("editor-resources").append("icons").append(
                        "SelectIconWhite.png"), false);
        selectIcon = selectIconTexture->ID();
        delete selectIconTexture;

        // TODO: use specific renderer (not OpenGL)
        Texture *editIconTexture = new OpenGLTexture(
                std::filesystem::current_path().append("resources").append("editor-resources").append("icons").append(
                        "EditIconDark.png"), false);
        editIcon = editIconTexture->ID();
        delete editIconTexture;
    }

    ImGuiEditorInspectorView::~ImGuiEditorInspectorView() {
        delete meshSelectorBrowser;
        delete luaScriptSelectorBrowser;
    }

    void ImGuiEditorInspectorView::setTextEditor(ImGuiTextEditor *imGuiTextEditor) {
        this->imGuiTextEditor = imGuiTextEditor;
    }

    void ImGuiEditorInspectorView::setAnimatorGraphEditor(ImGuiEditorAnimatorGraph *animatorGraphEditor) {
        this->animatorGraphEditor = animatorGraphEditor;
    }

    void ImGuiEditorInspectorView::update() {
        if (selectedEntity) {
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
                renderBoneComponent();
                renderSceneCameraComponent();
                renderCameraComponent();
                renderAddComponent();
                renderRemoveComponent();
            }
            ImGui::End();
        }
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
            } else if (componentID == Component::SceneCameraComponent::componentName) {
                selectedEntity.addComponent<Component::SceneCameraComponent>(45.0f);
                selectedEntity.getComponent<Component::TransformComponent>().rotation = {0, 0, -0.707, 0.707};
            } else if (componentID == Component::CameraComponent::componentName) {
                selectedEntity.addComponent<Component::CameraComponent>(45.0f);
            } else {
                Logger::fatal("Unknown component to add " + componentID);
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

        if (!selectedEntity.hasComponent<Component::SceneCameraComponent>() && !Project::getScene()->getSceneCamera()) {
            components.insert(std::make_pair("Scene Camera", Component::SceneCameraComponent::componentName));
        }

        if (!selectedEntity.hasComponent<Component::CameraComponent>()) {
            components.insert(std::make_pair("Camera", Component::CameraComponent::componentName));
        }

        if (!selectedEntity.hasComponent<Component::RootComponent>() && !components.empty()) {
            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
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


    void ImGuiEditorInspectorView::lookAtSelectedEntity() {
        if (selectedEntity) {
            Entity sceneCamera = Project::getScene()->getSceneCamera();
            if (sceneCamera) {
                auto selectedTrans = selectedEntity.getComponent<Component::TransformComponent>().translation;
                glm::vec3 offset = {2, -2, 0};
                glm::vec3 newPos = glm::vec3(-1 * selectedTrans.x, -1 * selectedTrans.y, selectedTrans.z) + offset;
                glm::vec3 lookAtPos = glm::vec3(-1 * selectedTrans.x, -1 * selectedTrans.y, selectedTrans.z);
                sceneCamera.getComponent<Component::TransformComponent>().translation = newPos;
                sceneCamera.getComponent<Component::SceneCameraComponent>().lookAt(sceneCamera, lookAtPos);
            }
        }
    }

    void ImGuiEditorInspectorView::renderVec3Control(const std::string &label, glm::vec3 &values, float contentWidth,
                                                     float resetValue, float vSpeed, std::pair<float, float> vMinMax1,
                                                     std::pair<float, float> vMinMax2,
                                                     std::pair<float, float> vMinMax3) {
        ImGuiIO &io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        ImGui::Text("%s", label.c_str());
        ImGui::SameLine();
        float inputsWidth = 180.0f;
        ImGui::SetCursorPosX(contentWidth - 100 - inputsWidth + 50);

        ImGui::PushMultiItemsWidths(3, inputsWidth);

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

        ImGui::Text("X");

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, vSpeed, vMinMax1.first, vMinMax1.second, "%.3f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::Text("Y");

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, vSpeed, vMinMax2.first, vMinMax2.second, "%.3f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::Text("Z");

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, vSpeed, vMinMax3.first, vMinMax3.second, "%.3f");
        ImGui::PopItemWidth();

        ImGui::PopID();
    }

    void ImGuiEditorInspectorView::renderTagComponent() {
        if (selectedEntity.hasComponent<Component::TagComponent>()) {
            auto &component = selectedEntity.getComponent<Component::TagComponent>();
            auto cursorPosX1 = ImGui::GetCursorPosX();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Tag",
                                                  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |
                                                  ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::SameLine();
            ImGui::Text("Tag");

            if (treeNodeOpen) {
                auto cursorPosX2 = ImGui::GetCursorPosX();
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1));
                ImGui::InputTextWithHint("##Tag", "Tag", &component.tag);
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderTransformComponent() {
        if (selectedEntity.hasComponent<Component::TransformComponent>()) {
            auto &component = selectedEntity.getComponent<Component::TransformComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Transform",
                                                  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |
                                                  ImGuiTreeNodeFlags_AllowItemOverlap);
            auto cursorPosX1 = ImGui::GetCursorPosX();

            ImGui::SameLine();
            ImGui::Text("Transform");

            if (treeNodeOpen) {
                auto cursorPosX2 = ImGui::GetCursorPosX();
                float contentWidth = ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1);
                renderVec3Control("Position", component.translation, contentWidth, 0.0f, 0.1);
                glm::vec3 eulerRot = glm::eulerAngles(component.rotation);
                renderVec3Control("Rotation", eulerRot, contentWidth, 0.0f, 0.1, {0, 0}, {-3.14 / 2, 3.14 / 2});
                component.rotation = glm::quat(eulerRot);
                renderVec3Control("Scale", component.scale, contentWidth, 0.0f, 0.1);
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
            auto cursorPosX1 = ImGui::GetCursorPosX();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Mesh",
                                                  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |
                                                  ImGuiTreeNodeFlags_AllowItemOverlap);
            bool canChangeMesh = component.fileId.empty();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::SameLine();
            ImGui::Text(canChangeMesh ? "Model" : "Mesh");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 5);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
            if (ImGui::Button("X", ImVec2(0.f, 0.f))) {
                SceneUtils::removeMeshReference(selectedEntity,
                                                selectedEntity.getComponent<Component::MeshComponent>().guid, true);
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            if (treeNodeOpen) {
                auto cursorPosX2 = ImGui::GetCursorPosX();
                // dropdown to allow user to change mesh type
                std::string dropdownPreview;

                if (component.meshType == Component::MeshComponent::MeshType::FROM_FILE) {
                    dropdownPreview = "File";
                } else if (component.meshType == Component::MeshComponent::MeshType::PRIMITIVE_SPHERE) {
                    dropdownPreview = "Sphere";
                } else if (component.meshType == Component::MeshComponent::MeshType::PRIMITIVE_CUBE) {
                    dropdownPreview = "Cube";
                }

                if (!(component.meshType == Component::MeshComponent::FROM_FILE && !component.fileId.empty())) {
                    ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1));
                    if (ImGui::BeginCombo("##Change Mesh Type", dropdownPreview.c_str())) {
                        bool alreadyChangedMeshType = false;
                        if (!alreadyChangedMeshType && dropdownPreview != "File" && ImGui::Selectable("File")) {
                            selectedEntity.getComponent<Component::MeshComponent>().changeMeshType(
                                    Component::MeshComponent::FROM_FILE, selectedEntity);
                            alreadyChangedMeshType = true;
                        }
                        if (!alreadyChangedMeshType && dropdownPreview != "Sphere" && ImGui::Selectable("Sphere")) {
                            selectedEntity.getComponent<Component::MeshComponent>().changeMeshType(
                                    Component::MeshComponent::PRIMITIVE_SPHERE, selectedEntity);
                            alreadyChangedMeshType = true;
                        }
                        if (!alreadyChangedMeshType && dropdownPreview != "Cube" && ImGui::Selectable("Cube")) {
                            selectedEntity.getComponent<Component::MeshComponent>().changeMeshType(
                                    Component::MeshComponent::PRIMITIVE_CUBE, selectedEntity);
                            alreadyChangedMeshType = true;
                        }
                        ImGui::EndCombo();
                    }
                }

                // allow user to select mesh file
                if (component.meshType == Component::MeshComponent::MeshType::FROM_FILE) {
                    std::string meshPath = StringUtils::getFilePathRelativeToProjectFolder(
                            Project::getResourceManager()->getFilePathFromGUID(component.guid));
                    ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() +
                                            (canChangeMesh ? (-18.f - cursorPosX2) : -(cursorPosX2 - cursorPosX1)));
                    ImGui::InputText(std::string("##MeshPath").c_str(), &meshPath, ImGuiInputTextFlags_ReadOnly);
                    if (canChangeMesh) {
                        ImGui::SameLine();
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
                        if (ImGui::ImageButton("##Select Mesh", (void *) (intptr_t) selectIcon, ImVec2(18, 18))) {
                            delete meshSelectorBrowser;
                            meshSelectorBrowser = new ImGui::FileBrowser();
                            meshSelectorBrowser->SetTitle("select mesh");
                            meshSelectorBrowser->SetPwd(Project::getPath());
                            meshSelectorBrowser->Open();
                            oldMeshGUID = component.guid;
                        }
                        ImGui::PopStyleVar();
                        ImGui::PopStyleColor();
                    }
                }

                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderMaterialComponent() {
        if (selectedEntity.hasComponent<Component::MaterialComponent>()) {
            float cursorPosX1 = ImGui::GetCursorPosX();
            auto &component = selectedEntity.getComponent<Component::MaterialComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Material",
                                                  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |
                                                  ImGuiTreeNodeFlags_AllowItemOverlap);

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
                float cursorPosX2 = ImGui::GetCursorPosX();
                std::string diffuseTexturePath = StringUtils::getFilePathRelativeToProjectFolder(
                        Project::getResourceManager()->getFilePathFromGUID(component.guid));
                ImGui::Text("Diffuse Texture");
                float cursorPosX3 = ImGui::GetCursorPosX();
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - (cursorPosX2) - 18);
                ImGui::InputText("##DiffuseTexturePath", &diffuseTexturePath, ImGuiInputTextFlags_ReadOnly);
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
                if (ImGui::ImageButton("##Select Material", (void *) (intptr_t) selectIcon, ImVec2(18, 18))) {
                    Logger::debug("TODO: allow selection of diffuse texture"); // TODO
                }
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderLuaScriptComponent() {
        if (selectedEntity.hasComponent<Component::LuaScriptComponent>()) {
            auto cursorPosX1 = ImGui::GetCursorPosX();

            if (luaScriptSelectorBrowser) {
                luaScriptSelectorBrowser->Display();
                if (luaScriptSelectorBrowser->HasSelected()) {
                    std::filesystem::path selectedFilePath = luaScriptSelectorBrowser->GetSelected();
                    selectedEntity.getComponent<Component::LuaScriptComponent>().changeScript(selectedFilePath);
                    luaScriptSelectorBrowser->ClearSelected();
                }
            }

            auto &component = selectedEntity.getComponent<Component::LuaScriptComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Lua Script",
                                                  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |
                                                  ImGuiTreeNodeFlags_AllowItemOverlap);

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
                auto cursorPosX2 = ImGui::GetCursorPosX();
                if (component.guid.empty()) {
                    ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - (cursorPosX2) - 18);
                    std::string noneText = "None";
                    ImGui::InputText("##LuaScriptPath", &noneText, ImGuiInputTextFlags_ReadOnly);

                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
                    if (ImGui::ImageButton("##Select Lua Script", (void *) (intptr_t) selectIcon, ImVec2(18, 18))) {
                        delete luaScriptSelectorBrowser;
                        luaScriptSelectorBrowser = new ImGui::FileBrowser();
                        luaScriptSelectorBrowser->SetTitle("select script");
                        luaScriptSelectorBrowser->SetPwd(Project::getPath());
                        luaScriptSelectorBrowser->Open();
                    }
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor();
                } else {
                    std::string scriptPath = Project::getResourceManager()->getFilePathFromGUID(component.guid);
                    std::string shortScriptPath = StringUtils::getFilePathRelativeToProjectFolder(scriptPath);
                    ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - (cursorPosX2) - 2 * 22);
                    ImGui::InputText("##LuaScriptPath", &shortScriptPath, ImGuiInputTextFlags_ReadOnly);

                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
                    if (ImGui::ImageButton("##Change Lua Script", (void *) (intptr_t) selectIcon, ImVec2(18, 18))) {
                        delete luaScriptSelectorBrowser;
                        luaScriptSelectorBrowser = new ImGui::FileBrowser();
                        luaScriptSelectorBrowser->SetTitle("change script");
                        luaScriptSelectorBrowser->SetPwd(Project::getPath());
                        luaScriptSelectorBrowser->Open();
                    }
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor();

                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
                    if (ImGui::ImageButton("##Edit Lua Script", (void *) (intptr_t) editIcon, ImVec2(18, 18))) {
                        this->imGuiTextEditor->open(scriptPath);
                    }
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor();
                }
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderAnimatorComponent() {
        auto cursorPosX1 = ImGui::GetCursorPosX();

        if (selectedEntity.hasComponent<Component::AnimatorComponent>()) {
            auto &component = selectedEntity.getComponent<Component::AnimatorComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Animator",
                                                  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |
                                                  ImGuiTreeNodeFlags_AllowItemOverlap);

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
                auto cursorPosX2 = ImGui::GetCursorPosX();
                std::string animatorPath = Project::getResourceManager()->getFilePathFromGUID(component.guid);
                std::string shortAnimatorPath = StringUtils::getFilePathRelativeToProjectFolder(animatorPath);
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - (cursorPosX2) - 2 * 22);
                ImGui::InputText("##AnimatorPath", &shortAnimatorPath, ImGuiInputTextFlags_ReadOnly);

                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
                if (ImGui::ImageButton("##Change Animator", (void *) (intptr_t) selectIcon, ImVec2(18, 18))) {
                    Logger::fatal("NOT IMPLEMENTED: allow animator to be changed");
                }
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();

                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
                if (ImGui::ImageButton("##Edit Animator", (void *) (intptr_t) editIcon, ImVec2(18, 18))) {
                    this->animatorGraphEditor->open(component.guid);
                }
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderBoneComponent() {
        if (selectedEntity.hasComponent<Component::BoneComponent>()) {
            auto &component = selectedEntity.getComponent<Component::BoneComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Bone",
                                                  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |
                                                  ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::SameLine();
            ImGui::Text("Bone");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 5);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
            if (ImGui::Button("X", ImVec2(0.f, 0.f))) {
                selectedEntity.removeComponent<Component::BoneComponent>();
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            if (treeNodeOpen) {
                ImGui::Text("ID");
                ImGui::SameLine();
                ImGui::Text("%d", component.boneID);

                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderSceneCameraComponent() {
        if (selectedEntity.hasComponent<Component::SceneCameraComponent>()) {
            auto &component = selectedEntity.getComponent<Component::SceneCameraComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Scene Camera",
                                                  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |
                                                  ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::SameLine();
            ImGui::Text("Scene Camera");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 5);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
            if (ImGui::Button("X", ImVec2(0.f, 0.f))) {
                selectedEntity.removeComponent<Component::SceneCameraComponent>();
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            if (treeNodeOpen) {
                ImGui::Text("Fov");
                ImGui::SameLine();
                ImGui::Text("%s", std::to_string(component.fov).c_str());
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderCameraComponent() {
        if (selectedEntity.hasComponent<Component::CameraComponent>()) {
            auto &component = selectedEntity.getComponent<Component::CameraComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Camera",
                                                  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |
                                                  ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::SameLine();
            ImGui::Text("Camera");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 5);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
            if (ImGui::Button("X", ImVec2(0.f, 0.f))) {
                selectedEntity.removeComponent<Component::CameraComponent>();
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            if (treeNodeOpen) {
                ImGui::Text("Fov");
                ImGui::SameLine();
                ImGui::Text("%s", std::to_string(component.fov).c_str());
                ImGui::TreePop();
            }
        }
    }

    Entity ImGuiEditorInspectorView::getSelectedEntity() {
        return selectedEntity;
    }

    void ImGuiEditorInspectorView::unselectEntity() {
        this->selectedEntity = Entity();
    }
}