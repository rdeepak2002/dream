//
// Created by Deepak Ramalingam on 11/12/22.
//

#include "dream/scene/Scene.h"
#include "dream/scene/Entity.h"

Dream::Scene::Scene() {

}

Dream::Scene::~Scene() {

}

Dream::Entity Dream::Scene::createEntity(const std::string &name) {
    Entity entity = { entityRegistry.create(), this };
    // TODO: automatically add id component
    // TODO: automatically add transform component
    // TODO: automatically add tag component
    return entity;
}

void Dream::Scene::update() {
    // TODO: populate this
}

void Dream::Scene::fixedUpdate() {
    // TODO: populate this
}

template<typename... Components>
auto Dream::Scene::getAllEntitiesWithComponents() {
    return entityRegistry.view<Components ...>();
}
