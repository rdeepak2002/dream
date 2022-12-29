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
    void CollisionComponent::updateColliderCompoundShape() {
        delete colliderCompoundShape;
        colliderCompoundShape = new btCompoundShape();
        for (const auto &collider : colliders) {
            btTransform t;
            t.setIdentity();
            t.setOrigin(btVector3(collider.offset.x, collider.offset.y, collider.offset.z));
            if (collider.type == BOX) {
                auto shape = new btBoxShape(btVector3(collider.halfExtents.x, collider.halfExtents.y, collider.halfExtents.z));
                colliderCompoundShape->addChildShape(t, shape);
            } else if (collider.type == CAPSULE) {
                btCapsuleShape *shape = nullptr;
                if (collider.axis == Y) {
                    shape = new btCapsuleShape(collider.radius, collider.height);
                } else if (collider.axis == X) {
                    shape = new btCapsuleShapeX(collider.radius, collider.height);
                } else if (collider.axis == Z) {
                    shape = new btCapsuleShapeZ(collider.radius, collider.height);
                } else {
                    Logger::fatal("Unknown capsule axis " + std::to_string(collider.axis));
                }
                colliderCompoundShape->addChildShape(t, shape);
            } else if (collider.type == CONE) {
                btConeShape *shape = nullptr;
                if (collider.axis == Y) {
                    shape = new btConeShape(collider.radius, collider.height);
                } else if (collider.axis == X) {
                    shape = new btConeShapeX(collider.radius, collider.height);
                } else if (collider.axis == Z) {
                    shape = new btConeShapeZ(collider.radius, collider.height);
                } else {
                    Logger::fatal("Unknown cone axis " + std::to_string(collider.axis));
                }
                colliderCompoundShape->addChildShape(t, shape);
            } else if (collider.type == CYLINDER) {
                btCylinderShape *shape = nullptr;
                if (collider.axis == Y) {
                    shape = new btCylinderShape(btVector3(collider.halfExtents.x, collider.halfExtents.y, collider.halfExtents.z));
                } else if (collider.axis == X) {
                    shape = new btCylinderShapeX(btVector3(collider.halfExtents.x, collider.halfExtents.y, collider.halfExtents.z));
                } else if (collider.axis == Z) {
                    shape = new btCylinderShapeZ(btVector3(collider.halfExtents.x, collider.halfExtents.y, collider.halfExtents.z));
                } else {
                    Logger::fatal("Unknown cylinder axis " + std::to_string(collider.axis));
                }
                colliderCompoundShape->addChildShape(t, shape);
            } else if (collider.type == MESH) {
//                auto *shape = btTriangleMeshShape();
//                colliderCompoundShape->addChildShape(t, shape);
                // TODO
                Logger::fatal("TODO: support mesh shape loading in CollisionComponent");
            } else if (collider.type == SPHERE) {
                auto *shape = new btSphereShape(collider.radius);
                colliderCompoundShape->addChildShape(t, shape);
            } else {
                Logger::fatal("Unknown collider type " + std::to_string(static_cast<int>(collider.type)));
            }
        }
    }

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
            entity.getComponent<CollisionComponent>().updateColliderCompoundShape();
        }
    }
}