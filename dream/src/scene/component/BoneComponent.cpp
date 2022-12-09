//
// Created by Deepak Ramalingam on 12/6/22.
//

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/util/YAMLUtils.h"

namespace Dream::Component {
    BoneComponent::BoneComponent(int boneID) {
        this->boneID = boneID;
    }

    void BoneComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            auto boneID = node[componentName][k_boneID].as<int>();
            entity.addComponent<BoneComponent>(boneID);
        }
    }

    void BoneComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<BoneComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_boneID << YAML::Value << entity.getComponent<BoneComponent>().boneID;
            out << YAML::EndMap;
        }
    }
}