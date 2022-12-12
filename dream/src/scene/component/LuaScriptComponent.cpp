//
// Created by Deepak Ramalingam on 11/26/22.
//

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/project/Project.h"
#include "dream/util/IDUtils.h"

namespace Dream::Component {
    LuaScriptComponent::~LuaScriptComponent() {
        if (table.valid()) {
            table.abandon();
        }
    }

    LuaScriptComponent::LuaScriptComponent(std::string guid) {
        this->guid = std::move(guid);
        this->needToInitTable = true;
    }

    void LuaScriptComponent::loadScriptPath() {
        if (!this->guid.empty()) {
            this->scriptPath = Project::getResourceManager()->getFilePathFromGUID(this->guid);
            this->needToInitTable = true;
        }
    }

    void LuaScriptComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<LuaScriptComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_guid << YAML::Value << entity.getComponent<LuaScriptComponent>().guid;
            out << YAML::EndMap;
        }
    }

    void LuaScriptComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            auto guid = node[componentName][k_guid].as<std::string>();
            entity.addComponent<LuaScriptComponent>(guid);
        }
    }

    void LuaScriptComponent::changeScript(std::string newScriptPath) {
        this->guid = IDUtils::getGUIDForFile(std::move(newScriptPath));
        if (table.valid()) {
            table.abandon();
        }
        this->needToInitTable = true;
        this->loadScriptPath();
    }
}