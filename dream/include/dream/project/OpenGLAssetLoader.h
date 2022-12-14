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

#ifndef DREAM_OPENGLMESHIMPORTER_H
#define DREAM_OPENGLMESHIMPORTER_H

#include "dream/project/AssetLoader.h"
#include "dream/renderer/AnimationData.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <iostream>

namespace Dream {
    class OpenGLAssetLoader : public AssetLoader {
    public:
        std::map<std::string, BoneInfo>
        loadMesh(std::string guid, bool createEntities = false, Entity rootEntity = {}) override;

        std::map<std::string, BoneInfo> loadBones(std::string guid) override;

    private:
        Entity processNode(std::string path, std::string guid, aiNode *node, const aiScene *scene, bool createEntities,
                           Entity rootEntity, bool createMeshObjects);

        Entity processMesh(std::string path, std::string guid, aiMesh *mesh, const aiScene *scene, bool createEntities,
                           bool createMeshObjects);

        void setVertexBoneData(Vertex &vertex, int boneID, float weight);

        void extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh);

        /**
         * Assign a unique ID per submesh within a single Mesh (allows for unique FileID's)
         */
        int meshID = 0;
        // bone variables
        int boneCount = 0;
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        std::vector<Entity> nodeEntities;
    };
}

#endif //DREAM_OPENGLMESHIMPORTER_H
