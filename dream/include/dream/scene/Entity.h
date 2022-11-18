//
// Created by Deepak Ramalingam on 11/12/22.
//

#ifndef DREAM_ENTITY_H
#define DREAM_ENTITY_H

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>
#include "dream/scene/Scene.h"

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
                std::cout << "Entity does not have component" << std::endl;
                exit(1);
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
                std::cout << "Entity does not have component" << std::endl;
                exit(1);
            }
            scene->entityRegistry.remove<T>(entityHandle);
        }
        void addChild(Entity entity);
        int numChildren();
        void serialize(YAML::Emitter &out);
        explicit operator bool() const;
        explicit operator entt::entity() const;
        bool operator==(const Entity& other) const;
        bool operator!=(const Entity& other) const;
    };
}

#endif //DREAM_ENTITY_H
