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

#ifndef DREAM_RESOURCEMANAGER_H
#define DREAM_RESOURCEMANAGER_H

#include <iostream>
#include <unordered_map>
#include <map>
#include "dream/renderer/Texture.h"
#include "dream/renderer/Mesh.h"

namespace Dream {
    class ResourceManager {
    private:
        /**
         * Map to find the path of a file from its GUID
         * Key: guid
         * Value: filePath
         */
        std::map<std::string, std::string> guidMap;

        /**
         * Map to get texture data
         * Key: <guid, fileID>
         * Value: texture data
         */
        std::map<std::pair<std::string, std::string>, std::shared_ptr<Texture>> textureDataMap;

        /**
         * Map to get mesh data
         * Key: <guid, fileID>
         * Value: mesh data
         */
        std::map<std::pair<std::string, std::string>, std::shared_ptr<Mesh>> meshDataMap;
    public:
        /**
         * Get the path of a file given a GUID
         * @param guid the GUID of the file
         * @return path of the file from the root of the project directory
         */
        std::string getFilePathFromGUID(const std::string &guid);

        /**
         * Make a GUID refer to a filepath
         * @param guid the GUID of the file
         * @param filepath path of the file that can be referenced from teh GUID
         */
        void setFilePathFromGUID(const std::string &guid, const std::string &filepath);

        std::shared_ptr<Texture> getTextureData(const std::string &guid, const std::string &fileID = "");

        void storeTextureData(Texture *texture, const std::string &guid, const std::string &fileID = "");

        bool hasTextureData(const std::string &guid, const std::string &fileID = "");

        std::shared_ptr<Mesh> getMeshData(const std::string &guid, const std::string &fileID = "");

        void storeMeshData(Mesh *texture, const std::string &guid, const std::string &fileID = "");

        bool hasMeshData(const std::string &guid, const std::string &fileID = "");
    };
}

#endif //DREAM_RESOURCEMANAGER_H
