//
// Created by Deepak Ramalingam on 11/12/22.
//

#include "dream/scene/Entity.h"

Dream::Entity::Entity(entt::entity handle, Dream::Scene *scene) : entityHandle(handle), scene(scene) {

}

template<typename T, typename... Args>
T &Dream::Entity::addComponent(Args &&... args) {
    T& component = scene->entityRegistry.template emplace_or_replace<T>(entityHandle, std::forward<Args>(args)...);
    return component;
}

template<typename T>
T &Dream::Entity::getComponent() {
    if (!hasComponent<T>()) {
        std::cout << "Entity does not have component" << std::endl;
        exit(1);
    }
    return scene->entityRegistry.get<T>(entityHandle);
}

template<typename T>
bool Dream::Entity::hasComponent() {
    if (auto *comp = scene->entityRegistry.try_get<T>(entityHandle)) {
        return true;
    } else {
        return false;
    }
}

template<typename T>
void Dream::Entity::removeComponent() {
    if (!hasComponent<T>()) {
        std::cout << "Entity does not have component" << std::endl;
        exit(1);
    }
    scene->entityRegistry.remove<T>(entityHandle);
}

Dream::Entity::operator bool() const {
    return this->entityHandle != entt::null;
}

bool Dream::Entity::operator==(const Dream::Entity &other) const {
    return entityHandle == other.entityHandle && scene == other.scene;
}

bool Dream::Entity::operator!=(const Dream::Entity &other) const {
    return !(*this == other);
}

Dream::Entity::operator entt::entity() const {
    return entityHandle;
}

