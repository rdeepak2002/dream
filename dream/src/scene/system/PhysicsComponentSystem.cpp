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

#include "dream/scene/system/PhysicsComponentSystem.h"
#include "dream/project/Project.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    PhysicsComponentSystem::PhysicsComponentSystem() {
        collisionConfiguration = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        overlappingPairCache = new btDbvtBroadphase();
        solver = new btSequentialImpulseConstraintSolver;
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
        dynamicsWorld->setGravity(btVector3(0, -10, 0));
    }

    PhysicsComponentSystem::~PhysicsComponentSystem() {
        delete dynamicsWorld;
        delete solver;
        delete overlappingPairCache;
        delete dispatcher;
        delete collisionConfiguration;
    }

    void PhysicsComponentSystem::update(float dt) {
        // update all entities with rigid bodies
        auto rigidBodyEntities = Project::getScene()->getEntitiesWithComponents<Component::RigidBodyComponent>();
        for (auto entityHandle: rigidBodyEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            if (!entity.getComponent<Component::RigidBodyComponent>().rigidBody) {
                // initialize rigid body if necessary
                entity.getComponent<Component::RigidBodyComponent>().updateRigidBody(entity);
            }
            if (entity.getComponent<Component::RigidBodyComponent>().rigidBody) {
                if (entity.getComponent<Component::RigidBodyComponent>().shouldBeAddedToWorld) {
                    // add rigid body to world if necessary
                    dynamicsWorld->addRigidBody(entity.getComponent<Component::RigidBodyComponent>().rigidBody);
                    entity.getComponent<Component::RigidBodyComponent>().shouldBeAddedToWorld = false;
                }
                // update transform and rotation components of entity
                btTransform trans;
                entity.getComponent<Component::RigidBodyComponent>().rigidBody->getMotionState()->getWorldTransform(
                        trans);
                auto &transformComponent = entity.getComponent<Component::TransformComponent>();
                transformComponent.translation = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(),
                                                           trans.getOrigin().getZ());
                transformComponent.rotation = glm::quat(trans.getRotation().getW(), trans.getRotation().getX(),
                                                        trans.getRotation().getY(), trans.getRotation().getZ());
            } else {
                Logger::warn(
                        "Rigid body not initialized for entity " + entity.getComponent<Component::TagComponent>().tag);
            }
        }
        // update dynamic world
        float timeStep = 0.0f;
        if (Project::isPlaying()) {
            timeStep = dt;
        }
        dynamicsWorld->stepSimulation(timeStep, 10);
    }

    void PhysicsComponentSystem::init() {

    }
}
