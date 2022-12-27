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

#include "dream/scene/component/Component.h"
#include "dream/util/IDUtils.h"

namespace Dream::Component {
    IDComponent::IDComponent() {
        this->id = IDUtils::newGUID();
    }

    IDComponent::IDComponent(std::string id) {
        this->id = std::move(id);
    }

    std::string IDComponent::getID() {
        return this->id;
    }

    void IDComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<IDComponent>()) {
            out << YAML::Key << IDComponent::componentName;
            out << YAML::BeginMap;
            out << YAML::Key << IDComponent::k_id << YAML::Value << entity.getComponent<IDComponent>().id;
            out << YAML::EndMap;
        }
    }

    void IDComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            auto id = node[componentName][k_id].as<std::string>();
            entity.addComponent<IDComponent>(id);
        }
    }
}