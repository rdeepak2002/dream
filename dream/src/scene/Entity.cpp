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

    void Entity::serialize(YAML::Emitter& out) {
        // serialize this entity
        out << YAML::BeginMap;
        out << YAML::Key << "Entity" << YAML::Value << getComponent<Dream::Component::IDComponent>().getID();
        if (hasComponent<Component::RootComponent>()) {
            getComponent<Component::RootComponent>().serialize(out);
        }
        if (hasComponent<Component::IDComponent>()) {
            getComponent<Component::IDComponent>().serialize(out);
        }
        if (hasComponent<Component::TagComponent>()) {
            getComponent<Component::TagComponent>().serialize(out);
        }
        if (hasComponent<Component::HierarchyComponent>()) {
            getComponent<Component::HierarchyComponent>().serialize(out);
        }
        out << YAML::EndMap;
        // serialize children
        Entity child = getComponent<Component::HierarchyComponent>().first;
        while (child) {
            child.serialize(out);
            child = child.getComponent<Component::HierarchyComponent>().next;
        }
    }
}
