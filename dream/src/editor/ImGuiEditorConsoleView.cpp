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

#include "dream/editor/ImGuiEditorConsoleView.h"
#include "dream/util/Logger.h"
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>

namespace Dream {
    void ImGuiEditorConsoleView::update(LogCollector* logCollector) {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 1.0));
        ImGuiWindowClass console_window_class;
        console_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&console_window_class);
        ImGui::Begin("Console");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::MenuItem("Clear")) {
                logCollector->clearLogs();
            }
            ImGui::EndPopup();
        }
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
            } else if (log.type == info) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 1.0, 0.0, 1.0));
                typeStr = "I";
            } else if (log.type == fatal) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
                typeStr = "F";
            } else {
                Logger::fatal("Unknown log type " + std::to_string(log.type));
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

