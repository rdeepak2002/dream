//
// Created by Deepak Ramalingam on 11/12/22.
//

#ifndef DREAM_ENTITY_H
#define DREAM_ENTITY_H

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>
#include "dream/scene/Scene.h"
#include "dream/util/Logger.h"

namespace Dream {
    class Entity {
    public:
        entt::entity entityHandle { entt::null };
        Scene* scene = nullptr;
        Entity();
        Entity(entt::entity handle, Scene* scene);
        template<typename T, typename ... Args>
        T& addComponent(Args&&... args) {
            T& component = scene->entityRegistry.emplace_or_replace<T>(entityHandle, std::forward<Args>(args)...);
            return component;
        }
        template<typename T>
        T& getComponent() {
            if (!hasComponent<T>()) {
                Logger::fatal("Entity does not have component");
            }
            return scene->entityRegistry.get<T>(entityHandle);
        }
        template<typename T>
        bool hasComponent() {
            if (auto *comp = scene->entityRegistry.try_get<T>(entityHandle)) {
                return true;
            } else {
                return false;
            }
        }
        template<typename T>
        void removeComponent() {
            if (!hasComponent<T>()) {
                Logger::fatal("Entity does not have component (for removal)");
            }
            scene->entityRegistry.remove<T>(entityHandle);
        }
        void addChild(Entity entity, bool atStart = true);
        int numChildren();
        void serialize(YAML::Emitter &out);
        void deserialize(YAML::Node node);
        std::string getID();
        bool isValid();
        explicit operator bool() const;
        explicit operator entt::entity() const;
        bool operator==(const Entity& other) const;
        bool operator!=(const Entity& other) const;
    };
}

#endif //DREAM_ENTITY_H
