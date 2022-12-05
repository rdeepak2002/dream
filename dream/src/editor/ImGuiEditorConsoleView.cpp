//
// Created by Deepak Ramalingam on 11/28/22.
//

#include "dream/editor/ImGuiEditorConsoleView.h"
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>

namespace Dream {
    void ImGuiEditorConsoleView::update() {
        ImGuiWindowClass console_window_class;
        console_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&console_window_class);
        ImGui::Begin("Console");
        ImGui::Text("TODO");
        ImGui::End();
    }
}

