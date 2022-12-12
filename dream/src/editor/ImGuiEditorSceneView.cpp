//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/editor/ImGuiEditorSceneView.h"

#include <imgui.h>
#include <imgui_internal.h>
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"
#include "dream/util/Logger.h"

namespace Dream {
    ImGuiEditorSceneView::ImGuiEditorSceneView() {
        inspectorView = nullptr;
        justSelectedEntity = false;
    }

    void ImGuiEditorSceneView::update() {
        justSelectedEntity = false;
        ImGuiWindowClass scene_window_class;
        scene_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&scene_window_class);
        ImGui::Begin("Scene");
        // render scene hierarchy
        Entity rootEntity = Project::getScene()->getRootEntity();
        if (rootEntity) {
            renderSceneViewEntity(rootEntity);
        }
        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::MenuItem("New entity")) {
                Project::getScene()->createEntity();
            }
            ImGui::EndPopup();
        }
        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();

        vMin.x += ImGui::GetWindowPos().x;
        vMin.y += ImGui::GetWindowPos().y;
        vMax.x += ImGui::GetWindowPos().x;
        vMax.y += ImGui::GetWindowPos().y;

        if (!justSelectedEntity && ImGui::IsMouseHoveringRect(vMin, vMax) && ImGui::IsMouseClicked(0)) {
            inspectorView->unselectEntity();
        }
        ImGui::End();
    }

    void ImGuiEditorSceneView::renderSceneViewEntity(Entity &entity) {
        std::string tagStr = entity.getComponent<Component::TagComponent>().tag;
        auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_SpanFullWidth;
        if (entity.numChildren() == 0) {
            treeNodeFlags = treeNodeFlags | ImGuiTreeNodeFlags_Leaf;
        }
        if (entity.hasComponent<Component::RootComponent>()) {
            treeNodeFlags = treeNodeFlags | ImGuiTreeNodeFlags_DefaultOpen;
        }
        if (inspectorView && entity == inspectorView->getSelectedEntity()) {
            treeNodeFlags = treeNodeFlags | ImGuiTreeNodeFlags_Selected;
        }
        bool treeNodeOpen = ImGui::TreeNodeEx(entity.getComponent<Component::IDComponent>().getID().c_str(), treeNodeFlags, "%s", tagStr.c_str());
        if (ImGui::IsItemClicked()) {
            if (inspectorView) {
                inspectorView->selectEntity(entity);
                justSelectedEntity = true;
            }
        }
        if (ImGui::BeginDragDropTarget()) {
            const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("moveEntity");
            if(payload) {
                int* internalEntityIDPtr = (int*)(payload->Data);
                Entity draggedEntity = Project::getScene()->getEntityByInternalID(*internalEntityIDPtr);
                if (draggedEntity) {
                    entity.addChild(draggedEntity);
                } else {
                    Logger::fatal("No dragged entity found");
                }
            }
            ImGui::EndDragDropTarget();
        }
        bool canMoveEntityInHierarchy = true;
        if (entity.hasComponent<Component::BoneComponent>()) {
            canMoveEntityInHierarchy = false;
        }
        if (canMoveEntityInHierarchy) {
            if (ImGui::BeginDragDropSource()) {
                int internalEntityID = (int) entity.entityHandle;
                ImGui::SetDragDropPayload("moveEntity", &internalEntityID, sizeof(int*));
                ImGui::Text("%s", entity.getComponent<Component::TagComponent>().tag.c_str());
                ImGui::EndDragDropSource();
            }
        }
        if (treeNodeOpen) {
            Entity child = entity.getComponent<Component::HierarchyComponent>().first;
            while (child) {
                Entity nextChild = child.getComponent<Component::HierarchyComponent>().next;
                if (!child.hasComponent<Component::SceneCameraComponent>()) {
                    this->renderSceneViewEntity(child);
                }
                child = nextChild;
            }
            ImGui::TreePop();
        }
    }

    void ImGuiEditorSceneView::setInspectorView(ImGuiEditorInspectorView *inspectorView) {
        this->inspectorView = inspectorView;
    }
}
