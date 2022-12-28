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