//
// Created by Deepak Ramalingam on 11/13/22.
//

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/project/Project.h"

namespace Dream::Component {
    HierarchyComponent::HierarchyComponent() {

    }

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
            newChild.getComponent<HierarchyComponent>().parentID = newParent.getID();
        } else {
            std::cout << "cannot add child to non-existing parent" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void HierarchyComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<HierarchyComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            auto& hierarchyComponent = entity.getComponent<HierarchyComponent>();
            if (hierarchyComponent.first) {
                out << YAML::Key << k_first << YAML::Value << hierarchyComponent.first.getComponent<IDComponent>().getID();
            }
            if (hierarchyComponent.prev) {
                out << YAML::Key << k_prev << YAML::Value << hierarchyComponent.prev.getComponent<IDComponent>().getID();
            }
            if (hierarchyComponent.next) {
                out << YAML::Key << k_next << YAML::Value << hierarchyComponent.next.getComponent<IDComponent>().getID();
            }
            if (hierarchyComponent.parent) {
                out << YAML::Key << k_parent << YAML::Value << hierarchyComponent.parent.getComponent<IDComponent>().getID();
            }
            out << YAML::EndMap;
        }
    }

    void HierarchyComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            // ignore root component
            if (!entity.hasComponent<RootComponent>()) {
                auto parentEntityID = node[componentName][k_parent].as<std::string>();
                Entity parentEntity = Project::getScene()->getEntityByID(parentEntityID);
                parentEntity.addChild(entity);  // TODO: call addChildEnd(entity) to add child to the end of the linked list
            }
        }
    }
}