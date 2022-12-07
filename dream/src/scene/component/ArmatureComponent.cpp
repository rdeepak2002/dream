//
// Created by Deepak Ramalingam on 12/7/22.
//

#include <utility>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "dream/scene/component/Component.h"
#include "dream/project/Project.h"

namespace Dream::Component {
    ArmatureComponent::ArmatureComponent(std::string boneName, int boneID) {
        this->boneName = boneName;
        this->boneID = boneID;
    }

    void ArmatureComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[ArmatureComponent::componentName]) {
            auto boneID = node[ArmatureComponent::componentName][ArmatureComponent::k_boneID].as<int>();
            auto boneName = node[ArmatureComponent::componentName][ArmatureComponent::k_boneName].as<std::string>();
            entity.addComponent<ArmatureComponent>(boneName, boneID);
        }
    }

    void ArmatureComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<ArmatureComponent>()) {
            out << YAML::Key << ArmatureComponent::componentName;
            out << YAML::BeginMap;
            out << YAML::Key << ArmatureComponent::k_boneID << YAML::Value << entity.getComponent<ArmatureComponent>().boneID;
            out << YAML::Key << ArmatureComponent::k_boneName << YAML::Value << entity.getComponent<ArmatureComponent>().boneName;
            out << YAML::EndMap;
        }
    }
}