//
// Created by Deepak Ramalingam on 11/12/22.
//

#include "dream/scene/Entity.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    Entity::Entity() {
        entityHandle = entt::null;
        scene = nullptr;
    }

    Entity::Entity(entt::entity handle, Dream::Scene *scene) : entityHandle(handle), scene(scene) {

    }

    Entity::operator bool() const {
        return this->entityHandle != entt::null;
    }

    bool Entity::operator==(const Dream::Entity &other) const {
        return entityHandle == other.entityHandle && scene == other.scene;
    }

    bool Entity::operator!=(const Dream::Entity &other) const {
        return !(*this == other);
    }

    Entity::operator entt::entity() const {
        return entityHandle;
    }

    void Entity::addChild(Entity entity) {
        getComponent<Component::HierarchyComponent>().addChild(entity, *this);
    }

    int Entity::numChildren() {
        return getComponent<Component::HierarchyComponent>().numChildren();
    }
}
