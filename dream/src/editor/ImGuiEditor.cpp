//
// Created by Deepak Ramalingam on 11/3/22.
//

#include "dream/editor/ImGuiEditor.h"

#include <filesystem>
#include <stack>
#include <imgui/imgui_internal.h>

namespace Dream {
    ImGuiEditor::ImGuiEditor(Dream::Window *window) : Editor(window) {
        this->rendererViewportWidth = 520;
        this->rendererViewportHeight = 557;
        this->textEditor = new ImGuiTextEditor();
        this->fileBrowser = new ImGuiFileBrowser();
        this->inspectorView = new ImGuiEditorInspectorView();
        this->inspectorView->setTextEditor(textEditor);
        this->sceneView = new ImGuiEditorSceneView();
        this->sceneView->setInspectorView(inspectorView);
        this->projectView = new ImGuiEditorProjectView();
        this->consoleView = new ImGuiEditorConsoleView();
        this->rendererView = new ImGuiEditorRendererView();
        this->menu = new ImGuiEditorMenu(this->fileBrowser);

        // setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.IniFilename = NULL;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.Fonts->AddFontFromFileTTF(std::filesystem::current_path().append("resources").append("editor-resources").append("fonts").append("font.ttf").c_str(), 16);

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
    }

    void ImGuiEditor::newFrame(Dream::Window *window) {

    }

    void ImGuiEditor::update(Dream::Window *window, unsigned int frameBufferTexture) {
        this->style();
        this->newFrame(window);
        this->setupDockSpace();

        // update utility panels
        textEditor->update();
        fileBrowser->update();

        // render panels
        menu->update();
        rendererView->update(this->rendererViewportWidth, this->rendererViewportHeight, frameBufferTexture);
        inspectorView->update();
        consoleView->update();
        projectView->update();
        sceneView->update();

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

    void ImGuiEditor::setupDockSpace() {
        ImGui::NewFrame();

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 viewportPosition = ImVec2(viewport->Pos.x, viewport->Pos.y);
        ImVec2 viewportSize = ImVec2(viewport->Size.x, viewport->Size.y);
        ImGui::SetNextWindowPos(viewportPosition);
        ImGui::SetNextWindowSize(viewportSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

            static auto first_time = true;
            if (first_time) {
                first_time = false;

                ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
                ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(dockspace_id, viewportSize);

                auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.35f, nullptr, &dockspace_id);
                auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);
                auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f, nullptr, &dockspace_id);

                ImGui::DockBuilderDockWindow("Renderer", dockspace_id);
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
