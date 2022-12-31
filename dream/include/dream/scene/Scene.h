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

#ifndef DREAM_SCENE_H
#define DREAM_SCENE_H

#include <iostream>
#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>
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

        Entity createEntity(const std::string &name = "Entity", bool rootEntity = false, bool addChildStart = true);

        void update(float dt);

        void fixedUpdate(float dt);

        void destroyComponentSystems();

        void resetComponentSystems();

        template<typename... Components>
        auto getEntitiesWithComponents() {
            return entityRegistry.view<Components ...>();
        }

        Entity getRootEntity();

        Entity getSceneCamera();

        Entity getMainCamera();

        Entity getEntityByID(const std::string &id);

        Entity getEntityByTag(const std::string &tag);

        Entity getEntityByInternalID(int internalID);

        void removeEntity(Entity &entity);

        void clear();

        void serialize(YAML::Emitter &out);

        PhysicsComponentSystem* getPhysicsComponentSystem();

    private:
        entt::registry entityRegistry;

        friend class Entity;

        PhysicsComponentSystem *physicsComponentSystem;
        AudioComponentSystem *audioComponentSystem;
        AnimatorComponentSystem *animatorComponentSystem;
        LuaScriptComponentSystem *luaScriptComponentSystem;
        bool shouldInitComponentSystems;
    };
}

#endif //DREAM_SCENE_H
