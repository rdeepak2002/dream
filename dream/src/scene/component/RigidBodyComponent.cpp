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
    RigidBodyComponent::~RigidBodyComponent() {
        if (rigidBody && rigidBody->isInWorld()) {
            delete rigidBody;
        }
    }

    void RigidBodyComponent::serialize(YAML::Emitter &out, Dream::Entity &entity) {
        if (entity.hasComponent<RigidBodyComponent>()) {
            auto &rigidBodyComponent = entity.getComponent<RigidBodyComponent>();
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_type << YAML::Value << rigidBodyComponent.type;
            out << YAML::Key << k_mass << YAML::Value << rigidBodyComponent.mass;
            out << YAML::Key << k_linearDamping << YAML::Value << rigidBodyComponent.linearDamping;
            out << YAML::Key << k_angularDamping << YAML::Value << rigidBodyComponent.angularDamping;
            out << YAML::Key << k_linearFactor << YAML::Value
                << YAML::convert<glm::vec3>().encode(rigidBodyComponent.linearFactor);
            out << YAML::Key << k_angularFactor << YAML::Value
                << YAML::convert<glm::vec3>().encode(rigidBodyComponent.angularFactor);
            out << YAML::Key << k_friction << YAML::Value << rigidBodyComponent.friction;
            out << YAML::Key << k_restitution << YAML::Value << rigidBodyComponent.restitution;
            out << YAML::EndMap;
        }
    }

    void RigidBodyComponent::deserialize(YAML::Node node, Dream::Entity &entity) {
        if (node[componentName]) {
            auto type = static_cast<RigidBodyType>(node[componentName][k_type].as<int>());
            auto mass = node[componentName][k_mass].as<float>();
            auto linearDamping = node[componentName][k_linearDamping].as<float>();
            auto angularDamping = node[componentName][k_angularDamping].as<float>();
            glm::vec3 linearFactor;
            YAML::convert<glm::vec3>().decode(node[componentName][k_linearFactor], linearFactor);
            glm::vec3 angularFactor;
            YAML::convert<glm::vec3>().decode(node[componentName][k_angularFactor], angularFactor);
            auto friction = node[componentName][k_friction].as<float>();
            auto restitution = node[componentName][k_restitution].as<float>();
            entity.addComponent<RigidBodyComponent>();
            entity.getComponent<RigidBodyComponent>().type = type;
            entity.getComponent<RigidBodyComponent>().mass = mass;
            entity.getComponent<RigidBodyComponent>().linearDamping = linearDamping;
            entity.getComponent<RigidBodyComponent>().angularDamping = angularDamping;
            entity.getComponent<RigidBodyComponent>().linearFactor = linearFactor;
            entity.getComponent<RigidBodyComponent>().angularFactor = angularFactor;
            entity.getComponent<RigidBodyComponent>().friction = friction;
            entity.getComponent<RigidBodyComponent>().restitution = restitution;
//            entity.getComponent<RigidBodyComponent>().updateRigidBody(entity);
        }
    }

    void RigidBodyComponent::updateRigidBody(Entity &entity) {
        delete rigidBody;

        const auto &transformComponent = entity.getComponent<TransformComponent>();
        const auto &translation = transformComponent.translation;
        const auto &rotation = transformComponent.rotation;

        btVector3 localInertia(0, 0, 0);
        if (entity.hasComponent<CollisionComponent>()) {
            if (!entity.getComponent<CollisionComponent>().colliderCompoundShape) {
                entity.getComponent<CollisionComponent>().updateColliderCompoundShape();
            }
            if (!entity.getComponent<CollisionComponent>().colliderCompoundShape) {
                Logger::fatal("Unable to initialize compound collider shape");
            }
            entity.getComponent<CollisionComponent>().colliderCompoundShape->calculateLocalInertia(mass, localInertia);
        } else {
            Logger::fatal("Rigid body cannot be initialized because entity " + entity.getComponent<TagComponent>().tag +
                          " does not have collision component");
        }

        if (type == RigidBodyComponent::DYNAMIC) {
            auto *motionState = new btDefaultMotionState(
                    btTransform(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w),
                                btVector3(translation.x, translation.y, translation.z)));
            btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState,
                                                                 entity.getComponent<CollisionComponent>().colliderCompoundShape,
                                                                 localInertia);
            rigidBody = new btRigidBody(rigidBodyCI);
            rigidBody->setFriction(friction);
            rigidBody->setAnisotropicFriction(
                    entity.getComponent<CollisionComponent>().colliderCompoundShape->getAnisotropicRollingFrictionDirection(),
                    btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
            rigidBody->setDamping(linearDamping, angularDamping);
            rigidBody->setRestitution(restitution);
            rigidBody->setLinearFactor(btVector3(linearFactor.x, linearFactor.y, linearFactor.z));
            rigidBody->setAngularFactor(btVector3(angularFactor.x, angularFactor.y, angularFactor.z));
            rigidBody->activate();
            rigidBody->setActivationState(DISABLE_DEACTIVATION);
        } else if (type == RigidBodyComponent::KINEMATIC) {
            auto *motionState = new btDefaultMotionState(
                    btTransform(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w),
                                btVector3(translation.x, translation.y, translation.z)));
            btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState,
                                                                 entity.getComponent<CollisionComponent>().colliderCompoundShape,
                                                                 localInertia);
            rigidBody = new btRigidBody(rigidBodyCI);
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            rigidBody->setFriction(friction);
            rigidBody->setAnisotropicFriction(
                    entity.getComponent<CollisionComponent>().colliderCompoundShape->getAnisotropicRollingFrictionDirection(),
                    btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
            rigidBody->setDamping(linearDamping, angularDamping);
            rigidBody->setRestitution(restitution);
            rigidBody->setLinearFactor(btVector3(linearFactor.x, linearFactor.y, linearFactor.z));
            rigidBody->setAngularFactor(btVector3(angularFactor.x, angularFactor.y, angularFactor.z));
            rigidBody->activate();
            rigidBody->setActivationState(DISABLE_DEACTIVATION);
        } else if (type == RigidBodyComponent::STATIC) {
            auto *motionState = new btDefaultMotionState(
                    btTransform(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w),
                                btVector3(translation.x, translation.y, translation.z)));
            btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState,
                                                                 entity.getComponent<CollisionComponent>().colliderCompoundShape,
                                                                 localInertia);
            rigidBody = new btRigidBody(rigidBodyCI);
            rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            rigidBody->setFriction(friction);
            rigidBody->setAnisotropicFriction(
                    entity.getComponent<CollisionComponent>().colliderCompoundShape->getAnisotropicRollingFrictionDirection(),
                    btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
            rigidBody->setDamping(linearDamping, angularDamping);
            rigidBody->setRestitution(restitution);
            rigidBody->setLinearFactor(btVector3(linearFactor.x, linearFactor.y, linearFactor.z));
            rigidBody->setAngularFactor(btVector3(angularFactor.x, angularFactor.y, angularFactor.z));
            rigidBody->activate();
            rigidBody->setActivationState(DISABLE_DEACTIVATION);
        } else {
            Logger::fatal("Unknown rigid body type " + std::to_string(static_cast<int>(type)));
        }
    }
}