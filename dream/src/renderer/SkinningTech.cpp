//
// Created by Deepak Ramalingam on 2/10/23.
//

#include "dream/renderer/SkinningTech.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    void SkinningTech::setJointUniforms(Entity entity, OpenGLShader *shader) {
        std::vector<glm::mat4> finalBoneMatrices;
        if (entity.hasComponent<Component::AnimatorComponent>()) {
            if (entity.hasComponent<Component::MeshComponent>()) {
                entity.getComponent<Component::MeshComponent>().loadMesh();
            } else {
                Logger::fatal("No mesh component for entity with animator so bones cannot be loaded");
            }
            if (entity.getComponent<Component::AnimatorComponent>().needsToLoadAnimations) {
                entity.getComponent<Component::AnimatorComponent>().loadStateMachine(entity);
            }
            finalBoneMatrices = entity.getComponent<Component::AnimatorComponent>().m_FinalBoneMatrices;
        }

        for (int i = 0; i < finalBoneMatrices.size(); i++) {
            shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", finalBoneMatrices[i]);
        }
    }
}