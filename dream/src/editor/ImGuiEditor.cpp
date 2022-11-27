//
// Created by Deepak Ramalingam on 11/3/22.
//

#include "dream/editor/ImGuiEditor.h"

#include <filesystem>
#include <imgui_internal.h>
#include <stack>
#include "dream/project/Project.h"

namespace Dream {
    ImGuiEditor::ImGuiEditor(Dream::Window *window) : Editor(window) {
        this->sceneView = new ImGuiEditorSceneView();
        this->inspectorView = new ImGuiEditorInspectorView();
        this->sceneView->setInspectorView(inspectorView);
        this->rendererViewportWidth = 520;
        this->rendererViewportHeight = 557;
        this->fileImporterBrowser = nullptr;
        this->textEditor = new TextEditor();

        // setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.IniFilename = NULL;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.Fonts->AddFontFromFileTTF(std::filesystem::current_path().append("examples").append("resources").append("fonts").append("font.ttf").c_str(), 16);

        // setup Dear ImGui style
        ImGui::StyleColorsDark();
    }

    ImGuiEditor::~ImGuiEditor() {
        delete sceneView;
        delete inspectorView;
        delete fileImporterBrowser;
        delete textEditor;
    }

    void ImGuiEditor::newFrame(Dream::Window *window) {

    }

    void ImGuiEditor::update(Dream::Window *window, unsigned int frameBufferTexture) {
        this->newFrame(window);
        ImGui::NewFrame();

        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        float yViewportOffset = 0.0;
        ImVec2 viewportPosition = ImVec2(viewport->Pos.x, viewport->Pos.y - yViewportOffset);
        ImVec2 viewportSize = ImVec2(viewport->Size.x, viewport->Size.y + yViewportOffset);
        ImGui::SetNextWindowPos(viewportPosition);
        ImGui::SetNextWindowSize(viewportSize);
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
                ImGui::DockBuilderSetNodeSize(dockspace_id, viewportSize);

                auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.35f, nullptr, &dockspace_id);
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

        if(ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if(ImGui::MenuItem("Save")) {
                    Project::saveScene();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Assets")) {
                if(ImGui::MenuItem("Import Folder")) {
                    delete fileImporterBrowser;
                    fileImporterBrowser = new ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
                    fileImporterBrowser->SetTitle("import folder");
                    fileImporterBrowser->SetPwd(Project::getPath());
                    fileImporterBrowser->Open();
                }
                if(ImGui::MenuItem("Import File")) {
                    delete fileImporterBrowser;
                    fileImporterBrowser = new ImGui::FileBrowser();
                    fileImporterBrowser->SetTitle("import file");
                    fileImporterBrowser->SetPwd(Project::getPath());
                    fileImporterBrowser->Open();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (fileImporterBrowser) {
            fileImporterBrowser->Display();
            if (fileImporterBrowser->HasSelected()) {
                std::filesystem::path selectedFilePath = fileImporterBrowser->GetSelected();
                Project::getAssetImporter()->importAsset(selectedFilePath);
                fileImporterBrowser->ClearSelected();
            }
        }

        if (textEditor) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(500, 500));
            ImGui::Begin("filename", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Save")) {
                        /// save text....
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            textEditor->SetPalette(TextEditor::GetDarkPalette());
            textEditor->Render("TextEditor");
            textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());

            ImGui::End();
            ImGui::PopStyleVar(1);
        }

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

        inspectorView->update();

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
}
