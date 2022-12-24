//
// Created by Deepak Ramalingam on 11/13/22.
//

#ifndef DREAM_IMGUIEDITORINSPECTORVIEW_H
#define DREAM_IMGUIEDITORINSPECTORVIEW_H

#include "dream/scene/Entity.h"
#include "dream/editor/ImGuiTextEditor.h"
#include "dream/editor/ImGuiEditorAnimatorGraph.h"
#include <imgui.h>
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <imgui-filebrowser/imfilebrowser.h>

namespace Dream {
    class ImGuiEditorInspectorView {
    public:
        ImGuiEditorInspectorView();
        ~ImGuiEditorInspectorView();
        void update();
        void lookAtSelectedEntity();
        void selectEntity(Entity& entity);
        void unselectEntity();
        void setTextEditor(ImGuiTextEditor* imGuiTextEditor);
        void setAnimatorGraphEditor(ImGuiEditorAnimatorGraph* animatorGraphEditor);
        Entity getSelectedEntity();
    private:


        // TODO: load select icon and swap buttons with image button for this
        // TODO: load select icon and swap buttons with image button for this
        // TODO: load select icon and swap buttons with image button for this
        // TODO: load select icon and swap buttons with image button for this
        // TODO: load select icon and swap buttons with image button for this
        // TODO: load select icon and swap buttons with image button for this
        // TODO: load select icon and swap buttons with image button for this
        // TODO: load select icon and swap buttons with image button for this
        // TODO: load select icon and swap buttons with image button for this
        unsigned int selectIcon, editIcon;


        Entity selectedEntity;
        void addComponent(std::string componentID);
        void renderRemoveComponent();
        void renderAddComponent();
        void renderVec3Control(const std::string& label, glm::vec3& values, float contentWidth, float resetValue);
        void renderTagComponent();
        void renderTransformComponent();
        void renderMeshComponent();
        void renderMaterialComponent();
        void renderLuaScriptComponent();
        void renderAnimatorComponent();
        void renderBoneComponent();
        void renderSceneCameraComponent();
        void renderCameraComponent();
        // TODO: move mesh component render method to new class
        // mesh component renderer variables
        ImGui::FileBrowser* meshSelectorBrowser;
        std::string oldMeshGUID;
        ImGui::FileBrowser* luaScriptSelectorBrowser;
        ImGuiTextEditor* imGuiTextEditor;
        ImGuiEditorAnimatorGraph* animatorGraphEditor;
    };
}

#endif //DREAM_IMGUIEDITORINSPECTORVIEW_H
