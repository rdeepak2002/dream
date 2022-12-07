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
        glm::quat rotation = {0, 0, 0, 1};
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
        // cache mesh in memory for quick usage during runtime
        Mesh* mesh = nullptr;
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
        void loadMesh();
        static void deserialize(YAML::Node node, Entity &entity);
        static void serialize(YAML::Emitter &out, Entity &entity);
        void readBones(Entity entity);
        static bool meshHasBones(Entity entity);
    };

    struct MaterialComponent : public Component {
        inline static std::string componentName = "MaterialComponent";
        Texture* diffuseTexture = nullptr;
        inline static std::string k_guid = "guid";
        std::string guid;
        inline static std::string k_isEmbedded = "isEmbedded";
        bool isEmbedded;
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
        explicit LuaScriptComponent(std::string guid);
        void loadScriptPath();
        void changeScript(std::string newScriptPath);
        static void deserialize(YAML::Node node, Entity &entity);
        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct AnimatorComponent : public Component {
        inline static std::string componentName = "AnimatorComponent";
        std::string foo = "Animator";
        explicit AnimatorComponent();
        static void deserialize(YAML::Node node, Entity &entity);
        static void serialize(YAML::Emitter &out, Entity &entity);
    };

    struct BoneComponent : public Component {
        inline static std::string componentName = "BoneComponent";
        inline static std::string k_boneName = "boneName";
        std::string boneName;
        inline static std::string k_boneID = "boneID";
        int boneID;
        inline static std::string k_offset = "offset";
        glm::mat4 offset;
        inline static std::string k_vertices = "vertices";
        std::vector<int> vertices;
        inline static std::string k_weights = "weights";
        std::vector<float> weights;
        explicit BoneComponent(std::string boneName, int boneID, glm::mat4 offset);
        BoneComponent();
        static void deserialize(YAML::Node node, Entity &entity);
        static void serialize(YAML::Emitter &out, Entity &entity);
    };
}

#endif //DREAM_COMPONENT_H
