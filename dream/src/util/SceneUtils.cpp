//
// Created by Deepak Ramalingam on 11/26/22.
//

#include "dream/util/SceneUtils.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    void SceneUtils::removeMeshReference(Dream::Entity entity, std::string meshGUID, bool recursively) {
        if (entity.hasComponent<Component::MeshComponent>() && entity.getComponent<Component::MeshComponent>().guid == meshGUID) {
            entity.removeComponent<Dream::Component::MeshComponent>();
        }
        if (recursively) {
            Entity child = entity.getComponent<Component::HierarchyComponent>().first;
            while (child) {
                removeMeshReference(child, meshGUID, recursively);
                child = child.getComponent<Component::HierarchyComponent>().next;
            }
        }
    }
}
