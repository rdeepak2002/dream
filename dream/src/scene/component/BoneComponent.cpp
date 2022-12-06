//
// Created by Deepak Ramalingam on 12/6/22.
//

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/util/YAMLUtils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Dream::Component {
    BoneComponent::BoneComponent(std::string boneName, int boneID, glm::mat4 offset) {
        this->boneName = boneName;
        this->boneID = boneID;
        this->offset = offset;
    }

    BoneComponent::BoneComponent() {
        this->boneName = "";
        this->boneID = -1;
        this->offset = glm::mat4(1.0);
    }

    void BoneComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            auto boneName = node[componentName][k_boneName].as<std::string>();
            auto boneID = node[componentName][k_boneID].as<int>();
            glm::mat4 offset;
            YAML::convert<glm::mat4>().decode(node[componentName][k_offset], offset);
            entity.addComponent<BoneComponent>(boneName, boneID, offset);
            auto vertices = node[componentName][k_vertices].as<std::vector<int>>();
            auto weights = node[componentName][k_weights].as<std::vector<float>>();
            entity.getComponent<BoneComponent>().vertices = vertices;
            entity.getComponent<BoneComponent>().weights = weights;
        }
    }

    void BoneComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<BoneComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_boneName << YAML::Value << entity.getComponent<BoneComponent>().boneName;
            out << YAML::Key << k_boneID << YAML::Value << entity.getComponent<BoneComponent>().boneID;
            out << YAML::Key << k_offset << YAML::Value << YAML::convert<glm::mat4>().encode(entity.getComponent<BoneComponent>().offset);
            out << YAML::Key << k_vertices << YAML::Value << entity.getComponent<BoneComponent>().vertices;
            out << YAML::Key << k_weights << YAML::Value << entity.getComponent<BoneComponent>().weights;
            out << YAML::EndMap;
        }
    }
}