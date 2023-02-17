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

#include "dream/editor/ImGuiEditorRendererView.h"
#include "dream/project/Project.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "dream/renderer/OpenGLTexture.h"
#include "dream/window/Input.h"
#include "dream/Application.h"

namespace Dream {
    ImGuiEditorRendererView::ImGuiEditorRendererView() {
        // TODO: use specific renderer (not OpenGL)
        Texture *playIconTexture = new OpenGLTexture(
                Application::getResourcesRoot().append("assets").append("icons").append(
                        "PlayIcon.png"), false);
        playIcon = playIconTexture->ID();
        delete playIconTexture;
        // TODO: use specific renderer (not OpenGL)
        Texture *stopIconTexture = new OpenGLTexture(
                Application::getResourcesRoot().append("assets").append("icons").append(
                        "StopIcon.png"), false);
        stopIcon = stopIconTexture->ID();
        delete stopIconTexture;
        // TODO: use specific renderer (not OpenGL)
        Texture *expandIconTexture = new OpenGLTexture(
                Application::getResourcesRoot().append("assets").append("icons").append(
                        "ExpandIcon.png"), false);
        expandIcon = expandIconTexture->ID();
        delete expandIconTexture;
        // TODO: use specific renderer (not OpenGL)
        Texture *collapseIconTexture = new OpenGLTexture(
                Application::getResourcesRoot().append("assets").append("icons").append(
                        "CollapseIcon.png"), false);
        collapseIcon = collapseIconTexture->ID();
        delete collapseIconTexture;
        // TODO: use specific renderer (not OpenGL)
        Texture *wrenchIconTexture = new OpenGLTexture(
                Application::getResourcesRoot().append("assets").append("icons").append(
                        "WrenchIconDark.png"), false);
        wrenchIcon = wrenchIconTexture->ID();
        delete wrenchIconTexture;
    }

    void ImGuiEditorRendererView::update(int &rendererViewportWidth, int &rendererViewportHeight,
                                         unsigned int frameBufferTexture) {
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
            if (ImGui::ImageButton("StopBtn", (void *) (intptr_t) stopIcon, ImVec2(btnWidth, btnWidth))) {
                Project::setIsPlaying(false);
                Project::reloadScene();
            }
        } else {
            if (ImGui::ImageButton("PlayBtn", (void *) (intptr_t) playIcon, ImVec2(btnWidth, btnWidth))) {
                Project::setIsPlaying(true);
                Project::saveScene(true);
            }
        }
        ImGui::SameLine();
        // TODO: instead of "gizmos" show wrench or gear icon
        auto gizmosTextWidth = ImGui::CalcTextSize("  ").x + 10.0f;
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - btnWidth - gizmosTextWidth);
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0);
            ImGui::Image((void *) (intptr_t) wrenchIcon, ImVec2(btnWidth, btnWidth)), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128);
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - btnWidth - gizmosTextWidth);
            // gizmos debug panel
            ImGui::PushItemWidth(gizmosTextWidth);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0, 0.0, 0.0, 0.0));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.0, 0.0, 0.0, 0.0));
            if (ImGui::BeginCombo("##gizmos", "  ", ImGuiComboFlags_NoArrowButton)) {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::Checkbox("Physics debugger", &(Project::getConfig().physicsConfig.physicsDebugger));
                ImGui::Checkbox("Physics debugger depth test", &(Project::getConfig().physicsConfig.depthTest));
                ImGui::Checkbox("Physics debugger while playing", &(Project::getConfig().physicsConfig.physicsDebuggerWhilePlaying));
                ImGui::Checkbox("Play animation in editor", &(Project::getConfig().animationConfig.playInEditor));
                // drop-down for rendering debugger views
                {
                    ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
                    std::string dropdownPreview = "";
                    if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::FINAL) {
                        dropdownPreview = "Final Render";
                    } else if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::DIFFUSE) {
                        dropdownPreview = "Diffuse Render";
                    } else if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::SPECULAR) {
                        dropdownPreview = "Specular Render";
                    } else if (Project::getConfig().renderingConfig.renderingType == Config::RenderingConfig::NORMAL) {
                        dropdownPreview = "Normal Render";
                    } else {
                        Logger::fatal("Unknown rendering config type " + std::to_string(static_cast<int>(Project::getConfig().renderingConfig.renderingType)));
                    }
                    if (ImGui::BeginCombo("##Change Rendering Type", dropdownPreview.c_str())) {
                        if (ImGui::Selectable("Final Render")) {
                            Project::getConfig().renderingConfig.renderingType = Config::RenderingConfig::FINAL;
                        }
                        if (ImGui::Selectable("Diffuse Render")) {
                            Project::getConfig().renderingConfig.renderingType = Config::RenderingConfig::DIFFUSE;
                        }
                        if (ImGui::Selectable("Specular Render")) {
                            Project::getConfig().renderingConfig.renderingType = Config::RenderingConfig::SPECULAR;
                        }
                        if (ImGui::Selectable("Normal Render")) {
                            Project::getConfig().renderingConfig.renderingType = Config::RenderingConfig::NORMAL;
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::PopItemWidth();
                }
                ImGui::EndCombo();
            } else {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
            ImGui::PopItemWidth();
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - btnWidth);
        if (Project::isEditorFullscreen()) {
            if (ImGui::ImageButton("CollapseBtn", (void *) (intptr_t) collapseIcon, ImVec2(btnWidth, btnWidth))) {
                Project::setIsEditorFullscreen(false);
            }
        } else {
            if (ImGui::ImageButton("ExpandBtn", (void *) (intptr_t) expandIcon, ImVec2(btnWidth, btnWidth))) {
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
        ImGui::Image(reinterpret_cast<ImTextureID>(frameBufferTexture), ImVec2(width, height), ImVec2(0, 1),
                     ImVec2(1, 0));
        Input::setPlayWindowActive(true);
        if (ImGui::IsItemHovered() || Input::pointerLockActivated()) {
            Input::setEditorRendererActive(true);
        } else {
            Input::setEditorRendererActive(false);
            Input::setPlayWindowActive(false);
        }
        ImGui::End();
        ImGui::PopStyleColor();
    }
}
