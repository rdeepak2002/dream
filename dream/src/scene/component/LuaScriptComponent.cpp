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