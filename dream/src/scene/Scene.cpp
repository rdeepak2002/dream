/**********************************************************************************
 *  Dream is a software for developing real-time 3D experiences.
 *  Copyright (C) 2023 Deepak Ramalignam
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **********************************************************************************/

#include "dream/scene/Scene.h"
#include "dream/scene/component/Component.h"
#include "dream/scene/Entity.h"
#include "dream/window/Input.h"
#include "dream/util/Logger.h"
#include "dream/project/Project.h"

namespace Dream {
    Scene::Scene() {
        physicsComponentSystem = new PhysicsComponentSystem();
        audioComponentSystem = new AudioComponentSystem();
        animatorComponentSystem = new AnimatorComponentSystem();
        luaScriptComponentSystem = new LuaScriptComponentSystem();
        collisionConfiguration = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        overlappingPairCache = new btDbvtBroadphase();
        solver = new btSequentialImpulseConstraintSolver;
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
        dynamicsWorld->setGravity(btVector3(0, -10, 0));
    }

    Scene::~Scene() {
        delete physicsComponentSystem;
        delete audioComponentSystem;
        delete animatorComponentSystem;
        delete luaScriptComponentSystem;
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

    void Scene::update(float dt) {
        if (shouldInitComponentSystems) {
            physicsComponentSystem->init();
            animatorComponentSystem->init();
            audioComponentSystem->init();
            luaScriptComponentSystem->init();
            shouldInitComponentSystems = false;
        }
        animatorComponentSystem->update(dt);
    }

    void Scene::fixedUpdate(float dt) {
        if (Project::isPlaying() && !shouldInitComponentSystems) {
            physicsComponentSystem->update(dt);
            audioComponentSystem->update(dt);
            luaScriptComponentSystem->update(dt);
        }
        if (Project::isPlaying()) {
            // TODO: move to component system
            Entity mainCamera = getMainCamera();
            if (mainCamera) {
                mainCamera.getComponent<Component::CameraComponent>().updateCameraVectors(mainCamera);
            }
        } else {
            // TODO: move to component system
            Entity sceneCamera = getSceneCamera();
            if (sceneCamera && Input::isEditorRendererActive()) {
                sceneCamera.getComponent<Component::SceneCameraComponent>().processInput(sceneCamera, dt);
            } else if (sceneCamera) {
                sceneCamera.getComponent<Component::SceneCameraComponent>().updateCameraVectors();
            }
        }
        Input::resetMouseDynamicState();
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
            Logger::fatal("More than one root entity");
            return {};
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

    Entity Scene::getEntityByTag(const std::string& tag) {
        auto entities = getEntitiesWithComponents<Component::TagComponent>();
        for(auto entityHandle : entities) {
            Entity entity = {entityHandle, this};
            if (entity.getComponent<Component::TagComponent>().tag == tag) {
                return entity;
            }
        }
        return {};
    }

    Entity Scene::getEntityByInternalID(int internalID) {
        auto entities = getEntitiesWithComponents<Component::IDComponent>();
        for(auto entityHandle : entities) {
            Entity entity = {entityHandle, this};
            if ((int) entityHandle == internalID) {
                return entity;
            }
        }
        return {};
    }

    Entity Scene::getSceneCamera() {
        auto entities = getEntitiesWithComponents<Component::SceneCameraComponent>();
        for(auto entityHandle : entities) {
            Entity entity = {entityHandle, this};
            return entity;
        }
        return {};
    }

    Entity Scene::getMainCamera() {
        auto entities = getEntitiesWithComponents<Component::CameraComponent>();
        for(auto entityHandle : entities) {
            Entity entity = {entityHandle, this};
            return entity;
        }
        return {};
    }
}
