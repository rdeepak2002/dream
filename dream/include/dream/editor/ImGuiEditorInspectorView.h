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

        void selectEntity(Entity &entity);

        void unselectEntity();

        void setTextEditor(ImGuiTextEditor *imGuiTextEditor);

        void setAnimatorGraphEditor(ImGuiEditorAnimatorGraph *animatorGraphEditor);

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

        bool renderVec3Control(const std::string &label, glm::vec3 &values, float contentWidth, float resetValue,
                               float vSpeed, int id = 0, std::pair<float, float> vMinMax1 = {0, 0},
                               std::pair<float, float> vMinMax2 = {0, 0}, std::pair<float, float> vMinMax3 = {0, 0});

        void renderTagComponent();

        void renderTransformComponent();

        void renderMeshComponent();

        void renderMaterialComponent();

        void renderLuaScriptComponent();

        void renderAnimatorComponent();

        void renderBoneComponent();

        void renderSceneCameraComponent();

        void renderCameraComponent();

        void renderCollisionComponent();

        void renderRigidBodyComponent();

        void renderLightComponent();

        void updateColliderAndRigidBody();

        // TODO: move mesh component render method to new class
        // mesh component renderer variables
        std::string oldMeshGUID;
        ImGui::FileBrowser *meshSelectorBrowser;
        ImGui::FileBrowser *luaScriptSelectorBrowser;
        ImGui::FileBrowser *animatorSelectorBrowser;
        ImGui::FileBrowser *collisionMeshSelectorBrowser;
        int colliderIndex;
        ImGuiTextEditor *imGuiTextEditor;
        ImGuiEditorAnimatorGraph *animatorGraphEditor;
    };
}

#endif //DREAM_IMGUIEDITORINSPECTORVIEW_H
