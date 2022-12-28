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

namespace Dream::Component {
    RootComponent::RootComponent() {
        this->name = "root";
    }

    RootComponent::RootComponent(std::string name) {
        this->name = std::move(name);
    }

    void RootComponent::serialize(YAML::Emitter &out, Entity &entity) {
        if (entity.hasComponent<RootComponent>()) {
            out << YAML::Key << componentName;
            out << YAML::BeginMap;
            out << YAML::Key << k_name << YAML::Value << entity.getComponent<RootComponent>().name;
            out << YAML::EndMap;
        }
    }

    void RootComponent::deserialize(YAML::Node node, Entity &entity) {
        if (node[componentName]) {
            auto name = node[componentName][k_name].as<std::string>();
            entity.addComponent<RootComponent>(name);
        }
    }
}