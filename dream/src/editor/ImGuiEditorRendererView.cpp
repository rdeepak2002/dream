//
// Created by Deepak Ramalingam on 11/28/22.
//

#include "dream/editor/ImGuiEditorRendererView.h"
#include "dream/project/Project.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "dream/renderer/OpenGLTexture.h"

namespace Dream {
    ImGuiEditorRendererView::ImGuiEditorRendererView() {
        // TODO: use specific renderer (not OpenGL)
        Texture* playIconTexture = new OpenGLTexture(std::filesystem::current_path().append("resources").append("editor-resources").append("icons").append("PlayIcon.png"), false);
        playIcon = playIconTexture->ID();
        delete playIconTexture;
        // TODO: use specific renderer (not OpenGL)
        Texture* stopIconTexture = new OpenGLTexture(std::filesystem::current_path().append("resources").append("editor-resources").append("icons").append("StopIcon.png"), false);
        stopIcon = stopIconTexture->ID();
        delete stopIconTexture;
        // TODO: use specific renderer (not OpenGL)
        Texture* expandIconTexture = new OpenGLTexture(std::filesystem::current_path().append("resources").append("editor-resources").append("icons").append("ExpandIcon.png"), false);
        expandIcon = expandIconTexture->ID();
        delete expandIconTexture;
        // TODO: use specific renderer (not OpenGL)
        Texture* collapseIconTexture = new OpenGLTexture(std::filesystem::current_path().append("resources").append("editor-resources").append("icons").append("CollapseIcon.png"), false);
        collapseIcon = collapseIconTexture->ID();
        delete collapseIconTexture;
    }

    void ImGuiEditorRendererView::update(int &rendererViewportWidth, int &rendererViewportHeight, unsigned int frameBufferTexture) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGuiWindowClass renderer_window_class;
        renderer_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        if (Project::isEditorFullscreen()) {
            renderer_window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoTabBar;
        }
        ImGui::SetNextWindowClass(&renderer_window_class);
        ImGuiWindowFlags renderer_window_flags = 0;
        renderer_window_flags |= ImGuiWindowFlags_NoScrollbar;
        ImGui::Begin("Renderer", nullptr, renderer_window_flags);
        float buttonHeight = ImGui::GetCursorPosY();
        float btnWidth = 18.f;
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() / 2 - btnWidth / 2);
        if (Project::isPlaying()) {
            if (ImGui::ImageButton("StopBtn", (void*)(intptr_t)stopIcon, ImVec2(btnWidth,btnWidth))) {
                Project::setIsPlaying(false);
            }
        } else {
            if (ImGui::ImageButton("PlayBtn", (void*)(intptr_t)playIcon, ImVec2(btnWidth,btnWidth))) {
                Project::setIsPlaying(true);
            }
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - btnWidth);
        if (Project::isEditorFullscreen()) {
            if (ImGui::ImageButton("CollapseBtn", (void*)(intptr_t)collapseIcon, ImVec2(btnWidth,btnWidth))) {
                Project::setIsEditorFullscreen(false);
            }
        } else {
            if (ImGui::ImageButton("ExpandBtn", (void*)(intptr_t)expandIcon, ImVec2(btnWidth,btnWidth))) {
                Project::setIsEditorFullscreen(true);
            }
        }
        buttonHeight = ImGui::GetCursorPosY() - buttonHeight;
        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        vMin.x += ImGui::GetWindowPos().x;
        vMin.y += ImGui::GetWindowPos().y;
        vMax.x += ImGui::GetWindowPos().x;
        vMax.y += ImGui::GetWindowPos().y;
        float width = vMax.x - vMin.x;
        float height = vMax.y - vMin.y - buttonHeight;
        rendererViewportWidth = int(width);
        rendererViewportHeight = int(height);
        ImGui::Image(reinterpret_cast<ImTextureID>(frameBufferTexture), ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        ImGui::PopStyleColor();
    }
}
