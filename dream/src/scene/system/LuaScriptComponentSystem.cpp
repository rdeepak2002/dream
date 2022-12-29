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

#include "dream/scene/system/LuaScriptComponentSystem.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"
#include "dream/util/Logger.h"
#include "dream/window/Input.h"
#include "dream/window/KeyCodes.h"

namespace Dream {
    Entity getEntityByTag(const std::string &tag) {
        return Project::getScene()->getEntityByTag(tag);
    }

    LuaScriptComponentSystem::LuaScriptComponentSystem() {
        // open libraries with lua
        lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::io);

        // define lua bindings
        lua.new_enum("Key",
                     "A", Key::A,
                     "B", Key::B,
                     "C", Key::C,
                     "D", Key::D,
                     "E", Key::E,
                     "F", Key::F,
                     "G", Key::G,
                     "H", Key::H,
                     "I", Key::I,
                     "J", Key::J,
                     "K", Key::K,
                     "L", Key::L,
                     "M", Key::M,
                     "N", Key::N,
                     "O", Key::O,
                     "P", Key::P,
                     "Q", Key::Q,
                     "R", Key::R,
                     "S", Key::S,
                     "T", Key::T,
                     "U", Key::U,
                     "V", Key::V,
                     "W", Key::W,
                     "X", Key::X,
                     "Y", Key::Y,
                     "Z", Key::Z,
                     "a", Key::a,
                     "b", Key::b,
                     "c", Key::c,
                     "d", Key::d,
                     "e", Key::e,
                     "f", Key::f,
                     "g", Key::g,
                     "h", Key::h,
                     "i", Key::i,
                     "j", Key::j,
                     "k", Key::k,
                     "l", Key::l,
                     "m", Key::m,
                     "n", Key::n,
                     "o", Key::o,
                     "p", Key::p,
                     "q", Key::q,
                     "r", Key::r,
                     "s", Key::s,
                     "t", Key::t,
                     "u", Key::u,
                     "v", Key::v,
                     "w", Key::w,
                     "x", Key::x,
                     "y", Key::y,
                     "z", Key::z,
                     "Space", Key::Space,
                     "Escape", Key::Escape,
                     "Right", Key::Right,
                     "Left", Key::Left,
                     "Down", Key::Down,
                     "Up", Key::Up,
                     "LeftShift", Key::LeftShift,
                     "LeftMouse", Key::LeftMouse,
                     "RightMouse", Key::RightMouse
        );

        lua.new_usertype<glm::vec2>("vec2", sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
                                    "x", &glm::vec2::x,
                                    "y", &glm::vec2::y,
                                    sol::meta_function::multiplication, sol::overload(
                        [](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 * v2; },
                        [](const glm::vec2 &v1, float f) -> glm::vec2 { return v1 * f; },
                        [](float f, const glm::vec2 &v1) -> glm::vec2 { return f * v1; }
                ),
                                    sol::meta_function::addition, sol::overload(
                        [](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 + v2; }
                ),
                                    sol::meta_function::subtraction, sol::overload(
                        [](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 - v2; }
                ),
                                    sol::meta_function::equal_to, sol::overload(
                        [](const glm::vec2 &v1, const glm::vec2 &v2) -> bool { return v1 == v2; }
                ),
                                    sol::meta_function::to_string, sol::overload(
                        [](const glm::vec2 &v1) -> std::string {
                            return "(" + std::to_string(v1.x) + ", " + std::to_string(v1.y) + ")";
                        }
                )
        );

        lua.new_usertype<glm::vec3>("vec3",
                                    sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
                                    "x", &glm::vec3::x,
                                    "y", &glm::vec3::y,
                                    "z", &glm::vec3::z,
                                    sol::meta_function::multiplication, sol::overload(
                        [](const glm::vec3 &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 * v2; },
                        [](const glm::vec3 &v1, float f) -> glm::vec3 { return v1 * f; },
                        [](float f, const glm::vec3 &v1) -> glm::vec3 { return f * v1; }
                ),
                                    sol::meta_function::addition, sol::overload(
                        [](const glm::vec3 &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 + v2; }
                ),
                                    sol::meta_function::subtraction, sol::overload(
                        [](const glm::vec3 &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 - v2; }
                ),
                                    sol::meta_function::equal_to, sol::overload(
                        [](const glm::vec3 &v1, const glm::vec3 &v2) -> bool { return v1 == v2; }
                ),
                                    sol::meta_function::to_string, sol::overload(
                        [](const glm::vec3 &v1) -> std::string {
                            return "(" + std::to_string(v1.x) + ", " + std::to_string(v1.y) + ", " +
                                   std::to_string(v1.z) + ")";
                        }
                )
        );

        lua.new_usertype<glm::quat>("quat", sol::constructors<glm::quat(), glm::quat(float, float, float, float)>(),
                                    "w", &glm::quat::w,
                                    "x", &glm::quat::x,
                                    "y", &glm::quat::y,
                                    "z", &glm::quat::z,
                                    sol::meta_function::multiplication, sol::overload(
                        [](const glm::quat &v1, const glm::quat &v2) -> glm::quat { return v1 * v2; },
                        [](const glm::quat &v1, float f) -> glm::quat { return v1 * f; },
                        [](float f, const glm::quat &v1) -> glm::quat { return f * v1; }
                ),
                                    sol::meta_function::addition, sol::overload(
                        [](const glm::quat &v1, const glm::quat &v2) -> glm::quat { return v1 + v2; }
                ),
                                    sol::meta_function::subtraction, sol::overload(
                        [](const glm::quat &v1, const glm::quat &v2) -> glm::quat { return v1 - v2; }
                ),
                                    sol::meta_function::equal_to, sol::overload(
                        [](const glm::quat &v1, const glm::quat &v2) -> bool { return v1 == v2; }
                ),
                                    sol::meta_function::to_string, sol::overload(
                        [](const glm::quat &v1) -> std::string {
                            return "(" + std::to_string(v1.w) + ", " + std::to_string(v1.x) + ", " +
                                   std::to_string(v1.y) + ", " + std::to_string(v1.z) + ")";
                        }
                )
        );

        lua.new_usertype<Logger>("Logger",
                                 "debug", sol::as_function(&Logger::debug),
                                 "warn", sol::as_function(&Logger::warn),
                                 "error", sol::as_function(&Logger::error),
                                 "info", sol::as_function(&Logger::info),
                                 "fatal", sol::as_function(&Logger::fatal)
        );

        lua.new_usertype<Input>("Input",
                                "getButtonDown", &Input::getButtonDown,
                                "getMousePosition", &Input::getMousePosition,
                                "getMouseMovement", &Input::getMouseMovement,
                                "getMouseScroll", &Input::getMouseScroll,
                                "activatePointerLock", sol::as_function(&Input::activatePointerLock),
                                "pointerLockActivated", &Input::pointerLockActivated
        );

        lua.new_usertype<Entity>("Entity",
                                 "new", sol::no_constructor,
                                 "getID", &Dream::Entity::getID,
                                 "isValid", &Dream::Entity::isValid,
                                 "getTransform", &Dream::Entity::getComponent<Dream::Component::TransformComponent>,
                                 "getCamera", &Dream::Entity::getComponent<Dream::Component::CameraComponent>,
                                 "getAnimator", &Dream::Entity::getComponent<Dream::Component::AnimatorComponent>,
                                 "getRigidBody", &Dream::Entity::getComponent<Dream::Component::RigidBodyComponent>
        );

        lua.new_usertype<Component::TransformComponent>("TransformComponent",
                                                        "translation", &Component::TransformComponent::translation,
                                                        "rotation", &Component::TransformComponent::rotation,
                                                        "scale", &Component::TransformComponent::scale
        );

        lua.new_usertype<Component::CameraComponent>("CameraComponent",
                                                     "lookAt", &Component::CameraComponent::lookAt
        );

        lua.new_usertype<Component::AnimatorComponent>("AnimatorComponent",
                                                       "setVariable",
                                                       sol::as_function(&Component::AnimatorComponent::setVariable)
        );

        lua.new_usertype<Component::RigidBodyComponent>("RigidBodyComponent",
                                                       "setLinearVelocity",
                                                       sol::as_function(&Component::RigidBodyComponent::setLinearVelocity)
        );

        lua.new_usertype<Scene>("Scene",
                                "getEntityByTag", sol::as_function(&getEntityByTag)
        );

        lua.end();
    }

    LuaScriptComponentSystem::~LuaScriptComponentSystem() {
        auto luaScriptEntities = Project::getScene()->getEntitiesWithComponents<Component::LuaScriptComponent>();
        for (auto entityHandle: luaScriptEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            auto &luaScriptComponent = entity.getComponent<Component::LuaScriptComponent>();
            if (luaScriptComponent.table.valid()) {
                luaScriptComponent.table.abandon();
            }
        }
    }

    void LuaScriptComponentSystem::update(float dt) {
        // update all entities with lua script
        auto luaScriptEntities = Project::getScene()->getEntitiesWithComponents<Component::LuaScriptComponent>();
        for (auto entityHandle: luaScriptEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            auto &component = entity.getComponent<Component::LuaScriptComponent>();
            std::string scriptGuid = component.guid;
            if (component.scriptPath.empty()) {
                component.loadScriptPath();
            }
            if (!component.scriptPath.empty()) {
                sol::protected_function_result scriptCompileResult = lua.safe_script_file(component.scriptPath,
                                                                                          &sol::script_pass_on_error);
                if (scriptCompileResult.valid()) {
                    if (component.needToInitTable) {
                        component.table = lua.create_table_with("value", "key");
                        component.needToInitTable = false;
                    }
                    lua["self"] = component.table;
                    sol::protected_function scriptUpdateFunction = lua["update"];
                    sol::protected_function_result functionResult = scriptUpdateFunction(entity, dt);
                    if (!functionResult.valid()) {
                        if (!LuaScriptComponentSystem::errorPrintedForScript.count(scriptGuid)) {
                            sol::error err = functionResult;
                            std::string what = err.what();
                            Logger::error("Lua function call error " + what);
                            LuaScriptComponentSystem::errorPrintedForScript.insert(scriptGuid);
                            return;
                        }
                    } else {
                        if (LuaScriptComponentSystem::errorPrintedForScript.count(scriptGuid) ||
                            LuaScriptComponentSystem::modifiedScripts.count(scriptGuid)) {
                            Logger::debug("Script has no errors " + component.scriptPath);
                            LuaScriptComponentSystem::errorPrintedForScript.erase(scriptGuid);
                            LuaScriptComponentSystem::modifiedScripts.erase(scriptGuid);
                        }
                    }
                } else {
                    if (!LuaScriptComponentSystem::errorPrintedForScript.count(scriptGuid)) {
                        sol::error err = scriptCompileResult;
                        std::string what = err.what();
                        Logger::error("Lua script parsing error " + what);
                        LuaScriptComponentSystem::errorPrintedForScript.insert(scriptGuid);
                        return;
                    }
                }
            }
        }
    }

    void LuaScriptComponentSystem::init() {

    }
}
