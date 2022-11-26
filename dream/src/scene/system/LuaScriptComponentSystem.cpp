//
// Created by Deepak Ramalingam on 11/26/22.
//

#include "dream/scene/system/LuaScriptComponentSystem.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"
#include "dream/util/Logger.h"
#include "dream/window/Input.h"
#include "dream/window/KeyCodes.h"

namespace Dream {
    LuaScriptComponentSystem::LuaScriptComponentSystem() {
        // open libraries with lua
        lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::io);

        // define lua bindings
        lua.new_enum("Key",
                     "Space", Key::Space,
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
                     "Right", Key::Right,
                     "Left", Key::Left,
                     "Down", Key::Down,
                     "Up", Key::Up,
                     "LeftShift", Key::LeftShift
        );

        lua.new_usertype<glm::vec3>("vec3",
                                    sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
                                    "x", &glm::vec3::x,
                                    "y", &glm::vec3::y,
                                    "z", &glm::vec3::z,
                                    sol::meta_function::multiplication, sol::overload(
                        [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1*v2; },
                        [](const glm::vec3& v1, float f) -> glm::vec3 { return v1*f; },
                        [](float f, const glm::vec3& v1) -> glm::vec3 { return f*v1; }
                ),
                                    sol::meta_function::addition, sol::overload(
                        [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1+v2; }
                ),
                                    sol::meta_function::subtraction, sol::overload(
                        [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1-v2; }
                ),
                                    sol::meta_function::equal_to, sol::overload(
                        [](const glm::vec3& v1, const glm::vec3& v2) -> bool { return v1 == v2; }
                ),
                                    sol::meta_function::to_string, sol::overload(
                        [](const glm::vec3& v1) -> std::string { return "(" + std::to_string(v1.x) + ", "+ std::to_string(v1.y) + ", " + std::to_string(v1.z) + ")"; }
                )
        );

        lua.new_usertype<glm::quat>("quat",
                                    sol::constructors<glm::quat(), glm::quat(float, float, float, float)>(),
                                    "w", &glm::quat::w,
                                    "x", &glm::quat::x,
                                    "y", &glm::quat::y,
                                    "z", &glm::quat::z,
                                    sol::meta_function::multiplication, sol::overload(
                        [](const glm::quat& v1, const glm::quat& v2) -> glm::quat { return v1*v2; },
                        [](const glm::quat& v1, float f) -> glm::quat { return v1*f; },
                        [](float f, const glm::quat& v1) -> glm::quat { return f*v1; }
                ),
                                    sol::meta_function::addition, sol::overload(
                        [](const glm::quat& v1, const glm::quat& v2) -> glm::quat { return v1+v2; }
                ),
                                    sol::meta_function::subtraction, sol::overload(
                        [](const glm::quat& v1, const glm::quat& v2) -> glm::quat { return v1-v2; }
                ),
                                    sol::meta_function::equal_to, sol::overload(
                        [](const glm::quat& v1, const glm::quat& v2) -> bool { return v1 == v2; }
                ),
                                    sol::meta_function::to_string, sol::overload(
                        [](const glm::quat& v1) -> std::string { return "(" + std::to_string(v1.w) + ", " + std::to_string(v1.x) + ", "+ std::to_string(v1.y) + ", " + std::to_string(v1.z) + ")"; }
                )
        );

        lua.new_usertype<Logger>("Logger",
                                 "Debug", sol::as_function(&Logger::Debug),
                                 "Warn", sol::as_function(&Logger::Warn),
                                 "Error", sol::as_function(&Logger::Error)
        );

        lua.new_usertype<Input>("Input",
                                "GetButtonDown", &Input::GetButtonDown,
                                "SetButtonDown", &Input::SetButtonDown
        );

        lua.new_usertype<Entity>("Entity",
                                  "new", sol::no_constructor,
                                  "getID", &Dream::Entity::getID,
                                  "getTransform", &Dream::Entity::getComponent<Dream::Component::TransformComponent>
        );

        lua.new_usertype<Component::TransformComponent>("TransformComponent",
                                                        "translation", &Component::TransformComponent::translation,
                                                        "rotation", &Component::TransformComponent::rotation,
                                                        "scale", &Component::TransformComponent::scale);

        // HOW TO ADD FUNCTION: "right", sol::as_function(&DeepsEngine::Component::Transform::right)

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