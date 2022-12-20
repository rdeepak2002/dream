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
        m_CurrentTime = 0.0;
        m_CurrentAnimation = nullptr;
        m_FinalBoneMatrices.reserve(MAX_BONES);
        for (int i = 0; i < MAX_BONES; i++) {
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        }
    }

    AnimatorComponent::AnimatorComponent(std::string animatorGUID) {
        this->guid = std::move(animatorGUID);
        m_CurrentTime = 0.0;
        m_CurrentAnimation = nullptr;
        m_FinalBoneMatrices.reserve(MAX_BONES);
        for (int i = 0; i < MAX_BONES; i++) {
            m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        }
    }

    AnimatorComponent::~AnimatorComponent() {
        // TODO: delete each animation in map
    }

    void AnimatorComponent::updateAnimation(float dt)
    {
        if (m_CurrentAnimation)
        {
            m_CurrentTime += ((Animation *) m_CurrentAnimation)->getTicksPerSecond() * dt;
            m_CurrentTime = fmod(m_CurrentTime, ((Animation *) m_CurrentAnimation)->getDuration());
            calculateBoneTransform(&((Animation *) m_CurrentAnimation)->getRootNode(), glm::mat4(1.0f));
        }
    }

    void AnimatorComponent::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform, int depth)
    {
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
        states.clear();
        std::string animatorFilePath = Project::getResourceManager()->getFilePathFromGUID(guid);
        // get guids for animation files from animator file
        YAML::Node doc = YAML::LoadFile(animatorFilePath);
        auto animationsNode = doc[k_states].as<std::vector<YAML::Node>>();
        for (const YAML::Node& animationGUIDNode : animationsNode) {
            states.push_back(animationGUIDNode.as<std::string>());
        }
        auto transitionsNode = doc[k_transitions].as<std::vector<YAML::Node>>();
        // load animation data from animation files
        if (!states.empty()) {
            for (const auto& animationGUID : states) {
                auto animationFilePath = Project::getResourceManager()->getFilePathFromGUID(animationGUID);
                auto *anim = new Animation(animationFilePath, modelEntity, 0);
                animationObjects[animationGUID] = anim;
                m_CurrentAnimation = anim;
                currentState = animationGUID;
            }
        } else {
            m_CurrentAnimation = nullptr;
            currentState = "";
            Logger::fatal("No animation found");
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

    void AnimatorComponent::playAnimation(std::string animationGUID) {
        if (animationObjects.count(animationGUID) > 0) {
            m_CurrentAnimation = animationObjects[animationGUID];
            currentState = animationGUID;
        } else {
            Logger::error("Unable to find animation with GUID " + animationGUID);
        }
    }
}