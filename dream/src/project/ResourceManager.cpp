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
    if (guid.empty()) {
        std::cout << "guid cannot be empty" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (fileID.empty()) {
        std::cout << "fileID cannot be empty" << std::endl;
        exit(EXIT_FAILURE);
    }
    fileIDMap[std::make_pair(guid, fileID)] = data;
}

void Dream::ResourceManager::storeData(const std::string &guid, void *data) {
    if (guid.empty()) {
        std::cout << "guid cannot be empty" << std::endl;
        exit(EXIT_FAILURE);
    }
    fileIDMap[std::make_pair(guid, "")] = data;
}