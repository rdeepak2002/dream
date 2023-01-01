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
        dynamicsWorld->setDebugDrawer(&openGlPhysicsDebugDrawer);
    }

    PhysicsComponentSystem::~PhysicsComponentSystem() {
//        clearWorld();
        delete dynamicsWorld;
        delete solver;
        delete overlappingPairCache;
        delete dispatcher;
        delete collisionConfiguration;
    }

    void PhysicsComponentSystem::clearWorld() {
        if (dynamicsWorld) {
            // delete rigid bodies
//            for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
//                btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
//                btRigidBody* body = btRigidBody::upcast(obj);
//                if (body && body->getMotionState()) {
//                    delete body->getMotionState();
//                }
//                dynamicsWorld->removeCollisionObject(obj);
//                delete obj;
//            }
            // delete rigid bodies
            for (int i = (int) rigidBodies.size() - 1; i >= 0; i--) {
                dynamicsWorld->removeRigidBody(rigidBodies.at(i));
                delete rigidBodies.at(i)->getMotionState();
                delete rigidBodies.at(i);
                rigidBodies.erase(rigidBodies.begin() + i);
            }
            // delete collision shapes
            for (int i = (int) colliderShapes.size() - 1; i >= 0; i--) {
                delete colliderShapes.at(i);
                colliderShapes.erase(colliderShapes.begin() + i);
            }
            // reference -1 for rigid body entities
            auto rigidBodyEntities = Project::getScene()->getEntitiesWithComponents<Component::RigidBodyComponent>();
            for (auto entityHandle: rigidBodyEntities) {
                Entity entity = {entityHandle, Project::getScene()};
                entity.getComponent<Component::RigidBodyComponent>().rigidBodyIndex = -1;
            }
            // reference -1 for collision entities
            auto collisionEntities = Project::getScene()->getEntitiesWithComponents<Component::CollisionComponent>();
            for (auto entityHandle: collisionEntities) {
                Entity entity = {entityHandle, Project::getScene()};
                entity.getComponent<Component::CollisionComponent>().colliderShapeIndex = -1;
            }
            // clear vectors
            colliderShapes.clear();
            rigidBodies.clear();
        }
    }

    void PhysicsComponentSystem::removeRigidBodyFromWorld(btRigidBody* rb) {
        if (dynamicsWorld) {
            // delete rigid bodies
            if (rb && rb->isInWorld()) {
                delete rb->getMotionState();
                delete rb->getCollisionShape();
                dynamicsWorld->removeRigidBody(rb);
                delete rb;
            }
        } else {
            Logger::fatal("Dynamics world not initialized");
        }
    }

    void PhysicsComponentSystem::update(float dt) {
        // update dynamic world
        float timeStep = dt;
        dynamicsWorld->stepSimulation(timeStep);

        // update all entities with rigid bodies
        auto rigidBodyEntities = Project::getScene()->getEntitiesWithComponents<Component::RigidBodyComponent>();
        for (auto entityHandle: rigidBodyEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            if (entity.getComponent<Component::RigidBodyComponent>().rigidBodyIndex == -1) {
                // initialize rigid body if necessary
                entity.getComponent<Component::RigidBodyComponent>().updateRigidBody(entity);
            }
            if (entity.getComponent<Component::RigidBodyComponent>().rigidBodyIndex == -1) {
                Logger::warn("Rigid body not initialized for entity " + entity.getComponent<Component::TagComponent>().tag);
                continue;
            }
            int idx = entity.getComponent<Component::RigidBodyComponent>().rigidBodyIndex;
            auto *rigidBody = rigidBodies.at(idx);
            if (entity.getComponent<Component::RigidBodyComponent>().shouldBeAddedToWorld) {
                // add rigid body to world if necessary
                dynamicsWorld->addRigidBody(rigidBody);
                entity.getComponent<Component::RigidBodyComponent>().shouldBeAddedToWorld = false;
            }
            // update transform and rotation components of entity
            btTransform trans = rigidBody->getWorldTransform();
            auto &transformComponent = entity.getComponent<Component::TransformComponent>();
            if (Project::isPlaying()) {
                transformComponent.translation = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(),
                                                           trans.getOrigin().getZ());
                transformComponent.rotation = glm::quat(trans.getRotation().getW(), trans.getRotation().getX(),
                                                        trans.getRotation().getY(), trans.getRotation().getZ());
            } else {
                auto entityTrans = transformComponent.translation;
                auto entityRot = transformComponent.rotation;
                rigidBody->getWorldTransform().setOrigin(btVector3(entityTrans.x, entityTrans.y, entityTrans.z));
                rigidBody->getWorldTransform().setRotation(btQuaternion(entityRot.x, entityRot.y, entityRot.z, entityRot.w));
            }
        }
    }

    void PhysicsComponentSystem::init() {

    }

    bool PhysicsComponentSystem::checkRaycast(glm::vec3 rayFromWorld, glm::vec3 rayToWorld) {
        auto rfw = btVector3(rayFromWorld.x, rayFromWorld.y, rayFromWorld.z);
        auto rtw = btVector3(rayToWorld.x, rayToWorld.y, rayToWorld.z);
        btCollisionWorld::ClosestRayResultCallback raycastResult(rfw, rtw);
        dynamicsWorld->rayTest(rfw, rtw, raycastResult);
        if (raycastResult.hasHit()) {
            return true;
        } else {
            return false;
        }
    }

    void PhysicsComponentSystem::debugDrawWorld() {
        if (dynamicsWorld) {
            dynamicsWorld->debugDrawWorld();
        }
    }

    int PhysicsComponentSystem::addColliderShape(btCompoundShape* colliderShape) {
        colliderShapes.push_back(colliderShape);
        return (int) colliderShapes.size() - 1;
    }

    btCompoundShape* PhysicsComponentSystem::getColliderShape(int index) {
        return colliderShapes.at(index);
    }

    int PhysicsComponentSystem::addRigidBody(btRigidBody* rigidBody) {
        rigidBodies.push_back(rigidBody);
        return (int) rigidBodies.size() - 1;
    }

    btRigidBody* PhysicsComponentSystem::getRigidBody(int index) {
        return rigidBodies.at(index);
    }

    void PhysicsComponentSystem::removeRigidBody(int index) {
        // delete rigid bodies
        dynamicsWorld->removeRigidBody(rigidBodies.at(index));
        delete rigidBodies.at(index)->getMotionState();
        delete rigidBodies.at(index);
        rigidBodies.erase(rigidBodies.begin() + index);

        // push back index for rigid body entities
        auto rigidBodyEntities = Project::getScene()->getEntitiesWithComponents<Component::RigidBodyComponent>();
        for (auto entityHandle: rigidBodyEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            if (entity.getComponent<Component::RigidBodyComponent>().rigidBodyIndex > index) {
                entity.getComponent<Component::RigidBodyComponent>().rigidBodyIndex -= 1;
            } else if (entity.getComponent<Component::RigidBodyComponent>().rigidBodyIndex == index) {
                entity.getComponent<Component::RigidBodyComponent>().rigidBodyIndex = -1;
            }
        }
    }

    void PhysicsComponentSystem::deleteCollisionShape(int index) {
        // delete collision shapes
        delete colliderShapes.at(index);
        colliderShapes.erase(colliderShapes.begin() + index);

        // push back index for collision entities
        auto collisionEntities = Project::getScene()->getEntitiesWithComponents<Component::CollisionComponent>();
        for (auto entityHandle: collisionEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            if (entity.getComponent<Component::CollisionComponent>().colliderShapeIndex > index) {
                entity.getComponent<Component::CollisionComponent>().colliderShapeIndex -= 1;
            } else if (entity.getComponent<Component::CollisionComponent>().colliderShapeIndex == index) {
                entity.getComponent<Component::CollisionComponent>().colliderShapeIndex = -1;
            }
        }
    }
}
