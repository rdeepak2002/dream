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

#include "dream/editor/ImGuiEditor.h"

#include <filesystem>
#include <stack>
#include <imgui/imgui_internal.h>
#include "dream/project/Project.h"
#include "dream/window/Input.h"
#include "dream/Application.h"

namespace Dream {
    ImGuiEditor::ImGuiEditor(Dream::Window *window) : Editor(window) {
        this->rendererViewportWidth = 520;
        this->rendererViewportHeight = 557;
        this->textEditor = new ImGuiTextEditor();
        this->fileBrowser = new ImGuiFileBrowser();
        this->animatorGraphEditor = new ImGuiEditorAnimatorGraph();
        this->inspectorView = new ImGuiEditorInspectorView();
        this->inspectorView->setTextEditor(textEditor);
        this->inspectorView->setAnimatorGraphEditor(animatorGraphEditor);
        this->sceneView = new ImGuiEditorSceneView();
        this->sceneView->setInspectorView(inspectorView);
        this->projectView = new ImGuiEditorProjectView(textEditor);
        this->consoleView = new ImGuiEditorConsoleView();
        this->rendererView = new ImGuiEditorRendererView();
        this->menu = new ImGuiEditorMenu(this->fileBrowser);

        // setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.IniFilename = NULL;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        float fontSize = 16.f;
        io.Fonts->AddFontFromFileTTF(
                Application::getResourcesRoot().append("assets").append("fonts").append(
                        "font.otf").c_str(), fontSize);

        // setup Dear ImGui style
        ImGui::StyleColorsDark();
    }

    ImGuiEditor::~ImGuiEditor() {
        delete sceneView;
        delete inspectorView;
        delete projectView;
        delete consoleView;
        delete rendererView;
        delete menu;
        delete fileBrowser;
        delete textEditor;
        delete animatorGraphEditor;
    }

    void ImGuiEditor::newFrame(Dream::Window *window) {

    }

    void ImGuiEditor::update(Dream::Window *window, unsigned int frameBufferTexture) {
        ImGuiIO &io = ImGui::GetIO();
        if (Input::pointerLockActivated()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        }

        this->style();
        this->newFrame(window);
        this->setupDockSpace();

        // update utility panels
        if (!Project::isEditorFullscreen()) {
            textEditor->update();
            fileBrowser->update();
            animatorGraphEditor->update();
        }

        // render panels
        rendererView->update(this->rendererViewportWidth, this->rendererViewportHeight, frameBufferTexture);
        if (!Project::isEditorFullscreen()) {
            menu->update();
            inspectorView->update();
            consoleView->update(logCollector);
            projectView->update();
            sceneView->update();
        }

        if (Input::pointerLockActivated()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        }

        ImGui::Render();
        this->renderDrawData();
    }

    void ImGuiEditor::renderDrawData() {

    }

    void ImGuiEditor::style() {
        auto &colors = ImGui::GetStyle().Colors;
        auto primary0 = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};        // example: background for windows
        auto primary1 = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};     // example: colors for separators
        auto primary2 = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};        // example: bg color of drop down
        auto primary3 = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};        // example: color of headers and hovered buttons
        auto dockingPreview = ImVec4{0.6, 0.6, 0.6, 0.6};
        auto btn = ImVec4{80.f / 255.0f, 76.f / 255.f, 76.f / 255.f, 1.0f};

        // Docking
        colors[ImGuiCol_DockingPreview] = dockingPreview;

        // Window
        colors[ImGuiCol_WindowBg] = primary0;

        // Menu
        colors[ImGuiCol_MenuBarBg] = primary1;

        // Borders
        colors[ImGuiCol_Border] = primary1;

        // Separators
        colors[ImGuiCol_Separator] = primary1;
        colors[ImGuiCol_SeparatorHovered] = colors[ImGuiCol_Separator];
        colors[ImGuiCol_SeparatorActive] = colors[ImGuiCol_Separator];

        // Headers
        colors[ImGuiCol_Header] = primary2;
        colors[ImGuiCol_HeaderHovered] = primary3;
        colors[ImGuiCol_HeaderActive] = primary1;

        // Buttons
        colors[ImGuiCol_Button] = btn;
        colors[ImGuiCol_ButtonHovered] = primary3;
        colors[ImGuiCol_ButtonActive] = primary1;

        // Frame BG
        colors[ImGuiCol_FrameBg] = primary2;
        colors[ImGuiCol_FrameBgHovered] = primary2;
        colors[ImGuiCol_FrameBgActive] = primary1;

        // Tabs
        colors[ImGuiCol_Tab] = primary1;
        colors[ImGuiCol_TabUnfocused] = primary1;
        colors[ImGuiCol_TabHovered] = primary0;
        colors[ImGuiCol_TabActive] = colors[ImGuiCol_TabHovered];
        colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabHovered];

        // Title
        colors[ImGuiCol_TitleBg] = primary1;
        colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_TitleBg];
        colors[ImGuiCol_TitleBgCollapsed] = colors[ImGuiCol_TitleBgCollapsed];
    }

    std::pair<int, int> ImGuiEditor::getRendererViewportDimensions() {
        return std::make_pair(this->rendererViewportWidth, this->rendererViewportHeight);
    }

    void ImGuiEditor::setupDockSpace() {
        ImGui::NewFrame();

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImVec2 viewportPosition = ImVec2(viewport->Pos.x, viewport->Pos.y);
        ImVec2 viewportSize = ImVec2(viewport->Size.x, viewport->Size.y);
        ImGui::SetNextWindowPos(viewportPosition);
        ImGui::SetNextWindowSize(viewportSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
        if (!Project::isEditorFullscreen()) {
            window_flags |= ImGuiWindowFlags_MenuBar;
        }
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("Editor");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

            static auto first_time = true;
            if (first_time) {
                first_time = false;

                ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
                ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(dockspace_id, viewportSize);

                auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr,
                                                                 &dockspace_id);
                auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.3f, nullptr,
                                                                &dockspace_id);
                auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr,
                                                                &dockspace_id);

                ImGui::DockBuilderDockWindow("Renderer", dockspace_id);
                ImGui::DockBuilderDockWindow("###File Editor", dockspace_id);
                ImGui::DockBuilderDockWindow("###Animator", dockspace_id);
                ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
                ImGui::DockBuilderDockWindow("Console", dock_id_down);
                ImGui::DockBuilderDockWindow("Project", dock_id_down);
                ImGui::DockBuilderDockWindow("Scene", dock_id_left);
                ImGui::DockBuilderFinish(dockspace_id);
            }
        }

        ImGui::End();
    }
}
