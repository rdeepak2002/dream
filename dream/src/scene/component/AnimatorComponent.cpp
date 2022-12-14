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

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/renderer/Animation.h"
#include "dream/project/Project.h"
#include "dream/util/Logger.h"
#include "dream/util/MathUtils.h"

namespace Dream::Component {
    AnimatorComponent::AnimatorComponent() {
        this->blendFactor = 1.0;
        this->guid = "";
        this->currentState = -1;
        this->nextState = -1;
        this->m_CurrentTime = 0.0;
//        this->m_CurrentAnimation = nullptr;
        this->m_FinalBoneMatrices.reserve(MAX_BONES);
        for (int i = 0; i < MAX_BONES; i++) {
            this->m_FinalBoneMatrices.emplace_back(1.0f);
        }
    }

    AnimatorComponent::AnimatorComponent(std::string animatorGUID) {
        this->blendFactor = 1.0;
        this->guid = std::move(animatorGUID);
        this->currentState = -1;
        this->nextState = -1;
        this->m_CurrentTime = 0.0;
//        this->m_CurrentAnimation = nullptr;
        this->m_FinalBoneMatrices.reserve(MAX_BONES);
        for (int i = 0; i < MAX_BONES; i++) {
            this->m_FinalBoneMatrices.emplace_back(1.0f);
        }
    }

    AnimatorComponent::~AnimatorComponent() {
        // TODO: delete each animation in map
    }

    void AnimatorComponent::updateAnimation(float dt) {
        if (currentState != -1 && nextState != -1) {
            auto *animation1 = (Animation *) animationObjects[states[currentState].Guid];
            auto *animation2 = (Animation *) animationObjects[states[nextState].Guid];
            blendTwoAnimations(animation1, animation2, blendFactor, dt);
            blendFactor += 5.0f * dt;
            if (blendFactor > 1.0) {
                blendFactor = 1.0;
                if (currentState != nextState) {
                    currentState = nextState;
                    numTimesAnimationPlayed = 0;
//                    currentTimeLayered = 0.0f;
//                    currentTimeBase = 0.0f;
                }
            }
        }
    }

    void AnimatorComponent::updateStateMachine(float dt) {
//        std::cout << "===========" << std::endl;
//        std::cout << "current state: " << currentState << std::endl;
//        for (int i = 0; i < variableNames.size(); ++i) {
//            std::cout << variableNames[i] << " : " << variableValues[i] << std::endl;
//        }
//        std::cout << "===========" << std::endl << std::endl;
        if (currentState != -1 && currentState == nextState) {
            for (const auto &transition: transitions) {
                if (transition.OutputStateID == currentState) {
                    bool allConditionsPassed = true;
                    for (const auto &condition: transition.Conditions) {
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
                        int numRequiredTimesToPlay = states[currentState].PlayOnce ? 1 : 0;
                        if (numTimesAnimationPlayed >= numRequiredTimesToPlay) {
                            nextState = transition.InputStateID;
                            blendFactor = transition.Blend ? 0.0 : 1.0;
                            numTimesAnimationPlayed = 0;
                            if (!transition.Blend) {
                                currentTimeLayered = 0.0f;
                                currentTimeBase = 0.0f;
                            }
                        }
                        break;
                    }
                }
            }
        }
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
            out << YAML::Key << AnimatorComponent::k_guid << YAML::Value
                << entity.getComponent<AnimatorComponent>().guid;
            out << YAML::EndMap;
        }
    }

    void AnimatorComponent::loadStateMachine(Entity modelEntity) {
        if (guid.empty()) {
            return;
        }
        currentState = 0;
        nextState = 0;
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
        for (const YAML::Node &animationNode: animationsNode) {
            states.push_back(State{
                    .Guid=animationNode["Guid"].as<std::string>(),
                    .PlayOnce=animationNode["PlayOnce"].as<bool>(),
                    .Name=animationNode["Name"].as<std::string>()
            });
        }
        // deserialize transitions
        auto transitionsNodes = doc[k_transitions].as<std::vector<YAML::Node>>();
        for (const YAML::Node &transitionNode: transitionsNodes) {
            std::vector<Condition> conditions;
            auto conditionNodes = transitionNode[k_transition_Conditions].as<std::vector<YAML::Node>>();
            for (const YAML::Node &conditionNode: conditionNodes) {
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
            auto blend = transitionNode["Blend"].as<bool>();
            AnimatorComponent::Transition transition = {
                    .InputStateID=inputNodeID,
                    .OutputStateID=outputNodeID,
                    .Conditions=conditions,
                    .Blend = blend
            };
            transitions.push_back(transition);
        }
        // load variables
        auto variablesNode = doc[k_variables].as<std::vector<YAML::Node>>();
        for (const YAML::Node &variableNode: variablesNode) {
            auto name = variableNode[k_variable_name].as<std::string>();
            int value = variableNode[k_variable_value].as<int>();
            variableNames.emplace_back(name);
            variableValues.emplace_back(value);
        }
        auto transitionsNode = doc[k_transitions].as<std::vector<YAML::Node>>();
        // load animation data from animation files
        if (!states.empty()) {
            for (const auto &state: states) {
                auto animationFilePath = Project::getResourceManager()->getFilePathFromGUID(state.Guid);
                auto *anim = new Animation(animationFilePath, modelEntity, 0);
                animationObjects[state.Guid] = anim;
                if (animationObjects.size() > INT_MAX) {
                    Logger::fatal("Too many animations to store in memory");
                }
                currentState = 0;
                nextState = 0;
            }
        }
        boneEntities.clear();
        loadBoneEntities(modelEntity);
        needsToFindBoneEntities = false;
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

    int AnimatorComponent::setVariable(const std::string &variableName, int value) {
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

    void AnimatorComponent::calculateBlendedBoneTransform(void *pAnimationBaseV, const AssimpNodeData *node,
                                                          void *pAnimationLayerV, const AssimpNodeData *nodeLayered,
                                                          const float currentTimeBase, const float currentTimeLayered,
                                                          const glm::mat4 &parentTransform,
                                                          const float blendFactor,
                                                          int depth) {
        Animation *pAnimationBase = (Animation *) pAnimationBaseV;
        Animation *pAnimationLayer = (Animation *) pAnimationLayerV;

        const std::string &nodeName = node->name;

        glm::mat4 nodeTransform = node->transformation;
        AnimationBone *pBone = pAnimationBase->findBone(nodeName);
        if (pBone) {
            pBone->update(currentTimeBase);
            nodeTransform = pBone->getLocalTransform();
        }

        glm::mat4 layeredNodeTransform = nodeLayered->transformation;
        pBone = pAnimationLayer->findBone(nodeName);
        if (pBone) {
            pBone->update(currentTimeLayered);
            layeredNodeTransform = pBone->getLocalTransform();
        }

        // Blend two matrices
        const glm::quat rot0 = glm::quat_cast(nodeTransform);
        const glm::quat rot1 = glm::quat_cast(layeredNodeTransform);
        const glm::quat finalRot = glm::slerp(rot0, rot1, blendFactor);
        glm::mat4 blendedMat = glm::mat4_cast(finalRot);
        blendedMat[3] = (1.0f - blendFactor) * nodeTransform[3] + layeredNodeTransform[3] * blendFactor;

        glm::mat4 globalTransformation = parentTransform * blendedMat;

        const auto &boneInfoMap = pAnimationBase->getBoneIdMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            const int index = boneInfoMap.at(nodeName).id;
            const glm::mat4 &offset = boneInfoMap.at(nodeName).offset;

            m_FinalBoneMatrices[index] = globalTransformation * offset;

            if (pBone) {
                if (boneEntities.count(pBone->getBoneID()) > 0) {
                    glm::vec3 scale;
                    glm::quat rotation;
                    glm::vec3 translation;
                    if (depth == 1) {
                        // we don't count 'RootNode' as a bone, so we have to do this to include its transformation
                        // for the first layer of bones
                        MathUtils::decomposeMatrix(parentTransform * blendedMat, translation, rotation, scale);
                    } else {
                        // all bones after the first layer (usually everything after hip bone for skeletons)
                        MathUtils::decomposeMatrix(blendedMat, translation, rotation, scale);
                    }
                    boneEntities[pBone->getBoneID()].getComponent<TransformComponent>().translation = translation;
                    boneEntities[pBone->getBoneID()].getComponent<TransformComponent>().rotation = rotation;
                    boneEntities[pBone->getBoneID()].getComponent<TransformComponent>().scale = scale;
                } else {
                    Logger::warn("Cannot find entity for bone " + pBone->getBoneName() + " with ID " +
                                 std::to_string(pBone->getBoneID()));
                }
            }
        }

        for (size_t i = 0; i < node->children.size(); ++i)
            calculateBlendedBoneTransform(pAnimationBase, &node->children[i], pAnimationLayer,
                                          &nodeLayered->children[i], currentTimeBase, currentTimeLayered,
                                          globalTransformation, blendFactor, depth + 1);
    }

    void AnimatorComponent::blendTwoAnimations(void *pBaseAnimationV, void *pLayeredAnimationV, float blendFactor,
                                               float deltaTime) {
        Animation *pBaseAnimation = (Animation *) pBaseAnimationV;
        Animation *pLayeredAnimation = (Animation *) pLayeredAnimationV;
        // Speed multipliers to correctly transition from one animation to another
        float a = 1.0f;
        float b = pBaseAnimation->getDuration() / pLayeredAnimation->getDuration();
        const float animSpeedMultiplierUp = (1.0f - blendFactor) * a + b * blendFactor; // Lerp

        a = pLayeredAnimation->getDuration() / pBaseAnimation->getDuration();
        b = 1.0f;
        const float animSpeedMultiplierDown = (1.0f - blendFactor) * a + b * blendFactor; // Lerp

        // Current time of each animation, "scaled" by the above speed multiplier variables
        currentTimeBase += pBaseAnimation->getTicksPerSecond() * deltaTime * animSpeedMultiplierUp;
        if (states[currentState].PlayOnce && currentTimeBase >= pBaseAnimation->getDuration()) {
            currentTimeBase = pBaseAnimation->getDuration() - 0.01f;
        } else {
            currentTimeBase = fmod(currentTimeBase, pBaseAnimation->getDuration());
        }

        currentTimeLayered += pLayeredAnimation->getTicksPerSecond() * deltaTime * animSpeedMultiplierDown;
        if (currentState == nextState) {
            if (currentTimeLayered > pLayeredAnimation->getDuration()) {
                numTimesAnimationPlayed += 1;
            }
        }

        if (states[currentState].PlayOnce && currentTimeLayered >= pLayeredAnimation->getDuration()) {
            currentTimeLayered = pLayeredAnimation->getDuration() - 0.01f;
        } else {
            currentTimeLayered = fmod(currentTimeLayered, pLayeredAnimation->getDuration());
        }

        calculateBlendedBoneTransform(pBaseAnimation, &pBaseAnimation->getRootNode(), pLayeredAnimation,
                                      &pLayeredAnimation->getRootNode(), currentTimeBase, currentTimeLayered,
                                      glm::mat4(1.0f), blendFactor);
    }

    std::string AnimatorComponent::getCurrentStateName() {
        if (currentState == -1) {
            return "";
        }
        return states.at(currentState).Name;
    }
}