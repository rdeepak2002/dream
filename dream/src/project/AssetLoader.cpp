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

#include "dream/project/AssetLoader.h"
#include <iostream>

namespace Dream {
    std::map<std::string, BoneInfo> AssetLoader::loadMesh(std::string guid, bool createEntities, Entity rootEntity) {
        return {};
    }

    std::map<std::string, BoneInfo> AssetLoader::loadBones(std::string guid) {
        return std::map<std::string, BoneInfo>();
    }
}
