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

#include "dream/scene/system/TerrainComponentSystem.h"
#include "dream/project/Project.h"
#include "dream/scene/Entity.h"
#include "dream/scene/component/Component.h"
#include "dream/window/Input.h"
#include "dream/window/KeyCodes.h"

namespace Dream {
    void TerrainComponentSystem::init() {

    }

    void TerrainComponentSystem::update(float dt) {
        // update all entities with terrain components
        auto rigidBodyEntities = Project::getScene()->getEntitiesWithComponents<Component::TerrainComponent>();
        for (auto entityHandle: rigidBodyEntities) {
            Entity entity = {entityHandle, Project::getScene()};
            if (Input::getButtonDown(Key::LeftMouse)) {
                // TODO: make renderer agnostic
                OpenGLBaseTerrain *terrain = entity.getComponent<Component::TerrainComponent>().terrain;
                if (terrain) {
                    std::cout << "Refreshing triangle list" << std::endl;
                    int x = terrain->getSize() / 2;
                    int z = terrain->getSize() / 2;
                    float deltaHeight = 10.0f;
                    float currentHeight = terrain->getHeight(x, z);
                    terrain->setHeight(x, z, currentHeight + deltaHeight * dt);
                }
            }
        }
    }
}