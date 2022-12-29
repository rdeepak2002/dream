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

#ifndef DREAM_COMPONENT_H
#define DREAM_COMPONENT_H

#include <iostream>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <yaml-cpp/yaml.h>
#include "dream/scene/Entity.h"
#include "dream/renderer/Mesh.h"
#include "dream/renderer/Texture.h"
#include "dream/renderer/AnimationData.h"
#include "dream/renderer/AssimpNodeData.h"

namespace Dream::Component {
    struct Component {
    };

    struct RootComponent : public Component {
        inline static std::string componentName = "RootComponent";
        inline static std::string k_name = "name";
        std::string name = "root";

        explicit RootComponent();

        RootComponent(std::string name);

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct IDComponent : public Component {
        inline static std::string componentName = "IDComponent";
        inline static std::string k_id = "id";
        std::string id;

        explicit IDComponent();

        IDComponent(std::string id);

        std::string getID();

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct TagComponent : public Component {
        inline static std::string componentName = "TagComponent";
        inline static std::string k_tag = "tag";
        std::string tag;

        explicit TagComponent(std::string tag);

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct HierarchyComponent : public Component {
        inline static std::string componentName = "HierarchyComponent";
        inline static std::string k_first = "first";
        Entity first{entt::null, nullptr};
        inline static std::string k_prev = "prev";
        Entity prev{entt::null, nullptr};
        inline static std::string k_next = "next";
        Entity next{entt::null, nullptr};
        inline static std::string k_parent = "parent";
        Entity parent{entt::null, nullptr};
        std::string parentID;

        explicit HierarchyComponent();

        void addChild(Entity &newChild, Entity &newParent, bool atStart = true);

        void removeChild(Entity &childToRemove);

        int numChildren();

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct TransformComponent : public Component {
        inline static std::string componentName = "TransformComponent";
        inline static std::string k_translation = "translation";
        glm::vec3 translation = {0, 0, 0};
        inline static std::string k_rotation = "rotation";
        glm::quat rotation = {1, 0, 0, 0};
        inline static std::string k_scale = "scale";
        glm::vec3 scale = {1, 1, 1};

        glm::mat4 getTransform(Entity &curEntity);

        explicit TransformComponent();

        TransformComponent(glm::vec3 translation, glm::quat rotation, glm::vec3 scale);

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct MeshComponent : public Component {
        inline static std::string componentName = "MeshComponent";
        enum MeshType {
            PRIMITIVE_SPHERE, PRIMITIVE_CUBE, FROM_FILE
        };
        inline static std::string k_meshType = "meshType";
        MeshType meshType = PRIMITIVE_CUBE;
        // data for meshes loaded from files
        inline static std::string k_guid = "guid";
        std::string guid;
        inline static std::string k_fileId = "fileId";
        std::string fileId;
        // runtime for bone info map
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCount = 0;
        // cache mesh in memory for quick usage during runtime
        Mesh *mesh = nullptr;
        bool needsToLoadBones = true;

        /**
         * Create MeshComponent from file
         * Example: loading a dragon 3D model from a file
         * @param guid
         * @param fileID
         */
        explicit MeshComponent(std::string guid, std::string fileID);

        /**
         * References the mesh file as a parent
         * @param guid
         */
        MeshComponent(std::string guid);

        /**
         * Create MeshComponent using primitive data
         * Example: cube, sphere, etc.
         * @param meshType
         * @param primitiveMeshData
         */
        MeshComponent(MeshType meshType, std::map<std::string, float> primitiveMeshData);

        MeshComponent(std::string guid, std::map<std::string, BoneInfo> boneMap);

        void loadMesh();

        void changeMeshType(MeshType newMeshType, Entity entity);

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct MaterialComponent : public Component {
        inline static std::string componentName = "MaterialComponent";
        Texture *diffuseTexture = nullptr;
        inline static std::string k_guid = "guid";
        std::string guid;
        inline static std::string k_isEmbedded = "isEmbedded";
        bool isEmbedded;
        inline static std::string k_diffuseColor = "diffuseColor";
        glm::vec4 diffuseColor = {1, 1, 1, 1};

//        glm::vec4 specularColor = {1, 1, 1, 1}; // TODO: implement and serialize this
//        glm::vec4 ambientColor = {1, 1, 1, 1}; // TODO: implement and serialize this
        explicit MaterialComponent(std::string guid, bool isEmbedded);

        Texture *getTexture();

        void loadTexture();

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct LuaScriptComponent : public Component {
        inline static std::string componentName = "LuaScriptComponent";
        inline static std::string k_guid = "guid";
        std::string guid;
        // runtime script path
        std::string scriptPath;
        // runtime 'self' variables for script
        sol::table table = {};
        bool needToInitTable;

        ~LuaScriptComponent();

        explicit LuaScriptComponent(std::string guid);

        void loadScriptPath();

        void changeScript(std::string newScriptPath);

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct AnimatorComponent : public Component {
        struct State {
            std::string Guid = "";
            bool PlayOnce = true;
        };
        struct Condition {
            int Variable1Idx = -1;
            int Variable1 = 0;
            std::string Operator = "==";
            int Variable2Idx = -1;
            int Variable2 = 0;
        };
        struct Transition {
            int InputStateID;
            int OutputStateID;
            std::vector<Condition> Conditions;
            bool Blend;
        };
        inline static std::string componentName = "AnimatorComponent";
        inline static std::string k_guid = "guid";          // guid of the animator file
        std::string guid;
        std::map<std::string, void *> animationObjects;      // each model file could have multiple animations
        std::vector<glm::mat4> m_FinalBoneMatrices;
//        void *m_CurrentAnimation = nullptr;
        float m_CurrentTime = 0;
        int numTimesAnimationPlayed = 0;
        bool needsToLoadAnimations = true;
        bool needsToFindBoneEntities = true;
        float blendFactor = 0.0f;
        std::map<int, Entity> boneEntities;
        // state machine variables
        int currentState, nextState;
        inline static std::string k_states = "States";
        std::vector<State> states;
        inline static std::string k_transitions = "Transitions";
        inline static std::string k_transition_InputStateID = "To";
        inline static std::string k_transition_OutputStateID = "From";
        inline static std::string k_transition_Conditions = "Conditions";
        std::vector<Transition> transitions;
        inline static std::string k_variables = "Variables";
        inline static std::string k_variable_name = "Name";
        inline static std::string k_variable_value = "Value";
        std::vector<std::string> variableNames;
        std::vector<int> variableValues;
        float currentTimeLayered = 0.0f;
        float currentTimeBase = 0.0f;

        explicit AnimatorComponent();

        ~AnimatorComponent();

        AnimatorComponent(std::string animatorGUID);

//        void calculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform, int depth = 0);

        void blendTwoAnimations(void *pBaseAnimationV, void *pLayeredAnimationV, float blendFactor, float deltaTime);

        void calculateBlendedBoneTransform(
                void *pAnimationBaseV, const AssimpNodeData *node,
                void *pAnimationLayerV, const AssimpNodeData *nodeLayered,
                const float currentTimeBase, const float currentTimeLayered,
                const glm::mat4 &parentTransform,
                const float blendFactor,
                int depth = 0);

        void updateAnimation(float dt);

        void updateStateMachine(float dt);

        void loadStateMachine(Entity modelEntity);

        void loadBoneEntities(Entity entity);

//        void playAnimation(int stateID);

        int setVariable(const std::string &variableName, int value);

//        std::vector<glm::mat4> computeFinalBoneMatrices(Entity armatureEntity, std::vector<Entity> bones);

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct BoneComponent : public Component {
        inline static std::string componentName = "BoneComponent";
        inline static std::string k_boneID = "boneID";
        int boneID;

        explicit BoneComponent(int boneID);

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct CameraComponent : public Component {
        inline static std::string componentName = "CameraComponent";
        inline static std::string k_fov = "fov";
        float fov = 45.0f;
        inline static std::string k_zNear = "zNear";
        float zNear = 0.1f;
        inline static std::string k_zFar = "zFar";
        float zFar = 100.0f;
        inline static std::string k_lookAt = "lookAt";
        glm::vec3 lookAt = {0, 0, 0};
        inline static std::string k_front = "front";
        glm::vec3 front;
        inline static std::string k_up = "up";
        glm::vec3 up;
        inline static std::string k_right = "right";
        glm::vec3 right;
        inline static std::string k_worldUp = "worldUp";
        glm::vec3 worldUp = {0, -1.f, 0};

        explicit CameraComponent(float fov);

        glm::mat4 getViewMatrix(Entity camera);

        void updateCameraVectors(Entity camera);

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct SceneCameraComponent : public Component {
        inline static std::string componentName = "SceneCameraComponent";
        inline static std::string k_fov = "fov";
        float fov = 45.0f;
        inline static std::string k_zNear = "zNear";
        float zNear = 0.1f;
        inline static std::string k_zFar = "zFar";
        float zFar = 100.0f;
        inline static std::string k_front = "front";
        glm::vec3 front;
        inline static std::string k_up = "up";
        glm::vec3 up;
        inline static std::string k_right = "right";
        glm::vec3 right;
        inline static std::string k_worldUp = "worldUp";
        glm::vec3 worldUp;
        inline static std::string k_yaw = "yaw";
        float yaw;
        inline static std::string k_pitch = "pitch";
        float pitch;

        explicit SceneCameraComponent(float fov);

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);

        glm::mat4 getViewMatrix(Entity sceneCamera);

        void processInput(Entity sceneCamera, float dt);

        void updateCameraVectors();

        void lookAt(Entity sceneCamera, glm::vec3 lookAtPos);
    };

    struct CollisionComponent : public Component {
        inline static std::string componentName = "CollisionComponent";
        enum Axis {
            X, Y, Z
        };
        enum ColliderType {
            BOX, CAPSULE, CONE, CYLINDER, MESH, SPHERE
        };
        struct Collider {
            // type of collision primitive
            inline static std::string k_type = "type";
            ColliderType type = ColliderType::BOX;
            // offset
            inline static std::string k_offset = "offset";
            glm::vec3 offset = {0, 0, 0};
            // (box only) half-extents of the collision box: a 3-dimensional vector: local space half-width, half-height, and half-depth
            inline static std::string k_halfExtents = "halfExtents";
            glm::vec3 halfExtents = {1, 1, 1};
            // (capsule only) aligns the capsule with the local-space X, Y or Z axis of the entity
            inline static std::string k_axis = "axis";
            Axis axis = Axis::Y;
            // (capsule only) tip-to-tip height of the capsule
            inline static std::string k_height = "height";
            float height = 1.0f;
            // (sphere and capsule only) radius of the sphere or capsule body
            inline static std::string k_radius = "radius";
            float radius = 1.0f;
            // (mesh only) model asset that will be used as a source for the triangle-based collision mesh
            inline static std::string k_assetGUID = "assetGUID";
            std::string assetGUID = "";
        };
        std::vector<Collider> colliders;

        // runtime created collider shape
        btCompoundShape *colliderCompoundShape = nullptr;

        void updateColliderCompoundShape();

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct RigidBodyComponent : public Component {
        inline static std::string componentName = "RigidBodyComponent";
        enum RigidBodyType {
            STATIC, DYNAMIC, KINEMATIC
        };
        // type of rigid body primitive
        inline static std::string k_type = "type";
        RigidBodyType type = RigidBodyType::DYNAMIC;
        // mass of the body in kg
        inline static std::string k_mass = "mass";
        float mass = 1.0f;
        // proportion of linear velocity that is lost by the body every second
        inline static std::string k_linearDamping = "linearDamping";
        float linearDamping = 0.0f;
        // proportion of angular velocity that is lost by the body every second
        inline static std::string k_angularDamping = "angularDamping";
        float angularDamping = 0.0f;
        // multiplier for a body's linear movement in each world axis (can set x or z to 0 to create 2D game)
        inline static std::string k_linearFactor = "linearFactor";
        glm::vec3 linearFactor = {1, 1, 1};
        // multiplier for a body's angular (rotational) movement about each world axis
        inline static std::string k_angularFactor = "angularFactor";
        glm::vec3 angularFactor = {1, 1, 1};
        // controls how quickly a body loses velocity when in contact with other bodies
        inline static std::string k_friction = "friction";
        float friction = 0.5f;
        // a measure of the bounciness of a body between 0 and 1
        // setting to 1 means a moving body will never come to a stop (unless colliding with other bodies with restitutions below 1, or unless a stop is scripted)
        inline static std::string k_restitution = "restitution";
        float restitution = 0.5f;

        static void deserialize(YAML::Node node, Entity &entity);

        static void serialize(YAML::Emitter &out, Entity &entity);
    };
}

#endif //DREAM_COMPONENT_H
