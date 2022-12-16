//
// Created by Deepak Ramalingam on 11/26/22.
//

#include "dream/util/SceneUtils.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    void SceneUtils::removeMeshReference(Dream::Entity entity, std::string meshGUID, bool recursively) {
        if (entity.hasComponent<Component::MeshComponent>() && entity.getComponent<Component::MeshComponent>().meshType == Component::MeshComponent::MeshType::FROM_FILE && entity.getComponent<Component::MeshComponent>().guid == meshGUID) {
            entity.removeComponent<Dream::Component::MeshComponent>();
            if (entity.hasComponent<Component::MaterialComponent>()) {
                entity.removeComponent<Component::MaterialComponent>();
            }
        }
        if (recursively) {
            Entity child = entity.getComponent<Component::HierarchyComponent>().first;
            while (child) {
                Entity nextChild = child.getComponent<Component::HierarchyComponent>().next;
                removeMeshReference(child, meshGUID, recursively);
                child = nextChild;
            }
        }
    }
}
