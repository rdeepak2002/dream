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

#ifndef DREAM_PHYSICSCOMPONENTSYSTEM_H
#define DREAM_PHYSICSCOMPONENTSYSTEM_H

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <glm/glm.hpp>
#include <vector>
#include "dream/renderer/OpenGLPhysicsDebugDrawer.h"

namespace Dream {
    class PhysicsComponentSystem {
    public:
        PhysicsComponentSystem();

        ~PhysicsComponentSystem();

        void init();

        void update(float dt);

        bool checkRaycast(glm::vec3 rayFromWorld, glm::vec3 rayToWorld);

        glm::vec3 raycastGetFirstHit(glm::vec3 rayFromWorld, glm::vec3 rayToWorld);

        void debugDrawWorld();

        int addColliderShape(btCompoundShape* colliderShape);

        btCompoundShape* getColliderShape(int index);

        int addRigidBody(btRigidBody* rigidBody);

        btRigidBody* getRigidBody(int index);

        void removeRigidBody(int index);

        void deleteCollisionShape(int index);

    private:
        btDefaultCollisionConfiguration *collisionConfiguration;
        btCollisionDispatcher *dispatcher;
        btBroadphaseInterface *overlappingPairCache;
        btSequentialImpulseConstraintSolver *solver;
        btDiscreteDynamicsWorld *dynamicsWorld;
        OpenGLPhysicsDebugDrawer openGlPhysicsDebugDrawer;
        std::vector<btCompoundShape*> colliderShapes;
        std::vector<btRigidBody*> rigidBodies;
    };
}

#endif //DREAM_PHYSICSCOMPONENTSYSTEM_H
