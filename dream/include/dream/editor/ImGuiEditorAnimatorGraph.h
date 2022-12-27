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

#ifndef DREAM_IMGUIEDITORANIMATORGRAPH_H
#define DREAM_IMGUIEDITORANIMATORGRAPH_H

#include <imgui_node_editor.h>
#include <imgui_internal.h>
#include <string>
#include <vector>
#include <iostream>
#include <imgui-filebrowser/imfilebrowser.h>
#include "dream/scene/component/Component.h"

namespace Dream {
    class ImGuiEditorAnimatorGraph {
    public:
        ImGuiEditorAnimatorGraph();
        ~ImGuiEditorAnimatorGraph();
        void update();
        void open(std::string guid);
        void loadStateMachine();
        void serializeStateMachine();
    private:
        void updatePopups();
        void updateStateMachinePanel(float stateMachinePanelWidth);
        void selectNewAnimation();
        bool visible;
        bool isFullscreen;
        bool shouldSetupPositionAndSize;
        ax::NodeEditor::EditorContext* m_Context;
        std::string animatorFileGUID;
        std::vector<Component::AnimatorComponent::State> states;
        std::vector<Component::AnimatorComponent::Transition> transitions;
        std::vector<std::string> variableNames;
        std::vector<int> variableValues;
        ImGui::FileBrowser* animationSelectorBrowser;
        std::string settingsFilePath;
        std::tuple<int, int, int> getNodeAndPinIDsFromStateID(int stateID);
        int getStateForPinID(int pinID);
        int getStateForNodeID(int nodeID);
        ax::NodeEditor::LinkId contextLinkId;
        ax::NodeEditor::NodeId contextNodeId;
        bool contextBackground;
    };
}

#endif //DREAM_IMGUIEDITORANIMATORGRAPH_H
