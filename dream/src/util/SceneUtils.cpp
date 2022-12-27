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

#include "dream/util/SceneUtils.h"
#include "dream/scene/component/Component.h"

namespace Dream {
    void SceneUtils::removeMeshReference(Dream::Entity entity, std::string meshGUID, bool recursively) {
        if (entity.hasComponent<Component::MeshComponent>() && entity.getComponent<Component::MeshComponent>().meshType == Component::MeshComponent::MeshType::FROM_FILE && entity.getComponent<Component::MeshComponent>().guid == meshGUID) {
            entity.removeComponent<Dream::Component::MeshComponent>();
            if (entity.hasComponent<Component::MaterialComponent>()) {
                entity.removeComponent<Component::MaterialComponent>();
            }
        }
        if (recursively) {
            Entity child = entity.getComponent<Component::HierarchyComponent>().first;
            while (child) {
                Entity nextChild = child.getComponent<Component::HierarchyComponent>().next;
                removeMeshReference(child, meshGUID, recursively);
                child = nextChild;
            }
        }
    }
}
