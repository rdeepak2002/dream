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

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/project/Project.h"
#include "dream/util/Logger.h"

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
            Logger::fatal("Cannot remove null child");
        }

        if (first == childToRemove) {
            // case where child to remove is head of list
            if (childToRemove.getComponent<HierarchyComponent>().next) {
                // update 'prev' pointer of sibling that will be the new first child
                auto &nextEntity = childToRemove.getComponent<HierarchyComponent>().next;
                nextEntity.getComponent<HierarchyComponent>().prev = Entity {entt::null, nullptr};
            }
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

    void HierarchyComponent::addChild(Entity &newChild, Entity &newParent, bool atStart) {
        if (newParent) {
            // remove child from old parent
            Entity oldParent = newChild.getComponent<HierarchyComponent>().parent;
            if (oldParent) {
                oldParent.getComponent<HierarchyComponent>().removeChild(newChild);
            }
            // add new child as child of parent
            auto &parentHierarchyComp = newParent.getComponent<HierarchyComponent>();
            if (parentHierarchyComp.first) {
                // insert at front or back of linked list
                if (atStart) {
                    // insert child into front of list
                    auto formerFirstChild = parentHierarchyComp.first;
                    formerFirstChild.getComponent<HierarchyComponent>().prev = Entity {newChild.entityHandle, Project::getScene()};
                    newChild.getComponent<HierarchyComponent>().next = Entity {formerFirstChild.entityHandle, Project::getScene()};
                    newChild.getComponent<HierarchyComponent>().prev = Entity {entt::null, nullptr};
                    parentHierarchyComp.first = newChild;
                } else {
                    // insert child into end of list
                    auto endChild = parentHierarchyComp.first;
                    while (endChild.getComponent<HierarchyComponent>().next) {
                        endChild = endChild.getComponent<HierarchyComponent>().next;
                    }
                    endChild.getComponent<HierarchyComponent>().next = Entity { newChild.entityHandle, Project::getScene() };
                    newChild.getComponent<HierarchyComponent>().next = Entity {entt::null, nullptr};
                    newChild.getComponent<HierarchyComponent>().prev = Entity { endChild.entityHandle, Project::getScene() } ;
                }
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
            Logger::fatal("Cannot add child to non-existing parent");
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
                if (parentEntity) {
                    parentEntity.addChild(entity, false);
                } else {
                    Logger::fatal("Parent does not exist for id: " + parentEntityID);
                }
            }
        }
    }
}