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

#include "dream/editor/ImGuiEditorAnimatorGraph.h"
#include "dream/util/Logger.h"
#include "dream/util/StringUtils.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"
#include "dream/util/IDUtils.h"
#include "imgui_node_editor.h"
#include <misc/cpp/imgui_stdlib.h>
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
        shouldSetupPositionAndSize = false;
        animationSelectorBrowser = nullptr;
        contextNodeId = -1;
        contextLinkId = -1;
        contextBackground = false;
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

            ImGui::Begin(std::string(filename + "###Animator").c_str(), &visible,
                         ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);

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

            // state machine panel
            float stateMachinePanelWidth = 400.f;
            updateStateMachinePanel(stateMachinePanelWidth);
            ImGui::SameLine();

            // graph panel
            ImGui::BeginChild("Graph", ImVec2(ImGui::GetWindowContentRegionWidth() - stateMachinePanelWidth - 10, 0));

            if (animationSelectorBrowser) {
                animationSelectorBrowser->Display();
                if (animationSelectorBrowser->HasSelected()) {
                    std::filesystem::path selectedFilePath = animationSelectorBrowser->GetSelected();
                    std::string newAnimationGUID = IDUtils::getGUIDForFile(selectedFilePath);
                    std::string animationFilePath = Project::getResourceManager()->getFilePathFromGUID(newAnimationGUID);
                    std::string shortenedAnimationFilePath = StringUtils::getFilePathRelativeToProjectFolder(
                            animationFilePath);
                    states.push_back(Component::AnimatorComponent::State{
                            .Guid=newAnimationGUID,
                            .PlayOnce=true,
                            .Name=shortenedAnimationFilePath
                    });
                    animationSelectorBrowser->ClearSelected();
                }
            }

            ax::NodeEditor::SetCurrentEditor(m_Context);
            ax::NodeEditor::Begin("Animator", ImVec2(0.0, 0.0f));
            ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_Bg, ImColor(0.1f, 0.105f, 0.11f, 1.0f));
            ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBg, ImColor(0.2f, 0.205f, 0.21f, 1.0f));
            // draw nodes
            for (int i = 0; i < states.size(); ++i) {
                auto state = states[i];
                auto nodeAndPinIDs = getNodeAndPinIDsFromStateID(i);
                auto inputPinID = std::get<0>(nodeAndPinIDs);
                auto nodeID = std::get<1>(nodeAndPinIDs);
                auto outputPinID = std::get<2>(nodeAndPinIDs);
                // example node
                ax::NodeEditor::BeginNode(nodeID);
//                std::string animationFilePath = Project::getResourceManager()->getFilePathFromGUID(state.Guid);
//                std::string shortenedAnimationFilePath = StringUtils::getFilePathRelativeToProjectFolder(
//                        animationFilePath);
//                ImGui::Text("%s", shortenedAnimationFilePath.c_str());
                ImGui::Text("%s", state.Name.c_str());
                ax::NodeEditor::BeginPin(inputPinID, ax::NodeEditor::PinKind::Input);
                ImGui::Text("-> To");   // In
                ax::NodeEditor::EndPin();
                ImGui::SameLine();
                ax::NodeEditor::BeginPin(outputPinID, ax::NodeEditor::PinKind::Output);
                ImGui::Text("From ->");  // Out
                ax::NodeEditor::EndPin();
                ax::NodeEditor::EndNode();
            }

            // draw links
            for (int transitionID = 0; transitionID < transitions.size(); ++transitionID) {
                auto transition = transitions[transitionID];
                auto node1InputPinID = std::get<0>(getNodeAndPinIDsFromStateID(transition.InputStateID));
                auto node2OutputPinID = std::get<2>(getNodeAndPinIDsFromStateID(transition.OutputStateID));
                ax::NodeEditor::Link(transitionID, node1InputPinID, node2OutputPinID);
            }

            // handle creation action, returns true if editor want to create new object (node or link)
            if (ax::NodeEditor::BeginCreate()) {
                ax::NodeEditor::PinId inputPinId, outputPinId;
                if (ax::NodeEditor::QueryNewLink(&inputPinId, &outputPinId)) {
                    if (inputPinId && outputPinId) {
                        int pin1IdCount = (int) ((inputPinId.Get() + 2) % 3);
                        int pin2IdCount = (int) ((outputPinId.Get() + 2) % 3);
                        // ensure connection is "From" to "To"
                        if (pin2IdCount == 1 && pin1IdCount == 0) {
                            if (ax::NodeEditor::AcceptNewItem()) {
                                std::vector<Component::AnimatorComponent::Condition> newConditions;
                                auto state1 = getStateForPinID((int) outputPinId.Get());
                                auto state2 = getStateForPinID((int) inputPinId.Get());
                                Component::AnimatorComponent::Transition newTransition = {
                                        .InputStateID=state1,
                                        .OutputStateID=state2,
                                        .Conditions=newConditions,
                                        .Blend=false
                                };
                                transitions.push_back(newTransition);
                            }
                        } else {
                            ax::NodeEditor::RejectNewItem(ImColor(1.0f, 0.0f, 0.0f, 1.0f), 1.0f);
                        }
                    }
                }
            }
            ax::NodeEditor::EndCreate(); // Wraps up object creation action handling.

            // handle deletion action
            if (ax::NodeEditor::BeginDelete()) {
                // There may be many links marked for deletion, let's loop over them.
                ax::NodeEditor::LinkId deletedLinkId;
                while (ax::NodeEditor::QueryDeletedLink(&deletedLinkId)) {
                    // If you agree that link can be deleted, accept deletion.
                    if (ax::NodeEditor::AcceptDeletedItem()) {
                        for (int i = 0; i < transitions.size(); ++i) {
                            auto &link = transitions[i];
                            if (i == (int) deletedLinkId.Get()) {
                                transitions.erase(transitions.begin() + i);
                                break;
                            }
                        }
                    }

                    // You may reject link deletion by calling:
                    // ed::RejectDeletedItem();
                }
            }
            ax::NodeEditor::EndDelete(); // Wrap up deletion action

            ax::NodeEditor::Suspend();
            if (ax::NodeEditor::ShowBackgroundContextMenu()) {
                contextBackground = true;
            }
            ax::NodeEditor::ShowLinkContextMenu(&contextLinkId);
            ax::NodeEditor::ShowNodeContextMenu(&contextNodeId);
            ax::NodeEditor::Resume();

            ax::NodeEditor::End();
            ax::NodeEditor::PopStyleColor(2);
            ax::NodeEditor::SetCurrentEditor(nullptr);
            ImGui::EndChild();

            // update popups
            updatePopups();

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
        settingsFilePath = Project::getResourceManager()->getFilePathFromGUID(animatorFileGUID).append(
                ".editor-layout.json.dream-ignore");
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
        for (const YAML::Node &animationNode: statesNode) {
            states.push_back(Component::AnimatorComponent::State{
                    .Guid=animationNode["Guid"].as<std::string>(),
                    .PlayOnce=animationNode["PlayOnce"].as<bool>(),
                    .Name=animationNode["Name"].as<std::string>()
            });
        }
        // deserialize transitions
        auto transitionsNodes = doc[Component::AnimatorComponent::k_transitions].as<std::vector<YAML::Node>>();
        for (const YAML::Node &transitionNode: transitionsNodes) {
            std::vector<Component::AnimatorComponent::Condition> conditions;
            auto conditionNodes = transitionNode[Component::AnimatorComponent::k_transition_Conditions].as<std::vector<YAML::Node>>();
            for (const YAML::Node &conditionNode: conditionNodes) {
                Component::AnimatorComponent::Condition condition = {
                        .Variable1Idx=conditionNode["Variable1Idx"] ? conditionNode["Variable1Idx"].as<int>() : -1,
                        .Variable1=conditionNode["Variable1"] ? conditionNode["Variable1"].as<int>() : 0,
                        .Operator=conditionNode["Operator"] ? conditionNode["Operator"].as<std::string>() : "==",
                        .Variable2Idx=conditionNode["Variable2Idx"] ? conditionNode["Variable2Idx"].as<int>() : -1,
                        .Variable2=conditionNode["Variable2"] ? conditionNode["Variable2"].as<int>() : 0,
                };
                conditions.push_back(condition);
            }
            auto inputNodeID = transitionNode[Component::AnimatorComponent::k_transition_InputStateID].as<int>();
            auto outputNodeID = transitionNode[Component::AnimatorComponent::k_transition_OutputStateID].as<int>();
            auto blend = transitionNode["Blend"].as<bool>();
            Component::AnimatorComponent::Transition transition = {
                    .InputStateID=inputNodeID,
                    .OutputStateID=outputNodeID,
                    .Conditions=conditions,
                    .Blend=blend
            };
            transitions.push_back(transition);
        }
        // deserialize variables
        auto variablesNode = doc[Component::AnimatorComponent::k_variables].as<std::vector<YAML::Node>>();
        for (const YAML::Node &variableNode: variablesNode) {
            auto name = variableNode[Component::AnimatorComponent::k_variable_name].as<std::string>();
            int value = variableNode[Component::AnimatorComponent::k_variable_value].as<int>();
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
        YAML::Node statesNode = YAML::Node(YAML::NodeType::Sequence);
        for (const auto &state: states) {
            YAML::Node stateNode = YAML::Node(YAML::NodeType::Map);
            stateNode["Guid"] = state.Guid;
            stateNode["PlayOnce"] = state.PlayOnce;
            stateNode["Name"] = state.Name;
            statesNode.push_back(stateNode);
        }
        out << YAML::Value << statesNode;
        // serialize transitions
        out << YAML::Key << Component::AnimatorComponent::k_transitions;
        YAML::Node transitionsNode = YAML::Node(YAML::NodeType::Sequence);
        for (const auto &transition: transitions) {
            YAML::Node transitionNode;
            transitionNode[Component::AnimatorComponent::k_transition_InputStateID] = transition.InputStateID;
            transitionNode[Component::AnimatorComponent::k_transition_OutputStateID] = transition.OutputStateID;
            YAML::Node conditionsNode = YAML::Node(YAML::NodeType::Sequence);
            for (const auto &condition: transition.Conditions) {
                YAML::Node conditionNode;
                conditionNode["Variable1Idx"] = condition.Variable1Idx;
                conditionNode["Variable1"] = condition.Variable1;
                conditionNode["Operator"] = condition.Operator;
                conditionNode["Variable2Idx"] = condition.Variable2Idx;
                conditionNode["Variable2"] = condition.Variable2;
                conditionsNode.push_back(conditionNode);
            }
            transitionNode[Component::AnimatorComponent::k_transition_Conditions] = conditionsNode;
            transitionNode["Blend"] = transition.Blend;
            transitionsNode.push_back(transitionNode);
        }
        out << YAML::Value << transitionsNode;
        // serialize variables
        out << YAML::Key << Component::AnimatorComponent::k_variables;
        YAML::Node variablesNode = YAML::Node(YAML::NodeType::Sequence);
        for (int i = 0; i < variableNames.size(); ++i) {
            YAML::Node variableNode;
            variableNode[Component::AnimatorComponent::k_variable_name] = variableNames[i];
            variableNode[Component::AnimatorComponent::k_variable_value] = variableValues[i];
            variablesNode.push_back(variableNode);
        }
        out << YAML::Value << variablesNode;
        out << YAML::EndMap;
        std::ofstream fout(animatorFilePath.c_str());
        fout << out.c_str();
        fout.close();
        // update all entities with this animator
        auto animatorEntities = Project::getScene()->getEntitiesWithComponents<Component::AnimatorComponent>();
        for (auto entityHandle: animatorEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            if (entity.getComponent<Component::AnimatorComponent>().guid == animatorFileGUID) {
                entity.getComponent<Component::AnimatorComponent>().loadStateMachine(entity);
            }
        }
    }

    std::tuple<int, int, int> ImGuiEditorAnimatorGraph::getNodeAndPinIDsFromStateID(int nodeIdx) {
        return std::make_tuple(3 * nodeIdx - 1, 3 * nodeIdx, 3 * nodeIdx + 1);
    }

    int ImGuiEditorAnimatorGraph::getStateForPinID(int pinID) {
        if (pinID % 3 == 0) {
            Logger::fatal("NodeID, not PinID");
        } else if (pinID % 3 == 2) {
            return (pinID + 1) / 3;
        } else if (pinID % 3 == 1) {
            return (pinID - 1) / 3;
        } else if (pinID % 3 == -1) {
            return 0;
        }
        Logger::fatal("Invalid state " + std::to_string(pinID % 3));
        return -1;
    }

    int ImGuiEditorAnimatorGraph::getStateForNodeID(int nodeID) {
        if (nodeID % 3 != 0) {
            Logger::fatal("PinID, not NodeID");
        }
        return nodeID / 3;
    }

    void ImGuiEditorAnimatorGraph::updatePopups() {
        if (contextBackground) {
            ImGui::OpenPopup("Background");
        } else if (contextLinkId.Get() != -1) {
            ImGui::OpenPopup("Link");
        } else if (contextNodeId.Get() != -1) {
            ImGui::OpenPopup("Node");
        }

        if (ImGui::BeginPopup("Background")) {
            if (ImGui::MenuItem("New animation")) {
                selectNewAnimation();
                contextBackground = false;
            }
            if (ImGui::MenuItem("Close popup")) {
                contextBackground = false;
            }
            ImGui::EndPopup();
        } else if (ImGui::BeginPopup("Link")) {
            if (ImGui::MenuItem("Remove link")) {
                int linkToRemove = (int) contextLinkId.Get();
                transitions.erase(transitions.begin() + linkToRemove);
                contextLinkId = -1;
            }
            if (ImGui::MenuItem("Close popup")) {
                contextLinkId = -1;
            }
            ImGui::EndPopup();
        } else if (ImGui::BeginPopup("Node")) {
            if (ImGui::MenuItem("Remove node")) {
                int stateToRemove = getStateForNodeID((int) contextNodeId.Get());
                std::cout << "Going to remove state " << stateToRemove << std::endl;
                // remove transitions that reference this node
                for (int i = (int) transitions.size() - 1; i >= 0; --i) {
                    auto transition = transitions[i];
                    if (transition.OutputStateID == stateToRemove || transition.InputStateID == stateToRemove) {
                        transitions.erase(transitions.begin() + i);
                    }
                }
                // increment id of transitions after this node
                for (int i = (int) transitions.size() - 1; i >= 0; --i) {
                    auto &transition = transitions[i];
                    if (transition.OutputStateID > stateToRemove) {
                        transition.OutputStateID--;
                    }
                    if (transition.InputStateID > stateToRemove) {
                        transition.InputStateID--;
                    }
                }
                // remove the node
                states.erase(states.begin() + stateToRemove);
                contextNodeId = -1;
            }
            if (ImGui::MenuItem("Close popup")) {
                contextNodeId = -1;
            }
            ImGui::EndPopup();
        }
    }

    void ImGuiEditorAnimatorGraph::updateStateMachinePanel(float stateMachinePanelWidth) {
        ImGui::BeginChild("State Machine", ImVec2(stateMachinePanelWidth, 0));
        auto cursorPosX1 = ImGui::GetCursorPosX();
        // states
        if (ImGui::TreeNodeEx("states", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth)) {
            auto cursorPosX2 = ImGui::GetCursorPosX();
            auto treeNodeWidth = ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1);
            for (int i = 0; i < states.size(); ++i) {
                auto label = "state " + std::to_string(i);
                if (ImGui::TreeNodeEx(label.c_str(),
                                      ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth)) {
                    auto cursorPosX3 = ImGui::GetCursorPosX();
                    auto innerTreeNodeWidth = ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1) -
                                              (cursorPosX3 - cursorPosX2);
                    ImGui::Text("Path: %s", StringUtils::getFilePathRelativeToProjectFolder(
                            Project::getResourceManager()->getFilePathFromGUID(states[i].Guid)).c_str());
                    ImGui::SetNextItemWidth(innerTreeNodeWidth);
                    ImGui::InputText(("##AnimationName /" + std::to_string(i)).c_str(), &states[i].Name);
                    ImGui::Checkbox(("Play at least once ##PlayOnceCheckBox" + std::to_string(i)).c_str(),
                                    &states[i].PlayOnce);
                    ImGui::TreePop();
                }
            }
            if (ImGui::Button("Add", ImVec2(treeNodeWidth, 0))) {
                selectNewAnimation();
            }
            ImGui::TreePop();
        }
        // transitions
        if (ImGui::TreeNodeEx("transitions", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth)) {
            auto cursorPosX2 = ImGui::GetCursorPosX();
            auto treeNodeWidth = ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1);
            for (int i = 0; i < transitions.size(); ++i) {
                auto &transition = transitions.at(i);
                auto label = "state " + std::to_string(transition.OutputStateID) + " to state " +
                             std::to_string(transition.InputStateID);
                if (ImGui::TreeNodeEx(label.c_str(),
                                      ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth)) {
                    auto cursorPosX3 = ImGui::GetCursorPosX();
                    {
                        // edit 'from' conditions for transition
                        ImGui::Text("from ");
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(120);
                        auto fromFilename = std::filesystem::path(Project::getResourceManager()->getFilePathFromGUID(
                                states[transition.OutputStateID].Guid)).filename();
                        if (ImGui::BeginCombo(("##From/" + std::to_string(i)).c_str(), fromFilename.c_str())) {
                            for (int j = 0; j < states.size(); ++j) {
                                auto filename = std::filesystem::path(
                                        Project::getResourceManager()->getFilePathFromGUID(states[j].Guid)).filename();
                                if (ImGui::Selectable(
                                        std::string("[" + std::to_string(j) + "] " + filename.string()).c_str())) {
                                    transition.OutputStateID = j;
                                }
                            }
                            ImGui::EndCombo();
                        }
                    }
                    {
                        // edit 'to' conditions for transition
                        ImGui::Text("to ");
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(120);
                        auto toFilename = std::filesystem::path(Project::getResourceManager()->getFilePathFromGUID(
                                states[transition.InputStateID].Guid)).filename();
                        if (ImGui::BeginCombo(("##To/" + std::to_string(i)).c_str(), toFilename.c_str())) {
                            for (int j = 0; j < states.size(); ++j) {
                                auto filename = std::filesystem::path(
                                        Project::getResourceManager()->getFilePathFromGUID(states[j].Guid)).filename();
                                if (ImGui::Selectable(
                                        std::string("[" + std::to_string(j) + "] " + filename.string()).c_str())) {
                                    transition.InputStateID = j;
                                }
                            }
                            ImGui::EndCombo();
                        }
                    }
                    {
                        // enable or disable blending
                        ImGui::Checkbox(("Blend ##BlendCheckBox" + std::to_string(i)).c_str(), &transition.Blend);
                    }
                    for (int j = 0; j < transition.Conditions.size(); ++j) {
                        Component::AnimatorComponent::Condition &condition = transition.Conditions.at(j);
                        {
                            // edit variable 1 for condition
                            ImGui::SetNextItemWidth(100);
                            auto uniqueLabel = std::to_string(i) + "/" + std::to_string(j);
                            if (ImGui::BeginCombo(("##Var1/" + uniqueLabel).c_str(),
                                                  condition.Variable1Idx == -1 ? "Custom" : variableNames.at(
                                                          condition.Variable1Idx).c_str())) {
                                if (ImGui::Selectable("custom")) {
                                    condition.Variable1Idx = -1;
                                }
                                for (int k = 0; k < variableNames.size(); ++k) {
                                    if (ImGui::Selectable(variableNames[k].c_str())) {
                                        condition.Variable1Idx = k;
                                    }
                                }
                                ImGui::EndCombo();
                            }
                            if (condition.Variable1Idx == -1) {
                                ImGui::SameLine();
                                ImGui::SetNextItemWidth(20);
                                ImGui::InputInt(("##Var1Input/" + uniqueLabel).c_str(), &condition.Variable1, 0);
                            }
                        }
                        {
                            // edit operator for condition
                            ImGui::SameLine();
                            auto uniqueLabel = std::to_string(i) + "/" + std::to_string(j);
                            ImGui::SetNextItemWidth(50);
                            if (ImGui::BeginCombo(("##Operator/" + uniqueLabel).c_str(), condition.Operator.c_str())) {
                                std::vector<std::string> operators;
                                operators.emplace_back("==");
                                operators.emplace_back(">");
                                operators.emplace_back("<");
                                operators.emplace_back(">=");
                                operators.emplace_back("<=");
                                for (auto const &op: operators) {
                                    if (ImGui::Selectable(op.c_str())) {
                                        condition.Operator = op;
                                    }
                                }
                                ImGui::EndCombo();
                            }
                        }
                        {
                            // edit variable 2 for condition
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(100);
                            auto uniqueLabel = std::to_string(i) + "/" + std::to_string(j);
                            if (ImGui::BeginCombo(("##Var2/" + uniqueLabel).c_str(),
                                                  condition.Variable2Idx == -1 ? "Custom" : variableNames.at(
                                                          condition.Variable2Idx).c_str())) {
                                if (ImGui::Selectable("custom")) {
                                    condition.Variable2Idx = -1;
                                }
                                for (int k = 0; k < variableNames.size(); ++k) {
                                    if (ImGui::Selectable(variableNames[k].c_str())) {
                                        condition.Variable2Idx = k;
                                    }
                                }
                                ImGui::EndCombo();
                            }
                            if (condition.Variable2Idx == -1) {
                                ImGui::SameLine();
                                ImGui::SetNextItemWidth(20);
                                ImGui::InputInt(("##Var2Input/" + uniqueLabel).c_str(), &condition.Variable2, 0);
                            }
                        }
                        // remove condition
                        ImGui::SameLine();
                        if (ImGui::Button(("X##CondRemoveBtn/" + std::to_string(i) + "/" + std::to_string(j)).c_str(),
                                          ImVec2(0.f, 0.f))) {
                            transition.Conditions.erase(transition.Conditions.begin() + j);
                        }
                    }
                    auto innerTreeNodeWidth = ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1) -
                                              (cursorPosX3 - cursorPosX2);
                    if (ImGui::Button(("Add ##Condition/" + std::to_string(i)).c_str(), ImVec2(innerTreeNodeWidth, 0))) {
                        transition.Conditions.push_back(Component::AnimatorComponent::Condition{
                                .Variable1Idx = -1,
                                .Variable1 = 0,
                                .Operator = "==",
                                .Variable2Idx = -1,
                                .Variable2 = 0
                        });
                    }
                    ImGui::TreePop();
                }
            }
            if (states.size() >= 2) {
                if (ImGui::Button("Add", ImVec2(treeNodeWidth, 0))) {
                    std::vector<Component::AnimatorComponent::Condition> conditions;
                    transitions.push_back(Component::AnimatorComponent::Transition{
                            .InputStateID=0,
                            .OutputStateID=1,
                            .Conditions=conditions,
                            .Blend=false
                    });
                }
            }
            ImGui::TreePop();
        }
        // variables
        if (ImGui::TreeNodeEx("variables", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth)) {
            auto cursorPosX2 = ImGui::GetCursorPosX();
            auto treeNodeWidth = ImGui::GetWindowContentRegionWidth() - (cursorPosX2 - cursorPosX1);
            for (int i = 0; i < variableNames.size(); ++i) {
                float InputWidth = 40.0f;
                ImGui::SetNextItemWidth(treeNodeWidth - InputWidth - 25);
                ImGui::InputText(("##variable" + std::to_string(i)).c_str(), &variableNames[i]);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(InputWidth - 8);
                ImGui::InputInt(("##variable-edit" + std::to_string(i)).c_str(), &variableValues[i], 0);
                ImGui::SameLine();
                if (ImGui::Button(("X##VariableRemoveBtn/" + std::to_string(i)).c_str())) {
                    variableNames.erase(variableNames.begin() + i);
                    variableValues.erase(variableValues.begin() + i);
                    for (auto &transition: transitions) {
                        // remove conditions that rely on this variable
                        for (int j = (int) transition.Conditions.size() - 1; j >= 0; --j) {
                            if (transition.Conditions[j].Variable1Idx == i ||
                                transition.Conditions[j].Variable2Idx == i) {
                                transition.Conditions.erase(transition.Conditions.begin() + j);
                            }
                        }
                        // update variable indices for transition conditions
                        for (auto &condition: transition.Conditions) {
                            if (condition.Variable1Idx > i) {
                                condition.Variable1Idx--;
                            }
                            if (condition.Variable2Idx > i) {
                                condition.Variable2Idx--;
                            }
                        }
                    }
                }
            }
            if (ImGui::Button("Add", ImVec2(treeNodeWidth, 0))) {
                variableNames.emplace_back("variable");
                variableValues.push_back(0);
            }
            ImGui::TreePop();
        }

        ImGui::EndChild();

    }

    void ImGuiEditorAnimatorGraph::selectNewAnimation() {
        delete animationSelectorBrowser;
        animationSelectorBrowser = new ImGui::FileBrowser();
        animationSelectorBrowser->SetTitle("add animation");
        animationSelectorBrowser->SetPwd(Project::getPath());
        animationSelectorBrowser->Open();
    }
}