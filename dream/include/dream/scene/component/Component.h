//
// Created by Deepak Ramalingam on 11/12/22.
//

#ifndef DREAM_COMPONENT_H
#define DREAM_COMPONENT_H

#include <iostream>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "dream/scene/Entity.h"
#include "dream/renderer/Mesh.h"

namespace Dream::Component {
    struct RootComponent {
        std::string name = "root";
    };

    struct TagComponent {
        std::string tag = "";
        explicit TagComponent(std::string tag);
    };

    struct HierarchyComponent {
        Entity first {entt::null, nullptr};
        Entity prev {entt::null, nullptr};
        Entity next {entt::null, nullptr};
        Entity parent {entt::null, nullptr};
        void addChild(Entity &newChild, Entity &newParent);
        void removeChild(Entity &childToRemove);
        int numChildren();
    };

    struct TransformComponent {
        glm::vec3 translation = {0, 0, 0};
        glm::quat rotation = {0, 0, 0, 1};
        glm::vec3 scale = {1, 1, 1};
        glm::mat4 getTransform();
    };

    struct MeshComponent {
        Mesh* mesh = nullptr;
        explicit MeshComponent(Mesh* meshIn);
    };
}

#endif //DREAM_COMPONENT_H
