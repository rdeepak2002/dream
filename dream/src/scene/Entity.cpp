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

#include "dream/scene/Entity.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    Entity::Entity() {
        entityHandle = entt::null;
        scene = nullptr;
    }

    Entity::Entity(entt::entity handle, Dream::Scene *scene) : entityHandle(handle), scene(scene) {

    }

    Entity::operator bool() const {
        return this->entityHandle != entt::null;
    }

    bool Entity::operator==(const Dream::Entity &other) const {
        return entityHandle == other.entityHandle && scene == other.scene;
    }

    bool Entity::operator!=(const Dream::Entity &other) const {
        return !(*this == other);
    }

    Entity::operator entt::entity() const {
        return entityHandle;
    }

    void Entity::addChild(Entity entity, bool atStart) {
        getComponent<Component::HierarchyComponent>().addChild(entity, *this, atStart);
    }

    int Entity::numChildren() {
        return getComponent<Component::HierarchyComponent>().numChildren();
    }

    void Entity::serialize(YAML::Emitter& out) {
        // serialize this entity
        out << YAML::BeginMap;
        out << YAML::Key << "Entity" << YAML::Value << getComponent<Dream::Component::IDComponent>().getID();
        Component::RootComponent::serialize(out, *this);
        Component::IDComponent::serialize(out, *this);
        Component::TagComponent::serialize(out, *this);
        Component::HierarchyComponent::serialize(out, *this);
        Component::TransformComponent::serialize(out, *this);
        Component::MeshComponent::serialize(out, *this);
        Component::MaterialComponent::serialize(out, *this);
        Component::LuaScriptComponent::serialize(out, *this);
        Component::AnimatorComponent::serialize(out, *this);
        Component::BoneComponent::serialize(out, *this);
        Component::SceneCameraComponent::serialize(out, *this);
        Component::CameraComponent::serialize(out, *this);
        out << YAML::EndMap;
        // serialize children
        Entity child = getComponent<Component::HierarchyComponent>().first;
        while (child) {
            child.serialize(out);
            child = child.getComponent<Component::HierarchyComponent>().next;
        }
    }

    void Entity::deserialize(YAML::Node node) {
        if (node["Entity"]) {
            auto id = node["Entity"].as<std::string>();
            addComponent<Component::IDComponent>(id);
        }
        Component::RootComponent::deserialize(node, *this);
        Component::IDComponent::deserialize(node, *this);
        Component::TagComponent::deserialize(node, *this);
        Component::HierarchyComponent::deserialize(node, *this);
        Component::TransformComponent::deserialize(node, *this);
        Component::MeshComponent::deserialize(node, *this);
        Component::MaterialComponent::deserialize(node, *this);
        Component::LuaScriptComponent::deserialize(node, *this);
        Component::AnimatorComponent::deserialize(node, *this);
        Component::BoneComponent::deserialize(node, *this);
        Component::SceneCameraComponent::deserialize(node, *this);
        Component::CameraComponent::deserialize(node, *this);
    }

    std::string Entity::getID() {
        return getComponent<Component::IDComponent>().getID();
    }

    bool Entity::isValid() {
        bool isInvalid = (scene == nullptr || entityHandle == entt::null);
        return !isInvalid;
    }
}
