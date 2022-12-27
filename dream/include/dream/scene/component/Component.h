//
// Created by Deepak Ramalingam on 11/12/22.
//

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
        Entity first {entt::null, nullptr};
        inline static std::string k_prev = "prev";
        Entity prev {entt::null, nullptr};
        inline static std::string k_next = "next";
        Entity next {entt::null, nullptr};
        inline static std::string k_parent = "parent";
        Entity parent {entt::null, nullptr};
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
        enum MeshType { PRIMITIVE_SPHERE, PRIMITIVE_CUBE, FROM_FILE };
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
        Mesh* mesh = nullptr;
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
        Texture* diffuseTexture = nullptr;
        inline static std::string k_guid = "guid";
        std::string guid;
        inline static std::string k_isEmbedded = "isEmbedded";
        bool isEmbedded;
        inline static std::string k_diffuseColor = "diffuseColor";
        glm::vec4 diffuseColor = {1, 1, 1, 1};
//        glm::vec4 specularColor = {1, 1, 1, 1}; // TODO: implement and serialize this
//        glm::vec4 ambientColor = {1, 1, 1, 1}; // TODO: implement and serialize this
        explicit MaterialComponent(std::string guid, bool isEmbedded);
        Texture* getTexture();
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
        };
        inline static std::string componentName = "AnimatorComponent";
        inline static std::string k_guid = "guid";          // guid of the animator file
        std::string guid;
        std::map<std::string, void*> animationObjects;      // each model file could have multiple animations
        std::vector<glm::mat4> m_FinalBoneMatrices;
        void* m_CurrentAnimation = nullptr;
        float m_CurrentTime = 0;
        int numTimesAnimationPlayed = 0;
        bool needsToLoadAnimations = true;
        bool needsToFindBoneEntities = true;
        std::map<int, Entity> boneEntities;
        // state machine variables
        int currentState;
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
        explicit AnimatorComponent();
        ~AnimatorComponent();
        AnimatorComponent(std::string animatorGUID);
        void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform, int depth=0);
        void blendTwoAnimations(void* pBaseAnimationV, void* pLayeredAnimationV, float blendFactor, float deltaTime);
        void calculateBlendedBoneTransform(
                void* pAnimationBaseV,  const AssimpNodeData* node,
                void* pAnimationLayerV, const AssimpNodeData* nodeLayered,
                const float currentTimeBase, const float currentTimeLayered,
                const glm::mat4& parentTransform,
                const float blendFactor);
        void updateAnimation(float dt);
        void updateStateMachine(float dt);
        void loadStateMachine(Entity modelEntity);
        void loadBoneEntities(Entity entity);
        void playAnimation(int stateID);
        int setVariable(const std::string& variableName, int value);
        std::vector<glm::mat4> computeFinalBoneMatrices(Entity armatureEntity, std::vector<Entity> bones);
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
}

#endif //DREAM_COMPONENT_H
