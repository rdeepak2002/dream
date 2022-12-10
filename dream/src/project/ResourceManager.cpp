//
// Created by Deepak Ramalingam on 11/17/22.
//

#include "dream/project/ResourceManager.h"
#include "dream/util/Logger.h"

std::string Dream::ResourceManager::getFilePathFromGUID(const std::string& guid) {
    return guidMap[guid];
}

void Dream::ResourceManager::setFilePathFromGUID(const std::string &guid, const std::string &filepath) {
    guidMap[guid] = filepath;
}

void *Dream::ResourceManager::getData(const std::string& guid, const std::string& fileID) {
    return fileIDMap[std::make_pair(guid, fileID)];
}

void Dream::ResourceManager::storeData(const std::string &guid, const std::string &fileID, void* data) {
    if (guid.empty()) {
        Logger::fatal("GUID cannot be empty");
    }
    if (fileID.empty()) {
        Logger::fatal("FileID cannot be empty");
    }
    fileIDMap[std::make_pair(guid, fileID)] = data;
}

void Dream::ResourceManager::storeData(const std::string &guid, void *data) {
    if (guid.empty()) {
        Logger::fatal("GUID cannot be empty");
    }
    fileIDMap[std::make_pair(guid, "")] = data;
}

bool Dream::ResourceManager::hasData(const std::string &guid, const std::string &fileID) {
    return fileIDMap.count(std::make_pair(guid, fileID)) > 0;
}

bool Dream::ResourceManager::hasData(const std::string &guid) {
    return fileIDMap.count(std::make_pair(guid, "")) > 0;
}
