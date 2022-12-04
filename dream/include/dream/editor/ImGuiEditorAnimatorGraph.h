//
// Created by Deepak Ramalingam on 12/3/22.
//

#ifndef DREAM_IMGUIEDITORANIMATORGRAPH_H
#define DREAM_IMGUIEDITORANIMATORGRAPH_H

# include <imgui_node_editor.h>
# define IMGUI_DEFINE_MATH_OPERATORS
# include <imgui_internal.h>

namespace Dream {
    class ImGuiEditorAnimatorGraph {
    public:
        ImGuiEditorAnimatorGraph();
        ~ImGuiEditorAnimatorGraph();
        void update();
    private:
        bool visible;
        bool isFullscreen;
        bool shouldSetupPositionAndSize;
        ax::NodeEditor::EditorContext* m_Context;
    };
}

#endif //DREAM_IMGUIEDITORANIMATORGRAPH_H
