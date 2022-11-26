//
// Created by Deepak Ramalingam on 11/26/22.
//

#include "dream/scene/system/LuaScriptComponentSystem.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"
#include "dream/util/Logger.h"

namespace Dream {
    LuaScriptComponentSystem::LuaScriptComponentSystem() {
        // open libraries with lua
        lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::io);
        lua.new_usertype<Logger>("Logger",
                                 "Debug", sol::as_function(&Logger::Debug),
                                 "Warn", sol::as_function(&Logger::Warn),
                                 "Error", sol::as_function(&Logger::Error)
        );
        lua.end();
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
