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

#include "dream/project/ResourceManager.h"
#include "dream/util/Logger.h"

namespace Dream {
    std::string ResourceManager::getFilePathFromGUID(const std::string &guid) {
        return guidMap[guid];
    }

    void ResourceManager::setFilePathFromGUID(const std::string &guid, const std::string &filepath) {
        guidMap[guid] = filepath;
    }

    void *ResourceManager::getData(const std::string &guid, const std::string &fileID) {
        return fileIDMap[std::make_pair(guid, fileID)];
    }

    void ResourceManager::storeData(const std::string &guid, const std::string &fileID, void *data) {
        if (guid.empty()) {
            Logger::fatal("GUID cannot be empty");
        }
        if (fileID.empty()) {
            Logger::fatal("FileID cannot be empty");
        }
        fileIDMap[std::make_pair(guid, fileID)] = data;
    }

    void ResourceManager::storeData(const std::string &guid, void *data) {
        if (guid.empty()) {
            Logger::fatal("GUID cannot be empty");
        }
        fileIDMap[std::make_pair(guid, "")] = data;
    }

    bool ResourceManager::hasData(const std::string &guid, const std::string &fileID) {
        return fileIDMap.count(std::make_pair(guid, fileID)) > 0;
    }

    bool ResourceManager::hasData(const std::string &guid) {
        return fileIDMap.count(std::make_pair(guid, "")) > 0;
    }

    std::shared_ptr<Texture> ResourceManager::getTextureData(const std::string &guid, const std::string &fileID) {
        return textureDataMap[std::make_pair(guid, fileID)];
    }

    void ResourceManager::storeTextureData(Texture *texture, const std::string &guid, const std::string &fileID) {
        std::shared_ptr<Texture> ptr(texture);
        textureDataMap[std::make_pair(guid, fileID)] = ptr;
    }

    bool ResourceManager::hasTextureData(const std::string &guid, const std::string &fileID) {
        return textureDataMap.count(std::make_pair(guid, fileID)) > 0;
    }
}
