//
// Created by Deepak Ramalingam on 11/12/22.
//

#include "dream/scene/Scene.h"
#include "dream/scene/component/Component.h"
#include "dream/scene/Entity.h"

namespace Dream {
    Scene::Scene() {
        collisionConfiguration = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        overlappingPairCache = new btDbvtBroadphase();
        solver = new btSequentialImpulseConstraintSolver;
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
        dynamicsWorld->setGravity(btVector3(0, -10, 0));
    }

    Scene::~Scene() {
        delete dynamicsWorld;
        delete solver;
        delete overlappingPairCache;
        delete dispatcher;
        delete collisionConfiguration;
    }

    Entity Dream::Scene::createEntity(const std::string &name, bool rootEntity, bool addChildStart) {
        Entity entity = { entityRegistry.create(), this };
        // add id component to entity
        entity.addComponent<Component::IDComponent>();
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
            getRootEntity().addChild(entity, addChildStart);
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

    void Scene::removeEntity(Entity& entity) {
        Entity child = entity.getComponent<Component::HierarchyComponent>().first;
        while (child) {
            Entity nextChild = child.getComponent<Component::HierarchyComponent>().next;
            removeEntity(child);
            child = nextChild;
        }
        if (!entity.hasComponent<Component::RootComponent>()) {
            entity.getComponent<Component::HierarchyComponent>().parent.getComponent<Component::HierarchyComponent>().removeChild(entity);
        }
        entityRegistry.destroy(entity.entityHandle);
    }

    void Scene::serialize(YAML::Emitter &out) {
        out << YAML::Key << "Scene" << YAML::Value << "main-scene";
        out << YAML::Key << "Entities" << YAML::Value;
        out << YAML::BeginSeq;
        Entity entity = getRootEntity();
        entity.serialize(out);
        out << YAML::EndSeq;
    }

    Entity Scene::getEntityByID(const std::string& id) {
        auto entities = getEntitiesWithComponents<Component::IDComponent>();
        for(auto entityHandle : entities) {
            Entity entity = {entityHandle, this};
            if (entity.getID() == id) {
                return entity;
            }
        }
        return {};
    }
}
