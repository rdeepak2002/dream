//
// Created by Deepak Ramalingam on 11/12/22.
//

#ifndef DREAM_SCENE_H
#define DREAM_SCENE_H

#include <iostream>
#include <entt/entt.hpp>

namespace Dream {
    class Entity;

    class Scene {
    public:
        Scene();
        ~Scene();
        Entity createEntity(const std::string& name = "Entity", bool rootEntity = false);
        void update();
        void fixedUpdate();
        template<typename... Components>
        auto getEntitiesWithComponents() {
            return entityRegistry.view<Components ...>();
        }
        Entity getRootEntity();
        void removeEntity(Entity &entity);
    private:
        entt::registry entityRegistry;
        friend class Entity;
    };
}

#endif //DREAM_SCENE_H
