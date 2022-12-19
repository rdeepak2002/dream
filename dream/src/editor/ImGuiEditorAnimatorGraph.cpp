//
// Created by Deepak Ramalingam on 12/3/22.
//

#include "dream/editor/ImGuiEditorAnimatorGraph.h"
#include "dream/util/Logger.h"
#include "dream/util/StringUtils.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"
#include "dream/util/IDUtils.h"
#include <iostream>
#include <utility>
#include <fstream>

namespace Dream {
    ImGuiEditorAnimatorGraph::ImGuiEditorAnimatorGraph() {
        ax::NodeEditor::Config config;
        config.SettingsFile = nullptr;
        m_Context = ax::NodeEditor::CreateEditor(&config);
        visible = false;
        isFullscreen = false;
        shouldSetupPositionAndSize = true;
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

            ImGui::Begin("Animator", nullptr,  ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);

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
                    animationGUIDS.push_back(newAnimationGUID);
                    animationSelectorBrowser->ClearSelected();
                }
            }

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Save")) {
                        serializeStateMachine();
                    }
                    if (ImGui::MenuItem("Close")) {
                        this->visible = false;
                        this->animationGUIDS.clear();
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
            for (const auto& animationGUID : animationGUIDS) {
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
            ImGui::End();
            ImGui::PopStyleVar();
        }
    }

    void ImGuiEditorAnimatorGraph::open(std::string guid) {
        this->visible = true;
        this->animatorFileGUID = guid;
        this->loadStateMachine();
    }

    void ImGuiEditorAnimatorGraph::loadStateMachine() {
        if (animatorFileGUID.empty()) {
            Logger::fatal("No file specified for animator");
        }
        animationGUIDS.clear();
        std::string animatorFilePath = Project::getResourceManager()->getFilePathFromGUID(animatorFileGUID);
        YAML::Node doc = YAML::LoadFile(animatorFilePath);
        auto animationsNode = doc["Animations"].as<std::vector<YAML::Node>>();
        for (const YAML::Node& animationGUIDNode : animationsNode) {
            animationGUIDS.push_back(animationGUIDNode.as<std::string>());
        }
    }

    void ImGuiEditorAnimatorGraph::serializeStateMachine() {
        if (animatorFileGUID.empty()) {
            Logger::fatal("No file specified for animator");
        }
        std::string animatorFilePath = Project::getResourceManager()->getFilePathFromGUID(animatorFileGUID);
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Animations";
        YAML::Node node;
        for (const auto &animationGUID : animationGUIDS) {
            node.push_back(animationGUID);
        }
        out  << YAML::Value << node;
        // TODO: serialize states
        out << YAML::Key << "States" << YAML::Value << YAML::Node(YAML::NodeType::Sequence);
        // TODO: serialize transitions
        out << YAML::Key << "Transitions" << YAML::Value << YAML::Node(YAML::NodeType::Sequence);
        // TODO: serialize conditions
        out << YAML::Key << "Conditions" << YAML::Value << YAML::Node(YAML::NodeType::Sequence);
        out << YAML::EndMap;
        std::ofstream fout(animatorFilePath.c_str());
        fout << out.c_str();
        fout.close();
        auto animatorEntities = Project::getScene()->getEntitiesWithComponents<Component::AnimatorComponent>();
        for(auto entityHandle : animatorEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            if (entity.getComponent<Component::AnimatorComponent>().guid == animatorFileGUID) {
                entity.getComponent<Component::AnimatorComponent>().loadAnimations(entity);
            }
        }
    }
}