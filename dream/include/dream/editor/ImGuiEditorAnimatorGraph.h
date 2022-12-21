//
// Created by Deepak Ramalingam on 12/3/22.
//

#ifndef DREAM_IMGUIEDITORANIMATORGRAPH_H
#define DREAM_IMGUIEDITORANIMATORGRAPH_H

#include <imgui_node_editor.h>
#include <imgui_internal.h>
#include <string>
#include <vector>
#include <iostream>
#include <imgui-filebrowser/imfilebrowser.h>
#include "dream/scene/component/Component.h"

#define MAX_STATES 5000

namespace Dream {
    struct LinkInfo {
        ax::NodeEditor::LinkId Id;
        ax::NodeEditor::PinId InputId;
        ax::NodeEditor::PinId OutputId;
    };

    class ImGuiEditorAnimatorGraph {
    public:
        ImGuiEditorAnimatorGraph();
        ~ImGuiEditorAnimatorGraph();
        void update();
        void open(std::string guid);
        void loadStateMachine();
        void serializeStateMachine();
    private:
        int nextLinkId;
        bool visible;
        bool isFullscreen;
        bool shouldSetupPositionAndSize;
        ax::NodeEditor::EditorContext* m_Context;
        std::string animatorFileGUID;
        std::vector<std::string> states;
        std::vector<Component::AnimatorComponent::Transition> transitions;
        std::vector<std::string> variableNames;
        std::vector<int> variableValues;
        ImGui::FileBrowser* animationSelectorBrowser;
        std::string settingsFilePath;
        // TODO: replace links with transitions data
        std::vector<LinkInfo> links;
        std::tuple<int, int, int> getNodeAndPinIDsFromStateID(int stateID);
        int getStateForPinID(int pinID);
        int getStateForNodeID(int nodeID);
    };
}

#endif //DREAM_IMGUIEDITORANIMATORGRAPH_H
