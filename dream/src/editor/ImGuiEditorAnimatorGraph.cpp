//
// Created by Deepak Ramalingam on 12/3/22.
//

#include "dream/editor/ImGuiEditorAnimatorGraph.h"
#include <iostream>

namespace Dream {
    ImGuiEditorAnimatorGraph::ImGuiEditorAnimatorGraph() {
        ax::NodeEditor::Config config;
        config.SettingsFile = nullptr;
        m_Context = ax::NodeEditor::CreateEditor(&config);
        visible = false;
        isFullscreen = false;
        shouldSetupPositionAndSize = true;
    }

    ImGuiEditorAnimatorGraph::~ImGuiEditorAnimatorGraph() {
        ax::NodeEditor::DestroyEditor(m_Context);
    }

    void ImGuiEditorAnimatorGraph::update() {
        if (visible) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(500, 300));

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

            ImGui::Begin("Animator", nullptr,  ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Save")) {
                        std::cout << "TODO: allow saving" << std::endl;
                    }
                    if (ImGui::MenuItem("Close")) {
                        this->visible = false;
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
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            ax::NodeEditor::SetCurrentEditor(m_Context);
            ax::NodeEditor::Begin("Animator", ImVec2(0.0, 0.0f));
            int uniqueId = 1;
            // draw nodes
            ax::NodeEditor::BeginNode(uniqueId++);
            ImGui::Text("Node A");
            ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
            ImGui::Text("-> In");
            ax::NodeEditor::EndPin();
            ImGui::SameLine();
            ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Output);
            ImGui::Text("Out ->");
            ax::NodeEditor::EndPin();
            ax::NodeEditor::EndNode();
            ax::NodeEditor::End();
            ax::NodeEditor::SetCurrentEditor(nullptr);
            ImGui::End();
            ImGui::PopStyleVar();
        }
    }
}