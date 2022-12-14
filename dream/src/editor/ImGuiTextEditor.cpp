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

#include "dream/editor/ImGuiTextEditor.h"
#include "imgui_internal.h"
#include "dream/util/Logger.h"
#include "dream/util/IDUtils.h"
#include "dream/scene/system/LuaScriptComponentSystem.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Dream {
    ImGuiTextEditor::ImGuiTextEditor() {
        this->textEditor = new TextEditor();
        this->visible = false;
        this->shouldSetupPositionAndSize = false;
        this->isFullscreen = false;
        this->isDarkMode = false;
    }

    ImGuiTextEditor::~ImGuiTextEditor() {
        delete this->textEditor;
    }

    void ImGuiTextEditor::open(const std::string &filepath) {
        shouldSetupPositionAndSize = false;
        path = std::filesystem::path(filepath);
        fileGuid = IDUtils::getGUIDForFile(path);
        if (path.extension() == ".lua") {
            textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
        } else if (path.extension() == ".vert" || path.extension() == ".frag") {
            textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
        } else {
            textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
        }

        if (path.extension() == ".py" || path.extension() == ".scene" || path.extension() == ".yml") {
            this->textEditor->SetShowWhitespaces(true);
        } else {
            this->textEditor->SetShowWhitespaces(false);
        }
        filename = path.filename();
        std::string fileContent;
        this->setVisibility(true);
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            file.open(filepath);
            std::stringstream fileStream;
            // read file's buffer contents into streams
            fileStream << file.rdbuf();
            // close file handlers
            file.close();
            // convert stream into string
            fileContent = fileStream.str();
            textEditor->SetText(fileContent);
            justLoaded = true;
        }
        catch (std::ifstream::failure &e) {
            Logger::fatal("Error reading file " + filepath + " [" + std::string(e.what()) + "]");
        }
    }

    void ImGuiTextEditor::save() {
        this->setVisibility(true);
    }

    void ImGuiTextEditor::loadEmptyFile() {
        this->setVisibility(true);
        this->justLoaded = true;
    }

    void ImGuiTextEditor::update() {
        if (textEditor->IsTextChanged() && !justLoaded) {
            unsavedChanges = true;
        }

        if (visible) {
            auto viewport = ImGui::GetMainViewport();
            if (isFullscreen) {
                ImGui::SetNextWindowPos(ImVec2(0, 0));
                ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y));
            } else if (shouldSetupPositionAndSize) {
                auto w = viewport->Size.x * 0.8f;
                auto h = viewport->Size.y * 0.8f;
                ImGui::SetNextWindowPos(ImVec2(viewport->Size.x / 2 - w / 2, viewport->Size.y / 2 - h / 2));
                ImGui::SetNextWindowSize(ImVec2(w, h));
                shouldSetupPositionAndSize = false;
            }

            ImGuiWindowClass editor_window_class;
            editor_window_class.DockNodeFlagsOverrideSet =
                    ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
            ImGui::SetNextWindowClass(&editor_window_class);
            ImGui::Begin(std::string(filename + " ###File Editor").c_str(), &visible,
                         ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Save")) {
                        std::ofstream fout(path);
                        fout << textEditor->GetText().c_str();
                        fout.close();
                        unsavedChanges = false;
                        LuaScriptComponentSystem::errorPrintedForScript.erase(fileGuid);
                        LuaScriptComponentSystem::modifiedScripts.insert(fileGuid);
                    }
                    if (ImGui::MenuItem("Close")) {
                        this->setVisibility(false);
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("View")) {
                    if (!isFullscreen && ImGui::MenuItem("Fullscreen")) {
                        isFullscreen = true;
                        shouldSetupPositionAndSize = true;
                    }
                    if (isFullscreen && ImGui::MenuItem("Windowed")) {
                        isFullscreen = false;
                        shouldSetupPositionAndSize = true;
                    }
                    if (isDarkMode && ImGui::MenuItem("Light Mode")) {
                        isDarkMode = false;
                    }
                    if (!isDarkMode && ImGui::MenuItem("Dark Mode")) {
                        isDarkMode = true;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            textEditor->SetPalette(isDarkMode ? TextEditor::GetDarkPalette() : TextEditor::GetLightPalette());
            ImGui::SetWindowFontScale(1.1);
            textEditor->Render("TextEditor");

            ImGui::End();
        }

        justLoaded = false;
    }

    void ImGuiTextEditor::setVisibility(bool isVisible) {
        this->visible = isVisible;
    }
}
