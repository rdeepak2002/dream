//
// Created by Deepak Ramalingam on 11/17/22.
//

#include "dream/project/ResourceManager.h"

std::string Dream::ResourceManager::getFilePathFromGUID(const std::string& guid) {
    return guidMap[guid];
}

void *Dream::ResourceManager::getData(const std::string& guid, const std::string& fileID) {
    return fileIDMap[std::make_pair(guid, fileID)];
}

void Dream::ResourceManager::storeData(const std::string &guid, const std::string &fileID, void* data) {
    fileIDMap[std::make_pair(guid, fileID)] = data;
}
