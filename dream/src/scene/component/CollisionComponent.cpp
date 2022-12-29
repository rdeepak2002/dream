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

#include "dream/scene/component/Component.h"

#include "dream/util/YAMLUtils.h"

namespace Dream::Component {
    void CollisionComponent::serialize(YAML::Emitter &out, Dream::Entity &entity) {
        if (entity.hasComponent<CollisionComponent>()) {
            auto &collisionComponent = entity.getComponent<CollisionComponent>();
            out << YAML::Key << componentName;
            YAML::Node collidersNode = YAML::Node(YAML::NodeType::Sequence);
            for (const auto &collider : collisionComponent.colliders) {
                YAML::Node colliderNode = YAML::Node(YAML::NodeType::Map);
                colliderNode[Collider::k_type] = static_cast<int>(collider.type);
                colliderNode[Collider::k_offset] = YAML::convert<glm::vec3>().encode(collider.offset);
                colliderNode[Collider::k_halfExtents] = YAML::convert<glm::vec3>().encode(collider.halfExtents);
                colliderNode[Collider::k_axis] = static_cast<int>(collider.axis);
                colliderNode[Collider::k_height] = collider.height;
                colliderNode[Collider::k_radius] = collider.radius;
                colliderNode[Collider::k_assetGUID] = collider.assetGUID;
                collidersNode.push_back(colliderNode);
            }
            out << collidersNode;
        }
    }

    void CollisionComponent::deserialize(YAML::Node node, Dream::Entity &entity) {
        if (node[componentName]) {
            std::vector<Collider> colliders;
            for (const auto &colliderNode : node[componentName]) {
                auto type = static_cast<ColliderType>(colliderNode[Collider::k_type].as<int>());
                glm::vec3 offset;
                YAML::convert<glm::vec3>().decode(colliderNode[Collider::k_offset], offset);
                glm::vec3 halfExtents;
                YAML::convert<glm::vec3>().decode(colliderNode[Collider::k_halfExtents], halfExtents);
                auto axis = static_cast<Axis>(colliderNode[Collider::k_axis].as<int>());
                auto height = colliderNode[Collider::k_height].as<float>();
                auto radius = colliderNode[Collider::k_radius].as<float>();
                auto assetGUID = colliderNode[Collider::k_assetGUID].as<std::string>();
                Collider collider = {
                        .type=type,
                        .offset=offset,
                        .halfExtents=halfExtents,
                        .axis=axis,
                        .height=height,
                        .radius=radius,
                        .assetGUID=assetGUID
                };
                colliders.push_back(collider);
            }
            entity.addComponent<CollisionComponent>();
            entity.getComponent<CollisionComponent>().colliders = std::move(colliders);
        }
    }
}