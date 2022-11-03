//
// Created by Deepak Ramalingam on 11/3/22.
//

#include "dream/editor/ImGuiEditor.h"

#include <iostream>
#include <imgui.h>
#include <imgui_internal.h>

namespace Dream {
    ImGuiEditor::ImGuiEditor(Dream::Window *window) {
        this->rendererViewportWidth = 520;
        this->rendererViewportHeight = 557;

        // setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.IniFilename = NULL;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // setup Dear ImGui style
        ImGui::StyleColorsDark();
    }

    void ImGuiEditor::newFrame(Dream::Window *window) {

    }

    void ImGuiEditor::update(Dream::Window *window, unsigned int frameBufferTexture) {
        this->newFrame(window);
        ImGui::NewFrame();

        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        // dockspace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

            static auto first_time = true;
            if (first_time)
            {
                first_time = false;

                ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
                ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

                auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);
                auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);
                auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f, nullptr, &dockspace_id);

                ImGui::DockBuilderDockWindow("Renderer", dockspace_id);
                ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
                ImGui::DockBuilderDockWindow("Project", dock_id_down);
                ImGui::DockBuilderDockWindow("Console", dock_id_down);
                ImGui::DockBuilderDockWindow("Scene", dock_id_left);
                ImGui::DockBuilderFinish(dockspace_id);
            }
        }

        ImGui::End();

        this->style();

        ImGuiWindowClass renderer_window_class;
        renderer_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&renderer_window_class);
        ImGuiWindowFlags renderer_window_flags = 0;
        renderer_window_flags |= ImGuiWindowFlags_NoScrollbar;
        ImGui::Begin("Renderer", nullptr, renderer_window_flags);
        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        vMin.x += ImGui::GetWindowPos().x;
        vMin.y += ImGui::GetWindowPos().y;
        vMax.x += ImGui::GetWindowPos().x;
        vMax.y += ImGui::GetWindowPos().y;
        float width = vMax.x - vMin.x;
        float height = vMax.y - vMin.y;
        this->rendererViewportWidth = int(width);
        this->rendererViewportHeight = int(height);
        ImGui::Image(reinterpret_cast<ImTextureID>(frameBufferTexture), ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();

        ImGuiWindowClass inspector_window_class;
        inspector_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&inspector_window_class);
        ImGui::Begin("Inspector");
        ImGui::Text(" ");
        ImGui::End();

        ImGuiWindowClass project_window_class;
        project_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&project_window_class);
        ImGui::Begin("Project");
        ImGui::Text(" ");
        ImGui::End();

        ImGuiWindowClass console_window_class;
        console_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&console_window_class);
        ImGui::Begin("Console");
        ImGui::Text(" ");
        ImGui::End();

        ImGuiWindowClass scene_window_class;
        scene_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;
        ImGui::SetNextWindowClass(&scene_window_class);
        ImGui::Begin("Scene");
        ImGui::Text(" ");
        ImGui::End();

        ImGui::Render();
        this->renderDrawData();
    }

    void ImGuiEditor::renderDrawData() {

    }

    void ImGuiEditor::style() {
        auto& colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

        // Headers
        colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Buttons
        colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Frame BG
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Tabs
        colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
        colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
        colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

        // Title
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    }

    std::pair<int, int> ImGuiEditor::getRendererViewportDimensions() {
        return std::make_pair(this->rendererViewportWidth, this->rendererViewportHeight);
    }
}