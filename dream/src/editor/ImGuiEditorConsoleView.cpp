//
// Created by Deepak Ramalingam on 11/28/22.
//

#include "dream/editor/ImGuiEditorConsoleView.h"
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>

namespace Dream {
    void ImGuiEditorConsoleView::update(LogCollector* logCollector) {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1.0));
        ImGuiWindowClass console_window_class;
        console_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&console_window_class);
        ImGui::Begin("Console");
        for (const auto& log : logCollector->getLogs()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
            std::string typeStr = "D";
            if (log.type == debug) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(69 / 255.0, 227 / 255.0, 245 / 255.0, 1.0));
                typeStr = "D";
            } else if (log.type == warn) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 0.0, 1.0));
                typeStr = "W";
            } else if (log.type == error) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
                typeStr = "E";
            } else {
                std::cout << "Error: unknown log type" << std::endl;
                exit(EXIT_FAILURE);
            }
            ImGui::Text("[%s]", typeStr.c_str());
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::Text("%s", log.text.c_str());
            ImGui::PopStyleColor();
        }
        ImGui::End();
        ImGui::PopStyleColor();
    }
}

