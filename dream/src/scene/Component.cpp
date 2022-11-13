//
// Created by Deepak Ramalingam on 11/12/22.
//

#include "dream/scene/Component.h"

namespace Dream {
    namespace Component {
        int HierarchyComponent::numChildren() {
            int count = 0;
            Entity c = first;
            while (c) {
                count += 1;
                c = c.getComponent<HierarchyComponent>().next;
            }
            return count;
        }

        void HierarchyComponent::removeChild(Entity &childToRemove) {
            if (!childToRemove) {
                std::cout << "cannot remove null child" << std::endl;
                exit(EXIT_FAILURE);
            }

            if (first == childToRemove) {
                // case where child to remove is head of list
                first = childToRemove.getComponent<HierarchyComponent>().next;
            } else {
                auto &prevEntity = childToRemove.getComponent<HierarchyComponent>().prev;
                auto &nextEntity = childToRemove.getComponent<HierarchyComponent>().next;
                if (prevEntity) {
                    prevEntity.getComponent<HierarchyComponent>().next = nextEntity;
                }
                if (nextEntity) {
                    nextEntity.getComponent<HierarchyComponent>().prev = prevEntity;
                }
            }
        }

        void HierarchyComponent::addChild(Entity &newChild, Entity &newParent) {
            if (newParent) {
                // remove child from old parent
                Entity oldParent = newChild.getComponent<HierarchyComponent>().parent;
                if (oldParent) {
                    oldParent.getComponent<HierarchyComponent>().removeChild(newChild);
                }
                // add new child as child of parent
                auto &parentHierarchyComp = newParent.getComponent<HierarchyComponent>();
                if (parentHierarchyComp.first) {
                    // insert child into front of list
                    auto& formerFirstChild = parentHierarchyComp.first;
                    formerFirstChild.getComponent<HierarchyComponent>().prev = newChild;
                    newChild.getComponent<HierarchyComponent>().next = formerFirstChild;
                    newChild.getComponent<HierarchyComponent>().prev = Entity {entt::null, nullptr};
                    parentHierarchyComp.first = newChild;
                } else {
                    // make child head of list
                    newChild.getComponent<HierarchyComponent>().prev = Entity {entt::null, nullptr};
                    newChild.getComponent<HierarchyComponent>().next = Entity {entt::null, nullptr};
                    parentHierarchyComp.first = newChild;
                }
                // update parent of new child
                newChild.getComponent<HierarchyComponent>().parent = newParent;
            } else {
                std::cout << "cannot add child to non-existing parent" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        MeshComponent::MeshComponent(Mesh* meshIn) {
            this->mesh = meshIn;
        }

        TagComponent::TagComponent(std::string tag) {
            this->tag = std::move(tag);
        }
    }
}
