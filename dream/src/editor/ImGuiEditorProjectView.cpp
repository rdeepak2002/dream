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
        ImGui::Text(" ");
        ImGui::Columns(6, nullptr);
        for (const auto & entry : std::filesystem::directory_iterator(currentPath)) {
            if (ignoredExtensions.count(entry.path().extension()) == 0 && ignoredFileNames.count(entry.path().filename()) == 0) {
                if (entry.is_directory()) {
                    if (ImGui::ImageButton(entry.path().c_str(), (void*)(intptr_t)folderIcon, ImVec2(30,30))) {
                        currentPath = currentPath.append(entry.path().filename().c_str());
                    }
                } else {
                    if (ImGui::ImageButton(entry.path().c_str(), (void*)(intptr_t)fileIcon, ImVec2(30,30))) {
                        std::cout << "TODO: open text editor for file" << std::endl;
                    }
                }
                ImGui::Text("%s", entry.path().filename().c_str());
                ImGui::NextColumn();
            }
        }
        ImGui::End();
    }
}
