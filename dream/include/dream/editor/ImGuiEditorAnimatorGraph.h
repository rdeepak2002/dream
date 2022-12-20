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
        bool visible;
        bool isFullscreen;
        bool shouldSetupPositionAndSize;
        ax::NodeEditor::EditorContext* m_Context;
        std::string animatorFileGUID;
        std::vector<std::string> states;
        std::bitset<16> variableBits;
        std::vector<std::string> variableNames;
        ImGui::FileBrowser* animationSelectorBrowser;
        std::string settingsFilePath;
    };
}

#endif //DREAM_IMGUIEDITORANIMATORGRAPH_H
