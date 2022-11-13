//
// Created by Deepak Ramalingam on 11/12/22.
//

#ifndef DREAM_ENTITY_H
#define DREAM_ENTITY_H

#include <entt/entt.hpp>
#include "dream/scene/Scene.h"

namespace Dream {
    class Entity {
    public:
        Entity(entt::entity handle, Scene* scene);
        template<typename T, typename ... Args>
        T& addComponent(Args&& ... args);
        template<typename T>
        T& getComponent();
        template<typename T>
        bool hasComponent();
        template<typename T>
        void removeComponent();
        explicit operator bool() const;
        explicit operator entt::entity() const;
        bool operator==(const Entity& other) const;
        bool operator!=(const Entity& other) const;
    private:
        entt::entity entityHandle { entt::null };
        Scene* scene = nullptr;
    };
}

#endif //DREAM_ENTITY_H
