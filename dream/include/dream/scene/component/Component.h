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
        virtual void serialize(YAML::Emitter &out) {
            std::cout << "ERROR: serialize not implemented for component" << std::endl;
            exit(EXIT_FAILURE);
        }
        virtual std::string getComponentName() {
            std::cout << "ERROR: getName not implemented for component" << std::endl;
            exit(EXIT_FAILURE);
        }
    };

    struct RootComponent : public Component {
        std::string name = "root";
        void serialize(YAML::Emitter &out) override;
        std::string getComponentName() override;
    };

    struct IDComponent : public Component {
        std::string id = "";
        explicit IDComponent();
        IDComponent(std::string id);
        std::string getID();
    };

    struct TagComponent : public Component{
        std::string tag = "";
        explicit TagComponent(std::string tag);
        void serialize(YAML::Emitter &out) override;
        std::string getComponentName() override;
    };

    struct HierarchyComponent : public Component{
        Entity first {entt::null, nullptr};
        Entity prev {entt::null, nullptr};
        Entity next {entt::null, nullptr};
        Entity parent {entt::null, nullptr};
        void addChild(Entity &newChild, Entity &newParent);
        void removeChild(Entity &childToRemove);
        int numChildren();
    };

    struct TransformComponent : public Component{
        glm::vec3 translation = {0, 0, 0};
        glm::quat rotation = {0, 0, 0, 1};
        glm::vec3 scale = {1, 1, 1};
        glm::mat4 getTransform(Entity &curEntity);
    };

    struct MeshComponent : public Component {
        enum MeshType { PRIMITIVE_SPHERE, PRIMITIVE_CUBE, FROM_FILE };
        MeshType meshType = PRIMITIVE_CUBE;
        // data for meshes loaded from files
        std::string guid;
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
         * Create MeshComponent using primitive data
         * Example: cube, sphere, etc.
         * @param meshType
         * @param primitiveMeshData
         */
        MeshComponent(MeshType meshType, std::map<std::string, float> primitiveMeshData);
        Mesh* getMesh();
    };

    struct MaterialComponent : public Component{
        Texture* texture = nullptr;
        std::string guid;
        explicit MaterialComponent(std::string guid);
        Texture* getTexture();
    };
}

#endif //DREAM_COMPONENT_H
