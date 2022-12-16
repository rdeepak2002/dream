//
// Created by Deepak Ramalingam on 11/12/22.
//

#ifndef DREAM_SCENE_H
#define DREAM_SCENE_H

#include <iostream>
#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>
#include <btBulletDynamicsCommon.h>
#include "dream/scene/system/LuaScriptComponentSystem.h"
#include "dream/scene/system/AnimatorComponentSystem.h"
#include "dream/scene/system/AudioComponentSystem.h"
#include "dream/scene/system/PhysicsComponentSystem.h"

namespace Dream {
    class Entity;

    class Scene {
    public:
        Scene();
        ~Scene();
        Entity createEntity(const std::string& name = "Entity", bool rootEntity = false, bool addChildStart = true);
        void update(float dt);
        void fixedUpdate(float dt);
        template<typename... Components>
        auto getEntitiesWithComponents() {
            return entityRegistry.view<Components ...>();
        }
        Entity getRootEntity();
        Entity getSceneCamera();
        Entity getMainCamera();
        Entity getEntityByID(const std::string& id);
        Entity getEntityByTag(const std::string& tag);
        Entity getEntityByInternalID(int internalID);
        void removeEntity(Entity &entity);
        void serialize(YAML::Emitter &out);
    private:
        entt::registry entityRegistry;
        friend class Entity;
        PhysicsComponentSystem* physicsComponentSystem;
        AudioComponentSystem* audioComponentSystem;
        AnimatorComponentSystem* animatorComponentSystem;
        LuaScriptComponentSystem* luaScriptComponentSystem;
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* overlappingPairCache;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;
        bool shouldInitComponentSystems;
    };
}

#endif //DREAM_SCENE_H
