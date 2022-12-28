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

#ifndef DREAM_ASSETLOADER_H
#define DREAM_ASSETLOADER_H

#include "dream/renderer/Mesh.h"
#include "dream/scene/Entity.h"
#include "dream/renderer/AnimationData.h"

namespace Dream {
    class AssetLoader {
    public:
        virtual std::map<std::string, BoneInfo>
        loadMesh(std::string guid, bool createEntities = false, Entity rootEntity = {});

        virtual std::map<std::string, BoneInfo> loadBones(std::string guid);
    };
}

#endif //DREAM_ASSETLOADER_H
