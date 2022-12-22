//
// Created by Deepak Ramalingam on 12/9/22.
//

#include "dream/scene/system/AnimatorComponentSystem.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    void AnimatorComponentSystem::init() {

    }

    void AnimatorComponentSystem::update(float dt) {
        auto animatorEntities = Project::getScene()->getEntitiesWithComponents<Component::AnimatorComponent>();
        for(auto entityHandle : animatorEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            entity.getComponent<Component::AnimatorComponent>().updateAnimation(dt);
            // TODO: this should be in fixed update?
            entity.getComponent<Component::AnimatorComponent>().updateStateMachine(dt);
        }
    }
}
