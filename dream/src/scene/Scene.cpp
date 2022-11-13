//
// Created by Deepak Ramalingam on 11/12/22.
//

#include "dream/scene/Scene.h"
#include "dream/scene/component/Component.h"
#include "dream/scene/Entity.h"

namespace Dream {
    Scene::Scene() {

    }

    Scene::~Scene() {

    }

    Entity Dream::Scene::createEntity(const std::string &name, bool rootEntity) {
        Entity entity = { entityRegistry.create(), this };
        // TODO: automatically add id component
        // add transform component to entity
        entity.addComponent<Component::TransformComponent>();
        // add tag component to entity
        auto &tag = entity.addComponent<Component::TagComponent>("Entity");
        if (!name.empty()) {
            tag.tag = name;
        }
        // add hierarchy component to entity
        entity.addComponent<Component::HierarchyComponent>();
        // make new entity child of root
        if (!rootEntity) {
            getRootEntity().addChild(entity);
        }
        return entity;
    }

    void Scene::update() {
        // TODO: populate this
    }

    void Scene::fixedUpdate() {
        // TODO: populate this
    }

    Entity Scene::getRootEntity() {
        auto rootEntities = getEntitiesWithComponents<Component::RootComponent>();
        if (rootEntities.empty()) {
            auto newRootEntity = createEntity("root", true);
            newRootEntity.addComponent<Component::RootComponent>();
            return newRootEntity;
        } else if (rootEntities.size() == 1) {
            return { rootEntities.front(), this };
        } else {
            std::cout << "More than one root entity" << std::endl;
            exit(1);
        }
    }
}
