//
// Created by Deepak Ramalingam on 11/28/22.
//

#include "dream/editor/ImGuiEditorProjectView.h"
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>

namespace Dream {
    void ImGuiEditorProjectView::update() {
        ImGuiWindowClass project_window_class;
        project_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&project_window_class);
        ImGui::Begin("Project");
        ImGui::Text(" ");
        ImGui::End();
    }
}
