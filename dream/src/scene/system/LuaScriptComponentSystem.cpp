//
// Created by Deepak Ramalingam on 11/26/22.
//

#include "dream/scene/system/LuaScriptComponentSystem.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    LuaScriptComponentSystem::LuaScriptComponentSystem() {

    }

    LuaScriptComponentSystem::~LuaScriptComponentSystem() {

    }

    void LuaScriptComponentSystem::update(float dt) {
        // update all entities with lua script
        auto luaScriptEntities = Project::getScene()->getEntitiesWithComponents<Component::LuaScriptComponent>();
        for(auto entityHandle : luaScriptEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            auto& component = entity.getComponent<Component::LuaScriptComponent>();
            if (component.scriptPath.empty()) {
                component.loadScriptPath();
                assert(!component.scriptPath.empty());
            }
            lua.script_file(component.scriptPath);
            lua["self"] = component.table;
            auto scriptUpdateFunction = lua["update"];
            if (scriptUpdateFunction.valid()) {
                scriptUpdateFunction(entity, dt);
            } else {
                std::cout << "Error: lua function is not valid" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
}
