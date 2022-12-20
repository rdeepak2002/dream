//
// Created by Deepak Ramalingam on 12/3/22.
//

#include "dream/editor/ImGuiEditorAnimatorGraph.h"
#include "dream/util/Logger.h"
#include "dream/util/StringUtils.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"
#include "dream/util/IDUtils.h"
#include <misc/cpp/imgui_stdlib.h>
#include <iostream>
#include <utility>
#include <fstream>

namespace Dream {
    ImGuiEditorAnimatorGraph::ImGuiEditorAnimatorGraph() {
        visible = false;
        isFullscreen = false;
        shouldSetupPositionAndSize = false;
        animationSelectorBrowser = nullptr;
    }

    ImGuiEditorAnimatorGraph::~ImGuiEditorAnimatorGraph() {
        ax::NodeEditor::DestroyEditor(m_Context);
        delete animationSelectorBrowser;
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

            std::string filename = "Animator";
            if (!animatorFileGUID.empty()) {
                std::filesystem::path path = Project::getResourceManager()->getFilePathFromGUID(animatorFileGUID);
                filename = path.filename();
            }

            ImGui::Begin(std::string(filename + "###Animator").c_str(), &visible,  ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);

            // menu bar
            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Save")) {
                        serializeStateMachine();
                    }
                    if (ImGui::MenuItem("Close")) {
                        this->visible = false;
                        this->states.clear();
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

            // variables panel
            float variablesPanelWidth = 200.f;
            ImGui::BeginChild("Variables", ImVec2(variablesPanelWidth, 0));
            for (int i = 0; i < variableNames.size(); ++i) {
                float InputWidth = 40.0f;
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - InputWidth);
                ImGui::InputText(("##variable" + std::to_string(i)).c_str(), &variableNames[i]);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(InputWidth);
                ImGui::InputInt(("##variable-edit" + std::to_string(i)).c_str(), &variableValues[i], 0);
            }
            if (ImGui::Button("Add", ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
                variableNames.emplace_back("variable");
                variableValues.push_back(0);
            }
            ImGui::EndChild();

            ImGui::SameLine();

            // graph panel
            ImGui::BeginChild("Graph", ImVec2(ImGui::GetWindowContentRegionWidth() - variablesPanelWidth, 0));
            if (ImGui::BeginPopupContextWindow()) {
                if (ImGui::MenuItem("New animation")) {
                    delete animationSelectorBrowser;
                    animationSelectorBrowser = new ImGui::FileBrowser();
                    animationSelectorBrowser->SetTitle("select animation");
                    animationSelectorBrowser->SetPwd(Project::getPath());
                    animationSelectorBrowser->Open();
                }
                ImGui::EndPopup();
            }

            if (animationSelectorBrowser) {
                animationSelectorBrowser->Display();
                if (animationSelectorBrowser->HasSelected()) {
                    std::filesystem::path selectedFilePath = animationSelectorBrowser->GetSelected();
                    std::string newAnimationGUID = IDUtils::getGUIDForFile(selectedFilePath);
                    states.push_back(newAnimationGUID);
                    animationSelectorBrowser->ClearSelected();
                }
            }

            ax::NodeEditor::SetCurrentEditor(m_Context);
            ax::NodeEditor::Begin("Animator", ImVec2(0.0, 0.0f));
            int uniqueId = 1;
            // draw nodes
            for (const auto& animationGUID : states) {
                // example node
                ax::NodeEditor::BeginNode(uniqueId++);
                std::string animationFilePath = Project::getResourceManager()->getFilePathFromGUID(animationGUID);
                std::string shortenedAnimationFilePath = StringUtils::getFilePathRelativeToProjectFolder(animationFilePath);
                ImGui::Text("%s", shortenedAnimationFilePath.c_str());
                ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
                ImGui::Text("-> In");
                ax::NodeEditor::EndPin();
                ImGui::SameLine();
                ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Output);
                ImGui::Text("Out ->");
                ax::NodeEditor::EndPin();
                ax::NodeEditor::EndNode();
            }
            ax::NodeEditor::End();
            ax::NodeEditor::SetCurrentEditor(nullptr);
            ImGui::EndChild();

            ImGui::End();
            ImGui::PopStyleVar();
        }
    }

    void ImGuiEditorAnimatorGraph::open(std::string guid) {
        this->visible = true;
        this->animatorFileGUID = guid;
        if (animatorFileGUID.empty()) {
            Logger::fatal("No file specified for animator (1)");
        }
        ax::NodeEditor::DestroyEditor(m_Context);
        ax::NodeEditor::Config config;
        settingsFilePath = Project::getResourceManager()->getFilePathFromGUID(animatorFileGUID).append(".editor-layout.json");
        config.SettingsFile = settingsFilePath.c_str();
        m_Context = ax::NodeEditor::CreateEditor(&config);
        ax::NodeEditor::SetCurrentEditor(m_Context);
        this->loadStateMachine();
    }

    void ImGuiEditorAnimatorGraph::loadStateMachine() {
        if (animatorFileGUID.empty()) {
            Logger::fatal("No file specified for animator (2)");
        }
        states.clear();
        variableNames.clear();
        variableValues.clear();
        std::string animatorFilePath = Project::getResourceManager()->getFilePathFromGUID(animatorFileGUID);
        YAML::Node doc = YAML::LoadFile(animatorFilePath);
        // deserialize states
        auto statesNode = doc[Component::AnimatorComponent::k_states].as<std::vector<YAML::Node>>();
        for (const YAML::Node& animationGUIDNode : statesNode) {
            states.push_back(animationGUIDNode.as<std::string>());
        }
        // TODO: deserialize transitions
        // deserialize variables
        auto variablesNode = doc[Component::AnimatorComponent::k_variables].as<std::vector<YAML::Node>>();
        for (const YAML::Node& variableNode : variablesNode) {
            auto name = variableNode["Name"].as<std::string>();
            int value = variableNode["Value"].as<int>();
            variableNames.push_back(name);
            variableValues.push_back(value);
        }
    }

    void ImGuiEditorAnimatorGraph::serializeStateMachine() {
        if (animatorFileGUID.empty()) {
            Logger::fatal("No file specified for animator");
        }
        // update the animator file
        std::string animatorFilePath = Project::getResourceManager()->getFilePathFromGUID(animatorFileGUID);
        YAML::Emitter out;
        out << YAML::BeginMap;
        // serialize states
        out << YAML::Key << Component::AnimatorComponent::k_states;
        YAML::Node statesNode;
        for (const auto &animationGUID : states) {
            statesNode.push_back(animationGUID);
        }
        out << YAML::Value << statesNode;
        // TODO: serialize transitions
        out << YAML::Key << Component::AnimatorComponent::k_transitions << YAML::Value << YAML::Node(YAML::NodeType::Sequence);
        // serialize variables
        out << YAML::Key << Component::AnimatorComponent::k_variables;
        YAML::Node variablesNode;
        for (int i = 0; i < variableNames.size(); ++i) {
            YAML::Node variableNode;
            variableNode["Name"] = variableNames[i];
            variableNode["Value"] = variableValues[i];
            variablesNode.push_back(variableNode);
        }
        out << YAML::Value << variablesNode;
        out << YAML::EndMap;
        std::ofstream fout(animatorFilePath.c_str());
        fout << out.c_str();
        fout.close();
        // update all entities with this animator
        auto animatorEntities = Project::getScene()->getEntitiesWithComponents<Component::AnimatorComponent>();
        for(auto entityHandle : animatorEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            if (entity.getComponent<Component::AnimatorComponent>().guid == animatorFileGUID) {
                entity.getComponent<Component::AnimatorComponent>().loadStateMachine(entity);
            }
        }
    }
}