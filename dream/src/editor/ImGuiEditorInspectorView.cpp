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
#include "dream/Application.h"

namespace Dream {
    ImGuiEditorInspectorView::ImGuiEditorInspectorView() {
        selectedEntity = Entity();
        meshSelectorBrowser = nullptr;
        luaScriptSelectorBrowser = nullptr;
        animatorSelectorBrowser = nullptr;
        collisionMeshSelectorBrowser = nullptr;
        colliderIndex = -1;

        // TODO: use specific renderer (not OpenGL)
        Texture *selectIconTexture = new OpenGLTexture(
                Application::getResourcesRoot().append("assets").append("icons").append(
                        "SelectIconWhite.png"), false);
        selectIcon = selectIconTexture->ID();
        delete selectIconTexture;

        // TODO: use specific renderer (not OpenGL)
        Texture *editIconTexture = new OpenGLTexture(
                Application::getResourcesRoot().append("assets").append("icons").append(
                        "EditIconDark.png"), false);
        editIcon = editIconTexture->ID();
        delete editIconTexture;
    }

    ImGuiEditorInspectorView::~ImGuiEditorInspectorView() {
        delete meshSelectorBrowser;
        delete luaScriptSelectorBrowser;
        delete animatorSelectorBrowser;
        delete collisionMeshSelectorBrowser;
    }

    void ImGuiEditorInspectorView::setTextEditor(ImGuiTextEditor *imGuiTextEditor) {
        this->imGuiTextEditor = imGuiTextEditor;
    }

    void ImGuiEditorInspectorView::setAnimatorGraphEditor(ImGuiEditorAnimatorGraph *animatorGraphEditor) {
        this->animatorGraphEditor = animatorGraphEditor;
    }

    void ImGuiEditorInspectorView::update() {
        if (selectedEntity && selectedEntity.isValid() && selectedEntity.hasComponent<Component::TagComponent>()) {
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
                renderCollisionComponent();
                renderRigidBodyComponent();
                renderLightComponent();
                renderTerrainComponent();
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
                selectedEntity.addComponent<Component::MaterialComponent>();
            } else if (componentID == Component::LuaScriptComponent::componentName) {
                selectedEntity.addComponent<Component::LuaScriptComponent>("");
            } else if (componentID == Component::AnimatorComponent::componentName) {
                selectedEntity.addComponent<Component::AnimatorComponent>();
            } else if (componentID == Component::SceneCameraComponent::componentName) {
                selectedEntity.addComponent<Component::SceneCameraComponent>(45.0f);
                selectedEntity.getComponent<Component::TransformComponent>().rotation = {0, 0, -0.707, 0.707};
            } else if (componentID == Component::CameraComponent::componentName) {
                selectedEntity.addComponent<Component::CameraComponent>(45.0f);
            } else if (componentID == Component::CollisionComponent::componentName) {
                selectedEntity.addComponent<Component::CollisionComponent>();
            } else if (componentID == Component::RigidBodyComponent::componentName) {
                if (!selectedEntity.hasComponent<Component::CollisionComponent>()) {
                    selectedEntity.addComponent<Component::CollisionComponent>();
                }
                selectedEntity.addComponent<Component::RigidBodyComponent>();
            } else if (componentID == Component::LightComponent::componentName) {
                selectedEntity.addComponent<Component::LightComponent>();
            } else if (componentID == Component::TerrainComponent::componentName) {
                selectedEntity.addComponent<Component::TerrainComponent>();
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

        if (!selectedEntity.hasComponent<Component::CollisionComponent>()) {
            components.insert(std::make_pair("Collision", Component::CollisionComponent::componentName));
        }

        if (!selectedEntity.hasComponent<Component::RigidBodyComponent>()) {
            components.insert(std::make_pair("Rigid Body", Component::RigidBodyComponent::componentName));
        }

        if (!selectedEntity.hasComponent<Component::LightComponent>()) {
            components.insert(std::make_pair("Light", Component::LightComponent::componentName));
        }

        if (!selectedEntity.hasComponent<Component::TerrainComponent>()) {
            components.insert(std::make_pair("Terrain", Component::TerrainComponent::componentName));
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
                glm::vec3 offset = {2, 2, 0};
                glm::vec3 newPos = glm::vec3(1 * selectedTrans.x, 1 * selectedTrans.y, selectedTrans.z) + offset;
                glm::vec3 lookAtPos = glm::vec3(1 * selectedTrans.x, 1 * selectedTrans.y, selectedTrans.z);
                sceneCamera.getComponent<Component::TransformComponent>().translation = newPos;
                sceneCamera.getComponent<Component::SceneCameraComponent>().lookAt(sceneCamera, lookAtPos);
            }
        }
    }

    bool ImGuiEditorInspectorView::renderVec3Control(const std::string &label, glm::vec3 &values, float contentWidth,
                                                     float resetValue, float vSpeed, int id,
                                                     std::pair<float, float> vMinMax1,
                                                     std::pair<float, float> vMinMax2,
                                                     std::pair<float, float> vMinMax3) {
        bool modified = false;
        ImGuiIO &io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID((label + "/" + std::to_string(id)).c_str());

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
        if (ImGui::IsItemEdited()) {
            modified = true;
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::Text("Y");

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, vSpeed, vMinMax2.first, vMinMax2.second, "%.3f");
        if (ImGui::IsItemEdited()) {
            modified = true;
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::Text("Z");

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, vSpeed, vMinMax3.first, vMinMax3.second, "%.3f");
        if (ImGui::IsItemEdited()) {
            modified = true;
        }
        ImGui::PopItemWidth();

        ImGui::PopID();
        return modified;
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
                renderVec3Control("Rotation", eulerRot, contentWidth, 0.0f, 0.1, 0, {0, 0}, {-3.14 / 2, 3.14 / 2});
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
                if (selectedEntity.getComponent<Component::MeshComponent>().meshType == Component::MeshComponent::MeshType::FROM_FILE) {
                    SceneUtils::removeMeshReference(selectedEntity, selectedEntity.getComponent<Component::MeshComponent>().guid, true);
                } else {
                    selectedEntity.removeComponent<Component::MeshComponent>();
                }
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
                } else if (component.meshType == Component::MeshComponent::MeshType::PRIMITIVE_QUAD) {
                    dropdownPreview = "Quad";
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
                        if (!alreadyChangedMeshType && dropdownPreview != "Quad" && ImGui::Selectable("Quad")) {
                            selectedEntity.getComponent<Component::MeshComponent>().changeMeshType(
                                    Component::MeshComponent::PRIMITIVE_QUAD, selectedEntity);
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

            // update file browsers
            {
                if (diffuseTextureBrowser) {
                    diffuseTextureBrowser->Display();
                    if (diffuseTextureBrowser->HasSelected()) {
                        std::filesystem::path selectedFilePath = diffuseTextureBrowser->GetSelected();
                        if (selectedEntity.getComponent<Component::MaterialComponent>().diffuseTextureGuids.empty()) {
                            selectedEntity.getComponent<Component::MaterialComponent>().diffuseTextureGuids.emplace_back("");
                        }
                        selectedEntity.getComponent<Component::MaterialComponent>().diffuseTextureGuids.at(0) = IDUtils::getGUIDForFile(selectedFilePath);
                        diffuseTextureBrowser->ClearSelected();
                    }
                }

                if (normalTextureBrowser) {
                    normalTextureBrowser->Display();
                    if (normalTextureBrowser->HasSelected()) {
                        std::filesystem::path selectedFilePath = normalTextureBrowser->GetSelected();
                        selectedEntity.getComponent<Component::MaterialComponent>().normalTextureGuid = IDUtils::getGUIDForFile(selectedFilePath);
                        normalTextureBrowser->ClearSelected();
                    }
                }

                if (specularTextureBrowser) {
                    specularTextureBrowser->Display();
                    if (specularTextureBrowser->HasSelected()) {
                        std::filesystem::path selectedFilePath = specularTextureBrowser->GetSelected();
                        selectedEntity.getComponent<Component::MaterialComponent>().specularTextureGuid = IDUtils::getGUIDForFile(selectedFilePath);
                        specularTextureBrowser->ClearSelected();
                    }
                }
            }

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
                // diffuse texture input
                {
                    std::string diffuseTexturePath = StringUtils::getFilePathRelativeToProjectFolder(
                            Project::getResourceManager()->getFilePathFromGUID(component.diffuseTextureGuids.empty() ? "" : component.diffuseTextureGuids.at(0)));
                    ImGui::Text("Diffuse Texture");
                    float cursorPosX3 = ImGui::GetCursorPosX();
                    ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - (cursorPosX2) - 18);
                    ImGui::InputText("##DiffuseTexturePath", &diffuseTexturePath, ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
                    if (ImGui::ImageButton("##Select Material Diffuse", (void *) (intptr_t) selectIcon, ImVec2(18, 18))) {
                        delete diffuseTextureBrowser;
                        diffuseTextureBrowser = new ImGui::FileBrowser();
                        diffuseTextureBrowser->SetTitle("select diffuse texture");
                        diffuseTextureBrowser->SetPwd(Project::getPath());
                        diffuseTextureBrowser->Open();
                    }
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor();
                }
                // normal texture input
                {
                    std::string diffuseTexturePath = StringUtils::getFilePathRelativeToProjectFolder(
                            Project::getResourceManager()->getFilePathFromGUID(component.normalTextureGuid));
                    ImGui::Text("Normal Texture");
                    float cursorPosX3 = ImGui::GetCursorPosX();
                    ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - (cursorPosX2) - 18);
                    ImGui::InputText("##NormalTexturePath", &diffuseTexturePath, ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
                    if (ImGui::ImageButton("##Select Material Normal", (void *) (intptr_t) selectIcon, ImVec2(18, 18))) {
                        delete normalTextureBrowser;
                        normalTextureBrowser = new ImGui::FileBrowser();
                        normalTextureBrowser->SetTitle("select normal texture");
                        normalTextureBrowser->SetPwd(Project::getPath());
                        normalTextureBrowser->Open();
                    }
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor();
                }
                // specular texture input
                {
                    std::string diffuseTexturePath = StringUtils::getFilePathRelativeToProjectFolder(
                            Project::getResourceManager()->getFilePathFromGUID(component.specularTextureGuid));
                    ImGui::Text("Specular Texture");
                    float cursorPosX3 = ImGui::GetCursorPosX();
                    ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - (cursorPosX2) - 18);
                    ImGui::InputText("##SpecularTexturePath", &diffuseTexturePath, ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
                    if (ImGui::ImageButton("##Select Material Specular", (void *) (intptr_t) selectIcon, ImVec2(18, 18))) {
                        delete specularTextureBrowser;
                        specularTextureBrowser = new ImGui::FileBrowser();
                        specularTextureBrowser->SetTitle("select specular texture");
                        specularTextureBrowser->SetPwd(Project::getPath());
                        specularTextureBrowser->Open();
                    }
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor();
                }
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

        if (animatorSelectorBrowser) {
            animatorSelectorBrowser->Display();
            if (animatorSelectorBrowser->HasSelected()) {
                std::filesystem::path selectedFilePath = animatorSelectorBrowser->GetSelected();
                selectedEntity.getComponent<Component::AnimatorComponent>().guid = IDUtils::getGUIDForFile(
                        selectedFilePath);
                selectedEntity.getComponent<Component::AnimatorComponent>().loadStateMachine(selectedEntity);
                animatorSelectorBrowser->ClearSelected();
            }
        }

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
                    delete animatorSelectorBrowser;
                    animatorSelectorBrowser = new ImGui::FileBrowser();
                    animatorSelectorBrowser->SetTitle("select animator");
                    animatorSelectorBrowser->SetPwd(Project::getPath());
                    animatorSelectorBrowser->Open();
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
            auto cursorPosX1 = ImGui::GetCursorPosX();

            if (treeNodeOpen) {
                auto cursorPosX2 = ImGui::GetCursorPosX();
                auto treeNodeWidth = ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1);
                // fov input
                {
                    auto cursorPosX3 = ImGui::GetCursorPosX();
                    ImGui::Text("Fov");
                    ImGui::SameLine();
                    float floatInputWidth = 100.0f;
                    ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                    ImGui::SetNextItemWidth(floatInputWidth);
                    ImGui::DragFloat("##SceneCameraComponentFov", &component.fov, 0.1f, 0.0f, 360.0f,
                                     "%.3f");
                }
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
            auto cursorPosX1 = ImGui::GetCursorPosX();

            if (treeNodeOpen) {
                auto cursorPosX2 = ImGui::GetCursorPosX();
                auto treeNodeWidth = ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1);
                // fov input
                {
                    auto cursorPosX3 = ImGui::GetCursorPosX();
                    ImGui::Text("Fov");
                    ImGui::SameLine();
                    float floatInputWidth = 100.0f;
                    ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                    ImGui::SetNextItemWidth(floatInputWidth);
                    ImGui::DragFloat("##CameraComponentFov", &component.fov, 0.1f, 0.0f, 360.0f,
                                     "%.3f");
                }
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderCollisionComponent() {
        if (collisionMeshSelectorBrowser) {
            collisionMeshSelectorBrowser->Display();
            if (collisionMeshSelectorBrowser->HasSelected()) {
                if (colliderIndex == -1) {
                    Logger::error("No collider to update assigned");
                } else {
                    std::filesystem::path selectedFilePath = collisionMeshSelectorBrowser->GetSelected();
                    selectedEntity.getComponent<Component::CollisionComponent>().colliders.at(
                            colliderIndex).assetGUID = IDUtils::getGUIDForFile(selectedFilePath);
                    collisionMeshSelectorBrowser->ClearSelected();
                    colliderIndex = -1;
                    updateColliderAndRigidBody();
                }
            }
        }

        if (selectedEntity.hasComponent<Component::CollisionComponent>()) {
            auto cursorPosX1 = ImGui::GetCursorPosX();
            auto &component = selectedEntity.getComponent<Component::CollisionComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Collision",
                                                  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |
                                                  ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::SameLine();
            ImGui::Text("Collision");
            if (!selectedEntity.hasComponent<Component::RigidBodyComponent>()) {
                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 5);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
                if (ImGui::Button("X", ImVec2(0.f, 0.f))) {
                    int idx = selectedEntity.getComponent<Component::CollisionComponent>().colliderShapeIndex;
                    if (idx != -1) {
                        Project::getScene()->getPhysicsComponentSystem()->deleteCollisionShape(idx);
                    }
                    selectedEntity.removeComponent<Component::CollisionComponent>();
                }
                ImGui::PopStyleVar();
            }
            ImGui::PopStyleColor();

            if (treeNodeOpen) {
                auto cursorPosX2 = ImGui::GetCursorPosX();
                auto treeNodeWidth = ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1);
                for (int i = 0; i < component.colliders.size(); ++i) {
                    auto &collider = component.colliders[i];
                    // type selector
                    {
                        std::string dropdownPreview = "";
                        if (collider.type == Component::CollisionComponent::BOX) {
                            dropdownPreview = "Box";
                        } else if (collider.type == Component::CollisionComponent::CAPSULE) {
                            dropdownPreview = "Capsule";
                        } else if (collider.type == Component::CollisionComponent::CONE) {
                            dropdownPreview = "Cone";
                        } else if (collider.type == Component::CollisionComponent::CYLINDER) {
                            dropdownPreview = "Cylinder";
                        } else if (collider.type == Component::CollisionComponent::MESH) {
                            dropdownPreview = "Mesh";
                        } else if (collider.type == Component::CollisionComponent::SPHERE) {
                            dropdownPreview = "Sphere";
                        } else if (collider.type == Component::CollisionComponent::HEIGHT_MAP) {
                            dropdownPreview = "Terrain";
                        } else {
                            Logger::fatal("Unknown collider type " + std::to_string(static_cast<int>(collider.type)));
                        }
                        ImGui::SetNextItemWidth(treeNodeWidth);
                        if (ImGui::BeginCombo(("##Change Collider Type /" + std::to_string(i)).c_str(),
                                              dropdownPreview.c_str())) {
                            Component::CollisionComponent::ColliderType oldColliderType = collider.type;
                            if (ImGui::Selectable("Box")) {
                                collider.type = Component::CollisionComponent::BOX;
                            }
                            if (ImGui::Selectable("Capsule")) {
                                collider.type = Component::CollisionComponent::CAPSULE;
                            }
                            if (ImGui::Selectable("Cone")) {
                                collider.type = Component::CollisionComponent::CONE;
                            }
                            if (ImGui::Selectable("Mesh")) {
                                collider.type = Component::CollisionComponent::MESH;
                            }
                            if (ImGui::Selectable("Sphere")) {
                                collider.type = Component::CollisionComponent::SPHERE;
                            }
                            if (ImGui::Selectable("Cylinder")) {
                                collider.type = Component::CollisionComponent::CYLINDER;
                            }
                            if (selectedEntity.hasComponent<Component::TerrainComponent>()) {
                                if (ImGui::Selectable("Terrain")) {
                                    if (!selectedEntity.hasComponent<Component::TerrainComponent>()) {
                                        Logger::error("Cannot add terrain collider to non-terrain entity");
                                    }
                                    collider.type = Component::CollisionComponent::HEIGHT_MAP;
                                    collider.offset = selectedEntity.getComponent<Component::TransformComponent>().translation;
                                    collider.offset.x *= -1;
                                    collider.offset.y += 100.0f;
                                    collider.offset.z *= -1;
                                }
                            }
                            if (collider.type != oldColliderType) {
                                updateColliderAndRigidBody();
                            }
                            ImGui::EndCombo();
                        }
                    }
                    // offset
                    {
                        bool modified = renderVec3Control("Offset", collider.offset, treeNodeWidth + 20, 1.0, 0.1, i);
                        if (modified) {
                            updateColliderAndRigidBody();
                        }
                    }
                    // half extents
                    {
                        if (collider.type == Component::CollisionComponent::BOX ||
                            collider.type == Component::CollisionComponent::CYLINDER) {
                            bool modified = renderVec3Control("Half Extents", collider.halfExtents, treeNodeWidth + 20, 1.0, 0.1, i);
                            if (modified) {
                                updateColliderAndRigidBody();
                            }
                        }
                    }
                    // axis selector
                    {
                        if (collider.type == Component::CollisionComponent::CAPSULE ||
                            collider.type == Component::CollisionComponent::CONE) {
                            auto cursorPosX3 = ImGui::GetCursorPosX();
                            ImGui::Text("Axis");
                            std::string capsuleAxis = "";
                            if (collider.axis == Component::CollisionComponent::X) {
                                capsuleAxis = "X";
                            } else if (collider.axis == Component::CollisionComponent::Y) {
                                capsuleAxis = "Y";
                            } else if (collider.axis == Component::CollisionComponent::Z) {
                                capsuleAxis = "Z";
                            }
                            ImGui::SameLine();
                            float axisComboWidth = 100.0f;
                            ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - axisComboWidth);
                            ImGui::SetNextItemWidth(axisComboWidth);
                            Component::CollisionComponent::Axis oldAxis = collider.axis;
                            if (ImGui::BeginCombo(("##Change Capsule Axis /" + std::to_string(i)).c_str(),
                                                  capsuleAxis.c_str())) {
                                if (ImGui::Selectable("X")) {
                                    collider.axis = Component::CollisionComponent::X;
                                }
                                if (ImGui::Selectable("Y")) {
                                    collider.axis = Component::CollisionComponent::Y;
                                }
                                if (ImGui::Selectable("Z")) {
                                    collider.axis = Component::CollisionComponent::Z;
                                }
                                ImGui::EndCombo();
                            }
                            if (collider.axis != oldAxis) {
                                updateColliderAndRigidBody();
                            }
                        }
                    }
                    // height input
                    {
                        if (collider.type == Component::CollisionComponent::CAPSULE ||
                            collider.type == Component::CollisionComponent::CONE) {
                            auto cursorPosX3 = ImGui::GetCursorPosX();
                            ImGui::Text("Height");
                            ImGui::SameLine();
                            float floatInputWidth = 100.0f;
                            ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                            ImGui::SetNextItemWidth(floatInputWidth);
                            ImGui::DragFloat(("##ColliderHeight/" + std::to_string(i)).c_str(), &collider.height, 0.1f,
                                             0.0f, 0.0f, "%.3f");
                            if (ImGui::IsItemEdited()) {
                                updateColliderAndRigidBody();
                            }
                        }
                    }
                    // radius input
                    {
                        if (collider.type == Component::CollisionComponent::CAPSULE ||
                            collider.type == Component::CollisionComponent::SPHERE ||
                            collider.type == Component::CollisionComponent::CONE) {
                            auto cursorPosX3 = ImGui::GetCursorPosX();
                            ImGui::Text("Radius");
                            ImGui::SameLine();
                            float floatInputWidth = 100.0f;
                            ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                            ImGui::SetNextItemWidth(floatInputWidth);
                            ImGui::DragFloat(("##ColliderRadius/" + std::to_string(i)).c_str(), &collider.radius, 0.1f,
                                             0.0f, 0.0f, "%.3f");
                            if (ImGui::IsItemEdited()) {
                                updateColliderAndRigidBody();
                            }
                        }
                    }
                    // mesh guid input
                    {
                        if (collider.type == Component::CollisionComponent::MESH) {
                            std::string shortMesPath = "";
                            if (!collider.assetGUID.empty()) {
                                std::string meshPath = Project::getResourceManager()->getFilePathFromGUID(
                                        collider.assetGUID);
                                shortMesPath = StringUtils::getFilePathRelativeToProjectFolder(meshPath);
                            }
                            ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - (cursorPosX2) - 22);
                            ImGui::InputText(("##ColliderMeshPath/" + std::to_string(i)).c_str(), &shortMesPath,
                                             ImGuiInputTextFlags_ReadOnly);
                            ImGui::SameLine();
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
                            if (ImGui::ImageButton(("##ChangeColliderMesh/" + std::to_string(i)).c_str(),
                                                   (void *) (intptr_t) selectIcon, ImVec2(18, 18))) {
                                delete collisionMeshSelectorBrowser;
                                collisionMeshSelectorBrowser = new ImGui::FileBrowser();
                                collisionMeshSelectorBrowser->SetTitle("select mesh");
                                collisionMeshSelectorBrowser->SetPwd(Project::getPath());
                                collisionMeshSelectorBrowser->Open();
                                colliderIndex = i;
                            }
                            ImGui::PopStyleVar();
                            ImGui::PopStyleColor();
                        }
                    }
                    // remove collider btn
                    if (ImGui::Button(("Remove ##RemoveCollider/" + std::to_string(i)).c_str(),
                                      ImVec2(treeNodeWidth, 0))) {
                        component.colliders.erase(component.colliders.begin() + i);
                        updateColliderAndRigidBody();
                    }
                }
                // add collider btn
                if (ImGui::Button("Add Collider", ImVec2(treeNodeWidth, 0))) {
                    component.colliders.emplace_back();
                    updateColliderAndRigidBody();
                }
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderRigidBodyComponent() {
        if (selectedEntity.hasComponent<Component::RigidBodyComponent>()) {
            auto cursorPosX1 = ImGui::GetCursorPosX();
            auto &component = selectedEntity.getComponent<Component::RigidBodyComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##RigidBody",
                                                  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |
                                                  ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::SameLine();
            ImGui::Text("Rigid Body");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 5);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
            if (ImGui::Button("X", ImVec2(0.f, 0.f))) {
                int idx = selectedEntity.getComponent<Component::RigidBodyComponent>().rigidBodyIndex;
                if (idx != -1) {
                    Project::getScene()->getPhysicsComponentSystem()->removeRigidBody(idx);
                }
                selectedEntity.removeComponent<Component::RigidBodyComponent>();
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            if (treeNodeOpen) {
                auto cursorPosX2 = ImGui::GetCursorPosX();
                auto treeNodeWidth = ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1);
                // type selector
                {
                    std::string dropdownPreview = "";
                    if (component.type == Component::RigidBodyComponent::STATIC) {
                        dropdownPreview = "Static";
                    } else if (component.type == Component::RigidBodyComponent::DYNAMIC) {
                        dropdownPreview = "Dynamic";
                    } else if (component.type == Component::RigidBodyComponent::KINEMATIC) {
                        dropdownPreview = "Kinematic";
                    } else {
                        Logger::fatal("Unknown rigid body type " + std::to_string(static_cast<int>(component.type)));
                    }
                    ImGui::SetNextItemWidth(treeNodeWidth);
                    if (ImGui::BeginCombo("##Change Rigid Body Type", dropdownPreview.c_str())) {
                        Component::RigidBodyComponent::RigidBodyType oldType = component.type;
                        if (ImGui::Selectable("Static")) {
                            component.type = Component::RigidBodyComponent::STATIC;
                        }
                        if (ImGui::Selectable("Dynamic")) {
                            component.type = Component::RigidBodyComponent::DYNAMIC;
                        }
                        if (ImGui::Selectable("Kinematic")) {
                            component.type = Component::RigidBodyComponent::KINEMATIC;
                        }
                        if (component.type != oldType) {
                            updateColliderAndRigidBody();
                        }
                        ImGui::EndCombo();
                    }
                }
                // mass input
                {
                    if (component.type == Component::RigidBodyComponent::DYNAMIC) {
                        auto cursorPosX3 = ImGui::GetCursorPosX();
                        ImGui::Text("Mass");
                        ImGui::SameLine();
                        float floatInputWidth = 100.0f;
                        ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                        ImGui::SetNextItemWidth(floatInputWidth);
                        ImGui::DragFloat("##RigidBodyMass", &component.mass, 0.1f, 0.0f, 0.0f, "%.3f");
                        if (ImGui::IsItemEdited()) {
                            updateColliderAndRigidBody();
                        }
                    }
                }
                // linear damping input
                {
                    if (component.type == Component::RigidBodyComponent::DYNAMIC) {
                        auto cursorPosX3 = ImGui::GetCursorPosX();
                        ImGui::Text("Linear Damping");
                        ImGui::SameLine();
                        float floatInputWidth = 100.0f;
                        ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                        ImGui::SetNextItemWidth(floatInputWidth);
                        ImGui::DragFloat("##RigidBodyLinearDamping", &component.linearDamping, 0.1f, 0.0f, 0.0f,
                                         "%.3f");
                        if (ImGui::IsItemEdited()) {
                            updateColliderAndRigidBody();
                        }
                    }
                }
                // angular damping input
                {
                    if (component.type == Component::RigidBodyComponent::DYNAMIC) {
                        auto cursorPosX3 = ImGui::GetCursorPosX();
                        ImGui::Text("Angular Damping");
                        ImGui::SameLine();
                        float floatInputWidth = 100.0f;
                        ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                        ImGui::SetNextItemWidth(floatInputWidth);
                        ImGui::DragFloat("##RigidBodyAngularDamping", &component.angularDamping, 0.1f, 0.0f, 0.0f,
                                         "%.3f");
                        if (ImGui::IsItemEdited()) {
                            updateColliderAndRigidBody();
                        }
                    }
                }
                // linear factor input
                {
                    if (component.type == Component::RigidBodyComponent::DYNAMIC) {
                        bool modified = renderVec3Control("Linear Factor", component.linearFactor, treeNodeWidth + 20, 1.0, 0.1);
                        if (modified) {
                            updateColliderAndRigidBody();
                        }
                    }
                }
                // angular factor input
                {
                    if (component.type == Component::RigidBodyComponent::DYNAMIC) {
                        bool modified = renderVec3Control("Angular Factor", component.angularFactor, treeNodeWidth + 20, 1.0, 0.1);
                        if (modified) {
                            updateColliderAndRigidBody();
                        }
                    }
                }
                // friction input
                {
                    auto cursorPosX3 = ImGui::GetCursorPosX();
                    ImGui::Text("Friction");
                    ImGui::SameLine();
                    float floatInputWidth = 100.0f;
                    ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                    ImGui::SetNextItemWidth(floatInputWidth);
                    ImGui::DragFloat("##RigidBodyFriction", &component.friction, 0.1f, 0.0f, 1.0f, "%.3f");
                    if (ImGui::IsItemEdited()) {
                        updateColliderAndRigidBody();
                    }
                    component.friction = fmin(component.friction, 1.0f);
                    component.friction = fmax(component.friction, 0.0f);
                }
                // restitution input
                {
                    auto cursorPosX3 = ImGui::GetCursorPosX();
                    ImGui::Text("Restitution");
                    ImGui::SameLine();
                    float floatInputWidth = 100.0f;
                    ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                    ImGui::SetNextItemWidth(floatInputWidth);
                    ImGui::DragFloat("##RigidBodyRestitution", &component.restitution, 0.1f, 0.0f, 1.0f, "%.3f");
                    if (ImGui::IsItemEdited()) {
                        updateColliderAndRigidBody();
                    }
                    component.restitution = fmin(component.restitution, 1.0f);
                    component.restitution = fmax(component.restitution, 0.0f);
                }
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderLightComponent() {
        if (selectedEntity.hasComponent<Component::LightComponent>()) {
            auto cursorPosX1 = ImGui::GetCursorPosX();
            auto &component = selectedEntity.getComponent<Component::LightComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Light",
                                                  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |
                                                  ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::SameLine();
            ImGui::Text("Light");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 5);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
            if (ImGui::Button("X", ImVec2(0.f, 0.f))) {
                selectedEntity.removeComponent<Component::LightComponent>();
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            if (treeNodeOpen) {
                auto cursorPosX2 = ImGui::GetCursorPosX();
                auto treeNodeWidth = ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1);
                // type selector
                {
                    std::string dropdownPreview = "";
                    if (component.type == Component::LightComponent::DIRECTIONAL) {
                        dropdownPreview = "Directional";
                    } else if (component.type == Component::LightComponent::POINT) {
                        dropdownPreview = "Point";
                    } else if (component.type == Component::LightComponent::SPOTLIGHT) {
                        dropdownPreview = "Spotlight";
                    } else {
                        Logger::fatal("Unknown light type " + std::to_string(static_cast<int>(component.type)));
                    }
                    ImGui::SetNextItemWidth(treeNodeWidth);
                    if (ImGui::BeginCombo("##Change Light Type", dropdownPreview.c_str())) {
                        if (ImGui::Selectable("Directional")) {
                            component.type = Component::LightComponent::DIRECTIONAL;
                        }
                        if (ImGui::Selectable("Point")) {
                            component.type = Component::LightComponent::POINT;
                        }
                        if (ImGui::Selectable("Spotlight")) {
                            component.type = Component::LightComponent::SPOTLIGHT;
                        }
                        ImGui::EndCombo();
                    }
                }
                // cut off input
                {
                    if (component.type == Component::LightComponent::SPOTLIGHT) {
                        auto cursorPosX3 = ImGui::GetCursorPosX();
                        ImGui::Text("Cut Off");
                        ImGui::SameLine();
                        float floatInputWidth = 100.0f;
                        ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                        ImGui::SetNextItemWidth(floatInputWidth);
                        ImGui::DragFloat("##SpotLightCutOff", &component.cutOff, 0.1f, 0.0f, 0.0f, "%.3f");
                    }
                }
                // outer cut off input
                {
                    if (component.type == Component::LightComponent::SPOTLIGHT) {
                        auto cursorPosX3 = ImGui::GetCursorPosX();
                        ImGui::Text("Outer Cut Off");
                        ImGui::SameLine();
                        float floatInputWidth = 100.0f;
                        ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                        ImGui::SetNextItemWidth(floatInputWidth);
                        ImGui::DragFloat("##OuterCutOff", &component.outerCutOff, 0.1f, 0.0f, 0.0f, "%.3f");
                    }
                }
                // constant input
                {
                    if (component.type == Component::LightComponent::SPOTLIGHT || component.type == Component::LightComponent::POINT) {
                        auto cursorPosX3 = ImGui::GetCursorPosX();
                        ImGui::Text("Constant");
                        ImGui::SameLine();
                        float floatInputWidth = 100.0f;
                        ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                        ImGui::SetNextItemWidth(floatInputWidth);
                        ImGui::DragFloat("##Constant", &component.constant, 0.1f, 0.0f, 0.0f, "%.3f");
                    }
                }
                // linear input
                {
                    if (component.type == Component::LightComponent::SPOTLIGHT || component.type == Component::LightComponent::POINT) {
                        auto cursorPosX3 = ImGui::GetCursorPosX();
                        ImGui::Text("Linear");
                        ImGui::SameLine();
                        float floatInputWidth = 100.0f;
                        ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                        ImGui::SetNextItemWidth(floatInputWidth);
                        ImGui::DragFloat("##Linear", &component.linear, 0.1f, 0.0f, 0.0f, "%.3f");
                    }
                }
                // quadratic input
                {
                    if (component.type == Component::LightComponent::SPOTLIGHT || component.type == Component::LightComponent::POINT) {
                        auto cursorPosX3 = ImGui::GetCursorPosX();
                        ImGui::Text("Quadratic");
                        ImGui::SameLine();
                        float floatInputWidth = 100.0f;
                        ImGui::SetCursorPosX(cursorPosX3 + treeNodeWidth - floatInputWidth);
                        ImGui::SetNextItemWidth(floatInputWidth);
                        ImGui::DragFloat("##Quadratic", &component.quadratic, 0.1f, 0.0f, 0.0f, "%.3f");
                    }
                }
                // color input
                {
                    renderVec3Control("Color", component.color, treeNodeWidth + 20, 1.0, 0.1, 3);
                }
                ImGui::TreePop();
            }
        }
    }

    void ImGuiEditorInspectorView::renderTerrainComponent() {
        if (selectedEntity.hasComponent<Component::TerrainComponent>()) {
            auto &component = selectedEntity.getComponent<Component::TerrainComponent>();
            bool treeNodeOpen = ImGui::TreeNodeEx("##Terrain",
                                                  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |
                                                  ImGuiTreeNodeFlags_AllowItemOverlap);

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::SameLine();
            ImGui::Text("Terrain");
            ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 5);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
            if (ImGui::Button("X", ImVec2(0.f, 0.f))) {
                selectedEntity.removeComponent<Component::TerrainComponent>();
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            auto cursorPosX1 = ImGui::GetCursorPosX();

            if (treeNodeOpen) {
                auto cursorPosX2 = ImGui::GetCursorPosX();
                auto treeNodeWidth = ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1);
                // guid
//                {
//                    if (component.guid.empty()) {
//                        ImGui::Text("None");
//                    } else {
//                        ImGui::Text("%s", component.guid.c_str());
//                    }
//                }
                // button to paint
                {
                    auto sceneCameraEntity = Project::getScene()->getSceneCamera();
                    if (sceneCameraEntity) {
                        auto &sceneCameraComponent = sceneCameraEntity.getComponent<Component::SceneCameraComponent>();
                        if (sceneCameraComponent.mode == Component::SceneCameraComponent::MOVE_1) {
                            if (ImGui::Button("Paint")) {
                                sceneCameraComponent.mode = Component::SceneCameraComponent::TERRAIN_PAINT;
                            }
                        } else if (sceneCameraComponent.mode == Component::SceneCameraComponent::TERRAIN_PAINT) {
                            if (ImGui::Button("Stop Painting")) {
                                sceneCameraComponent.mode = Component::SceneCameraComponent::MOVE_1;
                            }
                        }
                    }
                }
                // button to clear
                {
                    if (ImGui::Button("Clear")) {
                        if (component.terrain) {
                            for (int x = 0; x < component.terrain->getSize(); ++x) {
                                for (int z = 0; z < component.terrain->getSize(); ++z) {
                                    component.terrain->setHeight(x, z, 0.0f);
                                }
                            }
                            component.terrain->refreshTerrainTriangleList();
                        }
                    }
                }
                // save button
                {
                    if (ImGui::Button("Save")) {
                        auto filePath = Project::getResourceManager()->getFilePathFromGUID(component.guid);
                        if (component.terrain) {
                            component.terrain->saveToFile(filePath.c_str());
                        }
                    }
                }
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

    void ImGuiEditorInspectorView::updateColliderAndRigidBody() {
        if (selectedEntity.hasComponent<Component::CollisionComponent>()) {
            selectedEntity.getComponent<Component::CollisionComponent>().updateColliderShape(selectedEntity);
        }
        if (selectedEntity.hasComponent<Component::RigidBodyComponent>()) {
            selectedEntity.getComponent<Component::RigidBodyComponent>().updateRigidBody(selectedEntity);
        }
    }
}