//
// Created by Deepak Ramalingam on 11/12/22.
//

#ifndef DREAM_COMPONENT_H
#define DREAM_COMPONENT_H

#include <iostream>
#include <utility>
#include <entt/entt.hpp>

namespace Dream::Component {
    struct RootComponent {
        std::string name = "root";
    };

    struct HierarchyComponent {
        std::vector<entt::entity> children;
        void addChild(entt::entity child) {
            children.push_back(child);
        }
        std::vector<entt::entity> getChildren() {
            return children;
        }
    };

    struct TagComponent {
        std::string tag = "";
        explicit TagComponent(std::string tag) {
            this->tag = std::move(tag);
        }
    };

    struct MeshComponent {
        std::string name;
        explicit MeshComponent(std::string name) {
            this->name = std::move(name);
        }
    };
}

#endif //DREAM_COMPONENT_H
