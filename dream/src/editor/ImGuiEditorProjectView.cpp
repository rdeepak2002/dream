//
// Created by Deepak Ramalingam on 11/28/22.
//

#include "dream/editor/ImGuiEditorProjectView.h"
#include "dream/project/Project.h"
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>
#include "dream/renderer/OpenGLTexture.h"

namespace Dream {
    ImGuiEditorProjectView::ImGuiEditorProjectView() {
        auto assetsFolderPath = std::filesystem::path(Project::getPath()).append("assets");
        if (std::filesystem::exists(assetsFolderPath)) {
            currentPath = assetsFolderPath;
        } else {
            std::cout << "Assets folder does not exist in project" << std::endl;
            exit(EXIT_FAILURE);
        }
        ignoredExtensions.insert(".meta");
        ignoredFileNames.insert(".DS_Store");
        // TODO: use specific renderer (not OpenGL)
        Texture* fileIconTexture = new OpenGLTexture(std::filesystem::current_path().append("resources").append("editor-resources").append("icons").append("FileIcon.png"), false);
        fileIcon = fileIconTexture->ID();
        delete fileIconTexture;
        // TODO: use specific renderer (not OpenGL)
        Texture* folderIconTexture = new OpenGLTexture(std::filesystem::current_path().append("resources").append("editor-resources").append("icons").append("DirectoryIcon.png"), false);
        folderIcon = folderIconTexture->ID();
        delete folderIconTexture;
    }

    void ImGuiEditorProjectView::update() {
        ImGuiWindowClass project_window_class;
        project_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&project_window_class);
        ImGui::Begin("Project");
        std::vector<std::string> pathSplit = split(currentPath.c_str(), std::filesystem::path::preferred_separator);
        for (int i = 0; i < pathSplit.size(); ++i) {
            if (ImGui::Button(pathSplit[i].c_str())) {
                std::cout << "TODO: allow user to go back to this folder" << std::endl;
            }
            if (i != pathSplit.size() - 1) {
                ImGui::SameLine();
                std::string sep(1, std::filesystem::path::preferred_separator);
                ImGui::Text("%s", sep.c_str());
                ImGui::SameLine();
            }
        }

        ImGui::Columns(8, "grid");

        if (currentPath != Project::getPath()) {
            if (ImGui::ImageButton("..", (void*)(intptr_t)folderIcon, ImVec2(40,40))) {
                currentPath = currentPath.parent_path();
            }
            ImGui::Text("%s", "..");
            ImGui::NextColumn();
        }

        for (const auto & entry : std::filesystem::directory_iterator(currentPath)) {
            if (ignoredExtensions.count(entry.path().extension()) == 0 && ignoredFileNames.count(entry.path().filename()) == 0) {
                if (entry.is_directory()) {
                    if (ImGui::ImageButton(entry.path().c_str(), (void*)(intptr_t)folderIcon, ImVec2(40,40))) {
                        currentPath = currentPath.append(entry.path().filename().c_str());
                    }
                } else {
                    if (ImGui::ImageButton(entry.path().c_str(), (void*)(intptr_t)fileIcon, ImVec2(40,40))) {
                        std::cout << "TODO: open text editor for file" << std::endl;
                    }
                }
                ImGui::Text("%s", entry.path().filename().c_str());
                ImGui::NextColumn();
            }
        }
        ImGui::End();
    }

    std::vector<std::string> ImGuiEditorProjectView::split(const std::string &str, const char &ch) {
        std::string next;
        std::vector<std::string> result;

        for (char it : str) {
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
