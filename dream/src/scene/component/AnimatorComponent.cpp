//
// Created by Deepak Ramalingam on 12/3/22.
//

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/renderer/Animation.h"
#include "dream/project/Project.h"
#include "dream/util/Logger.h"
#include "dream/util/MathUtils.h"

namespace Dream::Component {
    AnimatorComponent::AnimatorComponent() {
        this->guid = "";
        this->currentState = -1;
        this->m_CurrentTime = 0.0;
        this->m_CurrentAnimation = nullptr;
        this->m_FinalBoneMatrices.reserve(MAX_BONES);
        for (int i = 0; i < MAX_BONES; i++) {
            this->m_FinalBoneMatrices.emplace_back(1.0f);
        }
    }

    AnimatorComponent::AnimatorComponent(std::string animatorGUID) {
        this->guid = std::move(animatorGUID);
        this->currentState = -1;
        this->m_CurrentTime = 0.0;
        this->m_CurrentAnimation = nullptr;
        this->m_FinalBoneMatrices.reserve(MAX_BONES);
        for (int i = 0; i < MAX_BONES; i++) {
            this->m_FinalBoneMatrices.emplace_back(1.0f);
        }
    }

    AnimatorComponent::~AnimatorComponent() {
        // TODO: delete each animation in map
    }

    void AnimatorComponent::updateAnimation(float dt) {
        if (m_CurrentAnimation) {
            auto* animation = (Animation *) m_CurrentAnimation;
            m_CurrentTime += animation->getTicksPerSecond() * dt;
            if (m_CurrentTime > animation->getDuration()) {
                numTimesAnimationPlayed = (numTimesAnimationPlayed + 1) % INT_MAX;
            }
            m_CurrentTime = fmod(m_CurrentTime, animation->getDuration());
            calculateBoneTransform(&animation->getRootNode(), glm::mat4(1.0f));
        }
    }

    void AnimatorComponent::updateStateMachine(float dt) {
//        std::cout << "===========" << std::endl;
//        std::cout << "current state: " << currentState << std::endl;
//        for (int i = 0; i < variableNames.size(); ++i) {
//            std::cout << variableNames[i] << " : " << variableValues[i] << std::endl;
//        }
//        std::cout << "===========" << std::endl << std::endl;
        if (m_CurrentAnimation) {
            for (const auto &transition : transitions) {
                if (transition.OutputStateID == currentState) {
                    bool allConditionsPassed = true;
                    for (const auto &condition : transition.Conditions) {
                        int variable1Value = condition.Variable1;
                        int variable2Value = condition.Variable2;
                        if (condition.Variable1Idx != -1) {
                            variable1Value = variableValues.at(condition.Variable1Idx);
                        }
                        if (condition.Variable2Idx != -1) {
                            variable2Value = variableValues.at(condition.Variable2Idx);
                        }
                        bool conditionPassed = false;
                        if (condition.Operator == "==") {
                            conditionPassed = variable1Value == variable2Value;
                        } else if (condition.Operator == ">") {
                            conditionPassed = variable1Value > variable2Value;
                        } else if (condition.Operator == "<") {
                            conditionPassed = variable1Value < variable2Value;
                        } else if (condition.Operator == ">=") {
                            conditionPassed = variable1Value >= variable2Value;
                        } else if (condition.Operator == "<=") {
                            conditionPassed = variable1Value <= variable2Value;
                        } else if (condition.Operator == "!=") {
                            conditionPassed = variable1Value != variable2Value;
                        } else {
                            Logger::fatal("Unknown operator for animator state machine " + condition.Operator);
                        }
                        if (!conditionPassed) {
                            allConditionsPassed = false;
                        }
                    }
                    if (allConditionsPassed) {
                        int numRequiredTimesToPlay = states[transition.OutputStateID].PlayOnce ? 1 : 0;
                        if (numTimesAnimationPlayed >= numRequiredTimesToPlay) {
                            playAnimation(transition.InputStateID);
                        } else {
                            // TODO: do animation blending here
//                            std::cout << "should transition to next state " << transition.InputStateID << std::endl;
                        }
                        break;
                    }
                }
            }
        }

//        if (m_CurrentAnimation) {
//            if (numTimesAnimationPlayed >= 1) {
//                for (const auto &transition : transitions) {
//                    if (transition.OutputStateID == currentState) {
//                        bool allConditionsPassed = true;
//                        for (const auto &condition : transition.Conditions) {
//                            int variable1Value = condition.Variable1;
//                            int variable2Value = condition.Variable2;
//                            if (condition.Variable1Idx != -1) {
//                                variable1Value = variableValues.at(condition.Variable1Idx);
//                            }
//                            if (condition.Variable2Idx != -1) {
//                                variable2Value = variableValues.at(condition.Variable2Idx);
//                            }
//                            bool conditionPassed = false;
//                            if (condition.Operator == "==") {
//                                conditionPassed = variable1Value == variable2Value;
//                            } else if (condition.Operator == ">") {
//                                conditionPassed = variable1Value > variable2Value;
//                            } else if (condition.Operator == "<") {
//                                conditionPassed = variable1Value < variable2Value;
//                            } else if (condition.Operator == ">=") {
//                                conditionPassed = variable1Value >= variable2Value;
//                            } else if (condition.Operator == "<=") {
//                                conditionPassed = variable1Value <= variable2Value;
//                            } else if (condition.Operator == "!=") {
//                                conditionPassed = variable1Value != variable2Value;
//                            } else {
//                                Logger::fatal("Unknown operator for animator state machine " + condition.Operator);
//                            }
//                            if (!conditionPassed) {
//                                allConditionsPassed = false;
//                            }
//                        }
//                        if (allConditionsPassed) {
//                            playAnimation(transition.InputStateID);
//                            break;
//                        }
//                    }
//                }
//            }
//        }
    }

    void AnimatorComponent::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform, int depth) {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        AnimationBone* Bone = ((Animation *) m_CurrentAnimation)->findBone(nodeName);

        if (Bone) {
            Bone->update(m_CurrentTime);
            nodeTransform = Bone->getLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = ((Animation *) m_CurrentAnimation)->getBoneIdMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = globalTransformation * offset;

            if (Bone) {
                if (boneEntities.count(Bone->getBoneID()) > 0) {
                    glm::vec3 scale;
                    glm::quat rotation;
                    glm::vec3 translation;
                    if (depth == 1) {
                        // we don't count 'RootNode' as a bone, so we have to do this to include its transformation
                        // for the first layer of bones
                        MathUtils::decomposeMatrix(parentTransform * nodeTransform, translation, rotation, scale);
                    } else {
                        // all bones after the first layer (usually everything after hip bone for skeletons)
                        MathUtils::decomposeMatrix(nodeTransform, translation, rotation, scale);
                    }
                    boneEntities[Bone->getBoneID()].getComponent<TransformComponent>().translation = translation;
                    boneEntities[Bone->getBoneID()].getComponent<TransformComponent>().rotation = rotation;
                    boneEntities[Bone->getBoneID()].getComponent<TransformComponent>().scale = scale;
                } else {
                    Logger::warn("Cannot find entity for bone " + Bone->getBoneName());
                }
            }
        }

        for (int i = 0; i < node->childrenCount; i++) {
            calculateBoneTransform(&node->children[i], globalTransformation, depth + 1);
        }
    }

    std::vector<glm::mat4> AnimatorComponent::computeFinalBoneMatrices(Entity armatureEntity, std::vector<Entity> bones) {
        std::vector<glm::mat4> finalBoneMatrices;
        for (int i = 0; i < MAX_BONES; ++i) {
            finalBoneMatrices.emplace_back(1.0);
        }

        for (auto boneEntity : bones) {
            auto boneID = boneEntity.getComponent<BoneComponent>().boneID;
            if (boneID < finalBoneMatrices.size()) {
                auto trans = glm::mat4(1.0);
//                trans = glm::scale(trans, glm::vec3(2, 3, 2));
                finalBoneMatrices.at(boneID) = trans;
            }
        }

        return finalBoneMatrices;
    }

    void AnimatorComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            if (node[componentName][AnimatorComponent::k_guid]) {
                auto guid = node[componentName][AnimatorComponent::k_guid].as<std::string>();
                entity.addComponent<AnimatorComponent>(guid);
            } else {
                entity.addComponent<AnimatorComponent>();
            }
        }
    }

    void AnimatorComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<AnimatorComponent>()) {
            out << YAML::Key << AnimatorComponent::componentName;
            out << YAML::BeginMap;
            out << YAML::Key << AnimatorComponent::k_guid << YAML::Value << entity.getComponent<AnimatorComponent>().guid;
            out << YAML::EndMap;
        }
    }

    void AnimatorComponent::loadStateMachine(Entity modelEntity) {
        if (guid.empty()) {
            return;
        }
        currentState = 0;
        // reset animations state
        states.clear();
        transitions.clear();
        variableNames.clear();
        variableValues.clear();
        // deserialize animator file
        std::string animatorFilePath = Project::getResourceManager()->getFilePathFromGUID(guid);
        YAML::Node doc = YAML::LoadFile(animatorFilePath);
        // load states
        auto animationsNode = doc[k_states].as<std::vector<YAML::Node>>();
        for (const YAML::Node& animationNode : animationsNode) {
            states.push_back(State{
                .Guid=animationNode["Guid"].as<std::string>(),
                .PlayOnce=animationNode["PlayOnce"].as<bool>()
            });
        }
        // deserialize transitions
        auto transitionsNodes = doc[k_transitions].as<std::vector<YAML::Node>>();
        for (const YAML::Node& transitionNode : transitionsNodes) {
            std::vector<Condition> conditions;
            auto conditionNodes = transitionNode[k_transition_Conditions].as<std::vector<YAML::Node>>();
            for (const YAML::Node& conditionNode : conditionNodes) {
                Condition condition = {
                        .Variable1Idx=conditionNode["Variable1Idx"] ? conditionNode["Variable1Idx"].as<int>() : -1,
                        .Variable1=conditionNode["Variable1"] ? conditionNode["Variable1"].as<int>() : 0,
                        .Operator=conditionNode["Operator"] ? conditionNode["Operator"].as<std::string>() : "==",
                        .Variable2Idx=conditionNode["Variable2Idx"] ? conditionNode["Variable2Idx"].as<int>() : -1,
                        .Variable2=conditionNode["Variable2"] ? conditionNode["Variable2"].as<int>() : 0,
                };
                conditions.push_back(condition);
            }
            auto inputNodeID = transitionNode[k_transition_InputStateID].as<int>();
            auto outputNodeID = transitionNode[k_transition_OutputStateID].as<int>();
            AnimatorComponent::Transition transition = {
                    .InputStateID=inputNodeID,
                    .OutputStateID=outputNodeID,
                    .Conditions=conditions
            };
            transitions.push_back(transition);
        }
        // load variables
        auto variablesNode = doc[k_variables].as<std::vector<YAML::Node>>();
        for (const YAML::Node& variableNode : variablesNode) {
            auto name = variableNode[k_variable_name].as<std::string>();
            int value = variableNode[k_variable_value].as<int>();
            variableNames.emplace_back(name);
            variableValues.emplace_back(value);
        }
        auto transitionsNode = doc[k_transitions].as<std::vector<YAML::Node>>();
        // load animation data from animation files
        if (!states.empty()) {
            for (const auto& state : states) {
                auto animationFilePath = Project::getResourceManager()->getFilePathFromGUID(state.Guid);
                auto *anim = new Animation(animationFilePath, modelEntity, 0);
                animationObjects[state.Guid] = anim;
//                m_CurrentAnimation = anim;
                if (animationObjects.size() > INT_MAX) {
                    Logger::fatal("Too many animations to store in memory");
                }
//                currentState = (int) animationObjects.size() - 1;
                currentState = 0;
            }
            if (currentState >= 0) {
                playAnimation(currentState);
            }
        }
        if (m_CurrentAnimation && needsToFindBoneEntities) {
            boneEntities.clear();
            loadBoneEntities(modelEntity);
            needsToFindBoneEntities = false;
        }
        this->needsToLoadAnimations = false;
    }

    void AnimatorComponent::loadBoneEntities(Entity entity) {
        if (entity.hasComponent<BoneComponent>()) {
            boneEntities[entity.getComponent<BoneComponent>().boneID] = entity;
        }
        Entity child = entity.getComponent<HierarchyComponent>().first;
        while (child) {
            loadBoneEntities(child);
            child = child.getComponent<HierarchyComponent>().next;
        }
    }

    void AnimatorComponent::playAnimation(int stateID) {
//        Logger::debug("Playing animation " + std::to_string(stateID));
        numTimesAnimationPlayed = 0;
        auto state = states[stateID];
        if (animationObjects.count(state.Guid) > 0) {
            m_CurrentAnimation = animationObjects[state.Guid];
            currentState = stateID;
        } else {
            Logger::fatal("Unable to find animation with GUID " + state.Guid);
        }
    }

    int AnimatorComponent::setVariable(const std::string& variableName, int value) {
        auto iter = std::find(variableNames.begin(), variableNames.end(), variableName);
        if (iter != variableNames.end()) {
            int index = (int) std::distance(variableNames.begin(), iter);
            variableValues[index] = value;
            return index;
        } else {
            Logger::error("Unable to find variable " + variableName + " in animator");
            return -1;
        }
    }
}