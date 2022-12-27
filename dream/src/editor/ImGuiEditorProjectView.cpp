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

#include "dream/editor/ImGuiEditorProjectView.h"
#include "dream/project/Project.h"
#include "dream/renderer/OpenGLTexture.h"
#include "dream/util/Logger.h"
#include "dream/scene/component/Component.h"
#include "dream/util/StringUtils.h"
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>
#include <fstream>
#include <sstream>

namespace Dream {
    ImGuiEditorProjectView::ImGuiEditorProjectView(ImGuiTextEditor *textEditor) {
        this->textEditor = textEditor;
        auto assetsFolderPath = std::filesystem::path(Project::getPath()).append("assets");
        if (std::filesystem::exists(assetsFolderPath)) {
            currentPath = assetsFolderPath;
        } else {
            Logger::fatal("Assets folder does not exist in project");
        }
        ignoredExtensions.insert(".dream-ignore");
        ignoredExtensions.insert(".meta");
        ignoredFileNames.insert(".DS_Store");
        // TODO: use specific renderer (not OpenGL)
        Texture *fileIconTexture = new OpenGLTexture(
                std::filesystem::current_path().append("resources").append("editor-resources").append("icons").append(
                        "FileIcon.png"), false);
        fileIcon = fileIconTexture->ID();
        delete fileIconTexture;
        // TODO: use specific renderer (not OpenGL)
        Texture *folderIconTexture = new OpenGLTexture(
                std::filesystem::current_path().append("resources").append("editor-resources").append("icons").append(
                        "DirectoryIcon.png"), false);
        folderIcon = folderIconTexture->ID();
        delete folderIconTexture;
    }

    ImGuiEditorProjectView::~ImGuiEditorProjectView() {

    }

    void ImGuiEditorProjectView::update() {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

        ImGuiWindowClass project_window_class;
        project_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&project_window_class);
        ImGui::Begin("Project");

        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::MenuItem("New script")) {
                auto filename = "script";
                auto extension = ".lua";
                auto path = Project::getPath().append("assets").append("scripts");
                if (!exists(path)) {
                    path = Project::getPath().append("assets");
                }
                if (!exists(path)) {
                    Logger::fatal("Cannot find assets folder to save new script to");
                }
                // find a unique file path to save new script file to
                int i = 0;
                while (exists(std::filesystem::path(path).append(filename + std::to_string(i) + extension))) {
                    i++;
                }
                path = path.append(filename + std::to_string(i) + extension);
                // create blank lua script
                std::ofstream fout(path);
                fout << "function update(entity, dt)" << std::endl;
                fout << std::endl;
                fout << "end" << std::endl;
                fout.close();
                Project::getAssetImporter()->createMetaFile(path);
                Project::recognizeResources();
                currentPath = path.parent_path();
            }
            if (ImGui::MenuItem("New animator")) {
                auto filename = "anim";
                auto extension = ".animator";
                auto path = Project::getPath().append("assets").append("animators");
                if (!exists(path)) {
                    path = Project::getPath().append("assets");
                }
                if (!exists(path)) {
                    Logger::fatal("Cannot find assets folder to save new animator to");
                }
                // find a unique file path to save new script file to
                int i = 0;
                while (exists(std::filesystem::path(path).append(filename + std::to_string(i) + extension))) {
                    i++;
                }
                path = path.append(filename + std::to_string(i) + extension);
                // create blank animator
                YAML::Emitter out;
                out << YAML::BeginMap;
                out << YAML::Key << Component::AnimatorComponent::k_states << YAML::Value
                    << YAML::Node(YAML::NodeType::Sequence);
                out << YAML::Key << Component::AnimatorComponent::k_transitions << YAML::Value
                    << YAML::Node(YAML::NodeType::Sequence);
                out << YAML::Key << Component::AnimatorComponent::k_variables << YAML::Value
                    << YAML::Node(YAML::NodeType::Sequence);
                out << YAML::EndMap;
                std::ofstream fout(path.c_str());
                fout << out.c_str();
                fout.close();
                Project::getAssetImporter()->createMetaFile(path);
                Project::recognizeResources();
                currentPath = path.parent_path();
            }
            ImGui::EndPopup();
        }

        std::vector<std::string> pathSplit = split(currentPath.c_str(), std::filesystem::path::preferred_separator);
        int start_i = split(Project::getPath().c_str(), std::filesystem::path::preferred_separator).size() - 1;
        std::filesystem::path p = std::filesystem::path(Project::getPath()).parent_path();
        for (int i = start_i; i < pathSplit.size(); ++i) {
            p = p.append(pathSplit[i]);
            if (ImGui::Button(pathSplit[i].c_str())) {
                currentPath = p;
            }
            if (i != pathSplit.size() - 1) {
                ImGui::SameLine();
                std::string sep(1, std::filesystem::path::preferred_separator);
                ImGui::Text("%s", sep.c_str());
                ImGui::SameLine();
            }
        }

        auto windowSize = ImGui::GetWindowSize();

        float imageSize = 40.0f;
        float columnWidth = 120.0f;
        int numColumns = (int) (windowSize.x / (columnWidth + 0.1f));
        if (numColumns == 0) {
            numColumns = 1;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
        ImGui::BeginColumns("projectGrid", numColumns,
                            ImGuiColumnsFlags_NoResize | ImGuiColumnsFlags_NoBorder | ImGuiTableColumnFlags_WidthFixed);
        ImGui::SetColumnWidth(-1, columnWidth);

        for (const auto &entry: std::filesystem::directory_iterator(currentPath)) {
            if (ignoredExtensions.count(entry.path().extension()) == 0 &&
                ignoredFileNames.count(entry.path().filename()) == 0) {
                float currentCursorPos = ImGui::GetCursorPosX();
                if (entry.is_directory()) {
                    ImGui::SetCursorPosX(currentCursorPos + (columnWidth - (imageSize + 0.0f)) * 0.5f);
                    ImGui::ImageButton(entry.path().c_str(), (void *) (intptr_t) folderIcon,
                                       ImVec2(imageSize, imageSize));
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                        currentPath = currentPath.append(entry.path().filename().c_str());
                    }
                } else {
                    ImGui::SetCursorPosX(currentCursorPos + (columnWidth - (imageSize + 0.0f)) * 0.5f);
                    ImGui::ImageButton(entry.path().c_str(), (void *) (intptr_t) fileIcon,
                                       ImVec2(imageSize, imageSize));
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                        textEditor->open(entry.path());
                    }
                }
                currentCursorPos = ImGui::GetCursorPosX();
                auto text = StringUtils::splitAndShorten(entry.path().filename(), true, 24, 12);
                ImVec2 textSize = ImGui::CalcTextSize(text.first.c_str());
                ImGui::SetCursorPosX(currentCursorPos + (columnWidth - textSize.x) * 0.5f);
                ImGui::Text("%s", text.first.c_str());
                textSize = ImGui::CalcTextSize(text.second.c_str());
                ImGui::SetCursorPosX(currentCursorPos + (columnWidth - textSize.x) * 0.5f);
                ImGui::Text("%s", text.second.c_str());
                ImGui::NextColumn();
                ImGui::SetColumnWidth(-1, columnWidth);
            }
        }

        ImGui::EndColumns();

        ImGui::PopStyleVar();
        ImGui::End();

        ImGui::PopStyleColor();
    }

    std::vector<std::string> ImGuiEditorProjectView::split(const std::string &str, const char &ch) {
        std::string next;
        std::vector<std::string> result;

        for (char it: str) {
            if (it == ch) {
                if (!next.empty()) {
                    result.push_back(next);
                    next.clear();
                }
            } else {
                next += it;
            }
        }

        if (!next.empty())
            result.push_back(next);
        return result;
    }
}
