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

#include "dream/editor/ImGuiEditorMenu.h"
#include "dream/project/Project.h"

namespace Dream {
    ImGuiEditorMenu::ImGuiEditorMenu(ImGuiFileBrowser *fileBrowser) {
        this->fileBrowser = fileBrowser;
    }

    void ImGuiEditorMenu::update() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save")) {
                    Project::saveScene();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Assets")) {
                if (ImGui::MenuItem("Import folder")) {
                    this->fileBrowser->selectDirectory();
                }
                if (ImGui::MenuItem("Import file")) {
                    this->fileBrowser->selectFile();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Debug")) {
                ImGui::Checkbox("Physics debugger", &(Project::getConfig().physicsConfig.physicsDebugger));
                ImGui::Checkbox("Physics debugger depth test", &(Project::getConfig().physicsConfig.depthTest));
                ImGui::Checkbox("Physics debugger while playing", &(Project::getConfig().physicsConfig.physicsDebuggerWhilePlaying));
                ImGui::Checkbox("Play animation in editor", &(Project::getConfig().animationConfig.playInEditor));
                // TODO: drop-down to switch between FINAL, DIFFUSE, SPECULAR, and NORMAL rendering modes to allow for rendering debugging
                ImGui::Text("TODO: drop-down to allow for switching rendering type to allow for debugging");
                {
                    std::string dropdownPreview = "";
                    if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::FINAL) {
                        dropdownPreview = "Final";
                    } else if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::DIFFUSE) {
                        dropdownPreview = "Diffuse";
                    } else if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::SPECULAR) {
                        dropdownPreview = "Specular";
                    } else if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::NORMAL) {
                        dropdownPreview = "Normal";
                    } else {
                        Logger::fatal("Unknown rendering config type " + std::to_string(static_cast<int>(Project::getConfig().renderingConfig.renderingType)));
                    }
                    if (ImGui::BeginCombo("##Change Rendering Type", dropdownPreview.c_str())) {
                        if (ImGui::Selectable("Final")) {
                            Project::getConfig().renderingConfig.renderingType = Config::RenderingConfig::FINAL;
                        }
                        if (ImGui::Selectable("Diffuse")) {
                            Project::getConfig().renderingConfig.renderingType = Config::RenderingConfig::DIFFUSE;
                        }
                        if (ImGui::Selectable("Specular")) {
                            Project::getConfig().renderingConfig.renderingType = Config::RenderingConfig::SPECULAR;
                        }
                        if (ImGui::Selectable("Normal")) {
                            Project::getConfig().renderingConfig.renderingType = Config::RenderingConfig::NORMAL;
                        }
                        ImGui::EndCombo();
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
}

